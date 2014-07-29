//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
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

#ifndef CARTESIAN_GRID_H_
#define CARTESIAN_GRID_H_

#include <igatools/base/config.h>
#include <igatools/base/tensor.h>
#include <igatools/utils/cartesian_product_array.h>
#include <igatools/base/logstream.h>
#include <igatools/geometry/grid_forward_iterator.h>
#include <igatools/geometry/unit_element.h>
#include <igatools/utils/dynamic_multi_array.h>

#include <array>
#include <vector>
#include <memory>

#include <boost/signals2.hpp>

IGA_NAMESPACE_OPEN

template <int> class CartesianGridElement;
template <int> class CartesianGridElementAccessor;

/**
 * @brief Grid in <tt>dim</tt>-dimensional space with cartesian-product structure.
 *
 *
 * ### Getting a CartesianGrid by an XML structure.
 * A CartesianGrid object can be obtained by a Boost XML structure using the function
 * get_cartesian_grid_from_xml().
 * An example of valid XML structure for a 2-dimensional CartesianGrid is given by the following
 * XML block
 * @code{.xml}
   <CartesianGrid Dim="2">
     <Knots Direction="0" Size="2">
       0.0 1.0
     </Knots>
     <Knots Direction="1" Size="2">
        0.0 1.0
     </Knots>
   </CartesianGrid>
 * @endcode
 * where the <tt>Dim</tt> attribute in the <tt>"CartesianGrid"</tt> tag is the dimensionality of the CartesianGrid
 * and the <tt>Size</tt> attribute in each <tt>"Knots"</tt> tag is the number of unique knots values (also called
 * "breakpoints") along each coordinate direction.
 *
 * @author M. Martinelli 2012, 2013, 2014
 * @author pauletti 2012, 2013
 *
 * @see get_cartesian_grid_from_xml()
 * @ingroup h_refinement
 * @ingroup containers
 * @todo document more
 */
