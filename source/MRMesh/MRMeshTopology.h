#pragma once

#include "MRId.h"
#include "MRVector.h"
#include "MRBitSet.h"
#include "MRPartMapping.h"
#include <fstream>

namespace MR
{

class MeshTopology
{
public:
    // creates an edge not associated with any vertex or face
    MRMESH_API EdgeId makeEdge();
    // checks whether the edge is disconnected from all other edges and disassociated from all vertices and faces (as if after makeEdge)
    MRMESH_API bool isLoneEdge( EdgeId a ) const;
    // returns last not lone edge id, or invalid id if no such edge exists
    MRMESH_API EdgeId lastNotLoneEdge() const;
    // returns the number of half-edge records including lone ones
    size_t edgeSize() const { return edges_.size(); }
    // returns the number of undirected edges (pairs of half-edges) including lone ones
    size_t undirectedEdgeSize() const { return edges_.size() >> 1; }
    // computes the number of not-lone (valid) undirected edges
    MRMESH_API size_t computeNotLoneUndirectedEdges() const;
    // sets the capacity of half-edges vector
    void edgeReserve( size_t newCapacity ) { edges_.reserve( newCapacity ); }
    // returns true if given edge is within valid range and not-lone
    bool hasEdge( EdgeId e ) const { assert( e.valid() ); return e < (int)edgeSize() && !isLoneEdge( e ); }

    // given two half edges do either of two:
    // 1) if a and b were from distinct rings, puts them in one ring;
    // 2) if a and b were from the same ring, puts them in separate rings;
    // the cut in rings in both cases is made after a and b
    MRMESH_API void splice( EdgeId a, EdgeId b );
    
    // next (counter clock wise) half-edge in the origin ring
    EdgeId next( EdgeId he ) const { assert(he.valid()); return edges_[he].next; }
    // previous (clock wise) half-edge in the origin ring
    EdgeId prev( EdgeId he ) const { assert(he.valid()); return edges_[he].prev; }
    // returns origin vertex of half-edge
    VertId org( EdgeId he ) const { assert(he.valid()); return edges_[he].org; }
    // returns destination vertex of half-edge
    VertId dest( EdgeId he ) const { assert(he.valid()); return edges_[he.sym()].org; }
    // returns left face of half-edge
    FaceId left( EdgeId he ) const { assert(he.valid()); return edges_[he].left; }
    // returns right face of half-edge
    FaceId right( EdgeId he ) const { assert(he.valid()); return edges_[he.sym()].left; }

    // sets new origin to the full origin ring including this edge;
    // edgePerVertex_ table is updated accordingly
    MRMESH_API void setOrg( EdgeId a, VertId v );
    // sets new left face to the full left ring including this edge;
    // edgePerFace_ table is updated accordingly
    MRMESH_API void setLeft( EdgeId a, FaceId f );

    // returns true if a and b are both from the same origin ring
    MRMESH_API bool fromSameOriginRing( EdgeId a, EdgeId b ) const;
    // returns true if a and b are both from the same left face ring
    MRMESH_API bool fromSameLeftRing( EdgeId a, EdgeId b ) const;

    // returns the number of edges around the left face: 3 for triangular faces, ...
    MRMESH_API int getLeftDegree( EdgeId a ) const;
    // returns the number of edges around the given face: 3 for triangular faces, ...
    int getFaceDegree( FaceId f ) const { return getLeftDegree( edgeWithLeft( f ) ); }
    // returns true if the cell to the left of a is triangular
    MRMESH_API bool isLeftTri( EdgeId a ) const;
    // gets 3 vertices of given triangular face;
    // the vertices are returned in counter-clockwise order if look from mesh outside
    void getTriVerts( FaceId f, VertId & v0, VertId & v1, VertId & v2 ) const { getLeftTriVerts( edgeWithLeft( f ), v0, v1, v2 ); }
    void getTriVerts( FaceId f, VertId (&v)[3] ) const { getTriVerts( f, v[0], v[1], v[2] ); }
    // gets 3 vertices of the left face ( face-id may not exist, but the shape must be triangular)
    // the vertices are returned in counter-clockwise order if look from mesh outside
    MRMESH_API void getLeftTriVerts( EdgeId a, VertId & v0, VertId & v1, VertId & v2 ) const;
               void getLeftTriVerts( EdgeId a, VertId (&v)[3] ) const { getLeftTriVerts( a, v[0], v[1], v[2] ); }
    // returns true if the cell to the left of a is quadrangular
    MRMESH_API bool isLeftQuad( EdgeId a ) const;

