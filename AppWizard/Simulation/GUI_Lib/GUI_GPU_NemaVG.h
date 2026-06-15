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
File        : GUI_GPU_NemaVG.h
Purpose     : NemaVG interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GPU_NEMAVG_H
#define GUI_GPU_NEMAVG_H

#include "GUI.h"
#include "GUI_GPU.h"

/*********************************************************************
*
*       Use legacy definitions from SVG driver
*
**********************************************************************
*/
#ifdef    GUI_SVG_NEMA_VG_HEADER
  #define GUI_GPU_NEMA_VG_HEADER           GUI_SVG_NEMA_VG_HEADER
#endif

#ifdef    GUI_SVG_NEMA_VG_VERSION_HEADER
  #define GUI_GPU_NEMA_VG_VERSION_HEADER   GUI_SVG_NEMA_VG_VERSION_HEADER
#endif

#ifdef    GUI_SVG_HAS_NEMAVG
  #define GUI_GPU_HAS_NEMAVG
#endif

/*********************************************************************
*
*       NemaVG header files.
*
**********************************************************************
*/
#ifndef   GUI_GPU_NEMA_VG_HEADER
  #define GUI_GPU_NEMA_VG_HEADER           <nema_vg.h>
#endif

#ifndef   GUI_GPU_NEMA_VG_VERSION_HEADER
  #define GUI_GPU_NEMA_VG_VERSION_HEADER   <nema_vg_version.h>
#endif

#ifdef GUI_GPU_HAS_NEMAVG
  #include GUI_GPU_NEMA_VG_HEADER
  #include GUI_GPU_NEMA_VG_VERSION_HEADER
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       API version
*
**********************************************************************
*/
//
// API versions
//
#define NEMA_VG_VERSION_1_1_5   (0x010105)
#define NEMA_VG_VERSION_1_1_7   (0x010107)

#ifndef GUI_GPU_HAS_NEMAVG
  //
  // NemaVG API version
  //
  #define NEMA_VG_API_VERSION            NEMA_VG_VERSION_1_1_5    // Minimum v1.1.5 required
#endif

/*********************************************************************
*
*       Type definitions
*
**********************************************************************
*/
#ifndef GUI_GPU_HAS_NEMAVG
  //
  // Handle types
  //
  #define NEMA_VG_HANDLE         void *
  #define NEMA_VG_PATH_HANDLE    NEMA_VG_HANDLE
  #define NEMA_VG_PAINT_HANDLE   NEMA_VG_HANDLE
  #define NEMA_VG_GRAD_HANDLE    NEMA_VG_HANDLE
  //
  // General types
  //
  typedef uint32_t nema_tex_format_t;
  typedef float    nema_vg_float_t;
  typedef uint8_t  nema_tex_mode_t;
  typedef float    nema_matrix3x3_t[3][3];
  //
  // Define public struct types.
  //
  DEFINE_STRUCT_TYPE(color_var_t,
    float r;
    float g;
    float b;
    float a;
  );
  DEFINE_STRUCT_TYPE(nema_buffer_t,
    int        size;      
    int        fd;        
    void     * base_virt; 
    uintptr_t  base_phys; 
  );
  DEFINE_STRUCT_TYPE(nema_cmdlist_t,
    nema_buffer_t bo;
    int           size;                     
    int           offset;                   
    uint32_t      flags;               
    int32_t       submission_id;
    void        * next; 
    void        * root; 
  );
  DEFINE_STRUCT_TYPE(nema_ringbuffer_t,
    nema_buffer_t bo;
    int           offset;
    int	          last_submission_id;
  );
  DEFINE_STRUCT_TYPE(nema_img_obj_t,
    nema_buffer_t bo;
    uint16_t      w;
    uint16_t      h;
    int           stride;
    uint32_t      color;
    uint8_t       format;
    uint8_t       sampling_mode;
  );
