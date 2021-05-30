/******************************************************************************

   Use 'Material' to specify custom mesh material parameters.
   'Materials' are usually created in the 'Material Editor' tool, and used by 'Meshes'.

/******************************************************************************/
enum MATERIAL_TECHNIQUE : Byte // Material Techniques
{
   MTECH_DEFAULT               , // standard rendering of solid (opaque) materials
   MTECH_ALPHA_TEST            , // indicates that textures alpha channel will be used as models transparency (this is slightly slower than Default as alpha testing may disable some hardware-level optimizations)
   MTECH_FUR                   , // mesh will be rendered with fur effect, the mesh will be wrapped with additional fur imitating textures, in this technique "detail scale" specifies fur intensity, "detail power" specifies fur length, supported only in Deferred Renderer
   MTECH_GRASS                 , // mesh vertexes will bend on the wind like grass, bending intensity is determined by mesh vertex source Y position, which should be in the range from 0 to 1
   MTECH_LEAF                  , // mesh vertexes will bend on the wind like tree leafs, to use this technique mesh must also contain leaf attachment positions, which can be generated in the Model Editor tool through menu options
   MTECH_BLEND                 , // mesh will be smoothly blended on the screen using alpha values, mesh will not be affected by lighting or shadowing
   MTECH_BLEND_LIGHT           , // works like Blend technique except that mesh will be affected by lighting or shadowing, however only the most significant directional light will be used (all other lights are ignored), due to additional lighting calculations this is slower than Blend technique
   MTECH_BLEND_LIGHT_GRASS     , // combination of Blend Light and Grass techniques
   MTECH_BLEND_LIGHT_LEAF      , // combination of Blend Light and Leaf  techniques
   MTECH_TEST_BLEND_LIGHT      , // works like MTECH_BLEND_LIGHT       with additional Alpha-Testing and Depth-Writing which enables correct Depth-Sorting
   MTECH_TEST_BLEND_LIGHT_GRASS, // works like MTECH_BLEND_LIGHT_GRASS with additional Alpha-Testing and Depth-Writing which enables correct Depth-Sorting
   MTECH_TEST_BLEND_LIGHT_LEAF , // works like MTECH_BLEND_LIGHT_LEAF  with additional Alpha-Testing and Depth-Writing which enables correct Depth-Sorting
   MTECH_GRASS_3D              , // mesh vertexes will bend on the wind like grass, bending intensity is determined by mesh vertex source Y position, which should be in the range from 0 to 1
   MTECH_LEAF_2D               , // mesh vertexes will bend on the wind like tree leafs, to use this technique mesh must also contain leaf attachment positions, which can be generated in the Model Editor tool through menu options
   MTECH_NUM                   , // number of Material Techniques
};
/******************************************************************************/
#define MATERIAL_REFLECT 0.04f // default Material reflectivity value
/******************************************************************************/
struct MaterialParams // Material Parameters
{
   Vec4 color_l    ; // color Linear Gamma (0,0,0,0) .. (1,1,1,1), default=(1,1,1,1)
   Vec  emissive   ; // emissive             (0,0,0) .. (1,1,1)  , default=(0,0,0)
   Flt  smooth     , // smoothness                 0 .. 1        , default=0
        reflect_mul, // reflectivity from metal texture          , use 'reflect' function
        reflect_add, // reflectivity base                        , use 'reflect' function
        glow       , // glow amount                0 .. 1        , default=0
        normal     , // normal map sharpness       0 .. 1        , default=0
        bump       , // bumpiness                  0 .. 0.09     , default=0
        det_power  , // detail     power           0 .. 1        , default=0.3
        det_scale  , // detail  UV scale           0 .. Inf      , default=4
         uv_scale  ; // texture UV scale           0 .. Inf      , default=1, this is used mainly for World terrain textures UV scaling

 C Vec4& colorL()C {return color_l;}   void colorL(C Vec4 &color_l) {T.color_l=color_l;} // get/set Linear Gamma color
   Vec4  colorS()C;                    void colorS(C Vec4 &color_s);                     // get/set sRGB   Gamma color

