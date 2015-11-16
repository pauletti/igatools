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

#ifndef __GRID_FUNCTION_ELEMENT_H_
#define __GRID_FUNCTION_ELEMENT_H_

#include <igatools/geometry/grid_function.h>
#include <igatools/geometry/grid_function_handler.h>
#include <igatools/geometry/grid_element.h>

IGA_NAMESPACE_OPEN

/**
 *
 * @ingroup elements
 */
template<int dim_, int space_dim_>
class GridFunctionElement
{
private:
  using self_t  = GridFunctionElement<dim_, space_dim_>;

public:
  using ContainerType = const GridFunction<dim_,space_dim_>;
  using GridElem = typename ContainerType::GridType::ElementAccessor;
  using ListIt = typename ContainerType::ListIt;

  using IndexType = typename Grid<dim_>::IndexType;

  using Value =  typename ContainerType::Value;
  template <int order>
  using Derivative = typename ContainerType::template Derivative<order>;


// using Gradient =  typename ContainerType_::Gradient;

  using Flags = grid_function_element::Flags;


  /** @name Constructors */
  ///@{
protected:
  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  GridFunctionElement() = default;

public:
  /**
   * Construct an accessor pointing to the element with
   * flat index @p elem_index of the Function @p func.
   */
  GridFunctionElement(const std::shared_ptr<ContainerType> &grid_function,
                      const ListIt &index,
                      const PropId &prop = ElementProperties::active);

  /**
   * Copy constructor. Not allowed to be used.
   */
  GridFunctionElement(const self_t &elem) = delete;

  /**
   * Move constructor.
   */
  GridFunctionElement(self_t &&elem) = default;

  /**
   * Destructor.
   */
  ~GridFunctionElement() = default;
  ///@}


  /**
   * @name Comparison operators
   * @note In order to be meaningful, the comparison must be performed on
   * elements defined on
   * the <b>same</b> GridFunction
   * (in the sense that the pointer to the GridFunction held by the elements must
   * point to the same GridFunction object).
   */
  ///@{
  /**
   * True if the elements have the same index.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator==(const self_t &elem) const;

  /**
   * True if the elements have different index.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator!=(const self_t &elem) const;

  /**
   * True if the flat-index of the element on the left is smaller than
   * the flat-index of the element on the right.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator<(const self_t &elem) const;

  /**
   * True if the flat-index of the element on the left is bigger than
   * the flat-index of the element on the right.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator>(const self_t &elem) const;
  ///@}



  virtual void operator++();


  virtual void move_to(const IndexType &elem_id);

  const GridElem &get_grid_element() const;

  GridElem &get_grid_element();


  const IndexType &get_index() const;

  void print_info(LogStream &out) const;

  void print_cache_info(LogStream &out) const;


public:

  template <class ValueType, int sdim>
  const auto &get_values_from_cache(const int s_id = 0) const
  {
    const auto &cache = local_cache_.template
                        get_sub_elem_cache<sdim>(s_id);
    return cache.template get_data<ValueType>();
  }

  const ValueVector<Value> &
  get_element_values_D0() const;

  const ValueVector<Derivative<1> > &
  get_element_values_D1() const;

  const ValueVector<Derivative<2> > &
  get_element_values_D2() const;

#if 0
  /**
   * @name Methods for the for the evaluations of Functions's derivatives
   *  without the use of the cache.
   */
  ///@{
  /**
   * Returns a ValueTable with the values specified by the template parameter
   * <tt>ValueType</tt>
   * at each point (in the unit domain) specified by the input argument <tt>points</tt>.
   * @note This function does not use the cache and therefore can be called any time without
   * needing to pre-call init_cache()/fill_cache().
   * @warning The evaluation <tt>points</tt> must belong to the unit hypercube
   * \f$ [0,1]^{\text{dim}} \f$ otherwise, in Debug mode, an assertion will be raised.
   */
  template <class ValueType>
  decltype(auto) evaluate_at_points(const std::shared_ptr<const Quadrature<dim_>> &points)
  {
    auto grid_func_elem_handler = this->grid_function_->create_cache_handler();
    grid_func_elem_handler->template set_flags<dim_>(ValueType::flag);
    grid_func_elem_handler->init_cache(*this,points);
    grid_func_elem_handler->template fill_cache<dim_>(*this,0);

    return this->template get_values_from_cache<ValueType,dim_>(0);
  }
  ///@}
#endif

public:
  template <int order>
  using _D = grid_function_element::_D<order>;

private:

  template <class ValueType>
  struct IsInCache
  {
    const static bool value =
      std::is_same<ValueType,_D<0>>::value ||
      std::is_same<ValueType,_D<1>>::value ||
      std::is_same<ValueType,_D<2>>::value;
  };

  using CType = boost::fusion::map<
                boost::fusion::pair<_D<0>, DataWithFlagStatus<ValueVector<Value>>>,
                boost::fusion::pair<_D<1>,DataWithFlagStatus<ValueVector<Derivative<1>>>>,
                boost::fusion::pair<_D<2>,DataWithFlagStatus<ValueVector<Derivative<2>>>>
                >;

public:

  using Cache = PointValuesCache<dim_,CType>;

  using CacheType = AllSubElementsCache<Cache>;

protected:
  std::shared_ptr<ContainerType> grid_function_;

private:
  std::unique_ptr<GridElem> grid_elem_;

  CacheType local_cache_;

  template <class Accessor> friend class GridIteratorBase;
  friend class GridFunctionHandler<dim_, space_dim_>;


public:
  /**
   * @name Methods for the for the evaluations of Functions's derivatives
   *  without the use of the cache.
   */
  ///@{

