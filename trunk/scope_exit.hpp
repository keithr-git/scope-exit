//
// scope_exit - execute code upon leaving a scope block, whether by
// falling off the end (or terminating the loop) or because of
// an exception.
//
// Copyright 2008, 2009 Keith Reynolds.
//
// This program is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU Lesser General Public License for
// more details.
//
// You should have received a copy of the GNU Lesser General
// Public License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//
// Basic Usage
//
// ON_SCOPE_EXIT(variable_list, code)
//
// Execute the specified code when the scope exits, making the
// variables in variable_list available to that code.  For example:
//
//     FILE* fp = fopen(file, "r");
//     ON_SCOPE_EXIT((fp), fclose(fp));
//
// The variable list must be in the form of a Boost.Preprocessor
// sequence:
// 
//     (var1) (var2) (var3)
//
// That is, each variable name must be enclosed in parentheses, and
// there must be no commas or other delmiters between the names.  Any
// amount of whitespace (including none) is OK.
//
// Class Methods
//
// Using ON_SCOPE_EXIT inside a class method is the same as for any
// other function, except that the this pointer is not available.
// It must be copied to another pointer:
//
// void class::method()
// {
//     SCOPE_EXIT_SELF_POINTER(self);
//
//     ON_SCOPE_EXIT((self), self->other_method());
// }
//
// There is also a SCOPE_EXIT_SELF_REFERENCE macro, which declares
// the variable as a reference to the current object instead of a
// pointer.
//
// Template Variables
//
// Variables of a template type must be declared using the
// typename keyword, and so they must be identified to
// ON_SCOPE_EXIT.  This is done with the SCOPE_EXIT_TEMPLATE_VAR
// macro:
//
// template<typename T>
// void f(T& t)
// {
//     int i, x;
//
//     ON_SCOPE_EXIT((i) SCOPE_EXIT_TEMPLATE_VAR(t) (x),
//         /* Do something with i, t, and x */
//     );
// }
//
// Rollback
//
// Built on top of ON_SCOPE_EXIT is a simple rollback facility.
// A sequence of statements and their associated rollback operations
// can be contained in a block, and the rollback operations will
// be carried out if the block exits because of an exception, but
// not if the block exits normally (unless rollback is explicitly
// forced).
//
// void Database::add(const Record& record)
// {
//     ROLLBACK_BLOCK(
//         m_table1.insert(record);
//         ON_ROLLBACK((m_table1) (record), m_table1.erase(record));
//         m_table2.insert(record);
//     );
// }
//
// In this example, if inserting the record into m_table2 throws
// an exception, the record will be erased from m_table1.
//
// A rollback block is just a special case of a scope block,
// so ON_SCOPE_EXIT can be used inside a rollback block for
// code that should always run when the scope exits.
//
// Rollback can be explicitly forced by using the ROLLBACK_FORCE macro.
// After this macro is called, all rollback operations will be
// performed when the block exits.  It does not cause the block
// to exit immediately; the remaining statements in the block will
// be executed normally.  This is because, while the ROLLBACK_BLOCK
// macro is implemented as a for loop, using break to terminate the
// loop early does not work if the ROLLBACK_FORCE macro is called
// from inside another nested loop.  For example:
//
// void f()
// {
//     ROLLBACK_BLOCK(
//         ROLLBACK_FORCE(); // Would exit the block if it contained a break.
//         for (int i = 0; i < 10; ++i) {
//             ROLLBACK_FORCE(); // Would terminate the loop instead.
//         }
//     );
// }
//
#ifndef SCOPE_EXIT_HPP
#define SCOPE_EXIT_HPP

#include <boost/preprocessor.hpp>
#include <boost/typeof/typeof.hpp>

namespace scope_exit
{
    //
    // BOOST_TYPEOF() discards const qualifiers (at least on compilers that
    // don't have a typeof operator).  The preserve_const struct template
    // is used to put the const back for const arguments.  It is
    // used as:
    //
    // preserve_const<BOOST_TYPEOF(arg), sizeof(is_const(arg).value)>::type
    // 
    template <typename T>
    struct is_const_helper
    {
        char value[1];
    };