#endif

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
#ifndef GUI_GPU_HAS_NEMAVG
  //
  // OSA layer
  //
  int32_t       nema_sys_init            (void);
  int           nema_wait_irq            (void);
  int           nema_wait_irq_cl         (int cl_id);
  int           nema_wait_irq_brk        (int brk_id);
  uint32_t      nema_reg_read            (uint32_t reg);
  void          nema_reg_write           (uint32_t reg, uint32_t value);
  nema_buffer_t nema_buffer_create       (int size);
  nema_buffer_t nema_buffer_create_pool  (int pool, int size);
  void        * nema_buffer_map          (nema_buffer_t *bo);
  void          nema_buffer_unmap        (nema_buffer_t *bo);
  void          nema_buffer_destroy      (nema_buffer_t *bo);
  uintptr_t     nema_buffer_phys         (nema_buffer_t *bo);
  void          nema_buffer_flush        (nema_buffer_t * bo);
  void        * nema_host_malloc         (unsigned size);
  void          nema_host_free           (void * ptr);
  int           nema_mutex_lock          (int mutex_id);
  int           nema_mutex_unlock        (int mutex_id);
  void          platform_disable_cache   (void);
  void          platform_invalidate_cache(void);
#endif

/*********************************************************************
*
*       Enums
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_NEMAVG_IRQ_INDEX
*
*  Description
*    Index of NemaVG interrupt handler, passed as parameter
*    to GUI_GPU_IRQHandler().
*/
typedef enum {
  GUI_GPU_NEMAVG_IRQ,        // Interrupt handler for main NemaVG interrupt.
  GUI_GPU_NEMAVG_ERROR_IRQ,  // Interrupt handler for NemaVG error interrupt.
} GUI_GPU_NEMAVG_IRQ_INDEX;

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
typedef int                  (GUI_GPU_NEMAVG_INIT_FUNC)               (void);
typedef void                 (GUI_GPU_NEMAVG_SETCLIP_FUNC)            (int32_t x, int32_t y, uint32_t w, uint32_t h);
typedef void                 (GUI_GPU_NEMAVG_BINDDSTTEX_FUNC)         (uintptr_t baseaddr_phys, uint32_t width, uint32_t height,
                                                                       nema_tex_format_t format, int32_t stride);
typedef void                 (GUI_GPU_NEMAVG_SETBLENDFILL_FUNC)       (uint32_t blending_mode);
typedef nema_cmdlist_t       (GUI_GPU_NEMAVG_CLCREATESIZED_FUNC)      (int size_bytes);
typedef void                 (GUI_GPU_NEMAVG_CLBINDCIRCULAR_FUNC)     (nema_cmdlist_t *cl);
typedef void                 (GUI_GPU_NEMAVG_CLSUBMIT_FUNC)           (nema_cmdlist_t *cl);
typedef int                  (GUI_GPU_NEMAVG_CLWAIT_FUNC)             (nema_cmdlist_t *cl);
typedef void                 (GUI_GPU_NEMAVG_CLREWIND_FUNC)           (nema_cmdlist_t *cl);
typedef void                 (GUI_GPU_NEMAVG_CLDESTROY_FUNC)          (nema_cmdlist_t *cl);
typedef void                 (GUI_GPU_NEMAVG_VGINIT_FUNC)             (int width, int height);
typedef void                 (GUI_GPU_NEMAVG_VGDEINIT_FUNC)           (void);
typedef void                 (GUI_GPU_NEMAVG_HANDLELARGECOORDS_FUNC)  (uint8_t enable, uint8_t allow_internal_alloc);
typedef void                 (GUI_GPU_NEMAVG_SETBLEND_FUNC)           (uint32_t blend);
typedef void                 (GUI_GPU_NEMAVG_SETFILLRULE_FUNC)        (uint8_t fill_rule);
typedef void                 (GUI_GPU_NEMAVG_SETQUALITY_FUNC)         (uint8_t quality);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWLINE_FUNC)           (float x1, float y1, float x2, float y2,
                                                                       nema_matrix3x3_t m, NEMA_VG_PAINT_HANDLE paint);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWPATH_FUNC)           (NEMA_VG_PATH_HANDLE path, NEMA_VG_PAINT_HANDLE paint);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWRECT_FUNC)           (float x, float y, float width, float height,
                                                                       nema_matrix3x3_t m, NEMA_VG_PAINT_HANDLE paint);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWROUNDEDRECT_FUNC)    (float x, float y, float width, float height,
                                                                       float rx, float ry, nema_matrix3x3_t m, NEMA_VG_PAINT_HANDLE paint);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWCIRCLE_FUNC)         (float cx, float cy, float r, nema_matrix3x3_t m, NEMA_VG_PAINT_HANDLE paint);
