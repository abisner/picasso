#ifndef CAJITA_INDEXSPACE_HPP
#define CAJITA_INDEXSPACE_HPP

#include <Kokkos_Core.hpp>

#include <vector>
#include <algorithm>
#include <string>

namespace Cajita
{
//---------------------------------------------------------------------------//
/*!
  \class IndexSpace
  \brief Structured index space.
 */
template<long N>
class IndexSpace
{
  public:

    //! Number of dimensions.
    static constexpr long Rank = N;

    /*!
      \brief Initializer list size constructor.
    */
    IndexSpace( const std::initializer_list<long>& size )
    {
        std::fill( _min.data(), _min.data() + Rank, 0 );
        std::copy( size.begin(), size.end(), _max.data() );
    }

    /*!
      \brief Initializer list range constructor.
    */
    IndexSpace( const std::initializer_list<long>& min,
                const std::initializer_list<long>& max )
    {
        std::copy( min.begin(), min.end(), _min.data() );
        std::copy( max.begin(), max.end(), _max.data() );
    }

    /*!
      \brief Vector size constructor.
    */
    IndexSpace( const std::vector<long>& size )
    {
        std::fill( _min.data(), _min.data() + Rank, 0 );
        std::copy( size.begin(), size.end(), _max.data() );
    }

    /*!
      \brief Vector range constructor.
    */
    IndexSpace( const std::vector<long>& min,
                const std::vector<long>& max )
    {
        std::copy( min.begin(), min.end(), _min.data() );
        std::copy( max.begin(), max.end(), _max.data() );
    }

    //! Comparison operator.
    bool operator==( const IndexSpace<N>& rhs ) const
    {
        for ( long i = 0; i < N; ++i )
        {
            if ( min(i) != rhs.min(i) || max(i) != rhs.max(i) )
                return false;
        }
        return true;
    }

    //! Comparison operator.
    bool operator!=( const IndexSpace<N>& rhs ) const
    {
        return !(operator==(rhs));
    }

    //! Get the minimum index in a given dimension.
    long min( const long dim ) const
    { return _min[dim]; }

    //! Get the minimum indices in all dimensions.
    Kokkos::Array<long,Rank> min() const
    { return _min; }

    //! Get the maximum index in a given dimension.
    long max( const long dim ) const
    { return _max[dim]; }

    //! Get the maximum indices in all dimensions.
    Kokkos::Array<long,Rank> max() const
    { return _max; }

    //! Get the range of a given dimension.
    std::pair<long,long> range( const long dim ) const
    { return std::make_pair(_min[dim],_max[dim]); }

    //! Get the number of dimensions.
    long rank() const
    { return Rank; }

    //! Get the extent of a given dimension.
    long extent( const long dim ) const
    { return _max[dim] - _min[dim]; }

    //! Get the total size of the index space.
    long size() const
    {
        long size = 1;
        for ( long d = 0; d < Rank; ++d )
            size *= extent(d);
        return size;
    }

  private:

    // Minimum index bounds.
    Kokkos::Array<long,Rank> _min;

