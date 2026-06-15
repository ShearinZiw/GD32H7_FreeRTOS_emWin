/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.52 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_GPU_VGLite.h
Purpose     : VGLite interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GPU_VGLITE_H
#define GUI_GPU_VGLITE_H

#include "GUI.h"
#include "GUI_GPU.h"

/*********************************************************************
*
*       Use legacy definitions from SVG driver
*
**********************************************************************
*/
#ifdef    GUI_SVG_VGLITE_HEADER
  #define GUI_GPU_VGLITE_HEADER     GUI_SVG_VGLITE_HEADER
#endif

#ifdef    GUI_SVG_VGLITE_OS_HEADER
  #define GUI_GPU_VGLITE_OS_HEADER  GUI_SVG_VGLITE_OS_HEADER
#endif

#ifdef    GUI_SVG_HAS_VGLITE
  #define GUI_GPU_HAS_VGLITE
#endif

/*********************************************************************
*
*       VGLite header files.
*
**********************************************************************
*/
#ifndef   GUI_GPU_VGLITE_HEADER
  #define GUI_GPU_VGLITE_HEADER      <vg_lite.h>
#endif

#ifndef   GUI_GPU_VGLITE_OS_HEADER
  #define GUI_GPU_VGLITE_OS_HEADER   <vg_lite_os.h>
#endif

#ifndef   GUI_GPU_VGLITE_HAL_HEADER
  #define GUI_GPU_VGLITE_HAL_HEADER  <vg_lite_hal.h>
#endif