    // for all valid vertices this vector contains an edge with the origin there
    const Vector<EdgeId, VertId> & edgePerVertex() const { return edgePerVertex_; }
    // returns valid edge if given vertex is present in the mesh
    EdgeId edgeWithOrg( VertId a ) const { assert( a.valid() ); return a < int(edgePerVertex_.size()) && edgePerVertex_[a].valid() ? edgePerVertex_[a] : EdgeId(); }
    // returns true if given vertex is present in the mesh
    bool hasVert( VertId a ) const { return validVerts_.test( a ); }
    // returns the number of valid vertices
    int numValidVerts() const { return numValidVerts_; }
    // returns last valid vertex id, or invalid id if no single valid vertex exists
    MRMESH_API VertId lastValidVert() const;
    // creates new vert-id not associated with any edge yet
    VertId addVertId() { edgePerVertex_.push_back( {} ); validVerts_.push_back( false ); return VertId( (int)edgePerVertex_.size() - 1 ); }
    // explicitly increases the size of verts vector
    void vertResize( size_t newSize ) { if ( edgePerVertex_.size() < newSize ) { edgePerVertex_.resize( newSize ); validVerts_.resize( newSize ); } }
    // sets the capacity of verts vector
    void vertReserve( size_t newCapacity ) { edgePerVertex_.reserve( newCapacity ); validVerts_.reserve( newCapacity ); }
    // returns the number of vertex records including invalid ones
    size_t vertSize() const { return edgePerVertex_.size(); }
     // returns cached set of all valid vertices
    const VertBitSet & getValidVerts() const { return validVerts_; }
    // if region pointer is not null then converts it in reference, otherwise returns all valid vertices in the mesh
    const VertBitSet & getVertIds( const VertBitSet * region ) const { return region ? *region : validVerts_; }

    // for all valid faces this vector contains an edge with that face at left
    const Vector<EdgeId, FaceId> & edgePerFace() const { return edgePerFace_; }
    // returns valid edge if given vertex is present in the mesh
    EdgeId edgeWithLeft( FaceId a ) const { assert( a.valid() ); return a < int(edgePerFace_.size()) && edgePerFace_[a].valid() ? edgePerFace_[a] : EdgeId(); }
    // returns true if given face is present in the mesh
    bool hasFace( FaceId a ) const { return validFaces_.test( a ); }
    // if two valid faces share the same edge then it is found and returned
    MRMESH_API EdgeId sharedEdge( FaceId l, FaceId r ) const;
    // if two valid faces share the same vertex then it is found and returned as Edge with this vertex in origin
    MRMESH_API EdgeId sharedVertInOrg( FaceId l, FaceId r ) const;
    // returns the number of valid faces
    int numValidFaces() const{ return numValidFaces_; }
    // returns last valid face id, or invalid id if no single valid face exists
    MRMESH_API FaceId lastValidFace() const;
    // creates new face-id not associated with any edge yet
    FaceId addFaceId() { edgePerFace_.push_back( {} ); validFaces_.push_back( false ); return FaceId( (int)edgePerFace_.size() - 1 ); }
    // explicitly increases the size of faces vector
    void faceResize( size_t newSize ) { if ( edgePerFace_.size() < newSize ) { edgePerFace_.resize( newSize ); validFaces_.resize( newSize ); } }
    // sets the capacity of faces vector
    void faceReserve( size_t newCapacity ) { edgePerFace_.reserve( newCapacity ); validFaces_.reserve( newCapacity ); }
    // returns the number of face records including invalid ones
    size_t faceSize() const { return edgePerFace_.size(); }
    // returns cached set of all valid faces
    const FaceBitSet & getValidFaces() const { return validFaces_; }
    // if region pointer is not null then converts it in reference, otherwise returns all valid faces in the mesh
    const FaceBitSet & getFaceIds( const FaceBitSet * region ) const { return region ? *region : validFaces_; }

