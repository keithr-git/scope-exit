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

#ifdef ASM_LIST
void throw_exception();
#else
static void throw_exception()
{
    printf("throwing\n");
    throw -1;
}
#endif // ASM_LST

int
main()
{
    int i = 1;

    try
    {
        ROLLBACK_BLOCK(
            i = 3;

            ON_ROLLBACK((i),
                printf("abort!\n");
                i = -1;
            );

            throw_exception();
        )
    }
    catch (...)
    {
        printf("exception!\n");
    }

    printf("i = %d\n", i);
    return 0;
}
