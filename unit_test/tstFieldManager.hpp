#include <Picasso_FieldManager.hpp>
#include <Picasso_FieldTypes.hpp>
#include <Picasso_InputParser.hpp>
#include <Picasso_Types.hpp>

#include <Cajita.hpp>

#include <Kokkos_Core.hpp>

#include <gtest/gtest.h>

using namespace Picasso;

namespace Test
{
//---------------------------------------------------------------------------//
template<class View, class IndexSpace>
void checkGather( const View& view,
                  const IndexSpace& index_space,
                  const double value )
{
    auto host_view = Kokkos::create_mirror_view_and_copy(
        Kokkos::HostSpace(), view );
    for( int i = 0; i < index_space.max(Dim::I); ++i )
        for( int j = 0; j < index_space.max(Dim::J); ++j )
            for( int k = 0; k < index_space.max(Dim::K); ++k )
                EXPECT_EQ( host_view(i,j,k,0), value );
}

//---------------------------------------------------------------------------//
void uniformTest()
{
    // Get inputs for mesh.
    InputParser parser( "uniform_mesh_test_1.json", "json" );
    Kokkos::Array<double,6> global_box = { -10.0, -10.0, -10.0,
                                           10.0, 10.0, 10.0 };
    int minimum_halo_size = 0;

    // Make mesh.
    auto mesh = std::make_shared<UniformMesh<TEST_MEMSPACE>>(
        parser.propertyTree(),
        global_box, minimum_halo_size, MPI_COMM_WORLD );

    // Make a field Manager
    FieldManager<UniformMesh<TEST_MEMSPACE> > fm( mesh );

    // Add some fields. Add the same field so we can insure the variants are
    // loaded into different maps.
    fm.add( FieldLocation::Node(), Field::Density() );
    fm.add( FieldLocation::Cell(), Field::Density() );
    fm.add( FieldLocation::Face<Dim::I>(), Field::Density() );
    fm.add( FieldLocation::Face<Dim::J>(), Field::Density() );
    fm.add( FieldLocation::Face<Dim::K>(), Field::Density() );
    fm.add( FieldLocation::Edge<Dim::I>(), Field::Density() );
    fm.add( FieldLocation::Edge<Dim::J>(), Field::Density() );
    fm.add( FieldLocation::Edge<Dim::K>(), Field::Density() );

    // Put data in the fields.
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Node(),Field::Density()), 1.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Cell(),Field::Density()), 2.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Face<Dim::I>(),Field::Density()),
        3.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Face<Dim::J>(),Field::Density()),
        4.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Face<Dim::K>(),Field::Density()),
        5.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Edge<Dim::I>(),Field::Density()),
        6.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Edge<Dim::J>(),Field::Density()),
        7.0, Cajita::Own() );
    Cajita::ArrayOp::assign(
        *fm.array(FieldLocation::Edge<Dim::K>(),Field::Density()),
        8.0, Cajita::Own() );

    // Gather into the ghost zones.
    fm.gather( FieldLocation::Node(), Field::Density() );
    fm.gather( FieldLocation::Cell(), Field::Density() );
    fm.gather( FieldLocation::Face<Dim::I>(), Field::Density() );
    fm.gather( FieldLocation::Face<Dim::J>(), Field::Density() );
    fm.gather( FieldLocation::Face<Dim::K>(), Field::Density() );
    fm.gather( FieldLocation::Edge<Dim::I>(), Field::Density() );
    fm.gather( FieldLocation::Edge<Dim::J>(), Field::Density() );
    fm.gather( FieldLocation::Edge<Dim::K>(), Field::Density() );

    // Check the gather.
    checkGather( fm.view(FieldLocation::Node(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Node(),Cajita::Local()),
                 1.0 );
    checkGather( fm.view(FieldLocation::Cell(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Cell(),Cajita::Local()),
                 2.0 );
    checkGather( fm.view(FieldLocation::Face<Dim::I>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Face<Dim::I>(),Cajita::Local()),
                 3.0 );
    checkGather( fm.view(FieldLocation::Face<Dim::J>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Face<Dim::J>(),Cajita::Local()),
                 4.0 );
    checkGather( fm.view(FieldLocation::Face<Dim::K>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Face<Dim::K>(),Cajita::Local()),
                 5.0 );
    checkGather( fm.view(FieldLocation::Edge<Dim::I>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Edge<Dim::I>(),Cajita::Local()),
                 6.0 );
    checkGather( fm.view(FieldLocation::Edge<Dim::J>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Edge<Dim::J>(),Cajita::Local()),
                 7.0 );
    checkGather( fm.view(FieldLocation::Edge<Dim::K>(),Field::Density()),
                 mesh->localGrid()->indexSpace(
                     Cajita::Ghost(),Cajita::Edge<Dim::K>(),Cajita::Local()),
                 8.0 );

    // Scatter back to owned.
    fm.scatter( FieldLocation::Node(), Field::Density() );
    fm.scatter( FieldLocation::Cell(), Field::Density() );
    fm.scatter( FieldLocation::Face<Dim::I>(), Field::Density() );
    fm.scatter( FieldLocation::Face<Dim::J>(), Field::Density() );
    fm.scatter( FieldLocation::Face<Dim::K>(), Field::Density() );
    fm.scatter( FieldLocation::Edge<Dim::I>(), Field::Density() );
    fm.scatter( FieldLocation::Edge<Dim::J>(), Field::Density() );
    fm.scatter( FieldLocation::Edge<Dim::K>(), Field::Density() );
}

//---------------------------------------------------------------------------//
void adaptiveTest()
{
    // Get inputs for mesh.
    InputParser parser( "adaptive_mesh_test_1.json", "json" );
    Kokkos::Array<double,6> global_box = { -10.0, -10.0, -10.0,
                                           10.0, 10.0, 10.0 };
    int minimum_halo_size = 1;
    double cell_size = 0.5;

    // Make mesh.
    auto mesh = std::make_shared<AdaptiveMesh<TEST_MEMSPACE>>(
        parser.propertyTree(),
        global_box, minimum_halo_size, MPI_COMM_WORLD, TEST_EXECSPACE() );

    // Make a field Manager
    FieldManager<AdaptiveMesh<TEST_MEMSPACE> > fm( mesh );

    // Check that the adaptive mesh coordinates got loaded.
    auto nodes = fm.array( FieldLocation::Node(), Field::PhysicalPosition() );
    auto host_coords = Kokkos::create_mirror_view_and_copy(
        Kokkos::HostSpace(), nodes->view() );
    auto global_space = nodes->layout()->localGrid()->indexSpace(
        Cajita::Ghost(), Cajita::Node(), Cajita::Global() );
    auto local_space = nodes->layout()->localGrid()->indexSpace(
        Cajita::Ghost(), Cajita::Node(), Cajita::Local() );
    for ( int i = local_space.min(0); i < local_space.max(0); ++i )
        for ( int j = local_space.min(1); j < local_space.max(1); ++j )
            for ( int k = local_space.min(2); k < local_space.max(2); ++k )
            {
                auto ig = global_space.min(0) + i;
                auto jg = global_space.min(1) + j;
                auto kg = global_space.min(2) + k;
                EXPECT_EQ( host_coords(i,j,k,0),
                           global_box[0] + ig*cell_size );
                EXPECT_EQ( host_coords(i,j,k,1),
                           global_box[1] + (jg-1)*cell_size );
                EXPECT_EQ( host_coords(i,j,k,2),
                           global_box[2] + kg*cell_size );
            }
}

//---------------------------------------------------------------------------//
// RUN TESTS
//---------------------------------------------------------------------------//
TEST( TEST_CATEGORY, uniform_test )
{
    uniformTest();
}

TEST( TEST_CATEGORY, adaptive_test )
{
    adaptiveTest();
}

//---------------------------------------------------------------------------//

} // end namespace Test