    // return true if left face of given edge belongs to region (or just have valid id if region is nullptr)
    bool isLeftInRegion( EdgeId e, const FaceBitSet * region = nullptr ) const { return contains( region, left( e ) ); }
    // return true if given edge is inner for given region (or for whole mesh if region is nullptr)
    bool isInnerEdge( EdgeId e, const FaceBitSet * region = nullptr ) const { return isLeftInRegion( e, region ) && isLeftInRegion( e.sym(), region ); }
    // return true if given edge is boundary for given region (or for whole mesh if region is nullptr)
    bool isBdEdge( EdgeId e, const FaceBitSet * region = nullptr ) const { return isLeftInRegion( e, region ) != isLeftInRegion( e.sym(), region ); }
    // returns the first boundary edge (for given region or for whole mesh if region is nullptr) in counter-clockwise order starting from given edge with the same origin;
    // returns invalid edge if no boundary edge is found
    MRMESH_API EdgeId bdEdgeSameOrigin( EdgeId e, const FaceBitSet * region = nullptr ) const;
    // returns true if edge's origin is on (region) boundary
    bool isBdVertexInOrg( EdgeId e, const FaceBitSet * region = nullptr ) const { return bdEdgeSameOrigin( e, region ).valid(); }
    // returns a boundary edge with given vertex in origin considering boundary of given region (or for whole mesh if region is nullptr);
    // returns invalid edge if no boundary edge is found
    EdgeId bdEdgeWithOrigin( VertId v, const FaceBitSet * region = nullptr ) const { return bdEdgeSameOrigin( edgeWithOrg( v ), region ); }
    // returns true if given vertex is on (region) boundary
    bool isBdVertex( VertId v, const FaceBitSet * region = nullptr ) const { return isBdVertexInOrg( edgeWithOrg( v ), region ); }
    // returns true if left face of given edge belongs to given region (if provided) and right face either does not exist or does not belong
    bool isLeftBdEdge( EdgeId e, const FaceBitSet * region = nullptr ) const { return region ? ( isLeftInRegion( e, region ) && !isLeftInRegion( e.sym(), region ) ) : !right( e ); }
    // return true if given edge is inner or boundary for given region (or for whole mesh if region is nullptr)
    bool isInnerOrBdEdge( EdgeId e, const FaceBitSet * region = nullptr ) const { return region ? ( isLeftInRegion( e, region ) || isLeftInRegion( e.sym(), region ) ) : true; }

    // finds and returns edge from o to d in the mesh; returns invalid edge otherwise
    MRMESH_API EdgeId findEdge( VertId o, VertId d ) const;
    // returns true if the mesh does not have any holes
    MRMESH_API bool isClosed() const;
    // returns true if the mesh region does not have any neighboring holes
    MRMESH_API bool isClosed( const FaceBitSet * region ) const;
    // returns closed loop of boundary edges starting from given boundary edge, 
    // which has region face to the right and does not have valid or in-region left face;
    // unlike MR::trackRegionBoundaryLoop this method returns loops in opposite orientation
    MRMESH_API EdgeLoop trackBoundaryLoop( EdgeId e0, const FaceBitSet * region = nullptr ) const;
    // returns all boundary loops, where each edge has region face to the right and does not have valid or in-region left face;
    // unlike MR::findRegionBoundary this method returns loops in opposite orientation
    MRMESH_API std::vector<EdgeLoop> findBoundary( const FaceBitSet * region = nullptr ) const;
    // returns one edge with no valid left face for every boundary in the mesh
    MRMESH_API std::vector<EdgeId> findHoleRepresentiveEdges() const;
    // returns all boundary edges, where each edge does not have valid left face
    MRMESH_API EdgeBitSet findBoundaryEdges() const;
    // returns all boundary faces, having at least one boundary edge
    MRMESH_API FaceBitSet findBoundaryFaces() const;
    // returns all boundary vertices, incident to at least one boundary edge
    MRMESH_API VertBitSet findBoundaryVerts() const;

