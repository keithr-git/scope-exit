The scope\_exit library provides a mechanism for invoking code when a C++ scope exits, either via an exception or after the last line of the block is executed.

# Basic Usage #

ON\_SCOPE\_EXIT(variable\_list, code)

Execute the specified code when the scope exits, making the
variables in variable\_list available to that code.  For example:

```
FILE* fp = fopen(file, "r");
ON_SCOPE_EXIT((fp), fclose(fp));
```

The variable list must be in the form of a Boost.Preprocessor
sequence:

```
(var1) (var2) (var3)
```

That is, each variable name must be enclosed in parentheses, and
there must be no commas or other delmiters between the names.  Any
amount of whitespace (including none) is OK.

# Class Methods #

Using ON\_SCOPE\_EXIT inside a class method is the same as for any
other function, except that the this pointer is not available.
It must be copied to another pointer:

```
void class::method()
{
    SCOPE_EXIT_SELF_POINTER(self);

    ON_SCOPE_EXIT((self), self->other_method());
}
```

There is also a SCOPE\_EXIT\_SELF\_REFERENCE macro, which declares
the variable as a reference to the current object instead of a
pointer.

# Template Variables #

Variables of a template type must be declared using the
typename keyword, and so they must be identified to
ON\_SCOPE\_EXIT.  This is done with the SCOPE\_EXIT\_TEMPLATE\_VAR
macro:

```
template<typename T>
void f(T& t)
{
    int i, x;

    ON_SCOPE_EXIT((i) SCOPE_EXIT_TEMPLATE_VAR(t) (x),
        /* Do something with i, t, and x */
    );
}
```

# Rollback #

Built on top of ON\_SCOPE\_EXIT is a simple rollback facility.
A sequence of statements and their associated rollback operations
can be contained in a block, and the rollback operations will
be carried out if the block exits because of an exception, but
not if the block exits normally (unless rollback is explicitly
forced).

```
void Database::add(const Record& record)
{
    ROLLBACK_BLOCK(
        m_table1.insert(record);
        ON_ROLLBACK((m_table1) (record), m_table1.erase(record));
        m_table2.insert(record);
    );
}
```

In this example, if inserting the record into m\_table2 throws
an exception, the record will be erased from m\_table1.

A rollback block is just a special case of a scope block,
so ON\_SCOPE\_EXIT can be used inside a rollback block for
code that should always run when the scope exits.

Rollback can be explicitly forced by using the ROLLBACK\_FORCE macro.
After this macro is called, all rollback operations will be
performed when the block exits.  It does not cause the block
to exit immediately; the remaining statements in the block will
be executed normally.  This is because, while the ROLLBACK\_BLOCK
macro is implemented as a for loop, using `break` to terminate the
loop early does not work if the ROLLBACK\_FORCE macro is called
from inside another nested loop.  For example:

```
void f()
{
    ROLLBACK_BLOCK(
        ROLLBACK_FORCE();  // Would exit the block if it contained a break.
        for (int i = 0; i < 10; ++i) {
            ROLLBACK_FORCE();  // Would terminate the loop instead.
        }
    );
}
```