typedef NEMA_VG_PATH_HANDLE  (GUI_GPU_NEMAVG_PATHCREATE_FUNC)         (void);
typedef void                 (GUI_GPU_NEMAVG_PATHSETMATRIX_FUNC)      (NEMA_VG_PATH_HANDLE path, nema_matrix3x3_t m);
typedef void                 (GUI_GPU_NEMAVG_PATHSETSHAPE_FUNC)       (NEMA_VG_PATH_HANDLE path, const size_t seg_size,
                                                                       const uint8_t* seg, const size_t data_size, const nema_vg_float_t* data);
typedef void                 (GUI_GPU_NEMAVG_PATHDESTROY_FUNC)        (NEMA_VG_PATH_HANDLE path);
typedef NEMA_VG_GRAD_HANDLE  (GUI_GPU_NEMAVG_GRADCREATE_FUNC)         (void);
typedef void                 (GUI_GPU_NEMAVG_GRADDESTROY_FUNC)        (NEMA_VG_GRAD_HANDLE grad);
typedef void                 (GUI_GPU_NEMAVG_GRADSET_FUNC)            (NEMA_VG_GRAD_HANDLE grad, int stops_count, float *stops, color_var_t* colors);
typedef NEMA_VG_PAINT_HANDLE (GUI_GPU_NEMAVG_PAINTCREATE_FUNC)        (void);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETTYPE_FUNC)       (NEMA_VG_PAINT_HANDLE paint, uint8_t type);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETGRADLINEAR_FUNC) (NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE grad,
                                                                       float x0, float y0, float x1, float y1, nema_tex_mode_t sampling_mode);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETGRADRADIAL2_FUNC)(NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE grad, float x0, float y0,
                                                                       float rx, float ry, nema_tex_mode_t sampling_mode);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETOPACITY_FUNC)    (NEMA_VG_PAINT_HANDLE paint, float opacity);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETPAINTCOLOR_FUNC) (NEMA_VG_PAINT_HANDLE paint, uint32_t rgba);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETSTROKEWIDTH_FUNC)(NEMA_VG_PAINT_HANDLE paint, float stroke_width);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETTEX_FUNC)        (NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* tex);
typedef void                 (GUI_GPU_NEMAVG_PAINTSETTEXMATRIX_FUNC)  (NEMA_VG_PAINT_HANDLE paint, nema_matrix3x3_t m);
typedef void                 (GUI_GPU_NEMAVG_PAINTDESTROY_FUNC)       (NEMA_VG_PAINT_HANDLE paint);
typedef void                 (GUI_GPU_NEMAVG_STROKESETCAPSTYLE_FUNC)  (uint8_t start_cap_style, uint8_t end_cap_style);
typedef void                 (GUI_GPU_NEMAVG_STROKESETJOINSTYLE_FUNC) (uint8_t join_style);
typedef void                 (GUI_GPU_NEMAVG_STROKESETMITERLIMIT_FUNC)(float MiterLimit);
typedef uint32_t             (GUI_GPU_NEMAVG_DRAWRINGGENERIC_FUNC)    (float cx, float cy, float ring_radius, float angle_start, float angle_end,
                                                                       NEMA_VG_PAINT_HANDLE paint, uint8_t has_caps);
