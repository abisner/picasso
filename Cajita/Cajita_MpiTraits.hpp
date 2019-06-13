#ifndef CAJITA_MPITRAITS_HPP
#define CAJITA_MPITRAITS_HPP

#include <Kokkos_Core.hpp>

#include <mpi.h>

namespace Cajita
{

//---------------------------------------------------------------------------//
// Type traits
template<typename T>
struct MpiTraits;

template<>
struct MpiTraits<char>
{
    static MPI_Datatype type() { return MPI_CHAR; }
};

template<>
struct MpiTraits<int>
{
    static MPI_Datatype type() { return MPI_INT; }
};

template<>
struct MpiTraits<long>
{
    static MPI_Datatype type() { return MPI_LONG; }
};

template<>
struct MpiTraits<float>
{
    static MPI_Datatype type() { return MPI_FLOAT; }
};

template<>
struct MpiTraits<double>
{
    static MPI_Datatype type() { return MPI_DOUBLE; }
};

//---------------------------------------------------------------------------//
// Data order.
template<typename Layout>
struct MpiOrder;

template<>
struct MpiOrder<Kokkos::LayoutLeft>
{
    static int value() { return MPI_ORDER_FORTRAN; }
};

template<>
struct MpiOrder<Kokkos::LayoutRight>
{
    static int value() { return MPI_ORDER_C; }
};

//---------------------------------------------------------------------------//

} // end namespace Cajita

#endif // end CAJITA_MPITRAITS_HPP