#ifdef GUI_GPU_HAS_VGLITE
  #include GUI_GPU_VGLITE_HEADER
  #include GUI_GPU_VGLITE_OS_HEADER
  #include GUI_GPU_VGLITE_HAL_HEADER
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Type definitions
*
**********************************************************************
*/
#ifndef GUI_GPU_HAS_VGLITE
  //
  // Define primitive types.
  //
  typedef float     vg_lite_float_t;
  typedef uint32_t  vg_lite_color_t;
  //
  // Define enums
  //
  typedef enum {
    GUI_VG_LITE_SUCCESS = 0,        
    GUI_VG_LITE_INVALID_ARGUMENT,   
    GUI_VG_LITE_OUT_OF_MEMORY,      
    GUI_VG_LITE_NO_CONTEXT,         
    GUI_VG_LITE_TIMEOUT,
    GUI_VG_LITE_OUT_OF_RESOURCES,   
    GUI_VG_LITE_GENERIC_IO,         
    GUI_VG_LITE_NOT_SUPPORT,        
    GUI_VG_LITE_MULTI_THREAD_FAIL,  
    GUI_VG_LITE_ALREADY_EXISTS,     
    GUI_VG_LITE_NOT_ALIGNED,          
  } DEFINE_ENUM_TYPE(vg_lite_error_t);
  //
  typedef enum {
    GUI_VG_LITE_S8,  
    GUI_VG_LITE_S16, 
    GUI_VG_LITE_S32, 
    GUI_VG_LITE_FP32,
  } DEFINE_ENUM_TYPE(vg_lite_format_t);
  //
  typedef enum {
    GUI_VG_LITE_HIGH,  
    GUI_VG_LITE_UPPER, 
    GUI_VG_LITE_MEDIUM,
    GUI_VG_LITE_LOW,   
  } DEFINE_ENUM_TYPE(vg_lite_quality_t);
  //
  typedef enum {
    GUI_VG_LITE_DRAW_FILL_PATH = 0, 
    GUI_VG_LITE_DRAW_STROKE_PATH,   
    GUI_VG_LITE_DRAW_FILL_STROKE_PATH,
  } DEFINE_ENUM_TYPE(vg_lite_draw_path_type_t);
  //
  typedef enum {
    GUI_VG_LITE_LINEAR,
    GUI_VG_LITE_TILED,
  } DEFINE_ENUM_TYPE(vg_lite_buffer_layout_t);
  //
  typedef enum {
    GUI_VG_LITE_RGBA8888,
    GUI_VG_LITE_BGRA8888,
    GUI_VG_LITE_RGBX8888,
    GUI_VG_LITE_BGRX8888,
    GUI_VG_LITE_RGB565,
    GUI_VG_LITE_BGR565,
    GUI_VG_LITE_RGBA4444,
    GUI_VG_LITE_BGRA4444,
    GUI_VG_LITE_BGRA5551,
    GUI_VG_LITE_A4,
    GUI_VG_LITE_A8,
    GUI_VG_LITE_L8,
    GUI_VG_LITE_YUYV,
    GUI_VG_LITE_YUY2,
    GUI_VG_LITE_NV12,
    GUI_VG_LITE_ANV12,
    GUI_VG_LITE_AYUY2,
    GUI_VG_LITE_YV12,
    GUI_VG_LITE_YV24,
    GUI_VG_LITE_YV16,
    GUI_VG_LITE_NV16,
    GUI_VG_LITE_YUY2_TILED, 
    GUI_VG_LITE_NV12_TILED,
    GUI_VG_LITE_ANV12_TILED,
    GUI_VG_LITE_AYUY2_TILED,
    GUI_VG_LITE_INDEX_1 = 100,
    GUI_VG_LITE_INDEX_2,
    GUI_VG_LITE_INDEX_4,
    GUI_VG_LITE_INDEX_8,
    GUI_VG_LITE_RGBA2222, 
    GUI_VG_LITE_BGRA2222, 
    GUI_VG_LITE_ABGR2222, 
    GUI_VG_LITE_ARGB2222, 
    GUI_VG_LITE_ABGR4444, 
    GUI_VG_LITE_ARGB4444, 
    GUI_VG_LITE_ABGR8888, 
    GUI_VG_LITE_ARGB8888, 
    GUI_VG_LITE_ABGR1555, 
    GUI_VG_LITE_RGBA5551, 
    GUI_VG_LITE_ARGB1555, 
    GUI_VG_LITE_XBGR8888, 
    GUI_VG_LITE_XRGB8888,
  } DEFINE_ENUM_TYPE(vg_lite_buffer_format_t);
  //
  typedef enum {
    GUI_VG_LITE_IMAGE_OPAQUE,
    GUI_VG_LITE_IMAGE_TRANSPARENT
  } DEFINE_ENUM_TYPE(vg_lite_buffer_transparency_mode_t);
  //
  typedef enum {
    GUI_VG_LITE_BLEND_NONE,    
    GUI_VG_LITE_BLEND_SRC_OVER,
    GUI_VG_LITE_BLEND_DST_OVER,
    GUI_VG_LITE_BLEND_SRC_IN,  
    GUI_VG_LITE_BLEND_DST_IN,  
    GUI_VG_LITE_BLEND_SCREEN,  
    GUI_VG_LITE_BLEND_MULTIPLY,
    GUI_VG_LITE_BLEND_ADDITIVE,
    GUI_VG_LITE_BLEND_SUBTRACT,
  } DEFINE_ENUM_TYPE(vg_lite_blend_t);
  //
  typedef enum {
    GUI_VG_LITE_NORMAL_IMAGE_MODE,
    GUI_VG_LITE_NONE_IMAGE_MODE,
    GUI_VG_LITE_MULTIPLY_IMAGE_MODE
  } DEFINE_ENUM_TYPE(vg_lite_buffer_image_mode_t);
  //
  typedef enum {
    GUI_VG_LITE_CAP_BUTT, 
    GUI_VG_LITE_CAP_ROUND,
    GUI_VG_LITE_CAP_SQUARE
  } DEFINE_ENUM_TYPE(vg_lite_cap_style_t);
  //
  typedef enum {
    GUI_VG_LITE_JOIN_MITER,
    GUI_VG_LITE_JOIN_ROUND,
    GUI_VG_LITE_JOIN_BEVEL
  } DEFINE_ENUM_TYPE(vg_lite_join_style_t);
  //
  typedef enum {
    GUI_VG_LITE_FILL_NON_ZERO, 
    GUI_VG_LITE_FILL_EVEN_ODD, 
  } DEFINE_ENUM_TYPE(vg_lite_fill_t);
  //
  typedef enum {
    GUI_VG_LITE_RADIAL_GRADIENT_SPREAD_FILL = 0,
    GUI_VG_LITE_RADIAL_GRADIENT_SPREAD_PAD,
    GUI_VG_LITE_RADIAL_GRADIENT_SPREAD_REPEAT,
    GUI_VG_LITE_RADIAL_GRADIENT_SPREAD_REFLECT,
  } DEFINE_ENUM_TYPE(vg_lite_radial_gradient_spreadmode_t);
  //
  typedef enum {
    GUI_gcFEATURE_BIT_VG_IM_INDEX_FORMAT,
    GUI_gcFEATURE_BIT_VG_PE_PREMULTIPLY,
    GUI_gcFEATURE_BIT_VG_BORDER_CULLING,
    GUI_gcFEATURE_BIT_VG_RGBA2_FORMAT,
    GUI_gcFEATURE_BIT_VG_QUALITY_8X,
    GUI_gcFEATURE_BIT_VG_RADIAL_GRADIENT,
    GUI_gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT,
    GUI_gcFEATURE_BIT_VG_COLOR_KEY,
    GUI_gcFEATURE_BIT_VG_DITHER,
    GUI_gcFEATURE_COUNT
  } DEFINE_ENUM_TYPE(vg_lite_feature_t);
  //
  typedef enum {
    GUI_VG_LITE_FILTER_POINT     = 0,      
    GUI_VG_LITE_FILTER_LINEAR    = 0x10000,
    GUI_VG_LITE_FILTER_BI_LINEAR = 0x20000,
  } DEFINE_ENUM_TYPE(vg_lite_filter_t);
  //
  typedef enum {
    GUI_VG_LITE_SWIZZLE_UV,
    GUI_VG_LITE_SWIZZLE_VU,
  } DEFINE_ENUM_TYPE(vg_lite_swizzle_t);
  //
  typedef enum {
    GUI_VG_LITE_YUV601,
    GUI_VG_LITE_YUV709,
  } DEFINE_ENUM_TYPE(vg_lite_yuv2rgb_t);
  //
  // Define structs with public members.
  //
  DEFINE_STRUCT_TYPE(vg_lite_matrix_t,
    float m[3][3];
  );
  #define MAX_COLOR_RAMP_STOPS            256
  DEFINE_STRUCT_TYPE(vg_lite_yuvinfo_t,
    vg_lite_swizzle_t   swizzle;      
    vg_lite_yuv2rgb_t   yuv2rgb;      
    uint32_t            uv_planar;    
    uint32_t            v_planar;     
    uint32_t            alpha_planar; 
    uint32_t            uv_stride;    
    uint32_t            v_stride;     
    uint32_t            alpha_stride; 
    uint32_t            uv_height;    
    uint32_t            v_height;     
    void *              uv_memory;    
    void *              v_memory;     
    void *              uv_handle;    
    void *              v_handle;     
  );
  DEFINE_STRUCT_TYPE(vg_lite_buffer_t,
    int32_t                            width;
    int32_t                            height;
    int32_t                            stride;
    vg_lite_buffer_layout_t            tiled;
    vg_lite_buffer_format_t            format;
    void                             * handle;
    void                             * memory;
    uint32_t                           address;
    vg_lite_yuvinfo_t                  yuv;
    vg_lite_buffer_image_mode_t        image_mode;
    vg_lite_buffer_transparency_mode_t transparency_mode;
  );
  DEFINE_STRUCT_TYPE(vg_lite_color_ramp_t,
    vg_lite_float_t stop;
    vg_lite_float_t red;
    vg_lite_float_t green;
    vg_lite_float_t blue;
    vg_lite_float_t alpha;
  );
  DEFINE_STRUCT_TYPE(vg_lite_linear_gradient_parameter_t,
    vg_lite_float_t X0;
    vg_lite_float_t Y0;
    vg_lite_float_t X1;
    vg_lite_float_t Y1;
  );
  DEFINE_STRUCT_TYPE(vg_lite_radial_gradient_parameter_t,
    vg_lite_float_t cx;
    vg_lite_float_t cy;
    vg_lite_float_t r;
    vg_lite_float_t fx;
    vg_lite_float_t fy;
  );
  DEFINE_STRUCT_TYPE(vg_lite_linear_gradient_ext_t,
    uint32_t                             count;            
    vg_lite_matrix_t                     matrix;   
    vg_lite_buffer_t                     image;    
    vg_lite_linear_gradient_parameter_t  linear_gradient;     
    uint32_t                             vg_color_ramp_length;  
    vg_lite_color_ramp_t                 vg_color_ramp[MAX_COLOR_RAMP_STOPS];
    uint32_t                             int_color_ramp_length;   
    vg_lite_color_ramp_t                 int_color_ramp[MAX_COLOR_RAMP_STOPS + 2];
    uint8_t                              color_ramp_premultiplied;
    vg_lite_radial_gradient_spreadmode_t spread_mode; 
  );
  DEFINE_STRUCT_TYPE(vg_lite_radial_gradient_t,
    uint32_t                             count;         
    vg_lite_matrix_t                     matrix;
    vg_lite_buffer_t                     image; 
    vg_lite_radial_gradient_parameter_t  radialGradient;
    vg_lite_radial_gradient_spreadmode_t SpreadMode;   
    uint32_t                             vgColorRampLength;       
    vg_lite_color_ramp_t                 vgColorRamp[MAX_COLOR_RAMP_STOPS]; 
    uint32_t                             intColorRampLength; 
    vg_lite_color_ramp_t                 intColorRamp[MAX_COLOR_RAMP_STOPS + 2];
    uint8_t                              colorRampPremultiplied;
  );
  DEFINE_STRUCT_TYPE(vg_lite_hw_memory_t,
    void    * handle;  
    void    * memory;  
    uint32_t  address; 
    uint32_t  bytes;   
    uint32_t  property;
  );
  DEFINE_STRUCT_TYPE(vg_lite_path_t,
    vg_lite_float_t          bounding_box[4];
    vg_lite_quality_t        quality;      
    vg_lite_format_t         format;        
    vg_lite_hw_memory_t      uploaded;   
    int32_t                  path_length;            
    void                   * path;                     
    int8_t                   path_changed;            
    int8_t                   pdata_internal;          
    void                   * stroke_conversion;
    vg_lite_draw_path_type_t path_type;            
    void                   * stroke_path_data;        
    int32_t                  stroke_path_size;      
    vg_lite_color_t          stroke_color; 
  );
