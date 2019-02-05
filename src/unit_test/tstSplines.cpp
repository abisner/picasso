#include <Harlow_Splines.hpp>
#include <Harlow_Types.hpp>

#include <vector>

#include <gtest/gtest.h>

using namespace Harlow;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//
namespace Test {

class harlow_splines : public ::testing::Test {
protected:
  static void SetUpTestCase() {
  }

  static void TearDownTestCase() {
  }
};

TEST_F( harlow_splines, linear_spline_test )
{
    // Check partition of unity for the linear spline.
    double xp = -1.4;
    double low_x = -3.43;
    double dx = 0.27;
    double rdx = 1.0 / dx;
    double values[2];

    double x0 = Spline<SplineOrder::Linear>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Linear>::value( x0, values );
    double sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = 2.1789;
    x0 = Spline<SplineOrder::Linear>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Linear>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = low_x + 5 * dx;
    x0 = Spline<SplineOrder::Linear>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Linear>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    // Check the stencil by putting a point in the center of a primal cell.
    int cell_id = 4;
    xp = low_x + (cell_id + 0.5) * dx;
    x0 = Spline<SplineOrder::Linear>::mapToLogicalGrid( xp, rdx, low_x );
    int offsets[2];
    Spline<SplineOrder::Linear>::stencil( offsets );
    EXPECT_EQ( int(x0) + offsets[0], cell_id );
    EXPECT_EQ( int(x0) + offsets[1], cell_id + 1);
}

TEST_F( harlow_splines, quadratic_spline_test )
{
    // Check partition of unity for the quadratic spline.
    double xp = -1.4;
    double low_x = -3.43;
    double dx = 0.27;
    double rdx = 1.0 / dx;
    double values[3];

    double x0 = Spline<SplineOrder::Quadratic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Quadratic>::value( x0, values );
    double sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = 2.1789;
    x0 = Spline<SplineOrder::Quadratic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Quadratic>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = low_x + 5 * dx;
    x0 = Spline<SplineOrder::Quadratic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Quadratic>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    // Check the stencil by putting a point in the center of a dual cell (on a
    // node).
    int node_id = 4;
    xp = low_x + node_id * dx;
    x0 = Spline<SplineOrder::Quadratic>::mapToLogicalGrid( xp, rdx, low_x );
    int offsets[3];
    Spline<SplineOrder::Quadratic>::stencil( offsets );
    EXPECT_EQ( int(x0) + offsets[0], node_id - 1);
    EXPECT_EQ( int(x0) + offsets[1], node_id);
    EXPECT_EQ( int(x0) + offsets[2], node_id + 1);
}

TEST_F( harlow_splines, cubic_spline_test )
{
    // Check partition of unity for the cubic spline.
    double xp = -1.4;
    double low_x = -3.43;
    double dx = 0.27;
    double rdx = 1.0 / dx;
    double values[4];

    double x0 = Spline<SplineOrder::Cubic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Cubic>::value( x0, values );
    double sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = 2.1789;
    x0 = Spline<SplineOrder::Cubic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Cubic>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    xp = low_x + 5 * dx;
    x0 = Spline<SplineOrder::Cubic>::mapToLogicalGrid( xp, rdx, low_x );
    Spline<SplineOrder::Cubic>::value( x0, values );
    sum = 0.0;
    for ( auto x : values ) sum += x;
    EXPECT_DOUBLE_EQ( sum, 1.0 );

    // Check the stencil by putting a point in the center of a primal cell.
    int cell_id = 4;
    xp = low_x + (cell_id + 0.5) * dx;
    x0 = Spline<SplineOrder::Cubic>::mapToLogicalGrid( xp, rdx, low_x );
    int offsets[4];
    Spline<SplineOrder::Cubic>::stencil( offsets );
    EXPECT_EQ( int(x0) + offsets[0], cell_id - 1 );
    EXPECT_EQ( int(x0) + offsets[1], cell_id );
    EXPECT_EQ( int(x0) + offsets[2], cell_id + 1 );
    EXPECT_EQ( int(x0) + offsets[3], cell_id + 2 );
}

} // end namespace Test