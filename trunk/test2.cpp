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
#include <iostream>
#include <vector>

#include "scope_exit.hpp"

#include <boost/typeof/std/vector.hpp>

template <typename T>
static inline void dump_vector(const std::vector<T>& tv)
{
    typedef typename std::vector<T>::const_iterator const_iterator;
    const_iterator begin = tv.begin();
    const_iterator end = tv.end();

    for (const_iterator i = begin; i != end; ++i)
    {
        if (i != begin)
        {
            std::cout << ", ";
        }

        std::cout << *i;
    }

    std::cout << std::endl;
}

int
main()
{
    std::vector<int> iv;
    int v1 = 42;

    iv.push_back(v1);

    dump_vector(iv);

    ROLLBACK_BLOCK(
        int v2 = 10;

        iv.push_back(v2);
        ON_ROLLBACK((iv), iv.pop_back());
        dump_vector(iv);

        ROLLBACK_FORCE();
    );

    dump_vector(iv);

    return 0;
}