#endif

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
typedef vg_lite_error_t    (GUI_GPU_VGLITE_BLIT_FUNC)            (vg_lite_buffer_t * target, vg_lite_buffer_t * source,
                                                                  vg_lite_matrix_t * matrix, vg_lite_blend_t blend,
                                                                  vg_lite_color_t color, vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_DRAWPATH_FUNC)        (vg_lite_buffer_t *target, vg_lite_path_t *path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t *matrix,
                                                                  vg_lite_blend_t blend, vg_lite_color_t color);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_INITPATH_FUNC)        (vg_lite_path_t *path, vg_lite_format_t data_format,
                                                                  vg_lite_quality_t quality, uint32_t path_length,
                                                                  void *path_data, vg_lite_float_t min_x,
                                                                  vg_lite_float_t min_y, vg_lite_float_t max_x,
                                                                  vg_lite_float_t max_y);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_CLEARPATH_FUNC)       (vg_lite_path_t *path);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETPATHDRAWTYPE_FUNC) (vg_lite_path_t *path, vg_lite_draw_path_type_t path_type);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETSTROKE_FUNC)       (vg_lite_path_t *path, vg_lite_cap_style_t stroke_cap_style,
                                                                  vg_lite_join_style_t stroke_join_style,
                                                                  vg_lite_float_t stroke_line_width,
                                                                  vg_lite_float_t stroke_miter_limit,
                                                                  vg_lite_float_t *stroke_dash_pattern,
                                                                  uint32_t stroke_dash_pattern_count,
                                                                  vg_lite_float_t stroke_dash_phase,
                                                                  vg_lite_color_t stroke_color);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_UPDATESTROKE_FUNC)    (vg_lite_path_t *path);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETLINGRAD_FUNC)      (vg_lite_linear_gradient_ext_t *grad, uint32_t count,
                                                                  vg_lite_color_ramp_t *vg_color_ramp,
                                                                  vg_lite_linear_gradient_parameter_t linear_gradient,
                                                                  vg_lite_radial_gradient_spreadmode_t spread_mode,
                                                                  uint8_t color_ramp_premultiplied);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_UPDATELINGRAD_FUNC)   (vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_matrix_t * (GUI_GPU_VGLITE_GETLINGRADMATRIX_FUNC)(vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_DRAWLINGRAD_FUNC)     (vg_lite_buffer_t * target, vg_lite_path_t * path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t * path_matrix,
                                                                  vg_lite_linear_gradient_ext_t *grad,
                                                                  vg_lite_color_t paint_color, vg_lite_blend_t blend,
                                                                  vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_CLEARLINGRAD_FUNC)    (vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETRADGRAD_FUNC)      (vg_lite_radial_gradient_t *grad, uint32_t count,
                                                                  vg_lite_color_ramp_t *vgColorRamp,
                                                                  vg_lite_radial_gradient_parameter_t radialGradient,
                                                                  vg_lite_radial_gradient_spreadmode_t spreadMode,
                                                                  uint8_t colorRampPremultiplied);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_UPDATERADGRAD_FUNC)   (vg_lite_radial_gradient_t *grad);
