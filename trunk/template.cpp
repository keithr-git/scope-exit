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

static void func(int& i)
{
    ++i;

    ON_SCOPE_EXIT((i),
        ++i;
    );
}

template <typename T1, typename T2>
static void func(T1& t1, const T2& t2)
{
    int i = 0;

    ON_SCOPE_EXIT((i),
        i = 5;
    );

    ON_SCOPE_EXIT(SCOPE_EXIT_TEMPLATE_VAR(t1) SCOPE_EXIT_TEMPLATE_VAR(t2),
        ++t1;
    );

    ROLLBACK_BLOCK(
        ON_ROLLBACK(SCOPE_EXIT_TEMPLATE_VAR(t1), ++t1);
        ROLLBACK_FORCE();
    );
}

int
main()
{
    int i = 2;
    double d = 5;
    
    printf("i = %d, d = %f\n", i, d);
    func(i);
    printf("i = %d, d = %f\n", i, d);
    func(d, i);
    printf("i = %d, d = %f\n", i, d);
}