  /**
   * Returns a ValueTable with the values specified by the template parameter
   * <tt>ValueType</tt>
   * at each point (in the unit domain) specified by the input argument <tt>points</tt>.
   * @note This function does not use the cache and therefore can be called any time without
   * needing to pre-call init_cache()/fill_cache().
   * @warning The evaluation <tt>points</tt> must belong to the unit hypercube
   * \f$ [0,1]^{\text{dim}} \f$ otherwise, in Debug mode, an assertion will be raised.
   */
  template <class ValueType>
  decltype(auto) evaluate_at_points(const std::shared_ptr<const Quadrature<dim_>> &quad,
                                    EnableIf< IsInCache<ValueType>::value > * = nullptr)
  {
    auto elem_handler = this->grid_function_->create_cache_handler();
    elem_handler->template set_flags<dim_>(ValueType::flag);
    elem_handler->init_cache(*this,quad);
    elem_handler->template fill_cache<dim_>(*this,0);

    return this->template get_values_from_cache<ValueType,dim_>(0);
  }

  template <class ValueType>
  decltype(auto) evaluate_at_points(const std::shared_ptr<const Quadrature<dim_>> &quad,
                                    EnableIf< !(IsInCache<ValueType>::value) > * = nullptr)
  {
    return grid_elem_->template evaluate_at_points<ValueType>(quad);
  }
  ///@}
};






template <int sdim,int dim,int space_dim>
class SubGridFunctionElement
  : public GridFunctionElement<sdim,space_dim>
{
private:
  using parent_t = GridFunctionElement<sdim,space_dim>;
  using self_t = SubGridFunctionElement<sdim,dim,space_dim>;

public:
  using ContainerType = const SubGridFunction<sdim,dim,space_dim>;
  using GridElem = typename ContainerType::GridType::ElementAccessor;
  using ListIt = typename ContainerType::ListIt;

  using IndexType = typename Grid<sdim>::IndexType;

  using Value =  typename ContainerType::Value;
  template <int order>
  using Derivative = typename ContainerType::template Derivative<order>;


// using Gradient =  typename ContainerType_::Gradient;

  using Flags = grid_function_element::Flags;


  /** @name Constructors */
  ///@{
protected:
  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  SubGridFunctionElement() = default;

public:
  /**
   * Construct an accessor pointing to the element with
   * flat index @p elem_index of the Function @p func.
   */
  SubGridFunctionElement(const std::shared_ptr<ContainerType> &sub_grid_function,
                         const ListIt &index,
                         const PropId &prop = ElementProperties::active)
    :
    parent_t(sub_grid_function,sub_grid_function->get_id_elems_sub_grid().begin(),prop),
    sup_grid_func_element_(
     std::make_shared<GridFunctionElement<dim,space_dim>>(
       sub_grid_function->get_sup_grid_function(),
       sub_grid_function->get_id_elems_sup_grid().begin(),
       prop))
  {}

  /**
   * Copy constructor. Not allowed to be used.
   */
  SubGridFunctionElement(const self_t &elem) = delete;

  /**
   * Move constructor.
   */
  SubGridFunctionElement(self_t &&elem) = default;

  /**
   * Destructor.
   */
  ~SubGridFunctionElement() = default;
  ///@}


  /**
   * @name Comparison operators
   * @note In order to be meaningful, the comparison must be performed on
   * elements defined on
   * the <b>same</b> GridFunction
   * (in the sense that the pointer to the GridFunction held by the elements must
   * point to the same GridFunction object).
   */
  ///@{
  /**
   * True if the elements have the same index.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator==(const self_t &elem) const;

  /**
   * True if the elements have different index.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator!=(const self_t &elem) const;

  /**
   * True if the flat-index of the element on the left is smaller than
   * the flat-index of the element on the right.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator<(const self_t &elem) const;

  /**
   * True if the flat-index of the element on the left is bigger than
   * the flat-index of the element on the right.
   *  @note In debug mode, it is also check they both refer to
   *  the same cartesian grid. No check is done on the cache.
   */
  bool operator>(const self_t &elem) const;
  ///@}



  virtual void operator++() override
  {
    parent_t::operator++();

    using SubGridFunc = SubGridFunction<sdim,dim,space_dim>;
    const auto grid_func =
      std::dynamic_pointer_cast<const SubGridFunc>(this->grid_function_);

    if (this->get_grid_element().get_index_iterator() != grid_func->get_id_elems_sub_grid().end())
    {
    	const auto & sub_elem_id = this->get_index();
    	const auto & sup_elem_id = grid_func->get_sup_element_id(sub_elem_id);

        sup_grid_func_element_->move_to(sup_elem_id);
        LogStream out;
        out.begin_item("operator++");
        out << "Sub elem ID: " << sub_elem_id << "    Sup elem ID: " << sup_elem_id << std::endl;
        out.end_item();
    }
    else
    {
    	sup_grid_func_element_->move_to(*(--grid_func->get_id_elems_sup_grid().end()));
    	++(*sup_grid_func_element_);
    }

//    Assert(false,ExcNotImplemented());
  }


  void move_to(const IndexType &elem_id) override
  {
    parent_t::move_to(elem_id);
    Assert(false,ExcNotImplemented());
  }



  void print_info(LogStream &out) const
  {
    Assert(false,ExcNotImplemented());
  }

  void print_cache_info(LogStream &out) const
  {
    Assert(false,ExcNotImplemented());
  }


  GridFunctionElement<dim,space_dim> &
  get_sup_grid_function_element()
  {
    return *sup_grid_func_element_;
  }

private:

  std::shared_ptr<GridFunctionElement<dim,space_dim>> sup_grid_func_element_;
};


IGA_NAMESPACE_CLOSE

#endif


