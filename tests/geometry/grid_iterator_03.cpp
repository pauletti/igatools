//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

/**
 *  @file
 *  @brief  Plain iteration
 *  @author pauletti
 *  @date  Aug 19, 2015
 */

#include "../tests.h"

#include <igatools/geometry/grid.h>
#include <igatools/geometry/grid_element.h>

template <int dim>
void iterate(const int n_knots = 5)
{
  OUTSTART

  auto grid = Grid<dim>::const_create(n_knots);

  for (auto &elem : *grid)
  {
    elem.print_info(out);
  }

  OUTEND
}



int main()
{
  iterate<0>();
  iterate<1>();
  iterate<2>();
  iterate<3>();

  return  0;
}
