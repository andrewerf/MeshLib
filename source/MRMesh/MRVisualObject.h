#pragma once

#include "MRObject.h"
#include "MRMeshTexture.h"
#include "MRVector.h"
#include "MRColor.h"
#include "MRPositionedText.h"
#include "MRIRenderObject.h"

namespace MR
{

/// \defgroup VisualObjectGroup Visual Object group
/// \ingroup DataModelGroup
/// \{

/// Type of mesh coloring,
/// \note that texture are applied over main coloring
enum class ColoringType
{
    SolidColor,   ///< Use simple color for whole mesh
    PrimitivesColorMap, ///< Use different color (taken from faces colormap) for each ptimitive
    FacesColorMap = PrimitivesColorMap, ///< Use different color (taken from faces colormap) for each face (primitive for object mesh)
    LinesColorMap = PrimitivesColorMap, ///< Use different color (taken from faces colormap) for each line (primitive for object lines)
    VertsColorMap  ///< Use different color (taken from verts colormap) for each vert
};

struct VisualizeMaskType
{
    enum Type : unsigned
    {
        Visibility,
        InvertedNormals,
        Name,
        Labels,
        CropLabelsByViewportRect,
        ClippedByPlane,
        DepthTest,

        VisualizePropsCount
    };
};

using AllVisualizeProperties = std::vector<ViewportMask>;

enum DirtyFlags
{
    DIRTY_NONE = 0x0000,
    DIRTY_POSITION = 0x0001,
    DIRTY_UV = 0x0002,
    DIRTY_VERTS_RENDER_NORMAL = 0x0004, //< gl normals
    DIRTY_FACES_RENDER_NORMAL = 0x0008, ///< gl normals
    DIRTY_CORNERS_RENDER_NORMAL = 0x0010, ///< gl normals
    DIRTY_RENDER_NORMALS = DIRTY_VERTS_RENDER_NORMAL | DIRTY_FACES_RENDER_NORMAL | DIRTY_CORNERS_RENDER_NORMAL,
    DIRTY_SELECTION = 0x0020,
    DIRTY_TEXTURE = 0x0040,
    DIRTY_PRIMITIVES = 0x0080,
    DIRTY_FACE = DIRTY_PRIMITIVES,
    DIRTY_BACK_FACES = 0x0100,
    DIRTY_VERTS_COLORMAP = 0x0200,
    DIRTY_PRIMITIVE_COLORMAP = 0x0400,
    DIRTY_FACES_COLORMAP = DIRTY_PRIMITIVE_COLORMAP,
    DIRTY_MESH = 0x07FF,
    DIRTY_BOUNDING_BOX = 0x0800,
    DIRTY_BORDER_LINES = 0x2000,
    DIRTY_EDGES_SELECTION = 0x4000,
    DIRTY_CACHES = DIRTY_BOUNDING_BOX,
    DIRTY_ALL = 0x3FFFF
};

/// Visual Object
class MRMESH_CLASS VisualObject : public Object
{
public:
    MRMESH_API VisualObject();

    VisualObject( VisualObject&& ) = default;
    VisualObject& operator = ( VisualObject&& ) = default;
    virtual ~VisualObject() = default;

    constexpr static const char* TypeName() noexcept { return "VisualObject"; }
    virtual const char* typeName() const override { return TypeName(); }

    /// set visual property in all viewports specified by the mask
    MRMESH_API void setVisualizeProperty( bool value, unsigned type, ViewportMask viewportMask );
    /// set visual property mask
    MRMESH_API virtual void setVisualizePropertyMask( unsigned type, ViewportMask viewportMask );
    /// returns true if the property is set at least in one viewport specified by the mask
    MRMESH_API bool getVisualizeProperty( unsigned type, ViewportMask viewportMask ) const;
    /// returns mask of viewports where given property is set
    MRMESH_API virtual const ViewportMask& getVisualizePropertyMask( unsigned type ) const;
    /// toggle visual property in all viewports specified by the mask
    MRMESH_API void toggleVisualizeProperty( unsigned type, ViewportMask viewportMask );

    /// get all visualize properties masks as array
    MRMESH_API virtual AllVisualizeProperties getAllVisualizeProperties() const;
    /// set all visualize properties masks from array
    MRMESH_API virtual void setAllVisualizeProperties( const AllVisualizeProperties& properties );

    /// shows/hides labels
    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    void showLabels( bool on ) { return setVisualizeProperty( on, unsigned( VisualizeMaskType::Labels ), ViewportMask::all() ); }
    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    bool showLabels() const { return getVisualizeProperty( unsigned( VisualizeMaskType::Labels ), ViewportMask::any() ); }