typedef vg_lite_matrix_t * (GUI_GPU_VGLITE_GETRADGRADMATRIX_FUNC)(vg_lite_radial_gradient_t *grad);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_DRAWRADGRAD_FUNC)     (vg_lite_buffer_t * target, vg_lite_path_t * path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t * path_matrix,
                                                                  vg_lite_radial_gradient_t *grad, vg_lite_color_t paint_color,
                                                                  vg_lite_blend_t blend, vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_CLEARRADGRAD_FUNC)    (vg_lite_radial_gradient_t *grad);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_ENABLESCISSOR_FUNC)   (void);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_DISABLESCISSOR_FUNC)  (void);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETSCISSOR_FUNC)      (int32_t x, int32_t y, int32_t width, int32_t height);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_FLUSH_FUNC)           (void);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_FINISH_FUNC)          (void);
typedef uint32_t           (GUI_GPU_VGLITE_QUERY_FEATURE_FUNC)   (vg_lite_feature_t feature);
typedef void               (GUI_GPU_VGLITE_IRQHANDLER_FUNC)      (void);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_INIT_FUNC)            (int32_t tessellation_width, int32_t tessellation_height);
typedef void               (GUI_GPU_VGLITE_INITMEM_FUNC)         (uint32_t register_mem_base, uint32_t gpu_mem_base,
                                                                  volatile void * contiguous_mem_base, uint32_t contiguous_mem_size);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETCMDBUFFERSIZE_FUNC)(uint32_t size);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_CLOSE_FUNC)           (void);