   Flt reflect   ()C {return reflect_add;}   void reflect(Flt reflect     ); // set reflectivity, 0..1, default=MATERIAL_REFLECT
   Flt reflectMax()C;                        void reflect(Flt min, Flt max); // advanced
#if EE_PRIVATE
   #if LINEAR_GAMMA
      INLINE C Vec4& colorD()C {return colorL();}
   #else
      INLINE   Vec4  colorD()C {return colorS();}
   #endif
#endif
};
struct Material : MaterialParams // Mesh Rendering Material - contains render parameters and textures
{
#if EE_PRIVATE
   // #MaterialTextureLayout #MaterialTextureLayoutDetail
#endif
   ImagePtr            base_0  , // base      texture #0, default=null, this texture contains data packed in following channel order: RGB, Alpha
                       base_1  , // base      texture #1, default=null, this texture contains data packed in following channel order: NormalX, NormalY
                       base_2  , // base      texture #2, default=null, this texture contains data packed in following channel order: Metal, Smooth, Bump, Glow
                     detail_map, // detail    texture   , default=null, this texture contains data packed in following channel order: NormalX, NormalY, Color, Rough
                      macro_map, // macro     texture   , default=null
                      light_map; // light map texture   , default=null
   Bool               cull     ; // face culling        , default=true
   MATERIAL_TECHNIQUE technique; // material technique  , default=MTECH_DEFAULT

   // get
   Bool needTanBin()C; // if this Material needs tangent/binormals

   // operations
   Material& validate(); // this needs to be called after manually changing the parameters/textures
   Material& reset   (); // reset to default values (automatically calls 'validate')

   // io
   Bool save(C Str &name)C; // save, false on fail
   Bool load(C Str &name) ; // load, false on fail

   Bool save(File &f, CChar *path=null)C; // save, 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
   Bool load(File &f, CChar *path=null) ; // load, 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
#if EE_PRIVATE
   Bool saveData(File     &f, CChar *path=null)C; // save binary, 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
   Bool loadData(File     &f, CChar *path=null) ; // load binary, 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
   Bool saveTxt (FileText &f, CChar *path=null)C; // save text  , 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
   Bool loadTxt (FileText &f, CChar *path=null) ; // load text  , 'path'=path at which resource is located (this is needed so that the sub-resources can be accessed with relative path), false on fail
   Bool saveTxt (C Str    &name               )C; // save text  , false on fail
   Bool loadTxt (C Str    &name               ) ; // load text  , false on fail
#endif

   Material();
  ~Material();

#if EE_PRIVATE
   Bool hasAlpha          ()C;                          // if material technique involves Alpha Blending or Testing
   Bool hasAlphaBlend     ()C;                          // if material technique involves Alpha Blending
   Bool hasAlphaTest      ()C {return _has_alpha_test;} // if material technique involves Alpha Testing
   Bool hasAlphaBlendLight()C;                          // if material technique involves Alpha Blending with Light
   Bool hasGrass          ()C;                          // if material technique involves    Grass Bending
   Bool hasGrass2D        ()C;                          // if material technique involves 2D Grass Bending
   Bool hasGrass3D        ()C;                          // if material technique involves 3D Grass Bending
   Bool hasLeaf           ()C;                          // if material technique involves    Leaf  Bending
   Bool hasLeaf2D         ()C;                          // if material technique involves 2D Leaf  Bending
   Bool hasLeaf3D         ()C;                          // if material technique involves 3D Leaf  Bending