typedef int                  (GUI_GPU_NEMAVG_RBINIT_FUNC)             (nema_ringbuffer_t *rb, int reset);

//
// OSA layer
//
typedef int32_t       (GUI_GPU_NEMAVG_OSA_SYS_INIT_FUNC)          (void);
typedef int           (GUI_GPU_NEMAVG_OSA_WAIT_IRQ_FUNC)          (void);
typedef int           (GUI_GPU_NEMAVG_OSA_WAIT_IRQ_CL_FUNC)       (int cl_id);
typedef int           (GUI_GPU_NEMAVG_OSA_WAIT_IRQ_BRK_FUNC)      (int brk_id);
typedef uint32_t      (GUI_GPU_NEMAVG_OSA_REG_READ_FUNC)          (uint32_t reg);
typedef void          (GUI_GPU_NEMAVG_OSA_REG_WRITE_FUNC)         (uint32_t reg, uint32_t value);
typedef nema_buffer_t (GUI_GPU_NEMAVG_OSA_BUFFER_CREATE_FUNC)     (int size);
typedef nema_buffer_t (GUI_GPU_NEMAVG_OSA_BUFFER_CREATE_POOL_FUNC)(int pool, int size);
typedef void        * (GUI_GPU_NEMAVG_OSA_BUFFER_MAP_FUNC)        (nema_buffer_t *bo);
typedef void          (GUI_GPU_NEMAVG_OSA_BUFFER_UNMAP_FUNC)      (nema_buffer_t *bo);
typedef void          (GUI_GPU_NEMAVG_OSA_BUFFER_DESTROY_FUNC)    (nema_buffer_t *bo);
typedef uintptr_t     (GUI_GPU_NEMAVG_OSA_BUFFER_PHYS_FUNC)       (nema_buffer_t *bo);
typedef void          (GUI_GPU_NEMAVG_OSA_BUFFER_FLUSH_FUNC)      (nema_buffer_t *bo);
typedef void        * (GUI_GPU_NEMAVG_OSA_HOST_MALLOC_FUNC)       (unsigned size);
typedef void          (GUI_GPU_NEMAVG_OSA_HOST_FREE_FUNC)         (void *ptr);
typedef int           (GUI_GPU_NEMAVG_OSA_MUTEX_LOCK_FUNC)        (int mutex_id);
typedef int           (GUI_GPU_NEMAVG_OSA_MUTEX_UNLOCK_FUNC)      (int mutex_id);
typedef void          (GUI_GPU_NEMAVG_OSA_DISABLE_CACHE_FUNC)     (void);
typedef void          (GUI_GPU_NEMAVG_OSA_INVALIDATE_CACHE_FUNC)  (void);