    /// shows/hides object name in all viewports
    void showName( bool on ) { return setVisualizeProperty( on, unsigned( VisualizeMaskType::Name ), ViewportMask::all() ); }
    /// returns whether object name is shown in any viewport
    bool showName() const { return getVisualizeProperty( unsigned( VisualizeMaskType::Name ), ViewportMask::any() ); }

    /// returns color of object when it is selected/not-selected (depending on argument) in given viewport
    MRMESH_API const Color& getFrontColor( bool selected = true, ViewportId viewportId = {} ) const;
    /// sets color of object when it is selected/not-selected (depending on argument) in given viewport
    MRMESH_API virtual void setFrontColor( const Color& color, bool selected, ViewportId viewportId = {} );

    /// returns color of object when it is selected/not-selected (depending on argument) in all viewports
    MRMESH_API const ViewportProperty<Color>& getFrontColorsForAllViewports( bool selected = true ) const;
    /// sets color of object when it is selected/not-selected (depending on argument) in all viewports
    MRMESH_API virtual void setFrontColorsForAllViewports( ViewportProperty<Color> val, bool selected = true );

    /// returns backward color of object in all viewports
    MRMESH_API const ViewportProperty<Color>& getBackColorsForAllViewports() const;
    /// sets backward color of object in all viewports
    MRMESH_API virtual void setBackColorsForAllViewports( ViewportProperty<Color> val );

    /// returns backward color of object in given viewport
    MRMESH_API const Color& getBackColor( ViewportId viewportId = {} ) const;
    /// sets backward color of object in given viewport
    MRMESH_API virtual void setBackColor( const Color& color, ViewportId viewportId = {} );

    /// returns global transparency alpha of object in given viewport
    MRMESH_API const uint8_t& getGlobalAlpha( ViewportId viewportId = {} ) const;
    /// sets global transparency alpha of object in given viewport
    MRMESH_API virtual void setGlobalAlpha( uint8_t alpha, ViewportId viewportId = {} );

    /// returns global transparency alpha of object in all viewports
    MRMESH_API const ViewportProperty<uint8_t>& getGlobalAlphaForAllViewports() const;
    /// sets global transparency alpha of object in all viewports
    MRMESH_API virtual void setGlobalAlphaForAllViewports( ViewportProperty<uint8_t> val );

    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    MRMESH_API const Color& getLabelsColor( ViewportId viewportId = {} ) const;
    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    MRMESH_API virtual void setLabelsColor( const Color& color, ViewportId viewportId = {} );

    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    MRMESH_API const ViewportProperty<Color>& getLabelsColorsForAllViewports() const;

    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    MRMESH_API virtual void setLabelsColorsForAllViewports( ViewportProperty<Color> val );

    /// sets some dirty flags for the object (to force its visual update)
    /// \param mask is a union of DirtyFlags flags
    MRMESH_API virtual void setDirtyFlags( uint32_t mask );
    /// returns current dirty flags for the object
    MRMESH_API const uint32_t& getDirtyFlags() const;
    /// resets all dirty flags (except for cache flags that will be reset automatically on cache update)
    MRMESH_API void resetDirty() const;

    /// returns bounding box of this object in local coordinates
    MRMESH_API Box3f getBoundingBox() const;
    /// returns bounding box of this object in given viewport in world coordinates,
    /// to get world bounding box of the object with all child objects, please call Object::getWorldTreeBox method
    MRMESH_API virtual Box3f getWorldBox( ViewportId = {} ) const override;
    /// returns true if the object must be redrawn (due to dirty flags) in one of specified viewports
    virtual bool getRedrawFlag( ViewportMask viewportMask ) const override 
    {
        return Object::getRedrawFlag( viewportMask ) || 
            ( isVisible( viewportMask ) &&
              ( dirty_ & ( ~( DIRTY_CACHES ) ) ) );
    }

    /// whether the object can be picked (by mouse) in any of given viewports
    bool isPickable( ViewportMask viewportMask = ViewportMask::any() ) const{return !(pickable_ & viewportMask).empty();}
    /// sets the object as can/cannot be picked (by mouse) in all of given viewports
    MRMESH_API virtual void setPickable( bool on, ViewportMask viewportMask = ViewportMask::all() );

    /// returns per-vertex colors of the object
    const Vector<Color, VertId>& getVertsColorMap() const { return vertsColorMap_; }
    /// sets per-vertex colors of the object
    virtual void setVertsColorMap( Vector<Color, VertId> vertsColorMap ) { vertsColorMap_ = std::move( vertsColorMap ); dirty_ |= DIRTY_VERTS_COLORMAP; }
    /// swaps per-vertex colors of the object with given argument
    virtual void updateVertsColorMap( Vector<Color, VertId>& vertsColorMap ) { std::swap( vertsColorMap_, vertsColorMap ); dirty_ |= DIRTY_VERTS_COLORMAP; }

