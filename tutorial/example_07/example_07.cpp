//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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

// [analytical map]
#include <igatools/base/function_lib.h>
#include <igatools/base/identity_function.h>
// [analytical map]

// [ig map bspline]
#include <igatools/base/ig_function.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/bspline_element.h>
// [ig map bspline]

// [ig map read nurb]
#include <igatools/io/reader.h>
#include <igatools/basis_functions/nurbs_element.h>
// [ig map read nurb]

// [old includes]
#include <igatools/io/writer.h>

using namespace iga;
using numbers::PI;
using namespace EpetraTools;

using std::string;
using std::to_string;
// [old includes]

// [an geometry]
template<int dim>
void analytical_geometry()
{
    using Function = functions::BallFunction<dim>;

    BBox<dim> box;
    box[0] = {{0.5,1}};
    for (int i=1; i<dim; ++i)
        box[i] = {{PI/4,PI/2}};

    const int n_knots = 3;
    auto grid = CartesianGrid<dim>::create(box, n_knots);
    auto map  = Function::create(grid, IdentityFunction<dim>::create(grid));

    const int n_plot_points = 2;
    Writer<dim> writer(map, n_plot_points);
    string filename = "ball_geometry-" + to_string(dim) + "d" ;
    writer.save(filename);
}
// [an geometry]


// [bspline geometry]
void nurb_geometry()
{
    const int dim = 2;
    using Function = IgFunction<dim,0,dim,1>;

    const int deg = 2;
    const int n_knots = 3;
    auto grid = CartesianGrid<dim>::create(n_knots);
    using Space = BSplineSpace<dim,dim>;
    auto space = Space::create(deg, grid);

    auto control_pts = create_vector(space,DofProperties::active);

    DynamicMultiArray<Points<dim>, dim> c_points(deg-1+n_knots);
    const Real eps = 0.2;
    c_points({0,0}) = {0.0, 0.0};
    c_points({1,0}) = {0.3, 0.0};
    c_points({2,0}) = {0.6, 0.0};
    c_points({3,0}) = {1.0, 0.0};
    c_points({0,1}) = {-eps, 0.3};
    c_points({1,1}) = {0.3-eps, 0.3};
    c_points({2,1}) = {0.6+eps, 0.3};
    c_points({3,1}) = {1.0+eps, 0.3};
    c_points({0,2}) = {0.0+eps, 0.6};
    c_points({1,2}) = {0.3+eps, 0.6};
    c_points({2,2}) = {0.6-eps, 0.6};
    c_points({3,2}) = {1.0-eps, 0.6};
    c_points({0,3}) = {0.0, 1.0};
    c_points({1,3}) = {0.3, 1.0};
    c_points({2,3}) = {0.6, 1.0};
    c_points({3,3}) = {1.0, 1.0};
    auto flat_points = c_points.get_data();
    const int n_points = c_points.flat_size();
    for (int i = 0; i < n_points; ++i)
    {
        (*control_pts)[i] = flat_points[i][0];
        (*control_pts)[i+n_points] = flat_points[i][1];
    }

    auto F = Function::create(space, control_pts);


    const int n_plot_points = 10;
    Writer<dim> writer(F, n_plot_points);
    string filename = "nurb_geometry-" + to_string(dim) + "d" ;
    writer.save(filename);
}
// [bspline geometry]



template<int dim>
void nurb_geometry_from_file()
{
    string input_file = "nurb_geometry-" + to_string(dim) + "d_v2.xml";

    auto map = get_mapping_from_file<dim>(input_file);

    const int n_plot_points = 10;
    Writer<dim> writer(map, n_plot_points);
    string filename = "nurb_geometry_from_file-" + to_string(dim) + "d" ;
    writer.save(filename);
}



int main()
{

    analytical_geometry<1>();
    analytical_geometry<2>();
    analytical_geometry<3>();

    nurb_geometry();

    nurb_geometry_from_file<2>();
    nurb_geometry_from_file<3>();

    return  0;
}