   void setSolid     (     )C;
   void setEmissive  (     )C;
   void setBlend     (     )C;
   void setBlendForce(     )C;
   void setOutline   (     )C;
   void setBehind    (     )C;
   void setShadow    (     )C;
   void setMulti     (Int i)C;
   void setAuto      (     )C;
#endif

#if !EE_PRIVATE
private:
#endif
   Bool  _depth_write, _has_alpha_test;
   Color _alpha_factor;
   struct Multi
   {
      Vec4 color;
      Vec  refl_smth_glow_mul, refl_smth_glow_add;
      Flt  normal, bump, det_mul, det_add, det_inv, macro, // medium prec
           uv_scale, det_scale; // high prec
   }_multi;
   struct MaterialShader // Material->Shader link
   {
      ShaderBase       *shader; // keep this as first member, because it's used most often
      Int next_material_shader, // index of the next shader for this material in 'MaterialShaders' container
          shader_material     ; // index of 'ShaderMaterial'
   #if EE_PRIVATE
      void clear ()  {shader=null; next_material_shader=shader_material=-1;}
      void unlink()  {shader=null; /*next_material_shader=-1;*/} // clearing 'next_material_shader' is not needed, because we check it only when "shader!=null"
      Bool empty ()C {return shader==null;}
   #endif
   }mutable _solid_material_shader, _shadow_material_shader; // have to keep 2 separate, because in forward renderer we queue solid draw calls, but before we draw them, we process shadows first. These store information about the first shader for this material (most materials will use only one shader during rendering), but if there are more shaders needed, then they contain indexes to next shaders in 'MaterialShaders' container
#if COUNT_MATERIAL_USAGE
    mutable Int _usage;
#endif
#if EE_PRIVATE
#if COUNT_MATERIAL_USAGE
   void clearUsage()C {_usage=0;}
   void   incUsage()C {_usage++;}
   void   decUsage()C {_usage--;}
   Bool emptyUsage()C {return !_usage;}
#else
   void clearUsage()C {}
   void   incUsage()C {}
   void   decUsage()C {}
   Bool emptyUsage()C {return true;}
#endif
   void  clearSolid ()C { _solid_material_shader.clear();}
   void  clearShadow()C {_shadow_material_shader.clear();}
   void  clear      ()C {clearSolid(); clearShadow(); clearUsage();}
   void unlinkSolid ()C { _solid_material_shader.unlink();}
   void unlinkShadow()C {_shadow_material_shader.unlink();}
   void unlink      ()C {unlinkSolid(); unlinkShadow();}
   Bool canBeRemoved()C {return _solid_material_shader.empty() && _shadow_material_shader.empty() && emptyUsage();}
#endif
public:
   void _adjustParams(UInt old_base_tex, UInt new_base_tex);
};
#if EE_PRIVATE
extern Material
   MaterialDefault      , // Default  Material
   MaterialDefaultNoCull; // Default  Material with no culling
extern const Material
  *MaterialLast         , // Last set Material
  *MaterialLast4[4]     ; // Last set Material (multi materials)
#endif
/******************************************************************************/
#if EE_PRIVATE
// unique combination of 4 materials
struct UniqueMultiMaterialKey
{
 C Material *m[4];

   UniqueMultiMaterialKey() {}
   UniqueMultiMaterialKey(C Material *a, C Material *b, C Material *c, C Material *d) {m[0]=a; m[1]=b; m[2]=c; m[3]=d;}
};
struct UniqueMultiMaterialData
{
   Material::MaterialShader material_shader;

   void clear () {material_shader.clear ();}
   void unlink() {material_shader.unlink();}

   UniqueMultiMaterialData() {clear();}
};
#endif
/******************************************************************************/
DECLARE_CACHE(Material, Materials, MaterialPtr); // 'Materials' cache storing 'Material' objects which can be accessed by 'MaterialPtr' pointer

#define BUMP_TO_NORMAL_SCALE (1.0f/64) // 0.015625, this value should be close to average 'Material.bump' which are 0.015, 0.03, 0.05 (remember that in Editor that value may be scaled)
#if EE_PRIVATE
extern MaterialPtr                                                    MaterialNull;
extern ThreadSafeMap<UniqueMultiMaterialKey, UniqueMultiMaterialData> UniqueMultiMaterialMap;

void MaterialClear();
void ShutMaterial();
void InitMaterial();

