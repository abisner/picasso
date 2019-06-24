#ifndef CAJITA_TYPES_HPP
#define CAJITA_TYPES_HPP

namespace Cajita
{

//---------------------------------------------------------------------------//
// Logical dimension index.
//---------------------------------------------------------------------------//
struct Dim
{
    enum Values {
        I = 0,
        J = 1,
        K = 2
    };
};

//---------------------------------------------------------------------------//
// Entity type tags.
//---------------------------------------------------------------------------//

// Mesh cell tag.
struct Cell {};

// Mesh node tag.
struct Node {};

// Mesh face tags.
template<int D>
struct Face;

// I-face tag.
template<>
struct Face<Dim::I> {};

// J-face tag.
template<>
struct Face<Dim::J> {};

// K-face tag.
template<>
struct Face<Dim::K> {};

//---------------------------------------------------------------------------//
// Decomposition tags.
//---------------------------------------------------------------------------//

// Owned decomposition tag.
struct Own {};

// Ghosted decomposition tag.
struct Ghost {};

//---------------------------------------------------------------------------//
// Index type tags.
//---------------------------------------------------------------------------//

// Local index tag.
struct Local {};

// Global index tag.
struct Global {};

//---------------------------------------------------------------------------//

} // end namespace Cajita

#endif // CAJITA_TYPES_HPP
