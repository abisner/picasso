#ifndef PICASSO_BATCHEDLINEARALGEBRA_HPP
#define PICASSO_BATCHEDLINEARALGEBRA_HPP

#include <Kokkos_Core.hpp>

#include <KokkosBatched_Copy_Decl.hpp>
#include <KokkosBatched_Copy_Impl.hpp>

#include <KokkosBatched_Set_Decl.hpp>
#include <KokkosBatched_Set_Impl.hpp>

#include <KokkosBatched_Gemm_Decl.hpp>
#include <KokkosBatched_Gemm_Serial_Impl.hpp>

#include <type_traits>

namespace Picasso
{
namespace LinearAlgebra
{
//---------------------------------------------------------------------------//
// Transpose tags.
struct NoTranspose
{
    using type = KokkosBatched::Trans::NoTranspose;
};

struct Transpose
{
    using type = KokkosBatched::Trans::Transpose;
};

//---------------------------------------------------------------------------//
// Dense matrix in row-major order with a KokkosKernels compatible data
// interface.
template<class T, int M, int N, class TransposeType = NoTranspose>
struct Matrix;

// No transpose
template<class T, int M, int N>
struct Matrix<T,M,N,NoTranspose>
{
    T _d[M][N];
    int _extent[2] = {M,N};

    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = typename std::add_const<T>::type&;

    // Default constructor.
    KOKKOS_DEFAULTED_FUNCTION
    Matrix() = default;

    // Initializer list constructor.
    KOKKOS_INLINE_FUNCTION
    Matrix( const std::initializer_list<std::initializer_list<T>> data )
    {
        int i = 0;
        int j = 0;
        for ( const auto& row : data )
        {
            j = 0;
            for ( const auto& value : row )
            {
                _d[i][j] = value;
                ++j;
            }
            ++i;
        }
    }

    // Deep copy constructor.
    Matrix( const Matrix<T,M,N,NoTranspose>& rhs )
    {
        KokkosBatched::SerialCopy<NoTranspose::type>::invoke(
            rhs, *this );
    }

    // Deep copy transpose constructor.
    Matrix( const Matrix<T,N,M,Transpose>& rhs )
    {
        KokkosBatched::SerialCopy<Transpose::type>::invoke(
            rhs, *this );
    }

    // Deep copy assignment operator.
    Matrix& operator=( const Matrix<T,M,N,NoTranspose>& rhs )
    {
        KokkosBatched::SerialCopy<NoTranspose::type>::invoke(
            rhs, *this );
        return *this;
    }

    // Deep copy transpose assignment operator.
    Matrix& operator=( const Matrix<T,M,N,Transpose>& rhs )
    {
        KokkosBatched::SerialCopy<Transpose::type>::invoke(
            rhs, *this );
        return *this;
    }

    // Scalar value assignment.
    KOKKOS_INLINE_FUNCTION
    Matrix& operator=( const T value )
    {
        KokkosBatched::SerialSet::invoke( value, *this );
        return *this;
    }

    // Transpose operator.
    Matrix<T,M,N,Transpose> operator~()
    {
        return Matrix<T,M,N,Transpose>( this->data() );
    }

    // Strides.
    KOKKOS_INLINE_FUNCTION
    int stride_0() const
    { return N; }

    KOKKOS_INLINE_FUNCTION
    int stride_1() const
    { return 1; }

    // Extent
    KOKKOS_INLINE_FUNCTION
    int extent( const int d ) const
    { return _extent[d]; }

    // Access an individual element.
    KOKKOS_INLINE_FUNCTION
    const_reference operator()( const int i, const int j ) const
    { return _d[i][j]; }

    KOKKOS_INLINE_FUNCTION
    reference operator()( const int i, const int j )
    { return _d[i][j]; }

    // Get the raw data.
    KOKKOS_INLINE_FUNCTION
    pointer data() const
    { return const_cast<pointer>(&_d[0][0]); }
};

// Transpose. This class is essentially a shallow-copy placeholder to enable
// tranpose matrix operations without copies.
template<class T, int M, int N>
struct Matrix<T,M,N,Transpose>
{
    T* _d;
    int _extent[2] = {M,N};

    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = typename std::add_const<T>::type&;

    // Pointer constructor.
    KOKKOS_INLINE_FUNCTION
    Matrix( T* data )
        : _d( data )
    {}

    // Deep copy constructor.
    Matrix( const Matrix& rhs )
    {
        KokkosBatched::SerialCopy<KokkosBatched::Trans::NoTranspose>::invoke(
            rhs, *this );
    }

    // Strides.
    KOKKOS_INLINE_FUNCTION
    int stride_0() const
    { return N; }

    KOKKOS_INLINE_FUNCTION
    int stride_1() const
    { return 1; }