INLINE C Material& GetMaterial      (C Material *material                    ) {return material ? *material : MaterialDefault;}
INLINE C Material& GetShadowMaterial(C Material *material, Bool reuse_default) {return reuse_default ? (material && !material->cull) ? MaterialDefaultNoCull : MaterialDefault : GetMaterial(material);}
#endif
/******************************************************************************/
enum BASE_TEX
{
   BT_COLOR =1<<0, // base texture contains color
   BT_ALPHA =1<<1, // base texture contains alpha
   BT_BUMP  =1<<2, // base texture contains bump
   BT_NORMAL=1<<3, // base texture contains normal
   BT_SMOOTH=1<<4, // base texture contains smoothness
   BT_METAL =1<<5, // base texture contains metallic
   BT_GLOW  =1<<6, // base texture contains glow
};
struct ImageSource
{
 C Image &image;
   VecI2  size  = 0; // desired image dimensions, if >0 then they will override 'image' size
   Int    filter=-1; // desired FILTER_TYPE     , if <0 then FILTER_BEST is used
   Bool   clamp =false;

   ImageSource(C Image &image, C VecI2 &size=0, Int filter=-1, Bool clamp=false) : image(image), size(size), filter(filter), clamp(clamp) {}
};
UInt  CreateBaseTextures(Image &base_0, Image &base_1, Image &base_2, C ImageSource &color, C ImageSource &alpha, C ImageSource &bump, C ImageSource &normal, C ImageSource &smooth, C ImageSource &metal, C ImageSource &glow, Bool resize_to_pow2=true, Bool flip_normal_y=false, Bool smooth_is_rough=false); // create 'base_0', 'base_1' and 'base_2' base material textures from given images, textures will be created as IMAGE_R8G8B8A8_SRGB, IMAGE_R8G8_SIGN, IMAGE_R8G8B8A8 IMAGE_SOFT, 'flip_normal_y'=if flip normal map Y channel, 'smooth_is_rough'=if smoothness map is actually roughness map, returns bit combination of BASE_TEX enums of what the base textures have
UInt ExtractBase0Texture(Image &base_0, Image *color, Image *alpha);
UInt ExtractBase1Texture(Image &base_1, Image *normal);
UInt ExtractBase2Texture(Image &base_2, Image *bump, Image *smooth, Image *metal, Image *glow);

void  CreateDetailTexture(  Image &detail, C ImageSource &color, C ImageSource &bump, C ImageSource &normal, C ImageSource &smooth, Bool resize_to_pow2=true, Bool flip_normal_y=false, Bool smooth_is_rough=false); // create 'detail' material texture from given images, texture  will be created as IMAGE_R8G8B8A8 IMAGE_SOFT, 'flip_normal_y'=if flip normal map Y channel, 'smooth_is_rough'=if smoothness map is actually roughness map
UInt ExtractDetailTexture(C Image &detail,   Image       *color,   Image       *bump,   Image       *normal,   Image       *smooth);

Bool CreateBumpFromColor(Image &bump, C Image &color, Flt min_blur_range=-1, Flt max_blur_range=-1, Bool clamp=false); // create 'bump' texture from color image, texture will be created as IMAGE_F32 IMAGE_SOFT, 'min_blur_range' and 'max_blur_range' are minimum and maximum blur ranges used for creating the bump map, use -1 for auto values

Bool MergeBaseTextures(Image &base_0, C Material &material, Int image_type=-1, Int max_image_size=-1, C Vec *light_dir=&NoTemp(!Vec(1, -1, 1)), Flt light_power=0.77f, Flt spec_mul=1.0f, FILTER_TYPE filter=FILTER_BEST); // create 'base_0' simplified base material texture out of existing 'material' textures, this works by merging base textures into one (thus removing bump, normal, smooth, metal and glow maps, and keeping only color and alpha maps), 'image_type'=new desired IMAGE_TYPE for texture (-1=don't modify and use existing type), 'max_image_size'=limit maximum texture resolution (value of <=0 does not apply any limit), 'light_dir'=specify direction of the light for baking the normal map onto the color map (use null for no baking), 'light_power'=intensity of light (0..1) used during baking the normal map on the color map (ignored if 'light_dir' is set to null), 'spec_mul'=specular multiplier (ignored if 'light_dir' is set to null), returns true if base textures were merged and the new image was created, returns false if the material does not use multiple base textures (in such case the 'base_0' is left unmodified)
/******************************************************************************/
