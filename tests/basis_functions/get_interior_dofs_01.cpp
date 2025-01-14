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
 *  Test for space get_interior_dofs
 *
 *  author: pauletti
 *  date: 2015-03-27
 *
 */

#include "../tests.h"

#include <igatools/basis_functions/spline_space.h>
#include <igatools/basis_functions/dof_distribution.h>

template<int dim, int range = 1, int rank = 1>
void get_interior_dof(const int deg = 1, const int n_knots = 3)
{
  OUTSTART
  auto grid = Grid<dim>::const_create(n_knots);

  auto space = SplineSpace<dim,range,rank>::const_create(deg,grid);
  auto int_dofs = space->get_dof_distribution()->get_interior_dofs();

  // TODO (pauletti, Mar 27, 2015): we should create iga::set with print_info
  for (auto &x : int_dofs)
    out << x << " ";
  out << endl;

  OUTEND
}



int main()
{
  get_interior_dof<1>();
  get_interior_dof<2>();
  get_interior_dof<3>();

  return 0;
}