    // Extent
    KOKKOS_INLINE_FUNCTION
    int extent( const int d ) const
    { return _extent[d]; }

    // Get the raw data.
    KOKKOS_INLINE_FUNCTION
    pointer data() const
    { return const_cast<pointer>(_d); }
};

//---------------------------------------------------------------------------//
// Dense vector with a KokkosKernels compatible data interface.
template<class T, int N>
class Vector
{
  private:

    T _d[N];
    int _extent[2] = {N,1};

  public:

    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = typename std::add_const<T>::type&;

    // Default constructor.
    KOKKOS_DEFAULTED_FUNCTION
    Vector() = default;

    // Initializer list constructor.
    KOKKOS_INLINE_FUNCTION
    Vector( const std::initializer_list<T> data )
    {
        int i = 0;
        for ( const auto& value : data )
        {
            _d[i] = value;
            ++i;
        }
    }

    // Deep copy constructor.
    Vector( const Vector& rhs )
    {
        KokkosBatched::SerialCopy<KokkosBatched::Trans::NoTranspose>::invoke(
            rhs, *this );
    }

    // Deep copy assignment operator.
    Vector& operator=( const Vector& rhs )
    {
        KokkosBatched::SerialCopy<KokkosBatched::Trans::NoTranspose>::invoke(
            rhs, *this );
        return *this;
    }


    // Scalar value assignment.
    KOKKOS_INLINE_FUNCTION
    Vector& operator=( const T value )
    {
        KokkosBatched::SerialSet::invoke( value, *this );
        return *this;
    }

    // Strides.
    KOKKOS_INLINE_FUNCTION
    int stride_0() const
    { return 1; }

    // Strides.
    KOKKOS_INLINE_FUNCTION
    int stride_1() const
    { return 0; }

    // Extent
    KOKKOS_INLINE_FUNCTION
    int extent( const int d ) const
    { return _extent[d]; }

    // Access an individual element.
    KOKKOS_INLINE_FUNCTION
    const_reference operator()( const int i ) const
    { return _d[i]; }

    KOKKOS_INLINE_FUNCTION
    reference operator()( const int i )
    { return _d[i]; }

    // Get the raw data.
    KOKKOS_INLINE_FUNCTION
    pointer data() const
    { return const_cast<pointer>(&_d[0]); }
};

//---------------------------------------------------------------------------//
// Matrix-matrix multiplication
//---------------------------------------------------------------------------//
// NoTranspose case.
template<class T, int M, int N, int K>
Matrix<T,M,N,NoTranspose>
operator*( const Matrix<T,M,K,NoTranspose>& a, const Matrix<T,K,N,NoTranspose>& b )
{
    Matrix<T,M,N,NoTranspose> c;
    KokkosBatched::SerialGemm<NoTranspose::type,
                              NoTranspose::type,
                              KokkosBatched::Algo::Gemm::Unblocked>::invoke(
                                  1.0, a, b, 1.0, c );
    return c;
}

//---------------------------------------------------------------------------//
// Transpose case.
template<class T, int M, int N, int K>
Matrix<T,M,N,NoTranspose>
operator*( const Matrix<T,K,M,Transpose>& a, const Matrix<T,N,K,Transpose>& b )
{
    Matrix<T,M,N,NoTranspose> c;
    KokkosBatched::SerialGemm<Transpose::type,
                              Transpose::type,
                              KokkosBatched::Algo::Gemm::Unblocked>::invoke(
                                  1.0, a, b, 1.0, c );
    return c;
}

//---------------------------------------------------------------------------//
// NoTranspose-Transpose case.
template<class T, int M, int N, int K>
Matrix<T,M,N,NoTranspose>
operator*( const Matrix<T,M,K,NoTranspose>& a, const Matrix<T,N,K,Transpose>& b )
{
    Matrix<T,M,N,NoTranspose> c;
    KokkosBatched::SerialGemm<NoTranspose::type,
                              Transpose::type,
                              KokkosBatched::Algo::Gemm::Unblocked>::invoke(
                                  1.0, a, b, 1.0, c );
    return c;
}

//---------------------------------------------------------------------------//
// Transpose-NoTranspose case.
template<class T, int M, int N, int K>
Matrix<T,M,N,NoTranspose>
operator*( const Matrix<T,K,M,Transpose>& a, const Matrix<T,K,N,NoTranspose>& b )
{
    Matrix<T,M,N,NoTranspose> c;
    KokkosBatched::SerialGemm<Transpose::type,
                              NoTranspose::type,
                              KokkosBatched::Algo::Gemm::Unblocked>::invoke(
                                  1.0, a, b, 1.0, c );
    return c;
}

//---------------------------------------------------------------------------//

} // end namespace LinearAlgebra
} // end namespace Picasso

#endif // end PICASSO_BATCHEDLINEARALGEBRA_HPP
