#pragma once

#include "MRMeshFwd.h"
#include "MRProgressCallback.h"
#include <filesystem>
#include <tl/expected.hpp>

namespace MR
{

/// \ingroup DataModelGroup
/// \{

/// loads mesh from given file in new object
MRMESH_API tl::expected<ObjectMesh, std::string> makeObjectMeshFromFile( const std::filesystem::path& file, ProgressCallback callback = {} );

/// loads lines from given file in new object
MRMESH_API tl::expected<ObjectLines, std::string> makeObjectLinesFromFile( const std::filesystem::path& file, ProgressCallback callback = {} );

/// loads points from given file in new object
MRMESH_API tl::expected<ObjectPoints, std::string> makeObjectPointsFromFile( const std::filesystem::path& file, ProgressCallback callback = {} );

/// loads distance map from given file in new object
MRMESH_API tl::expected<ObjectDistanceMap, std::string> makeObjectDistanceMapFromFile( const std::filesystem::path& file, ProgressCallback callback = {} );

#if !defined(__EMSCRIPTEN__) && !defined(MRMESH_NO_VOXEL)
/// loads voxels from given file in new object
MRMESH_API tl::expected<std::vector<std::shared_ptr<ObjectVoxels>>, std::string> makeObjectVoxelsFromFile( const std::filesystem::path& file, ProgressCallback callback = {} );
#endif

/**
 * \brief load object (mesh, lines, points, voxels or scene) from file
 * \param callback - callback function to set progress (for progress bar)
 * \return empty string if no error or error text
 */
MRMESH_API tl::expected<std::vector<std::shared_ptr<Object>>, std::string> loadObjectFromFile( const std::filesystem::path& filename,
                                                                                               ProgressCallback callback = {} );

// check if there are any supported files folder and subfolders
MRMESH_API bool isSupportedFileInSubfolders( const std::filesystem::path& folder );

/// loads meshes from given folder in new container object
MRMESH_API tl::expected<Object, std::string> makeObjectTreeFromFolder( const std::filesystem::path& folder, ProgressCallback callback = {} );

//tries to load scene from every format listed in SceneFormatFilters
MRMESH_API tl::expected<std::shared_ptr<Object>, std::string> loadSceneFromAnySupportedFormat( const std::filesystem::path& path, ProgressCallback callback = {} );

/// \}

} // namespace MR