/*********************************************************************
*
*       API struct for NemaVG
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_NEMAVG_API_STRUCT
*
*  Description
*    Maps the required functions of the ThinkSilicon NemaVG API.
*
*    A structure of this type can be set with GUI_GPU_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the NemaVG code (meaning \c{GUI_GPU_HAS_NEMAVG} was not defined.
*
*    For more details about these functions, please refer to the NemaVG
*    API documentation.
*/
typedef struct {
  GUI_GPU_NEMAVG_INIT_FUNC                * pfInit;                    // Pointer to NemaVG function \c{nema_init()}
  GUI_GPU_NEMAVG_SETCLIP_FUNC             * pfSetClip;                 // Pointer to NemaVG function \c{nema_set_clip()}
  GUI_GPU_NEMAVG_BINDDSTTEX_FUNC          * pfBindDstTex;              // Pointer to NemaVG function \c{nema_bind_dst_tex()}
  GUI_GPU_NEMAVG_SETBLENDFILL_FUNC        * pfSetBlendFill;            // Pointer to NemaVG function \c{nema_set_blend_fill()}
  GUI_GPU_NEMAVG_CLCREATESIZED_FUNC       * pfClCreateSized;           // Pointer to NemaVG function \c{nema_cl_create_sized()}
  GUI_GPU_NEMAVG_CLBINDCIRCULAR_FUNC      * pfClBindCircular;          // Pointer to NemaVG function \c{nema_cl_bind_circular()}
  GUI_GPU_NEMAVG_CLSUBMIT_FUNC            * pfClSubmit;                // Pointer to NemaVG function \c{nema_cl_submit()}
  GUI_GPU_NEMAVG_CLWAIT_FUNC              * pfClWait;                  // Pointer to NemaVG function \c{nema_cl_wait()}
  GUI_GPU_NEMAVG_CLREWIND_FUNC            * pfClRewind;                // Pointer to NemaVG function \c{nema_cl_rewind()}
  GUI_GPU_NEMAVG_CLDESTROY_FUNC           * pfClDestroy;               // Pointer to NemaVG function \c{nema_cl_destroy()}
  GUI_GPU_NEMAVG_VGINIT_FUNC              * pfVgInit;                  // Pointer to NemaVG function \c{nema_vg_init()}
  GUI_GPU_NEMAVG_VGDEINIT_FUNC            * pfVgDeinit;                // Pointer to NemaVG function \c{nema_vg_deinit()}
  GUI_GPU_NEMAVG_HANDLELARGECOORDS_FUNC   * pfVgHandleLargeCoords;     // Pointer to NemaVG function \c{nema_vg_handle_large_coords()}
  GUI_GPU_NEMAVG_SETBLEND_FUNC            * pfVgSetBlend;              // Pointer to NemaVG function \c{nema_vg_set_blend()}
  GUI_GPU_NEMAVG_SETFILLRULE_FUNC         * pfVgSetFillRule;           // Pointer to NemaVG function \c{nema_vg_set_fill_rule()}
  GUI_GPU_NEMAVG_SETQUALITY_FUNC          * pfVgSetQuality;            // Pointer to NemaVG function \c{nema_vg_set_quality()}
  GUI_GPU_NEMAVG_DRAWLINE_FUNC            * pfVgDrawLine;              // Pointer to NemaVG function \c{nema_vg_draw_line()}
  GUI_GPU_NEMAVG_DRAWPATH_FUNC            * pfVgDrawPath;              // Pointer to NemaVG function \c{nema_vg_draw_path()}
  GUI_GPU_NEMAVG_DRAWRECT_FUNC            * pfVgDrawRect;              // Pointer to NemaVG function \c{nema_vg_draw_rect()}
  GUI_GPU_NEMAVG_DRAWROUNDEDRECT_FUNC     * pfVgDrawRoundedRect;       // Pointer to NemaVG function \c{nema_vg_draw_rounded_rect()}
  GUI_GPU_NEMAVG_DRAWCIRCLE_FUNC          * pfVgDrawCircle;            // Pointer to NemaVG function \c{nema_vg_draw_circle()}
  GUI_GPU_NEMAVG_PATHCREATE_FUNC          * pfVgPathCreate;            // Pointer to NemaVG function \c{nema_vg_path_create()}
  GUI_GPU_NEMAVG_PATHSETMATRIX_FUNC       * pfVgPathSetMatrix;         // Pointer to NemaVG function \c{nema_vg_path_set_matrix()}
  GUI_GPU_NEMAVG_PATHSETSHAPE_FUNC        * pfVgPathSetShape;          // Pointer to NemaVG function \c{nema_vg_path_set_shape()}
  GUI_GPU_NEMAVG_PATHDESTROY_FUNC         * pfVgPathDestroy;           // Pointer to NemaVG function \c{nema_vg_path_destroy()}
  GUI_GPU_NEMAVG_GRADCREATE_FUNC          * pfVgGradCreate;            // Pointer to NemaVG function \c{nema_vg_grad_create()}
  GUI_GPU_NEMAVG_GRADSET_FUNC             * pfVgGradSet;               // Pointer to NemaVG function \c{nema_vg_grad_set()}
  GUI_GPU_NEMAVG_GRADDESTROY_FUNC         * pfVgGradDestroy;           // Pointer to NemaVG function \c{nema_vg_grad_destroy()}
  GUI_GPU_NEMAVG_PAINTCREATE_FUNC         * pfVgPaintCreate;           // Pointer to NemaVG function \c{nema_vg_paint_create()}
  GUI_GPU_NEMAVG_PAINTSETTYPE_FUNC        * pfVgPaintSetType;          // Pointer to NemaVG function \c{nema_vg_paint_set_type()}
  GUI_GPU_NEMAVG_PAINTSETGRADLINEAR_FUNC  * pfVgPaintSetGradLinear;    // Pointer to NemaVG function \c{nema_vg_paint_set_grad_linear()}
  GUI_GPU_NEMAVG_PAINTSETGRADRADIAL2_FUNC * pfVgPaintSetGradRadial2;   // Pointer to NemaVG function \c{nema_vg_paint_set_grad_radial2()}
  GUI_GPU_NEMAVG_PAINTSETOPACITY_FUNC     * pfVgPaintSetOpacity;       // Pointer to NemaVG function \c{nema_vg_paint_set_opacity()}
  GUI_GPU_NEMAVG_PAINTSETPAINTCOLOR_FUNC  * pfVgPaintSetPaintColor;    // Pointer to NemaVG function \c{nema_vg_paint_set_paint_color()}
  GUI_GPU_NEMAVG_PAINTSETSTROKEWIDTH_FUNC * pfVgPaintSetStrokeWidth;   // Pointer to NemaVG function \c{nema_vg_paint_set_stroke_width()}
  GUI_GPU_NEMAVG_PAINTSETTEX_FUNC         * pfVgPaintSetTex;           // Pointer to NemaVG function \c{nema_vg_paint_set_tex()}
  GUI_GPU_NEMAVG_PAINTSETTEXMATRIX_FUNC   * pfVgPaintSetTexMatrix;     // Pointer to NemaVG function \c{nema_vg_paint_set_tex_matrix()}
  GUI_GPU_NEMAVG_PAINTDESTROY_FUNC        * pfVgPaintDestroy;          // Pointer to NemaVG function \c{nema_vg_paint_destroy()}
  GUI_GPU_NEMAVG_STROKESETCAPSTYLE_FUNC   * pfVgStrokeSetCapStyle;     // Pointer to NemaVG function \c{nema_vg_stroke_set_cap_style()}
  GUI_GPU_NEMAVG_STROKESETJOINSTYLE_FUNC  * pfVgStrokeSetJoinStyle;    // Pointer to NemaVG function \c{nema_vg_stroke_set_join_style()}
  GUI_GPU_NEMAVG_STROKESETMITERLIMIT_FUNC * pfVgStrokeSetMiterLimit;   // Pointer to NemaVG function \c{nema_vg_stroke_set_miter_limit()}
  GUI_GPU_NEMAVG_DRAWRINGGENERIC_FUNC     * pfVgDrawRingGeneric;       // Pointer to NemaVG function \c{nema_vg_draw_ring_generic()}
  GUI_GPU_NEMAVG_RBINIT_FUNC              * pfRbInit;                  // Pointer to NemaVG function \c{nema_rb_init()}
} GUI_GPU_NEMAVG_API_STRUCT;