template< int dim_ >
class CartesianGrid :
    public std::enable_shared_from_this<CartesianGrid<dim_> >
{
public:
    /** Dimensionality of the grid. */
    static const int dim = dim_ ;

    /**
     * Types of grid for future optimization
     */
    enum class Kind
    {
        uniform, direction_uniform, non_uniform
    };

    /** Type for the face of the grid
     * @note for the case dim==0 (with non existent face type)
     * we use CartesianGrid<0>, but any function dealing with face
     * should generate an exception if called with dim=0.
     */
    using FaceType = Conditional<(dim>0), CartesianGrid<dim-1>, CartesianGrid<0>>;


    /** Type for iterator over the elements.*/
    using ElementIterator = GridForwardIterator<CartesianGridElementAccessor<dim> >;


    /** @name Constructors*/
    ///@{
    /**
     * Construct a uniform cartesian grid of the unit <tt>dim</tt>-dimensional
     * hypercube \f$[0,1]^{dim}\f$, with @p n knots (equally spaced) in each dimension.
     */
    explicit CartesianGrid(const Size n = 2);

    /**
     * Construct a uniform cartesian grid of the unit <tt>dim</tt>-dimensional
     * hypercube \f$[0,1]^{dim}\f$,
     * with <tt>n[0],..,n[dim-1</tt>] knots in each dimension
     * respectively.
     */
    explicit CartesianGrid(const TensorSize<dim> &n_knots);

    /**
     * @todo Document me
     */
    explicit CartesianGrid(const BBox<dim> &end_points,
                           const Size n_knots);

    /**
     * @todo Document me
     */
    explicit CartesianGrid(const BBox<dim> &end_points,
                           const TensorSize<dim> &n_knots);

    /**
     * Construct a cartesian grid where the knot coordinate in each
     * direction is provided as CartesianProductArray object.
     *
     * The knot coordinate in each direction must be sorted and without
     * repetition.
     * @note In Debug mode, a check for this precondition (up to machine precision)
     * is perform and if not satistified an exception is raised.
     */
    explicit
    CartesianGrid(const CartesianProductArray<Real,dim> &knot_coordinates);


    /**
     * Construct a cartesian grid where the knot coordinate in each
     * direction is provided as std::array of std::vector<Real>.
     *
     * The knot coordinate in each direction must be sorted and without
     * repetition.
     * @note In Debug mode, a check for this precondition (up to machine precision)
     * is perform and if not satistified an exception is raised.
     */
    explicit
    CartesianGrid(const std::array<std::vector<Real>,dim> &knot_coordinates);


    /**
     * Copy constructor.
     * Perform a deep copy of the member variables except the
     * signal_refine_ variable, that is not copied at all.
     */
    CartesianGrid(const CartesianGrid<dim_> &grid);

    /**  Move constructor */
    CartesianGrid(CartesianGrid<dim_> &&grid) = default;

    /** Destructor */
    ~CartesianGrid() = default;
    ///@}


    /**
     * @name Creators
     * @note The functions here return a CartesianGrid<dim> object wrapped by
     * a std::shared_ptr
     */
    ///@{

    /**
     * Creates a uniform cartesian grid of the unit <tt>dim</tt>-dimensional
     * hypercube \f$[0,1]^{dim}\f$, with @p n knots (equally spaced) in each dimension.
     */
    static std::shared_ptr<CartesianGrid<dim_> > create(const Index n = 2);


    /**
     * Creates a uniform cartesian grid of the unit <tt>dim</tt>-dimensional
     * hypercube \f$[0,1]^{dim}\f$,
     * with <tt>n[0],..,n[dim-1</tt>] knots in each dimension
     * respectively.
     */
    static std::shared_ptr< CartesianGrid<dim_> > create(const TensorSize<dim> &n);


    /**
     * Construct a cartesian grid where the knot coordinate in each
     * direction is provided as CartesianProductArray object.
     *
     * The knot coordinate in each direction must be sorted and without
     * repetition.
     * @note In Debug mode, a check for this precondition (up to machine precision)
     * is perform and if not satistified an exception is raised.
     */
    static std::shared_ptr< CartesianGrid<dim_> >
    create(const CartesianProductArray<Real,dim> &knot_coordinates) ;

    /**
     * Construct a cartesian grid where the knot coordinate in each
     * direction is provided as std::array of std::vector<Real>.
     *
     * The knot coordinate in each direction must be sorted and without
     * repetition.
     * @note In Debug mode, a check for this precondition (up to machine precision)
     * is perform and if not satistified an exception is raised.
     */
    static std::shared_ptr< CartesianGrid<dim_> >
    create(const std::array<std::vector<Real>,dim> &knot_coordinates) ;


    static std::shared_ptr< CartesianGrid<dim_> >
    create(const BBox<dim> &end_points,
           const Size n_knots);

    /**
     * @todo document me
     */
    static std::shared_ptr< CartesianGrid<dim_> >
    create(const BBox<dim> &end_points,
           const TensorSize<dim> &n);
    ///@}


    /**
     * @name Assignment operators
     */
    ///@{

    /**
     * Copy assignment operator.
     */
    CartesianGrid<dim_> &operator=(const CartesianGrid<dim_> &grid) = default;

    /**
     * Move assignment operator.
     */
    CartesianGrid<dim_> &operator=(CartesianGrid<dim_> &&grid) = default;

    ///@}


    ///@name Getting grid information
    ///@{
    /**
     * Query the total number of elements.
     */
    Size get_num_elements() const;

    /**
     * Query the total number of elements along each coordinate direction.
     */
    TensorSize<dim> get_num_elements_dim() const;

    /**
     * Query the number of knot values along each coordinate direction
     * represented by the CartesianGrid.
     */
    TensorSize<dim> get_num_knots_dim() const;

    /**
     * Returns the knot coordinates along the direction @p i.
     */
    std::vector<Real> const &get_knot_coordinates(const int i) const;

    /**
     * Returns the knot coordinates along all the directions (const version).
     */
    CartesianProductArray<Real, dim> const &get_knot_coordinates() const;


    /**
     * Returns the knot coordinates along the direction @p i.
     */
    CartesianProductArray<Real, dim> get_element_lengths() const ;

    /**
     * Returns the smallest <tt>dim</tt>-dimensional bounding box enclosing the domain represented
     * by the CartesianGrid object.
     */
    BBox<dim> get_bounding_box() const;
    ///@}


    ///@name Iterating of grid elements
    ///@{
    /**
     * This function returns a element iterator to the first element of the patch.
     */
    ElementIterator begin() const;

    /**
     * This function returns a element iterator to one-pass the end of patch.
     */
    ElementIterator end() const;
    ///@}

    ///@name Dealing with boundary information
    ///@{
    /**
     * Get the patch @p face boundary id.
     */
    boundary_id get_boundary_id(const int face) const;

    /**
     * Set the patch @p face to have the boundary @p id.
     */
    void set_boundary_id(const int face, const boundary_id id);

    /**
     * Returns the outward pointing
     * unit normal vector to the face number @p face_no.
     */
    Points<dim> get_face_normal(const int face_no) const;

    /**
     * Returns the CartesianGrid on the @p face_id.
     */
    std::shared_ptr<FaceType> get_face_grid(const int face_id, std::map<int,int> &elem_map) const;

    ///@}


    /**
     * Returns the flat id of the element that contains the <tt>point</tt>.
     * @note If the points belongs to the boundaries of two elements, then the owner will be the
     * element with smaller id.
     * @note If the point does not belong to the domain represented by the CartesianGrid object,
     * then an assertion will be raised (in Debug mode).
     */
    Index get_element_flat_id_from_point(const Points<dim> &point) const;


    /**
     * Returns the flat representation of an element index from its tensor-like representation.
     * @note In Debug mode, the element index is checked for its validity in the CartesianGrid.
     */
    Index tensor_to_flat_element_index(const TensorIndex<dim> &tensor_id) const;

    /**
     * Returns the tensor-like representation of an element index from its flat representation.
     * @note In Debug mode, the element index is checked for its validity in the CartesianGrid.
     */
    TensorIndex<dim> flat_to_tensor_element_index(const Index flat_id) const;


    /**
     * Prints the CartesianGrid on a LogStream.
     */
    void print_info(LogStream &out) const;


    /**
     * Comparison operator. Returns true if the knot coordinates of two grid are identical.
     */
    bool operator==(const CartesianGrid<dim> &grid) const;

private:
    /** Type for the refinement signal. */
    using signal_refine_t = boost::signals2::signal<
            void (const std::array<bool,dim> &,const CartesianGrid<dim> &)>;

public:

    /** Slot type for the refinement signal. */
    using SignalRefineSlot = typename signal_refine_t::slot_type;

    /** @name Functions for performing grid refinement */
    ///@{
    /**
     * Perform a uniform refinement of the grid along the @p direction_id direction,
     * dividing each interval into @p n_subdivisions intervals.
     * @param[in] direction_id Direction along which the refinement is performed.
     * @param[in] n_subdivisions Number of subdivision in which each interval in the grid
     * along the specified direction is divided. This value must be >= 2.
     */
    void refine_direction(const int direction_id, const Size n_subdivisions);

    /**
     * Refine the cartesian grid and the objects connected to it (if any),
     * e.g. maps, spaces, etc.
     * along the directions specified by the true values in the entries of the
     * array of bools @p refinement_directions,
     * and with a number of subdivisions for each interval (along each direction)
     * specified by @p n_subdivisions.
     *
     * @note If the i-th direction is not active for the refinement
     * (i.e. <tt>refinement_directions[i] == false</tt>),
     *  then the corresponding value <tt>n_subdivisions[i]</tt> will be ignored.
     */
    void refine_directions(
        const std::array<bool,dim> &refinement_directions,
        const std::array<Size,dim> &n_subdivisions) ;

    /**
     * Refine the cartesian grid and the objects connected to it (if any),
     * e.g. maps, spaces, etc.
     *
     * Each interval in the unrefined grid is uniformly divided in @p n_subdivisions
     * sub-intervals.
     */
    void refine(const Size n_subdivisions = 2);

    /**
     *  Connect a slot (i.e. a function pointer) to the refinement signals which will be
     *  emitted whenever a refine() function is called by an object holding a CartesianGrid member.
     */
    boost::signals2::connection connect_refinement(const SignalRefineSlot &subscriber) ;


    /**
     * Returns the grid before the last refinement. If no refinement is performed,
     * this function returns a null pointer.
     */
    std::shared_ptr<const CartesianGrid<dim> > get_grid_pre_refinement() const;
    ///@}

private:

    Kind kind_ = Kind::non_uniform;

    /**
     * Boundary ids, one id per face
     */
    std::array< boundary_id, UnitElement<dim>::faces_per_element > boundary_id_;

    /**
     *  Knot coordinates along each coordinate direction.
     *  For each direction the knot coordinates are sorted in an increasing
     *  order and does not contain duplicate values.
     */
    CartesianProductArray<Real,dim> knot_coordinates_;

    /**
     * Weights used for fast conversion of element index (flat-to-tensor and tensor-to-flat);
     */
    TensorIndex<dim> weight_elem_id_;

    /**
     * Container for the flags indicating the elements with influence (used to define hierarchical spaces).
     */
    DynamicMultiArray<bool,dim> influence_flags_container_;

    /**
     * Container for the flags indicating the active elements (used to define hierarchical spaces).
     */
    DynamicMultiArray<bool,dim> active_flags_container_;




    /**
     * Perform a uniform refinement of the knots along the @p direction_id direction,
     * dividing each interval in the knot vector into @p n_subdivisions intervals.
     * @param[in] direction_id Direction along which the refinement is performed.
     * @param[in] n_subdivisions Number of subdivision in which each interval in the knot vector is
     * divided. This value must be >= 2.
     */
    void refine_knots_direction(const int direction_id,
                                const Size n_subdivisions) ;

    /**
     * This class member is the grid before the last refinement. If no refinement is performed,
     * this is a null pointer.
     */
    std::shared_ptr<const CartesianGrid<dim> > grid_pre_refinement_ = nullptr;

    /**
     * Signals for the h-refinement. It can be viewed as a FIFO list of function pointers.
     */
    signal_refine_t refine_signals_;

    friend class CartesianGridElement<dim>;

    friend class CartesianGridElementAccessor<dim>;
};



