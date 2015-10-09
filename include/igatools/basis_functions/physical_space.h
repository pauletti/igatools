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

#ifndef __PHYSICAL_SPACE_H_
#define __PHYSICAL_SPACE_H_

#include <igatools/base/config.h>
#include <igatools/geometry/push_forward.h>
#include <igatools/basis_functions/space.h>
#include <igatools/basis_functions/reference_space.h>
#include <igatools/basis_functions/dof_distribution.h>
#include <igatools/geometry/domain.h>
#include <igatools/geometry/grid_iterator.h>
#include <igatools/utils/static_multi_array.h>

IGA_NAMESPACE_OPEN

class SpaceManager;


template <int,int,int,int,Transformation> class PhysicalSpaceElement;

template <int,int,int,int,Transformation> class PhysSpaceElementHandler;

/**
 *
 * @sa FunctionSpace
 *
 * @ingroup containers
 * @ingroup serializable
 *
 */
template <int dim_, int range_= 1, int rank_ = 1, int codim_ = 0,
          Transformation type_= Transformation::h_grad>
class PhysicalSpace :
  public Space<dim_,codim_,range_,rank_>
{
private:
  using base_t = Space<dim_,codim_,range_,rank_>;
  using self_t = PhysicalSpace<dim_, range_, rank_, codim_, type_>;

public:
  ///@{
  /**
   * See documentation in \ref Space
   *
   * @see Space
   */
  using PushFwd = PushForward<dim_, codim_>;

  using PhysDomain = Domain<dim_, codim_>;

  using RefSpace = ReferenceSpace<dim_,range_,rank_>;

  using GridType = Grid<dim_>;
  ///@}
  using ElementHandler = PhysSpaceElementHandler<dim_,range_,rank_,codim_,type_>;

  static const int dim = dim_;

  static const int codim = codim_;

  static const int space_dim = dim+codim;

//  static const int range = PushFwd::template PhysRange<range_>::value;
  static const int range = range_;

  static const int rank = rank_;

  static const bool is_physical_space = true;

  static constexpr int n_components = constexpr_pow(range, rank);

  static const SafeSTLArray<int, n_components> components;


  using IndexType = TensorIndex<dim_>;
  using PropertyList = PropertiesIdContainer<IndexType>;
  using List = typename PropertyList::List;
  using ListIt = typename PropertyList::List::iterator;

public:
  using Func = Function<dim, codim, range, rank>;
  template <int order>
  using Derivative = typename Func::template Derivative<order>;
  using Point = typename Func::Point;
  using Value = typename Func::Value;
  using Gradient = typename Func::Gradient;
  using Div   = typename Func::Div;

  using RefPoint = typename RefSpace::Point;


public:
  template< class T>
  using ComponentContainer = typename RefSpace::template ComponentContainer<T>;

  using TensorSizeTable = typename RefSpace::TensorSizeTable;

  using DegreeTable = typename RefSpace::DegreeTable;

public:


  using ElementAccessor = PhysicalSpaceElement<dim_,range_,rank_,codim_,type_>;
  using ElementIterator = GridIterator<ElementAccessor>;


  PhysicalSpace(const self_t &phys_space) = delete;

  virtual ~PhysicalSpace() = default;

  static std::shared_ptr<self_t>
  create(const std::shared_ptr<RefSpace> &ref_space,
         const std::shared_ptr<PhysDomain> &phys_domain,
         const Transformation &transformation_type = Transformation::h_grad);

  static std::shared_ptr<const self_t>
  const_create(const std::shared_ptr<const RefSpace> &ref_space,
               const std::shared_ptr<const PhysDomain> &phys_domain,
               const Transformation &transformation_type = Transformation::h_grad);

  /**
   * Create an element (defined on this grid) with a given index.
   */
  std::unique_ptr<SpaceElement<dim_,codim_,range_,rank_> >
  create_element(const ListIt &index, const PropId &property) const override final;


  /** Returns the container with the global dof distribution (const version). */
  std::shared_ptr<const DofDistribution<dim, range, rank> >
  get_ptr_const_dof_distribution() const override final;

  /** Returns the container with the global dof distribution (non const version). */
  std::shared_ptr<DofDistribution<dim, range, rank> >
  get_ptr_dof_distribution() override final;



  template <int k>
  using SubSpace = PhysicalSpace<k, range, rank, codim + dim-k, type_>;

  template <int sdim>
  using SubGridMap = typename RefSpace::GridType::template SubGridMap<sdim>;

//    using InterGridMap = std::map<Index,Index>;

  template <int k>
  using InterSpaceMap = typename RefSpace::template InterSpaceMap<k>;



  template<int k>
  std::shared_ptr<SubSpace<k> >
  get_sub_space(const int s_id, InterSpaceMap<k> &dof_map,
                std::shared_ptr<Grid<k>> sub_grid,
                SubGridMap<k> &elem_map) const;



  void get_element_dofs(
    const IndexType element_id,
    SafeSTLVector<Index> &dofs_global,
    SafeSTLVector<Index> &dofs_local_to_patch,
    SafeSTLVector<Index> &dofs_local_to_elem,
    const std::string &dofs_property = DofProperties::active) const override final;


  std::shared_ptr<const RefSpace> get_reference_space() const;

  std::shared_ptr<RefSpace> get_reference_space();




  void print_info(LogStream &out) const override final;

  std::unique_ptr<SpaceElementHandler<dim_,codim_,range_,rank_>>
                                                              create_cache_handler() const override final;



  /**
   * Return the maximum value of the polynomial degree, for each component, for each direction;
   */
  virtual int get_max_degree() const override final;

  std::shared_ptr<const Domain<dim_,codim_>> get_physical_domain() const
  {
    return phys_domain_.get_ptr_const_data();
  }


  virtual std::shared_ptr<Grid<dim_>> get_ptr_grid() const override final
  {
    return ref_space_.get_ptr_data()->get_ptr_grid();
  }

  virtual std::shared_ptr<const Grid<dim_>> get_ptr_const_grid() const override final
  {
    return ref_space_.get_ptr_const_data()->get_ptr_const_grid();
  }

  Transformation get_transformation_type() const;

private:

  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  PhysicalSpace() = default;

  PhysicalSpace(const SharedPtrConstnessHandler<RefSpace> &ref_space,
                const SharedPtrConstnessHandler<PhysDomain> &phys_domain,
                const Transformation &transformation_type);



  SharedPtrConstnessHandler<RefSpace> ref_space_;

  SharedPtrConstnessHandler<PhysDomain> phys_domain_;


  std::shared_ptr<const self_t> phys_space_previous_refinement_ = nullptr;


  friend ElementAccessor;


  /**
   * Returns the current object wrapped by a std::shared_ptr.
   *
   * @note Internally uses the shared_from_this() function.
   */
  std::shared_ptr<const self_t > get_this_space() const;


#ifdef MESH_REFINEMENT

  /**
   * Rebuild the internal state of the object after an insert_knots() function is invoked.
   *
   * @pre Before invoking this function, must be invoked the function grid_->insert_knots().
   * @note This function is connected to the Grid's signal for the refinement, and
   * it is necessary in order to avoid infinite loops in the insert_knots() function calls.
   *
   * @ingroup h_refinement
   */
  void rebuild_after_insert_knots(
    const SafeSTLArray<SafeSTLVector<Real>,dim> &knots_to_insert,
    const Grid<dim> &old_grid);

  void create_connection_for_insert_knots(std::shared_ptr<self_t> space);

public:

  std::shared_ptr<const base_t> get_space_previous_refinement() const
  {
    return phys_space_previous_refinement_;
  }

#endif // MESH_REFINEMENT

private:

#if 0
#ifdef SERIALIZATION
  /**
   * @name Functions needed for boost::serialization
   * @see <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   */
  ///@{
  friend class cereal::access;

  template<class Archive>
  void
  serialize(Archive &ar)
  {
    ar &boost::serialization::make_nvp("PhysicalSpace_base_t",
                                       boost::serialization::base_object<base_t>(*this));

    ar.template register_type<BSplineSpace<dim_,range_,rank_> >();
#ifdef NURBS
    ar.template register_type<NURBSSpace<dim_,range_,rank_> >();
#endif // NURBS

    ar &boost::serialization::make_nvp("ref_space_",ref_space_);

    auto tmp = const_pointer_cast<self_t>(phys_space_previous_refinement_);
    ar &boost::serialization::make_nvp("phys_space_previous_refinement_",tmp);
    phys_space_previous_refinement_ = tmp;
  }

  ///@}
#endif // SERIALIZATION
#endif


  const Transformation transformation_type_;
};

IGA_NAMESPACE_CLOSE

#endif
