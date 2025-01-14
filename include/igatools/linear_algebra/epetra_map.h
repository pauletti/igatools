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

#ifndef __EPETRA_MAP_H_
#define __EPETRA_MAP_H_

#include <igatools/base/config.h>

#include <igatools/linear_algebra/dense_vector.h>
#include <igatools/linear_algebra/dense_matrix.h>
#include <igatools/utils/safe_stl_vector.h>

#ifdef IGATOOLS_USES_TRILINOS
#include <Epetra_SerialComm.h>
#include <Epetra_Map.h>
#endif

#include <set>

IGA_NAMESPACE_OPEN

#ifdef IGATOOLS_USES_TRILINOS

/**
 * @brief This namespace contains some basic tools/interfaces for the creations of Trilinos/Epetra objects.
 */
namespace EpetraTools
{
using Comm = Epetra_Comm;
using CommPtr = std::shared_ptr<const Comm>;

using Map = Epetra_Map;
using MapPtr = std::shared_ptr<Map>;


/**
 * Create an Epetra_Map object (wrapped by a shared pointer) from a set of @p dofs.
 */
MapPtr
create_map(const std::set<Index> &dofs,const Comm &comm);

/**
 * Create an Epetra_Map object (wrapped by a shared pointer) from a @p basis and the @p dofs_property
 * used to extract the dofs from the basis.
 */
template<class Basis>
MapPtr create_map(const Basis &basis,
                  const std::string &property,
                  const Comm &comm)
{
  const auto &dof_dist = *basis.get_spline_space()->get_dof_distribution();

  return create_map(dof_dist.get_global_dofs(property),comm);
}


}

#endif // IGATOOLS_USES_TRILINOS

IGA_NAMESPACE_CLOSE

#endif