/**
 * Given one grid <tt>grid_coarse</tt> and a refinement <tt>grid_fine</tt>,
 * this function builds and returns the one-to-one mapping between the elements on the
 * fine and the elements on the coarse.
 * @code{.cpp}
   fine_to_coarse_elements = build_map_elements_between_cartesian_grids(grid_fine,grid_coarse);
   // fine_to_coarse_elements[i] is the flat id of the element on the coarse grid that fully contains the
   // element on the fine grid with flat_id equal to i.
   @endcode
 *
 * @warning The grid must be defined on the same domain and each element on the fine grid must be
 * FULLY contained in one element of the coarse grid, otherwise an exception will be raised
 * (in Debug mode).
 *
 * @relates CartesianGrid
 */
template <int dim>
std::vector<Index>
build_map_elements_between_cartesian_grids(
    const CartesianGrid<dim> &grid_fine,
    const CartesianGrid<dim> &grid_coarse);


/**
 * Given two grids <tt>grid_1</tt> and <tt>grid_2</tt> defined over the same domain,
 * this function returns the grid that contains both. Moreover, this function gives back
 * the one-to-one mapping between the flat id of the elements in the CartesianGrid union with the
 * elements in the two starting grids.
 *
 * @param[in] grid_1 First grid.
 * @param[in] grid_2 Second grid.
 * @param[out] map_elem_grid_union_to_elem_grid_1 One-to-one mapping between the elements in the
 * CartesianGrid union and the elements in the first grid.
 * @param[out] map_elem_grid_union_to_elem_grid_2 One-to-one mapping between the elements in the
 * CartesianGrid union and the elements in the second grid.
 * @return The CartesianGrid that contains the all the knots from <tt>grid_1</tt> and <tt>grid_2</tt>.
 *
 * @warning In Debug mode an assertion will be raised if
 * the two grids <tt>grid_1</tt> and <tt>grid_2</tt> are not defined on the same domain.
 *
 * @relates CartesianGrid
 */
template <int dim>
CartesianGrid<dim>
build_cartesian_grid_union(
    const CartesianGrid<dim> &grid_1,
    const CartesianGrid<dim> &grid_2,
    std::vector<Index> &map_elem_grid_union_to_elem_grid_1,
    std::vector<Index> &map_elem_grid_union_to_elem_grid_2);


IGA_NAMESPACE_CLOSE

#endif /* CARTESIAN_GRID_H_ */