typedef vg_lite_error_t    (GUI_GPU_VGLITE_SETCLUT_FUNC)         (uint32_t count, uint32_t *colors);

/*********************************************************************
*
*       API struct for VGLite
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_VGLITE_API_STRUCT
*
*  Description
*    Maps the required functions of the Vivante VGLite API.
*
*    A structure of this type can be set with GUI_GPU_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the VGLite code (meaning \c{GUI_GPU_HAS_VGLITE} was not defined.
*
*    For more details about what each function does, please refer to the VGLite
*    API documentation.
*/
typedef struct {
  GUI_GPU_VGLITE_BLIT_FUNC              * pfBlit;             // Pointer to VGLite function \c{vg_lite_blit()}
  GUI_GPU_VGLITE_DRAWPATH_FUNC          * pfDrawPath;         // Pointer to VGLite function \c{vg_lite_draw()}
  GUI_GPU_VGLITE_INITPATH_FUNC          * pfInitPath;         // Pointer to VGLite function \c{vg_lite_init_path()}
  GUI_GPU_VGLITE_INITPATH_FUNC          * pfInitArcPath;      // Pointer to VGLite function \c{vg_lite_init_arc_path()}
  GUI_GPU_VGLITE_CLEARPATH_FUNC         * pfClearPath;        // Pointer to VGLite function \c{vg_lite_clear_path()}
  GUI_GPU_VGLITE_SETPATHDRAWTYPE_FUNC   * pfSetDrawPathType;  // Pointer to VGLite function \c{vg_lite_set_draw_path_type()}
  GUI_GPU_VGLITE_SETSTROKE_FUNC         * pfSetStroke;        // Pointer to VGLite function \c{vg_lite_set_stroke()}
  GUI_GPU_VGLITE_UPDATESTROKE_FUNC      * pfUpdateStroke;     // Pointer to VGLite function \c{vg_lite_update_stroke()}
  GUI_GPU_VGLITE_SETLINGRAD_FUNC        * pfSetLinGrad;       // Pointer to VGLite function \c{vg_lite_set_linear_grad()}
  GUI_GPU_VGLITE_UPDATELINGRAD_FUNC     * pfUpdateLinGrad;    // Pointer to VGLite function \c{vg_lite_update_linear_grad()}
  GUI_GPU_VGLITE_GETLINGRADMATRIX_FUNC  * pfGetLinGradMatrix; // Pointer to VGLite function \c{vg_lite_get_linear_grad_matrix()}
  GUI_GPU_VGLITE_DRAWLINGRAD_FUNC       * pfDrawLinGrad;      // Pointer to VGLite function \c{vg_lite_draw_linear_gradient()}
  GUI_GPU_VGLITE_CLEARLINGRAD_FUNC      * pfClearLinGrad;     // Pointer to VGLite function \c{vg_lite_clear_linear_grad()}
  GUI_GPU_VGLITE_SETRADGRAD_FUNC        * pfSetRadGrad;       // Pointer to VGLite function \c{vg_lite_set_rad_grad()}
  GUI_GPU_VGLITE_UPDATERADGRAD_FUNC     * pfUpdateRadGrad;    // Pointer to VGLite function \c{vg_lite_update_rad_grad()}
  GUI_GPU_VGLITE_GETRADGRADMATRIX_FUNC  * pfGetRadGradMatrix; // Pointer to VGLite function \c{vg_lite_get_rad_grad_matrix()}
  GUI_GPU_VGLITE_DRAWRADGRAD_FUNC       * pfDrawRadGrad;      // Pointer to VGLite function \c{vg_lite_draw_radial_gradient()}
  GUI_GPU_VGLITE_CLEARRADGRAD_FUNC      * pfClearRadGrad;     // Pointer to VGLite function \c{vg_lite_clear_rad_grad()}
  GUI_GPU_VGLITE_ENABLESCISSOR_FUNC     * pfEnableScissor;    // Pointer to VGLite function \c{vg_lite_enable_scissor()}
  GUI_GPU_VGLITE_DISABLESCISSOR_FUNC    * pfDisableScissor;   // Pointer to VGLite function \c{vg_lite_disable_scissor()}
  GUI_GPU_VGLITE_SETSCISSOR_FUNC        * pfSetScissor;       // Pointer to VGLite function \c{vg_lite_set_scissor()}
  GUI_GPU_VGLITE_FLUSH_FUNC             * pfFlush;            // Pointer to VGLite function \c{vg_lite_flush()}
  GUI_GPU_VGLITE_FINISH_FUNC            * pfFinish;           // Pointer to VGLite function \c{vg_lite_finish()}
  GUI_GPU_VGLITE_QUERY_FEATURE_FUNC     * pfQueryFeature;     // Pointer to VGLite function \c{vg_lite_query_feature()}
  GUI_GPU_VGLITE_IRQHANDLER_FUNC        * pfIRQHandler;       // Pointer to VGLite function \c{vg_lite_IRQHandler()}
  GUI_GPU_VGLITE_INIT_FUNC              * pfInit;             // Pointer to VGLite function \c{vg_lite_init()}
  GUI_GPU_VGLITE_INITMEM_FUNC           * pfInitMem;          // Pointer to VGLite function \c{vg_lite_init_mem()}
  GUI_GPU_VGLITE_SETCMDBUFFERSIZE_FUNC  * pfSetCmdBufferSize; // Pointer to VGLite function \c{vg_lite_set_command_buffer_size()}
  GUI_GPU_VGLITE_CLOSE_FUNC             * pfClose;            // Pointer to VGLite function \c{vg_lite_close()}
  GUI_GPU_VGLITE_SETCLUT_FUNC           * pfSetCLUT;          // Pointer to VGLite function \c{vg_lite_set_CLUT()}
} GUI_GPU_VGLITE_API_STRUCT;

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_VGLITE_API
* 
*  Description
*    Macro to fill a structure of the type GUI_GPU_VGLITE_API_STRUCT
*    filled with the correct function pointers.
*/
#define GUI_GPU_VGLITE_API           \
    vg_lite_blit,                    \
    vg_lite_draw,                    \
    vg_lite_init_path,               \
    vg_lite_init_arc_path,           \
    vg_lite_clear_path,              \
    vg_lite_set_draw_path_type,      \
    vg_lite_set_stroke,              \
    vg_lite_update_stroke,           \
    vg_lite_set_linear_grad,         \
    vg_lite_update_linear_grad,      \
    vg_lite_get_linear_grad_matrix,  \
    vg_lite_draw_linear_gradient,    \
    vg_lite_clear_linear_grad,       \
    vg_lite_set_rad_grad,            \
    vg_lite_update_rad_grad,         \
    vg_lite_get_rad_grad_matrix,     \
    vg_lite_draw_radial_gradient,    \
    vg_lite_clear_rad_grad,          \
    vg_lite_enable_scissor,          \
    vg_lite_disable_scissor,         \
    vg_lite_set_scissor,             \
    vg_lite_flush,                   \
    vg_lite_finish,                  \
    vg_lite_query_feature,           \
    vg_lite_IRQHandler,              \
    vg_lite_init,                    \
    vg_lite_init_mem,                \
    vg_lite_set_command_buffer_size, \
    vg_lite_close,                   \
    vg_lite_set_CLUT

/* emDoc stop */
//
// Legacy macros
//
#define GUI_SVG_DECLARE_VGLITE_API(VAR_NAME)  \
  static const GUI_GPU_VGLITE_API_STRUCT VAR_NAME = { GUI_GPU_VGLITE_API }

/*********************************************************************
*
*       Config struct
*
**********************************************************************
*/
/*********************************************************************
* 
*       GUI_GPU_VGLITE_CONFIG
* 
*  Description
*    Configures the VGLite GPU driver when it is created.
*/
typedef struct {
  void     * pGPU2D;              // Pointer to the GPU2D/VGLite register.
  unsigned   BufferSize;          // Buffer size in bytes for VGLite tessellation buffer. Pass 0 to use the default size of 16 MB.
  unsigned   CommandBufferSize;   // Command buffer size for VGLite, pass 0 to use the default size of 256 KB.
} GUI_GPU_VGLITE_CONFIG;

#if defined(__cplusplus)
}
#endif

#endif /* GUI_GPU_VGLITE_H */

/*************************** End of file ****************************/