    template <typename T>
    struct is_const_helper<const T>
    {
        char value[2];
    };

    template <typename T>
    is_const_helper<T> is_const(T& t);

    template <typename T>
    is_const_helper<const T> is_const(const T& t);

    template <typename T, std::size_t IS_CONST>
    struct preserve_const;

    template <typename T>
    struct preserve_const<T, 1>
    {
        typedef T type;
    };

    template <typename T>
    struct preserve_const<T, 2>
    {
        typedef const T type;
    };
} // namespace scope_exit

//
// Variables of template type need the typename keyword, but using
// typename outside of a template is an error.   So we have to figure
// out when to use it and when to leave it out.  This is accomplished
// by transforming the passed-in variable names into a two-element
// sequence: the first element is the variable name, and the second
// is either "typename" if the typename keyword is required, or an
// empty element () if it is not.
//
// The only Boost.Preprocessor data structure that can handle a
// single element without any decoration is a list: BOOST_PP_LIST_SIZE(a)
// returns 0, while BOOST_PP_LIST_SIZE(a, b) returns 1.  Using this,
// SCOPE_EXIT_TEMPLATE_VAR(var) expands to ((var, typename)), and
// the call:
//
//     ON_SCOPE_EXIT((a) SCOPE_EXIT_TEMPLATE_VAR(t1) (b), code);
//
// yields a variable sequence that looks like:
//
//     (a) ((var, typename)) (b)
//
// We can use BOOST_PP_LIST_SIZE() to determine whether a given
// element is a regular or template variable, and convert the sequence
// into:
//
//     ((a) ()) ((var) (typename)) ((b) ())
//
// Each element is a two-element sequence, which means the rest of
// the code can treat them the same.  The SCOPE_EXIT_VAR_NAME() macro
// extracts the name from an element, and SCOPE_EXIT_VAR_TYPENAME()
// expands to either "typename" or an empty string.
//
#define SCOPE_EXIT_VAR(r, data, var) \
        BOOST_PP_IF(BOOST_PP_LIST_SIZE(var), \
                ((BOOST_PP_LIST_FIRST(var)) (BOOST_PP_LIST_REST(var))), \
                ((var) (BOOST_PP_EMPTY())))

#define SCOPE_EXIT_VARS(vars) \
        BOOST_PP_SEQ_FOR_EACH(SCOPE_EXIT_VAR, 0, vars)

#define SCOPE_EXIT_TEMPLATE_VAR(var) \
        ((var, typename))

#define SCOPE_EXIT_VAR_NAME(var) \
        BOOST_PP_SEQ_ELEM(0, var)

#define SCOPE_EXIT_VAR_TYPENAME(var) \
        BOOST_PP_SEQ_ELEM(1, var)

//
// On compilers that don't have a typeof keyword, Boost.Typeof uses
// a Boost.MPL vector to derive the type.  The effect that we're
// concerned with is the fact that it requires a second use of the
// typename keyword.  SCOPE_EXIT_MAYBE_TYPENAME(typename) expands
// to "typename" if the second use of typename is needed, and to
// nothing otherwise.
//
#ifdef BOOST_TYPEOF_EMULATION
# define SCOPE_EXIT_MAYBE_TYPENAME(t) t
#else
# define SCOPE_EXIT_MAYBE_TYPENAME(t)
#endif // BOOST_TYPEOF_EMULATION

//
// Given a variable name, return a declaration of its type,
// with suitable use of the typename keyword.
//
#define SCOPE_EXIT_TYPEOF(var) \
        SCOPE_EXIT_VAR_TYPENAME(var) \
            ::scope_exit::preserve_const<\
                SCOPE_EXIT_MAYBE_TYPENAME(SCOPE_EXIT_VAR_TYPENAME(var)) \
                    BOOST_TYPEOF(SCOPE_EXIT_VAR_NAME(var)), \
                        sizeof(::scope_exit::is_const( \
                                    SCOPE_EXIT_VAR_NAME(var)).value)>::type

