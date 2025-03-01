#include "MRCudaContoursDistanceMap.h"
#include "MRMesh/MRAABBTreePolyline.h"
#include "MRCudaContoursDistanceMap.cuh"

namespace MR
{

namespace Cuda
{

DistanceMap distanceMapFromContours( const MR::Polyline2& polyline, const ContourToDistanceMapParams& params )
{
    const auto& tree = polyline.getAABBTree();
    const auto& nodes = tree.nodes();
    const auto& edges = polyline.topology.edges();

    cudaSetDevice( 0 );
    const size_t size = size_t( params.resolution.x ) * params.resolution.y;

    DynamicArray<float2> cudaPts;
    cudaPts.fromVector( polyline.points.vec_ );

    DynamicArray<Node2> cudaNodes;
    cudaNodes.fromVector( nodes.vec_ );

    DynamicArray<PolylineHalfEdgeRecord> cudaEdges;
    cudaEdges.fromVector( edges.vec_ );

    DynamicArray<float> cudaRes( size );

    // kernel
    contoursDistanceMapProjectionKernel( 
        { params.orgPoint.x + params.pixelSize.x * 0.5f, params.orgPoint.y + params.pixelSize.y * 0.5f }, 
        { params.resolution.x, params.resolution.y }, 
        { params.pixelSize.x, params.pixelSize.y }, 
        cudaNodes.data(), cudaPts.data(), cudaEdges.data(), cudaRes.data(), size );
    
    DistanceMap res( params.resolution.x, params.resolution.y );
    std::vector<float> vec( size );
    cudaRes.toVector( vec );
    res.set( std::move( vec ) );

    return res;
}

}

}