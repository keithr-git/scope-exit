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

static void foo(const int& c, volatile double& d)
{
    printf("c = %d, d = %f\n", c, d);
}

static volatile double d = 2;

int
main()
{
    int i = 1;
    const int c = 5;

    printf("i = %d, d = %f\n", i, d);
    {
        ON_SCOPE_EXIT((c) (i) (d),
            i = 5;
            ++d;
            foo(c, d);
            d = 42.42;
        );

        ON_SCOPE_EXIT(, printf("%s", "done!\n"));
    }

    printf("i = %d, d = %f\n", i, d);
    return 0;
}