    // returns all vertices incident to path edges
    MRMESH_API VertBitSet getPathVertices( const EdgePath & path ) const;
    // returns all valid left faces of path edges
    MRMESH_API FaceBitSet getPathLeftFaces( const EdgePath & path ) const;
    // returns all valid right faces of path edges
    MRMESH_API FaceBitSet getPathRightFaces( const EdgePath & path ) const;

    // given the edge with left and right triangular faces, which form together a quadrangle,
    // rotates the edge counter-clockwise inside the quadrangle
    MRMESH_API void flipEdge( EdgeId e );

    // split given edge on two parts, with e pointing on the second part with the same destination vertex but new origin vertex (which is returned);
    // left and right faces if valid are also subdivide by new edge each;
    // if left or right faces of the original edge were in the region, then includes new parts of these faces in the region
    MRMESH_API VertId splitEdge( EdgeId e, FaceBitSet * region = nullptr );

    // split given triangle on three triangles, introducing new vertex (which is returned) inside original triangle and connecting it to its vertices;
    // if region is given, then it must include (f) and new faces will be added there as well
    MRMESH_API VertId splitFace( FaceId f, FaceBitSet * region = nullptr );

    // flip orientation (normals) of all faces
    MRMESH_API void flipOrientation();

    // for each triangle selects edgeWithLeft with minimal origin vertex
    MRMESH_API void rotateTriangles();
    // appends mesh topology (from) in addition to the current topology: creates new edges, faces, verts;
    // if (rearrangeTriangles) then the order of triangles is selected according to the order of their vertices,
    // please call rotateTriangles() first
    MRMESH_API void addPart( const MeshTopology & from,
        // optionally returns mappings: from.id -> this.id
        FaceMap * outFmap = nullptr, VertMap * outVmap = nullptr, EdgeMap * outEmap = nullptr, bool rearrangeTriangles = false );

    // the same but copies only portion of (from) specified by fromFaces,
    MRMESH_API void addPartByMask( const MeshTopology & from, const FaceBitSet & fromFaces, const PartMapping & map = {} );
    [[deprecated]]
    MRMESH_API void addPartByMask( const MeshTopology & from, const FaceBitSet & fromFaces,
        // optionally returns mappings: from.id -> this.id
        FaceMap * outFmap, VertMap * outVmap = nullptr, EdgeMap * outEmap = nullptr );
    // this version has more parameters:
    //   if flipOrientation then every from triangle is inverted before adding
    MRMESH_API void addPartByMask( const MeshTopology & from, const FaceBitSet & fromFaces, bool flipOrientation,
        const std::vector<std::vector<EdgeId>> & thisContours, // contours on this mesh (no left face) that have to be stitched with
        const std::vector<std::vector<EdgeId>> & fromContours, // contours on from mesh during addition (no left face if flipOrientation otherwise no right face)
        const PartMapping & map = {} );
    [[deprecated]]
    MRMESH_API void addPartByMask( const MeshTopology & from, const FaceBitSet & fromFaces, bool flipOrientation,
        const std::vector<std::vector<EdgeId>> & thisContours, // contours on this mesh (no left face) that have to be stitched with
        const std::vector<std::vector<EdgeId>> & fromContours, // contours on from mesh during addition (no left face if flipOrientation otherwise no right face)
        // optionally returns mappings: from.id -> this.id
        FaceMap * outFmap, VertMap * outVmap = nullptr, EdgeMap * outEmap = nullptr );

    // tightly packs all arrays eliminating lone edges and invalid face and verts,
    // optionally returns mappings: old.id -> new.id;
    // if (rearrangeTriangles) then calls rotateTriangles()
    //   and selects the order of triangles according to the order of their vertices
    MRMESH_API void pack( FaceMap * outFmap = nullptr, VertMap * outVmap = nullptr, EdgeMap * outEmap = nullptr, bool rearrangeTriangles = false );