/*********************************************************************
*
*       GUI_GPU_NEMAVG_OSA_API_STRUCT
*
*  Description
*    Maps the required functions of the OSA layer of the ThinkSilicon
*    NemaVG API.
* 
*    The layer can be switched out via the configuration of the
*    driver (see GUI_GPU_NEMAVG_CONFIG).
* 
*    For more details about these functions and the OSA layer,
*    please refer to the NemaVG API documentation.
*/
typedef struct {
  GUI_GPU_NEMAVG_OSA_SYS_INIT_FUNC           * pfSysInit;           // Pointer to wrapper of NemaVG OSA function \c{nema_sys_init()}
  GUI_GPU_NEMAVG_OSA_WAIT_IRQ_FUNC           * pfWaitIrq;           // Pointer to wrapper of NemaVG OSA function \c{nema_wait_irq()}
  GUI_GPU_NEMAVG_OSA_WAIT_IRQ_CL_FUNC        * pfWaitIrqCl;         // Pointer to wrapper of NemaVG OSA function \c{nema_wait_irq_cl()}
  GUI_GPU_NEMAVG_OSA_WAIT_IRQ_BRK_FUNC       * pfWaitIrqBrk;        // Pointer to wrapper of NemaVG OSA function \c{nema_wait_irq_brk()}
  GUI_GPU_NEMAVG_OSA_REG_READ_FUNC           * pfRegRead;           // Pointer to wrapper of NemaVG OSA function \c{nema_reg_read()}
  GUI_GPU_NEMAVG_OSA_REG_WRITE_FUNC          * pfRegWrite;          // Pointer to wrapper of NemaVG OSA function \c{nema_reg_write()}
  GUI_GPU_NEMAVG_OSA_BUFFER_CREATE_FUNC      * pfBufferCreate;      // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_create()}
  GUI_GPU_NEMAVG_OSA_BUFFER_CREATE_POOL_FUNC * pfBufferCreatePool;  // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_create_pool()}
  GUI_GPU_NEMAVG_OSA_BUFFER_MAP_FUNC         * pfBufferMap;         // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_map()}
  GUI_GPU_NEMAVG_OSA_BUFFER_UNMAP_FUNC       * pfBufferUnmap;       // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_unmap()}
  GUI_GPU_NEMAVG_OSA_BUFFER_DESTROY_FUNC     * pfBufferDestroy;     // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_destroy()}
  GUI_GPU_NEMAVG_OSA_BUFFER_PHYS_FUNC        * pfBufferPhys;        // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_phys()}
  GUI_GPU_NEMAVG_OSA_BUFFER_FLUSH_FUNC       * pfBufferFlush;       // Pointer to wrapper of NemaVG OSA function \c{nema_buffer_flush()}
  GUI_GPU_NEMAVG_OSA_HOST_MALLOC_FUNC        * pfHostMalloc;        // Pointer to wrapper of NemaVG OSA function \c{nema_host_malloc()}
  GUI_GPU_NEMAVG_OSA_HOST_FREE_FUNC          * pfHostFree;          // Pointer to wrapper of NemaVG OSA function \c{nema_host_free()}
  GUI_GPU_NEMAVG_OSA_MUTEX_LOCK_FUNC         * pfMutexLock;         // Pointer to wrapper of NemaVG OSA function \c{nema_mutex_lock()}
  GUI_GPU_NEMAVG_OSA_MUTEX_UNLOCK_FUNC       * pfMutexUnlock;       // Pointer to wrapper of NemaVG OSA function \c{nema_mutex_unlock()}
  void                                      (* cbCommandListCompleted)(uint32_t last_cl_id);  // Pointer to callback that is triggered when a
                                                                                              // command list has finished processing.
  GUI_GPU_NEMAVG_OSA_DISABLE_CACHE_FUNC      * pfDisableCache;      // Pointer to wrapper of NemaVG OSA function \c{platform_disable_cache()}
  GUI_GPU_NEMAVG_OSA_INVALIDATE_CACHE_FUNC   * pfInvalidateCache;   // Pointer to wrapper of NemaVG OSA function \c{platform_invalidate_cache()}
} GUI_GPU_NEMAVG_OSA_API_STRUCT;

