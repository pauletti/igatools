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
#include <igatools/geometry/mapping.h>
#include <igatools/geometry/push_forward.h>
#include <igatools/geometry/cartesian_grid_iterator.h>
#include <igatools/basis_functions/space.h>
#include <igatools/basis_functions/reference_space.h>
#include <igatools/basis_functions/dof_distribution.h>
#include <igatools/utils/static_multi_array.h>

IGA_NAMESPACE_OPEN

class SpaceManager;

//template <int,int,int> class BSplineSpace;
//template <int,int,int> class NURBSSpace;
template <int,int,int,int> class IgFunction;


template <int,int,int,int> class PhysicalSpaceElement;

template <int,int,int,int> class PhysSpaceElementHandler;

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
    public std::enable_shared_from_this<PhysicalSpace<dim_, range_, rank_, codim_, type_>>,
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
    using PushForwardType = PushForward<type_, dim_, codim_>;

    using RefSpace = ReferenceSpace<dim_,range_,rank_>;

    using GridType = CartesianGrid<dim_>;
    ///@}
    using ElementHandler = PhysSpaceElementHandler<dim_,range_,rank_,codim_>;

    static const int dim = dim_;

    static const int codim = PushForwardType::codim;

    static const int space_dim = PushForwardType::space_dim;

    static const int range = PushForwardType::template PhysRange<range_>::value;

    static const int rank = rank_;

    static const bool is_physical_space = true;

    using MapFunc =  MapFunction<dim, space_dim>;

    static constexpr int n_components = constexpr_pow(range, rank);

    static const SafeSTLArray<int, n_components> components;


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


    using ElementAccessor = PhysicalSpaceElement<dim_,range_,rank_,codim_>;
    using ElementIterator = CartesianGridIterator<ElementAccessor>;


    PhysicalSpace(const self_t &phys_space) = delete;

    static std::shared_ptr<self_t>
    create(std::shared_ptr<RefSpace> ref_space,
           std::shared_ptr<MapFunc> map_func);

    /**
     * Create an element (defined on this grid) with a given flat_index.
     */
    std::shared_ptr<SpaceElement<dim_,codim_,range_,rank_>>
                                                         create_element(const Index flat_index) const;

    /**
     * Total number of dofs of the space.
     */
    Index get_num_basis() const;


    /** Returns the container with the global dof distribution (const version). */
    std::shared_ptr<const DofDistribution<dim, range, rank> >
    get_dof_distribution() const override final;

    /** Returns the container with the global dof distribution (non const version). */
    std::shared_ptr<DofDistribution<dim, range, rank> >
    get_dof_distribution();


    /*
    auto get_num_all_element_basis() const
    {
        return ref_space_->get_num_all_element_basis();
    }
    //*/

    template <int k>
    using SubSpace = PhysicalSpace<k, range, rank, codim + dim-k, type_>;

    template <int k>
    using InterGridMap = typename RefSpace::GridType::template InterGridMap<k>;

    template <int k>
    using InterSpaceMap = typename RefSpace::template InterSpaceMap<k>;

    template<int k>
    std::shared_ptr<SubSpace<k> >
    get_sub_space(const int s_id, InterSpaceMap<k> &dof_map,
                  std::shared_ptr<CartesianGrid<k>> sub_grid,
                  std::shared_ptr<InterGridMap<k>> elem_map) const;



    void get_element_dofs(
        const Index element_id,
        SafeSTLVector<Index> &dofs_global,
        SafeSTLVector<Index> &dofs_local_to_patch,
        SafeSTLVector<Index> &dofs_local_to_elem,
        const std::string &dofs_property = DofProperties::active) const override final;


    std::shared_ptr<const RefSpace> get_reference_space() const;

    std::shared_ptr<RefSpace> get_reference_space();

    std::shared_ptr<MapFunc> get_map_func() const;

    template<int k>
    std::set<Index> get_boundary_dofs(const int s_id) const
    {
        return ref_space_->template get_boundary_dofs<k>(s_id);
    }

    std::set<Index> get_interior_dofs() const;

    void print_info(LogStream &out) const;

    std::shared_ptr<SpaceElementHandler<dim_,codim_,range_,rank_>> get_elem_handler() const;


    std::shared_ptr<const self_t> get_space_previous_refinement() const
    {
        Assert(false,ExcNotImplemented());
        AssertThrow(false,ExcNotImplemented());
        return phys_space_previous_refinement_;
    }

private:

    /**
     * Default constructor. It does nothing but it is needed for the
     * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
     * mechanism.
     */
    PhysicalSpace() = default;

    PhysicalSpace(std::shared_ptr<RefSpace> ref_space,
                  std::shared_ptr<MapFunc>  map_func);


    std::shared_ptr<RefSpace> ref_space_;
    std::shared_ptr<MapFunc>  map_func_;

    std::shared_ptr<self_t> phys_space_previous_refinement_ = nullptr;


    friend ElementAccessor;


#ifdef SERIALIZATION
    /**
     * @name Functions needed for boost::serialization
     * @see <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
     */
    ///@{
    friend class boost::serialization::access;

    template<class Archive>
    void
    serialize(Archive &ar, const unsigned int version)
    {
        ar &boost::serialization::make_nvp("PhysicalSpace_base_t",
                                           boost::serialization::base_object<base_t>(*this));

        ar.template register_type<BSplineSpace<dim_,range_,rank_> >();
        ar.template register_type<NURBSSpace<dim_,range_,rank_> >();
        ar &boost::serialization::make_nvp("ref_space_",ref_space_);
        Assert(ref_space_ != nullptr,ExcNullPtr());

        ar.template register_type<IgFunction<dim,0,dim+codim,1> >();
        ar &boost::serialization::make_nvp("map_func_",map_func_);
        Assert(map_func_ != nullptr,ExcNullPtr());

        ar &boost::serialization::make_nvp("phys_space_previous_refinement_",phys_space_previous_refinement_);
    }

    ///@}
#endif
};

IGA_NAMESPACE_CLOSE

#endif
