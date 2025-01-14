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

#ifndef __PHYSICAL_BASIS_H_
#define __PHYSICAL_BASIS_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/basis.h>
#include <igatools/basis_functions/reference_basis.h>
#include <igatools/basis_functions/dof_distribution.h>
#include <igatools/geometry/domain.h>
#include <igatools/geometry/grid_iterator.h>
#include <igatools/utils/static_multi_array.h>

IGA_NAMESPACE_OPEN

template <int,int> class PushForward;

template <int,int,int,int> class PhysicalBasisElement;

template <int,int,int,int> class PhysicalBasisHandler;

/**
 *
 *
 * @ingroup containers
 * @ingroup serializable
 *
 */
template <int dim_, int range_= 1, int rank_ = 1, int codim_ = 0>
class PhysicalBasis :
  public Basis<dim_,codim_,range_,rank_>
{
private:
  using base_t = Basis<dim_,codim_,range_,rank_>;
  using self_t = PhysicalBasis<dim_, range_, rank_, codim_>;

public:
  ///@{
  /**
   * See documentation in \ref Basis
   *
   * @see Basis
   */
  using PushFwd = PushForward<dim_, codim_>;

  using PhysDomain = Domain<dim_, codim_>;

  using RefBasis = ReferenceBasis<dim_,range_,rank_>;

  using GridType = Grid<dim_>;
  ///@}
  using Handler = PhysicalBasisHandler<dim_,range_,rank_,codim_>;

  static const int dim = dim_;

  static const int codim = codim_;

  static const int space_dim = dim+codim;

//  static const int range = PushFwd::template PhysRange<range_>::value;
  static const int range = range_;

  static const int rank = rank_;

  static const bool is_physical_basis = true;

  static constexpr int n_components = constexpr_pow(range, rank);

  static const SafeSTLArray<int, n_components> components;


  using IndexType = typename GridType::IndexType;
  using PropertyList = PropertiesIdContainer<IndexType>;
  using List = typename PropertyList::List;
  using ListIt = typename PropertyList::List::iterator;

public:
  template <int order>
  using Derivative = typename base_t::template Derivative<order>;
  using Point = typename base_t::Point;
  using Value = typename base_t::Value;
  using Gradient = typename base_t::Gradient;
  using Div   = typename base_t::Div;

  using RefPoint = typename RefBasis::Point;


public:
  template< class T>
  using ComponentContainer = typename RefBasis::template ComponentContainer<T>;


public:


  using ElementAccessor = PhysicalBasisElement<dim_,range_,rank_,codim_>;
  using ElementIterator = GridIterator<ElementAccessor>;

  /**
   * Default constructor. It does nothing but it is needed for the serialization
   * mechanism.
   */
  PhysicalBasis() = default;

  PhysicalBasis(const self_t &phys_basis) = delete;

  virtual ~PhysicalBasis() = default;

  static std::shared_ptr<self_t>
  create(const std::shared_ptr<RefBasis> &ref_basis,
         const std::shared_ptr<PhysDomain> &phys_domain,
         const Transformation &transformation_type = Transformation::h_grad);

  static std::shared_ptr<const self_t>
  const_create(const std::shared_ptr<const RefBasis> &ref_basis,
               const std::shared_ptr<const PhysDomain> &phys_domain,
               const Transformation &transformation_type = Transformation::h_grad);

  std::shared_ptr<const RefBasis> get_reference_basis() const;

  std::shared_ptr<RefBasis> get_reference_basis();

  Transformation get_transformation_type() const;

  std::shared_ptr<Domain<dim_,codim_> > get_domain();

  std::shared_ptr<const Domain<dim_,codim_> > get_domain() const;


  // TODO: This function should not be in this class
  virtual std::shared_ptr<const Grid<dim_>> get_grid() const override final;
  // TODO: This function should not be in this class
  std::shared_ptr<const SplineSpace<dim_,range_,rank_> >
  get_spline_space() const override final;

  std::shared_ptr<SplineSpace<dim_,range_,rank_> >
  get_spline_space() override final;


  template <int k>
  using SubBasis = PhysicalBasis<k, range, rank, codim + dim-k>;

  template <int sdim>
  using SubGridMap = typename RefBasis::GridType::template SubGridMap<sdim>;

//    using InterGridMap = std::map<Index,Index>;

  template <int k>
  using InterBasisMap = typename RefBasis::template InterBasisMap<k>;

  template<int k>
  std::shared_ptr<const SubBasis<k> >
  get_sub_basis(const int s_id, InterBasisMap<k> &dof_map,
                const std::shared_ptr<const Grid<k>> &sub_grid,
                SubGridMap<k> &elem_map,
                EnableIf<(dim_ != 0) &&(k>=0)> * = nullptr) const;


  void print_info(LogStream &out) const override final;

  std::unique_ptr<BasisElement<dim_,codim_,range_,rank_> >
  create_element_end(const PropId &property) const override final;

  std::unique_ptr<BasisElement<dim_,codim_,range_,rank_> >
  create_element_begin(const PropId &property) const override final;

  std::unique_ptr<BasisHandler<dim_,codim_,range_,rank_> >
  create_cache_handler() const override final;





private:


  PhysicalBasis(const SharedPtrConstnessHandler<RefBasis> &ref_basis,
                const SharedPtrConstnessHandler<PhysDomain> &phys_domain,
                const Transformation &transformation_type);



  SharedPtrConstnessHandler<RefBasis> ref_basis_;

  SharedPtrConstnessHandler<PhysDomain> phys_domain_;

  const Transformation transformation_type_ = Transformation::h_grad;


  std::shared_ptr<const self_t> phys_basis_previous_refinement_ = nullptr;


  friend ElementAccessor;


  /**
   * Returns the current object wrapped by a std::shared_ptr.
   *
   * @note Internally uses the shared_from_this() function.
   */
  std::shared_ptr<const self_t > get_this_basis() const;


#ifdef IGATOOLS_WITH_MESH_REFINEMENT

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
    const Grid<dim> &old_grid) override final;

public:

  virtual void refine_h(const Size n_subdivisions = 2) override final;

  void create_connection_for_insert_knots(const std::shared_ptr<self_t> &basis);


  std::shared_ptr<const base_t> get_basis_previous_refinement() const;

#endif // IGATOOLS_WITH_MESH_REFINEMENT

private:

#ifdef IGATOOLS_WITH_SERIALIZATION
  /**
   * @name Functions needed for the serialization
   * @see <a href="http://uscilab.github.io/cereal/">Cereal serialization library</a>
   */
  ///@{
  friend class cereal::access;

  template<class Archive>
  void
  serialize(Archive &ar);
  ///@}
#endif // IGATOOLS_WITH_SERIALIZATION


};

IGA_NAMESPACE_CLOSE

#endif // __PHYSICAL_BASIS_H_