    // Maximum index bounds.
    Kokkos::Array<long,Rank> _max;
};

//---------------------------------------------------------------------------//
/*!
  \brief Create a multi-dimensional execution policy over an index space.

  Rank-1 specialization.
*/
template<class ExecutionSpace>
Kokkos::RangePolicy<ExecutionSpace>
createExecutionPolicy( const IndexSpace<1>& index_space,
                       const ExecutionSpace& )
{
    return Kokkos::RangePolicy<ExecutionSpace>(
        index_space.min(0), index_space.max(0) );
}

//---------------------------------------------------------------------------//
/*!
  \brief Create a multi-dimensional execution policy over an index space.
*/
template<class IndexSpace_t, class ExecutionSpace>
Kokkos::MDRangePolicy<ExecutionSpace,Kokkos::Rank<IndexSpace_t::Rank>>
createExecutionPolicy( const IndexSpace_t& index_space,
                       const ExecutionSpace& )
{
    return Kokkos::MDRangePolicy<ExecutionSpace,
                                 Kokkos::Rank<IndexSpace_t::Rank> >(
                                     index_space.min(), index_space.max() );
}

//---------------------------------------------------------------------------//
/*!
  \brief Given an index space create a view over the extent of that index
  space.

  Rank-1 specialization.
*/
template<class Scalar, class ... Params>
Kokkos::View<Scalar*,Params...>
createView( const std::string& label, const IndexSpace<1>& index_space )
{
    return Kokkos::View<Scalar*,Params...>(
        Kokkos::ViewAllocateWithoutInitializing(label),
        index_space.extent(0) );
}

//---------------------------------------------------------------------------//
/*!
  \brief Given an index space create a view over the extent of that index
  space.

  Rank-2 specialization.
*/
template<class Scalar, class ... Params>
Kokkos::View<Scalar**,Params...>
createView( const std::string& label, const IndexSpace<2>& index_space )
{
    return Kokkos::View<Scalar**,Params...>(
        Kokkos::ViewAllocateWithoutInitializing(label),
        index_space.extent(0),
        index_space.extent(1) );
}

//---------------------------------------------------------------------------//
/*!
  \brief Given an index space create a view over the extent of that index
  space.

  Rank-3 specialization.
*/
template<class Scalar, class ... Params>
Kokkos::View<Scalar***,Params...>
createView( const std::string& label, const IndexSpace<3>& index_space )
{
    return Kokkos::View<Scalar***,Params...>(
        Kokkos::ViewAllocateWithoutInitializing(label),
        index_space.extent(0),
        index_space.extent(1),
        index_space.extent(2) );
}

//---------------------------------------------------------------------------//
/*!
  \brief Given an index space create a view over the extent of that index
  space.

  Rank-4 specialization.
*/
template<class Scalar, class ... Params>
Kokkos::View<Scalar****,Params...>
createView( const std::string& label, const IndexSpace<4>& index_space )
{
    return Kokkos::View<Scalar****,Params...>(
        Kokkos::ViewAllocateWithoutInitializing(label),
        index_space.extent(0),
        index_space.extent(1),
        index_space.extent(2),
        index_space.extent(3) );
}

//---------------------------------------------------------------------------//
/*!
  \brief Given a view create a subview over the given index space.

  Rank-1 specialization.
*/
template<class ViewType>
auto createSubview( const ViewType& view,
                    const IndexSpace<1>& index_space )
    -> decltype( Kokkos::subview(view,
                                 index_space.range(0)) )
{
    static_assert( 1 == ViewType::Rank, "Incorrect view rank" );
    return Kokkos::subview(view,
                           index_space.range(0));
}

//---------------------------------------------------------------------------//
/*!
  \brief Given a view create a subview over the given index space.

  Rank-2 specialization.
*/
template<class ViewType>
auto createSubview( const ViewType& view,
                    const IndexSpace<2>& index_space )
    -> decltype( Kokkos::subview(view,
                                 index_space.range(0),
                                 index_space.range(1)) )
{
    static_assert( 2 == ViewType::Rank, "Incorrect view rank" );
    return Kokkos::subview(view,
                           index_space.range(0),
                           index_space.range(1));
}

//---------------------------------------------------------------------------//
/*!
  \brief Given a view create a subview over the given index space.

  Rank-3 specialization.
*/
template<class ViewType>
auto createSubview( const ViewType& view,
                    const IndexSpace<3>& index_space )
    -> decltype( Kokkos::subview(view,
                                 index_space.range(0),
                                 index_space.range(1),
                                 index_space.range(2)) )
{
    static_assert( 3 == ViewType::Rank, "Incorrect view rank" );
    return Kokkos::subview(view,
                           index_space.range(0),
                           index_space.range(1),
                           index_space.range(2));
}

//---------------------------------------------------------------------------//
/*!
  \brief Given a view create a subview over the given index space.

  Rank-4 specialization.
*/
template<class ViewType>
auto createSubview( const ViewType& view,
                    const IndexSpace<4>& index_space )
    -> decltype( Kokkos::subview(view,
                                 index_space.range(0),
                                 index_space.range(1),
                                 index_space.range(2),
                                 index_space.range(3)) )
{
    static_assert( 4 == ViewType::Rank, "Incorrect view rank" );
    return Kokkos::subview(view,
                           index_space.range(0),
                           index_space.range(1),
                           index_space.range(2),
                           index_space.range(3));
}

//---------------------------------------------------------------------------//
// Given an N-dimensional index space append an additional dimension with the
// given size.
template<long N>
IndexSpace<N+1> appendDimension( const IndexSpace<N>& index_space,
                                 const long size )
{
    std::vector<long> min( N+1 );
    for ( int d = 0; d < N; ++d )
        min[d] = index_space.min(d);
    min[N] = 0;

    std::vector<long> max( N+1 );
    for ( int d = 0; d < N; ++d )
        max[d] = index_space.max(d);
    max[N] = size;

    return IndexSpace<N+1>( min, max );
}

//---------------------------------------------------------------------------//
// Given an N-dimensional index space append an additional dimension with the
// given range.
template<long N>
IndexSpace<N+1> appendDimension( const IndexSpace<N>& index_space,
                                 const long min,
                                 const long max )
{
    std::vector<long> range_min( N+1 );
    for ( int d = 0; d < N; ++d )
        range_min[d] = index_space.min(d);
    range_min[N] = min;

    std::vector<long> range_max( N+1 );
    for ( int d = 0; d < N; ++d )
        range_max[d] = index_space.max(d);
    range_max[N] = max;

    return IndexSpace<N+1>( range_min, range_max );
}

//---------------------------------------------------------------------------//

} // end namespace Cajita

#endif // end CAJITA_INDEXSPACE_HPP
