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

/*
 *  Test for bernstein extraction class
 *  author: pauletti
 *  date:
 *
 */

#include "bernstein_extraction_tests_common.h"
#include <igatools/basis_functions/bernstein_extraction.h>

template <int dim>
void
test()
{
  using SplineSpace = SplineSpace<dim>;


  typename SplineSpace::DegreeTable deg {{2}};

  auto grid = Grid<dim>::const_create(4);

  auto int_mult = SplineSpace::get_multiplicity_from_regularity(InteriorReg::maximum,
                  deg, grid->get_num_intervals());
  auto sp_spec = SplineSpace::const_create(deg, grid, int_mult);

  //CartesianProductArray<Real,2> bn_x {{-0.5, 0, 0}, {1.1, 1.2, 1.3}};
  typename SplineSpace::BoundaryKnotsTable bdry_knots;

  SafeSTLArray<BasisEndBehaviour,dim> eb(BasisEndBehaviour::interpolatory);
  typename SplineSpace::EndBehaviourTable ebt(eb);
  auto rep_knots = compute_knots_with_repetitions(*sp_spec,bdry_knots,ebt);

  auto acum_mult = sp_spec->accumulated_interior_multiplicities();

  rep_knots.print_info(out);

  BernsteinExtraction<dim> operators(*grid, rep_knots, acum_mult, deg);
  operators.print_info(out);
}

int main()
{
  out.depth_console(10);
  test<1>();


//    {
//        const int dim=1;
//        using SplineSpace = SplineSpace<dim>;
//        using MultiplicityTable = typename SplineSpace::MultiplicityTable;
//
//        typename SplineSpace::DegreeTable deg {{3}};
//
//        CartesianProductArray<Real,dim> knots({{0,1,2,3,4}});
//        auto grid = Grid<dim>::const_create(knots);
//
//        SplineSpace sp_spec(deg, grid, InteriorReg::maximum);
//
//
//        auto rep_knots = sp_spec.compute_knots_with_repetition(sp_spec.get_end_behaviour());
//        auto acum_mult = sp_spec.accumulated_interior_multiplicities();
//
//
//        BernsteinExtraction<dim> operators(grid, rep_knots, acum_mult, deg);
//        operators.print_info(out);
//    }

  return 0;
}