#define SCOPE_EXIT_STRUCT_NAME(line) \
        BOOST_PP_CAT(_scope_exit_, line)

#define SCOPE_EXIT_TYPE_NAME(line, var) \
        BOOST_PP_CAT(_scope_exit_, BOOST_PP_CAT(line, \
                    BOOST_PP_CAT(_typeof_, SCOPE_EXIT_VAR_NAME(var))))

#define SCOPE_EXIT_ARG_NAME(i) \
        BOOST_PP_CAT(arg, i)

#define SCOPE_EXIT_TYPE_DECL(r, line, i, var) \
        typedef SCOPE_EXIT_TYPEOF(var) SCOPE_EXIT_TYPE_NAME(line, var);

#define SCOPE_EXIT_MEMBER_DECL(r, line, i, var) \
        SCOPE_EXIT_TYPE_NAME(line, var)& SCOPE_EXIT_VAR_NAME(var);

#define SCOPE_EXIT_EXPAND(...) __VA_ARGS__

#define SCOPE_EXIT_INITIALIZER(r, line, i, var) \
        BOOST_PP_COMMA_IF(i) SCOPE_EXIT_VAR_NAME(var)

#define SCOPE_EXIT_IMPL(line, vars, ...) \
        BOOST_PP_SEQ_FOR_EACH_I(SCOPE_EXIT_TYPE_DECL, line, vars) \
        struct SCOPE_EXIT_STRUCT_NAME(line) { \
            BOOST_PP_SEQ_FOR_EACH_I(SCOPE_EXIT_MEMBER_DECL, line, vars) \
            ~SCOPE_EXIT_STRUCT_NAME(line)() { __VA_ARGS__; } \
            void use() const {} \
        } SCOPE_EXIT_STRUCT_NAME(line) \
            BOOST_PP_EXPAND(SCOPE_EXIT_EXPAND \
                BOOST_PP_IF(BOOST_PP_SEQ_SIZE(vars), \
                    (= {BOOST_PP_SEQ_FOR_EACH_I(SCOPE_EXIT_INITIALIZER, \
                        line, vars)}), ())); \
        SCOPE_EXIT_STRUCT_NAME(line).use();

#define ON_SCOPE_EXIT(vars, ...) \
        SCOPE_EXIT_IMPL(__LINE__, SCOPE_EXIT_VARS(vars), __VA_ARGS__)

#define SCOPE_EXIT_SELF_POINTER(var) \
        SCOPE_EXIT_TYPEOF((*this)())* (var) = this

#define SCOPE_EXIT_SELF_REFERENCE(var) \
        SCOPE_EXIT_TYPEOF((*this)())& (var) = *this

namespace scope_exit
{
    //
    // Enum for the rollback state.  Setting it to ROLLBACK_FORCED
    // prevents the change to ROLLBACK_COMMITTED at the end of the
    // rollback block, thereby causing the rollback actions to take
    // place.
    //
    typedef enum {
        ROLLBACK_PENDING,
        ROLLBACK_COMMITTED,
        ROLLBACK_FORCED
    } rollback_state;
} // namespace scope_exit

#ifdef BOOST_TYPEOF
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(::scope_exit::rollback_state)
#endif

#define ROLLBACK_BLOCK(...) \
        for (::scope_exit::rollback_state _rollback_state = \
                    ::scope_exit::ROLLBACK_PENDING; \
                _rollback_state == ::scope_exit::ROLLBACK_PENDING; \
                _rollback_state = ::scope_exit::ROLLBACK_FORCED) \
        { \
            __VA_ARGS__ \
            if (_rollback_state == ::scope_exit::ROLLBACK_PENDING) \
                _rollback_state = ::scope_exit::ROLLBACK_COMMITTED; \
        }

#define ROLLBACK_FORCE() \
        _rollback_state = ::scope_exit::ROLLBACK_FORCED

#define ON_ROLLBACK(vars, ...) \
        ON_SCOPE_EXIT((_rollback_state) vars, \
                if (_rollback_state != ::scope_exit::ROLLBACK_COMMITTED) \
                    __VA_ARGS__)

#endif /* SCOPE_EXIT_HPP */
