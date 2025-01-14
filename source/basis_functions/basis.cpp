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

#include <igatools/basis_functions/basis.h>
#include <igatools/utils/unique_id_generator.h>
#include <igatools/basis_functions/basis_element.h>
#include <igatools/functions/ig_function.h>
#include <igatools/functions/identity_function.h>
#include <igatools/basis_functions/dof_distribution.h>
#include <igatools/basis_functions/nurbs.h>



using std::shared_ptr;
using std::unique_ptr;

IGA_NAMESPACE_OPEN







template <int dim_,int codim_,int range_,int rank_>
Basis<dim_,codim_,range_,rank_>::
Basis()
  :
  object_id_(UniqueIdGenerator::get_unique_id())
{}



template <int dim_,int codim_,int range_,int rank_>
Index
Basis<dim_,codim_,range_,rank_>::
get_object_id() const
{
  return object_id_;
}


template <int dim_,int codim_,int range_,int rank_>
const std::string &
Basis<dim_,codim_,range_,rank_>::
get_name() const
{
  return name_;
}

template <int dim_,int codim_,int range_,int rank_>
void
Basis<dim_,codim_,range_,rank_>::
set_name(const std::string &name)
{
  name_ = name;
}

template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
begin(const PropId &prop) const -> ElementIterator
{
  return this->cbegin(prop);
}



template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
end(const PropId &prop) const -> ElementIterator
{
  return this->cend(prop);
}

template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
cbegin(const PropId &prop) const -> ElementIterator
{
  return ElementIterator(this->create_element_begin(prop));
}



template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
cend(const PropId &prop) const -> ElementIterator
{
  return ElementIterator(this->create_element_end(prop));
}


template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
get_num_basis() const -> Size
{
  return this->get_dof_distribution()->get_num_dofs_table().total_dimension();
}

#if 0
template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
get_global_dofs(const std::string &dof_prop) const
-> const std::set<Index> &
{
  return this->get_dof_distribution()->get_global_dofs(dof_prop);
}
#endif


template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
get_dof_distribution() const -> std::shared_ptr<const DofDistribution<dim_,range_,rank_> >
{
  return this->get_spline_space()->get_dof_distribution();
}


template <int dim_,int codim_,int range_,int rank_>
auto
Basis<dim_,codim_,range_,rank_>::
get_dof_distribution()  -> std::shared_ptr<DofDistribution<dim_,range_,rank_> >
{
  return this->get_spline_space()->get_dof_distribution();
}




#ifdef IGATOOLS_WITH_SERIALIZATION

template <int dim_,int codim_,int range_,int rank_>
template<class Archive>
void
Basis<dim_,codim_,range_,rank_>::
serialize(Archive &ar)
{
  ar &make_nvp("object_id_",object_id_);

  ar &make_nvp("name_",name_);
}
#endif // IGATOOLS_WITH_SERIALIZATION

IGA_NAMESPACE_CLOSE


#include <igatools/basis_functions/basis.inst>