    // saves and loads in binary stream
    MRMESH_API void write( std::ostream & s ) const;
    MRMESH_API bool read( std::istream & s );

    // comparison via edges (all other members are considered as not important caches)
    bool operator ==( const MeshTopology & b ) const { return edges_ == b.edges_; }
    bool operator !=( const MeshTopology & b ) const { return edges_ != b.edges_; }

    // These function are for parallel mesh creation from different threads. If you are not sure, do not use them.
    //
    // resizes all internal vectors and sets the numbers of valid elements in preparation for addPackedPart
    MRMESH_API void resizeBeforeParallelAdd( size_t edgeSize, size_t vertSize, size_t faceSize );
    // copies topology (from) into this;
    // (from) edges must be tightly packes without any lone edges, and they are mapped [0, from.edges.size()) -> [toEdgeId, toEdgeId + from.edges.size());
    // mapping of vertices and faces is given in fmap and vmap
    MRMESH_API void addPackedPart( const MeshTopology & from, EdgeId toEdgeId,
        // given vertices and faces mapping: from.id -> this.id
        const FaceMap & fmap, const VertMap & vmap );
    // after all packed parts have been added, compute 
    // 1) numValidVerts_ and validVerts_ from edgePerVertex_
    // 2) numValidFaces_ and validFaces_ from edgePerFace_
    MRMESH_API void computeValidsFromEdges();

    // verifies that all internal data structures are valid
    MRMESH_API bool checkValidity() const;

private:
    friend class MeshDiff;
    // computes from edges_ all remaining fields:
    // 1) numValidVerts_, 2) validVerts_, 3) edgePerVertex_
    // 4) numValidFaces_, 5) validFaces_, 6) edgePerFace_
    MRMESH_API void computeAllFromEdges_();

private:
    // sets new origin to the full origin ring including this edge, without updating edgePerVertex_ table
    void setOrg_( EdgeId a, VertId v );
    // sets new left face to the full left ring including this edge, without updating edgePerFace_ table
    void setLeft_( EdgeId a, FaceId f );

    // data of every half-edge
    struct HalfEdgeRecord
    {
        EdgeId next; // next counter clock wise half-edge in the origin ring
        EdgeId prev; // next clock wise half-edge in the origin ring
        VertId org;  // vertex at the origin of the edge
        FaceId left; // face at the left of the edge

        bool operator ==( const HalfEdgeRecord & b ) const
            { return next == b.next && prev == b.prev && org == b.org && left == b.left; }
        bool operator !=( const HalfEdgeRecord & b ) const
            { return !( *this == b ); }
    };
    // translates all fields in the record for this edge given maps
    HalfEdgeRecord translate_( EdgeId i, const FaceMap & fmap, const VertMap & vmap, const EdgeMap & emap, bool flipOrientation ) const;
    HalfEdgeRecord translate_( EdgeId i, const FaceHashMap & fmap, const VertHashMap & vmap, const EdgeHashMap & emap, bool flipOrientation ) const;

    // edges_: EdgeId -> edge data
    Vector<HalfEdgeRecord, EdgeId> edges_;

    // edgePerVertex_: VertId -> one edge id of one of edges with origin there
    Vector<EdgeId, VertId> edgePerVertex_;
    VertBitSet validVerts_; // each true bit here corresponds to valid element in edgePerVertex_

    // edgePerFace_: FaceId -> one edge id with this face at left
    Vector<EdgeId, FaceId> edgePerFace_;
    FaceBitSet validFaces_; // each true bit here corresponds to valid element in edgePerFace_

    int numValidVerts_ = 0; // the number of valid elements in edgePerVertex_ or set bits in validVerts_
    int numValidFaces_ = 0; // the number of valid elements in edgePerFace_ or set bits in validFaces_
};

MRMESH_API void loadMeshDll();

} //namespace MR
