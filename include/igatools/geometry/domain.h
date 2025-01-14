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

#ifndef __DOMAIN_H_
#define __DOMAIN_H_

#include <igatools/base/config.h>
#include <igatools/functions/ig_grid_function.h>
#include <igatools/functions/formula_grid_function.h>
//#include <igatools/functions/grid_function_handler.h>

IGA_NAMESPACE_OPEN

template <int, int> class DomainElement;
template <int, int> class DomainHandler;

/**
 * @brief The mapping is a deformation \f$ F : \hat\Omega \to \Omega\f$
 * which maps the reference domain \f$\hat\Omega \in \mathbb{R}^{dim}\f$ to the
 * physical domain \f$\Omega \in \mathbb{R}^{dim+codim}\f$.
 *
 * Domain is the physical domain.
 *
 *
 *
 * @ingroup containers
 * @ingroup serializable
 *
 * @author pauletti 2014, 2015
 * @author M. Martinelli, 2015
 */
template<int dim_, int codim_ = 0>
class Domain :
  public std::enable_shared_from_this<Domain<dim_,codim_> >
{
private:
  using self_t = Domain<dim_, codim_>;

public:
  static const int space_dim = dim_ + codim_;
  static const int dim = dim_;

  using GridFuncType = GridFunction<dim, space_dim>;

  using ElementAccessor = DomainElement<dim_, codim_>;
  using ElementIterator = GridIterator<ElementAccessor>;

  using Handler = DomainHandler<dim_, codim_>;

  using List = typename GridFuncType::List;
  using ListIt = typename GridFuncType::ListIt;


public:
  //using GridPoint = typename GridFuncType::Point;
  using Point =  Points<space_dim>;
  template <int order>
  using Derivative = Derivatives<dim, space_dim, 1, order>;


//  /** Type for the return of the function.*/
//  using Value = Values<dim, space_dim, 1>;
//
//  /**
//   * Type for the derivative of the function.
//   */

//
//  /**
//   * Type for the gradient of the function.
//   */
  using Gradient = Derivative<1>;
//
//  /**
//   * Type for the hessian of the function.
//   */
//  using Hessian = Derivative<2>;
  ///@}

private:
  /**
   * Default constructor. It does nothing but it is needed for the serialization mechanism.
   */
  Domain() = default;

protected:
  Domain(const SharedPtrConstnessHandler<GridFuncType> &func);

public:
  virtual ~Domain() = default;

  static std::shared_ptr<self_t>
  create(const std::shared_ptr<GridFuncType> &func);


  static std::shared_ptr<const self_t>
  const_create(const std::shared_ptr<const GridFuncType> &func);

  std::shared_ptr<const GridFuncType> get_grid_function() const;

  std::shared_ptr<GridFuncType> get_grid_function();

public:
  std::unique_ptr<Handler>
  create_cache_handler() const;

#if 0
  std::unique_ptr<ElementAccessor>
  create_element(const ListIt &index, const PropId &prop) const;
#endif

public:
  std::unique_ptr<ElementAccessor>
  create_element_begin(const PropId &prop) const;

  std::unique_ptr<ElementAccessor>
  create_element_end(const PropId &prop) const;

  ///@name Iterating of grid elements
  ///@{
  /**
   * This function returns a element (const) iterator to the first element of the patch.
   */
  ElementIterator begin(const PropId &prop = ElementProperties::active) const;

  /**
   * This function returns a element (const) iterator to one-pass the end of patch.
   */
  ElementIterator end(const PropId &prop = ElementProperties::active) const;

  /**
   * This function returns a element (const) iterator to the first element of the patch.
   */
  ElementIterator cbegin(const PropId &prop = ElementProperties::active) const;

  /**
   * This function returns a element (const) iterator to one-pass the end of patch.
   */
  ElementIterator cend(const PropId &prop = ElementProperties::active) const;
  ///@}


  void print_info(LogStream &out) const;


  /**
   * Get the name associated to the object instance.
   */
  const std::string &get_name() const;

  /**
   * Set the name associated to the object instance.
   */
  void set_name(const std::string &name);

  /**
   * Returns the unique identifier associated to each object instance.
   */
  int get_object_id() const;


private:
  SharedPtrConstnessHandler<GridFunction<dim, space_dim>> grid_func_;

  /// Name.
  std::string name_;

  /**
   * Unique identifier associated to each object instance.
   */
  int object_id_;

  friend class DomainElement<dim_, codim_>;

#ifdef IGATOOLS_WITH_SERIALIZATION
  /**
   * @name Functions needed for serialization
   */
  ///@{
  friend class cereal::access;

  template<class Archive>
  void
  serialize(Archive &ar);
  ///@}
#endif // IGATOOLS_WITH_SERIALIZATION


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
    const SafeSTLArray<SafeSTLVector<Real>,dim_> &knots_to_insert,
    const Grid<dim_> &old_grid);


  void create_connection_for_insert_knots(const std::shared_ptr<self_t> &domain);


  std::shared_ptr<const self_t> domain_previous_refinement_;

public:

  std::shared_ptr<const self_t> get_domain_previous_refinement() const
  {
    return domain_previous_refinement_;
  }

  /**
   *  Connect a slot (i.e. a function pointer) to the refinement signals
   *  which will be
   *  emitted whenever a insert_knots() function is called by the underlying
   *  a Grid member.
   */
  boost::signals2::connection
  connect_insert_knots(const typename Grid<dim_>::SignalInsertKnotsSlot &subscriber);

#endif // IGATOOLS_WITH_MESH_REFINEMENT



public:

  template <int sdim>
  using SubGridElemMap = typename Grid<dim>::template SubGridMap<sdim>;

  template <int sdim>
  std::shared_ptr<const Domain<sdim,space_dim-sdim> >
  get_sub_domain(const int s_id,
                 const SubGridElemMap<sdim> &sub_grid_elem_map,
                 const std::shared_ptr<const Grid<sdim>> &sub_grid) const
  {
    std::shared_ptr<const GridFunction<sdim,space_dim>> sub_func;

    const auto grid_func = this->get_grid_function();
    using IgGridFunc = IgGridFunction<dim,space_dim>;
    auto ig_grid_func = std::dynamic_pointer_cast<const IgGridFunc>(grid_func);
    using FormulaGridFunc = FormulaGridFunction<dim,space_dim>;
    auto formula_grid_func = std::dynamic_pointer_cast<const FormulaGridFunc>(grid_func);
    if (ig_grid_func)
    {
      sub_func = ig_grid_func->get_sub_function(s_id,sub_grid);
    }
    else if (formula_grid_func)
    {
      sub_func = formula_grid_func->get_sub_function(s_id,sub_grid_elem_map,sub_grid);
    }
    else
    {
      AssertThrow(false,ExcNotImplemented());
    }
    auto sub_domain = Domain<sdim,space_dim-sdim>::const_create(sub_func);

    /*
            LogStream out;
            out.begin_item("Sub-Function:");
            sub_func->print_info(out);
            out.end_item();
            //*/
    /*
    auto sub_func_elem = sub_func->begin();
    auto sub_func_end = sub_func->end();

    out.begin_item("Sub-function elements:");
    for (; sub_func_elem != sub_func_end ; ++sub_func_elem)
      sub_func_elem->print_info(out);
    //    for (const auto & sub_func_elem : *sub_func)
    //      sub_func_elem.print_info(out);
    out.end_item();
    //*/


    return sub_domain;
  }
};

IGA_NAMESPACE_CLOSE

#endif