    /// returns the current coloring mode of the object
    ColoringType getColoringType() const { return coloringType_; }
    /// sets coloring mode of the object with given argument
    MRMESH_API virtual void setColoringType( ColoringType coloringType );

    /// returns the current shininess visual value
    float getShininess() const { return shininess_; }
    /// sets shininess visual value of the object with given argument
    virtual void setShininess( float shininess ) { shininess_ = shininess; needRedraw_ = true; }

    /// returns intensity of reflections
    float getSpecularStrength() const { return specularStrength_; }
    /// sets intensity of reflections
    virtual void setSpecularStrength( float specularStrength ) { specularStrength_ = specularStrength; needRedraw_ = true; }

    /// returns intensity of non-directional light
    float getAmbientStrength() const { return ambientStrength_; }
    /// sets intensity of non-directional light
    virtual void setAmbientStrength( float ambientStrength ) { ambientStrength_ = ambientStrength; needRedraw_ = true; }

    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    const std::vector<PositionedText>& getLabels() const { return labels_; }
    [[deprecated( "please use ObjectLabel mechanism instead" )]]
    virtual void setLabels( std::vector<PositionedText> labels ) { labels_ = std::move( labels ); needRedraw_ = true; }

    /// clones this object only, without its children,
    /// making new object the owner of all copied resources
    MRMESH_API virtual std::shared_ptr<Object> clone() const override;
    /// clones this object only, without its children,
    /// making new object to share resources with this object
    MRMESH_API virtual std::shared_ptr<Object> shallowClone() const override;

    /// draws this object for visualization
    MRMESH_API virtual void render( const RenderParams& ) const;
    /// draws this object for picking
    MRMESH_API virtual void renderForPicker( const BaseRenderParams&, unsigned ) const;
    // Binds all graphical data to GPU and allows to clear this object CPU representation to free memory
    // note that object without CPU model is not really valid and the model will be lost on serialization, also some caches will lost with CPU model
    MRMESH_API virtual void bindAllVisualization() const;

    /// is object has any visual representation (faces, edges, etc.)
    virtual bool hasVisualRepresentation() const { return false; }

    /// this ctor is public only for std::make_shared used inside clone()
    VisualObject( ProtectedStruct, const VisualObject& obj ) : VisualObject( obj ) {}

    /// returns the amount of memory this object occupies on heap
    [[nodiscard]] MRMESH_API virtual size_t heapBytes() const override;
    /// return several info lines that can better describe the object in the UI
    MRMESH_API virtual std::vector<std::string> getInfoLines() const override;

protected:

    MRMESH_API VisualObject( const VisualObject& obj );

    /// swaps this object with other
    MRMESH_API virtual void swapBase_( Object& other ) override;

    /// each renderable child of VisualObject should imlpement this method
    /// and assign renderObj_ inside
    virtual void setupRenderObject_() const {}
    mutable std::unique_ptr<IRenderObject> renderObj_;

    /// Visualization options
    /// Each option is a binary mask specifying on which viewport each option is set.
    /// When using a single viewport, standard boolean can still be used for simplicity.
    ViewportMask clipByPlane_;
    ViewportMask showLabels_;
    ViewportMask showName_;
    ViewportMask cropLabels_ = ViewportMask::all(); 
    ViewportMask pickable_ = ViewportMask::all(); ///< enable picking by gl    
    ViewportMask invertNormals_; ///< invert mesh normals
    ViewportMask depthTest_ = ViewportMask::all();

    ViewportProperty<Color> labelsColor_ = {};

    float shininess_{35.0f}; ///< specular exponent
    float specularStrength_{ 0.5f }; // reflection intensity
    float ambientStrength_{ 0.1f }; //non - directional light intensity

    /// Main coloring options
    ColoringType coloringType_{ColoringType::SolidColor};
    Vector<Color, VertId> vertsColorMap_;
    ViewportProperty<Color> selectedColor_;
    ViewportProperty<Color> unselectedColor_;
    ViewportProperty<Color> backFacesColor_;
    ViewportProperty<uint8_t> globalAlpha_{ 255 };

    std::vector<PositionedText> labels_;

    MRMESH_API ViewportMask& getVisualizePropertyMask_( unsigned type );

    /// Marks dirty buffers that need to be uploaded to OpenGL
    mutable uint32_t dirty_{DIRTY_ALL};

    MRMESH_API virtual void serializeFields_( Json::Value& root ) const override;

    MRMESH_API void deserializeFields_( const Json::Value& root ) override;

    virtual Box3f computeBoundingBox_() const { return Box3f(); }

    /// adds information about bounding box in res
    MRMESH_API void boundingBoxToInfoLines_( std::vector<std::string> & res ) const;

private:
    mutable Box3f boundingBoxCache_;

    /// this is private function to set default colors of this type (Visual Object) in constructor only
    void setDefaultColors_();
};

/// \}

} // namespace MR