/*********************************************************************
*
*       Config struct
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_NEMAVG_CONFIG
*
*  Description
*    Configures the NemaVG GPU driver when it is created.
*/
typedef struct {
  void                                * pGPU2D; // Pointer to the GPU2D/NemaVG register.
  const GUI_GPU_NEMAVG_OSA_API_STRUCT * pOSA;   // Optional pointer to custom NemaVG OSA layer implementation,
                                                // can be left as NULL to use the default implementation.
} GUI_GPU_NEMAVG_CONFIG;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// These functions are only available since v1.1.7, make sure they are
// not referenced in GUI_GPU_DECLARE_NEMAVG_API if unavailable.
//
#if (NEMA_VG_API_VERSION < NEMA_VG_VERSION_1_1_7)
  #define NEMA_VG_FUNCTIONS_1_1_7         NULL,    \
                                          NULL,    \
                                          NULL,    \
                                          NULL
#else
  #define NEMA_VG_FUNCTIONS_1_1_7         nema_vg_stroke_set_cap_style,    \
                                          nema_vg_stroke_set_join_style,   \
                                          nema_vg_stroke_set_miter_limit,  \
                                          nema_vg_draw_ring_generic
#endif

/*********************************************************************
*
*       GUI_GPU_NEMAVG_API
*
*  Description
*    Macro to fill a structure of the type GUI_GPU_NEMAVG_API_STRUCT
*    with the correct function pointers.
*/
#define GUI_GPU_NEMAVG_API                                 \
  nema_init,                                               \
  nema_set_clip,                                           \
  nema_bind_dst_tex,                                       \
  nema_set_blend_fill,                                     \
  nema_cl_create_sized,                                    \
  nema_cl_bind_circular,                                   \
  nema_cl_submit,                                          \
  nema_cl_wait,                                            \
  nema_cl_rewind,                                          \
  nema_cl_destroy,                                         \
  nema_vg_init,                                            \
  (GUI_GPU_NEMAVG_VGDEINIT_FUNC *)nema_vg_deinit,          \
  nema_vg_handle_large_coords,                             \
  nema_vg_set_blend,                                       \
  nema_vg_set_fill_rule,                                   \
  nema_vg_set_quality,                                     \
  nema_vg_draw_line,                                       \
  nema_vg_draw_path,                                       \
  nema_vg_draw_rect,                                       \
  nema_vg_draw_rounded_rect,                               \
  nema_vg_draw_circle,                                     \
  (GUI_GPU_NEMAVG_PATHCREATE_FUNC *)nema_vg_path_create,   \
  nema_vg_path_set_matrix,                                 \
  nema_vg_path_set_shape,                                  \
  nema_vg_path_destroy,                                    \
  nema_vg_grad_create,                                     \
  nema_vg_grad_set,                                        \
  nema_vg_grad_destroy,                                    \
  (GUI_GPU_NEMAVG_PAINTCREATE_FUNC *)nema_vg_paint_create, \
  nema_vg_paint_set_type,                                  \
  nema_vg_paint_set_grad_linear,                           \
  nema_vg_paint_set_grad_radial2,                          \
  nema_vg_paint_set_opacity,                               \
  nema_vg_paint_set_paint_color,                           \
  nema_vg_paint_set_stroke_width,                          \
  nema_vg_paint_set_tex,                                   \
  nema_vg_paint_set_tex_matrix,                            \
  nema_vg_paint_destroy,                                   \
  NEMA_VG_FUNCTIONS_1_1_7,                                 \
  nema_rb_init

/* emDoc stop */
//
// Legacy macro
//
#define GUI_SVG_DECLARE_NEMAVG_API(VAR_NAME)   \
  static const GUI_GPU_NEMAVG_API_STRUCT VAR_NAME = { GUI_GPU_NEMAVG_API }


#if defined(__cplusplus)
}
#endif

#endif /* GUI_GPU_NEMAVG_H */

/*************************** End of file ****************************/
