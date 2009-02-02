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
#include <stdio.h>

#include "scope_exit.hpp"

#include <boost/typeof/typeof.hpp>
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

class testclass
{
    int i_;

public:
    void f() const;
    void g() const;
    void h();
};

BOOST_TYPEOF_REGISTER_TYPE(testclass)

void testclass::f() const
{
    printf("f!\n");
}

void testclass::g() const
{
    SCOPE_EXIT_SELF_REFERENCE(self);

    ON_SCOPE_EXIT((self),
        self.f();
    );
}

void testclass::h()
{
    SCOPE_EXIT_SELF_POINTER(self);

    ON_SCOPE_EXIT((self),
        self->i_ = 5;
        self->f();
    );
}

int
main()
{
    testclass test;

    test.g();
    test.h();

    return 0;
}
