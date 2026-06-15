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
File        : GUI_GPU_Dave2D.h
Purpose     : Header of Dave2D GPU driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GPU_DAVE2D_H
#define GUI_GPU_DAVE2D_H

#include "GUI_GPU.h"

/*********************************************************************
*
*       Dave2D header files
*
**********************************************************************
*/
#ifndef   GUI_GPU_DAVE2D_DRIVER_HEADER
  #define GUI_GPU_DAVE2D_DRIVER_HEADER    <dave_driver.h>
#endif

#ifdef GUI_GPU_HAS_DAVE2D
  #include GUI_GPU_DAVE2D_DRIVER_HEADER
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
#ifndef GUI_GPU_HAS_DAVE2D
  typedef void          d2_device;
  typedef void          d2_renderbuffer;
  typedef U8            d2_u8;
  typedef I16           d2_s16;
  typedef U16           d2_u16;
  typedef I32           d2_s32;
  typedef U32           d2_u32;
  typedef unsigned long d2_color;
  typedef short         d2_point;
  typedef d2_u16        d2_blitpos;
  typedef d2_s16        d2_border;
  typedef d2_s16        d2_width;
  typedef d2_u32        d2_modeflags;
  typedef d2_u32        d2_texturemodes;
  typedef d2_u32        d2_blitflags;
  typedef d2_u32        d2_blendflags;
  typedef d2_u32        d2_blendmodes;
  typedef d2_u8         d2_alpha;
#endif

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
typedef d2_device       * (GUI_GPU_DAVE2D_OPENDEVICE_FUNC)         (d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_CLOSEDEVICE_FUNC)        (d2_device * handle);
typedef d2_s32            (GUI_GPU_DAVE2D_INITHW_FUNC)             (d2_device * handle, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_DEINITHW)                (d2_device * handle);
typedef d2_renderbuffer * (GUI_GPU_DAVE2D_NEWRENDERBUFFER_FUNC)    (d2_device * handle, d2_u32 initialsize, d2_u32 stepsize);
typedef d2_s32            (GUI_GPU_DAVE2D_FREERENDERBUFFER_FUNC)   (d2_device * handle, d2_renderbuffer * buffer);
typedef d2_s32            (GUI_GPU_DAVE2D_FBBLITCOPY_FUNC)         (d2_device * handle, d2_u16 width, d2_u16 height,
                                                                    d2_blitpos srcx, d2_blitpos srcy, d2_blitpos dstx,
                                                                    d2_blitpos dsty, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_FRAMEBUFFER_FUNC)        (d2_device * handle, void * ptr, d2_s32 pitch, d2_u32 width,
                                                                    d2_u32 height, d2_s32 format);
typedef d2_s32            (GUI_GPU_DAVE2D_SELECTRENDERBUFFER_FUNC) (d2_device * handle, d2_renderbuffer * buffer);
typedef d2_s32            (GUI_GPU_DAVE2D_EXECUTERENDERBUFFER_FUNC)(d2_device * handle, d2_renderbuffer * buffer, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_FLUSHFRAME_FUNC)         (d2_device * handle);
typedef d2_s32            (GUI_GPU_DAVE2D_SETCOLOR_FUNC)           (d2_device * handle, d2_s32 index, d2_color color);
typedef d2_s32            (GUI_GPU_DAVE2D_CLIPRECT_FUNC)           (d2_device * handle, d2_border xmin, d2_border ymin,
                                                                    d2_border xmax, d2_border ymax);
typedef d2_s32            (GUI_GPU_DAVE2D_BLITCOPY_FUNC)           (d2_device * handle, d2_s32 srcwidth, d2_s32 srcheight, d2_blitpos srcx, d2_blitpos srcy, d2_width dstwidth, d2_width dstheight, d2_point dstx, d2_point dsty, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_SETBLITSRC_FUNC)         (d2_device * handle, void * ptr, d2_s32 pitch, d2_s32 width, d2_s32 height, d2_u32 format);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERBOX_FUNC)          (d2_device * handle, d2_point x1, d2_point y1, d2_width w, d2_width h);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERCIRCLE_FUNC)       (d2_device * handle, d2_point x, d2_point y, d2_width r, d2_width w);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERPOLYGON_FUNC)      (d2_device * handle, const d2_point * data, d2_u32 count, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERPOLYLINE_FUNC)     (d2_device * handle, const d2_point * data, d2_u32 count, d2_width w, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERLINE_FUNC)         (d2_device * handle, d2_point x1, d2_point y1, d2_point x2, d2_point y2, d2_width w, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_RENDERWEDGE_FUNC)        (d2_device * handle, d2_point x, d2_point y, d2_width r, d2_width w, d2_s32 nx1, d2_s32 ny1, d2_s32 nx2, d2_s32 ny2, d2_u32 flags);
typedef d2_s32            (GUI_GPU_DAVE2D_SELECTRENDERMODE_FUNC)   (d2_device * handle, d2_u32 mode);
typedef d2_s32            (GUI_GPU_DAVE2D_SETALPHABLENDMODEEX_FUNC)(d2_device * handle, d2_u32 srcfactor, d2_u32 dstfactor, d2_u32 blendflags);
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXOPPARAM_FUNC)      (d2_device * handle, d2_u32 index, d2_u32 p1, d2_u32 p2);
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXTUREOPERATION_FUNC)(d2_device * handle, d2_u8 amode, d2_u8 rmode, d2_u8 gmode, d2_u8 bmode);
typedef d2_s32            (GUI_GPU_DAVE2D_SETFILLMODE_FUNC)        (d2_device * handle, d2_u32 mode);
typedef d2_u8             (GUI_GPU_DAVE2D_GETFILLMODE_FUNC)        (d2_device * handle);
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXCLUTFORMAT_FUNC)   (d2_device * handle, d2_u32 format);
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXTURE_FUNC)         (d2_device * handle, void *ptr, d2_s32 pitch, d2_s32 width, d2_s32 height, d2_u32 format);
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXCLUT_FUNC)         (d2_device * handle, d2_color* clut );
typedef d2_s32            (GUI_GPU_DAVE2D_SETTEXTUREMAPPING_FUNC)  (d2_device * handle, d2_point x, d2_point y, d2_s32 u0, d2_s32 v0, d2_s32 dxu, d2_s32 dyu, d2_s32 dxv, d2_s32 dyv);
typedef d2_s32            (GUI_GPU_DAVE2D_SETALPHA_FUNC)           (d2_device *	handle,	d2_alpha alpha);
typedef d2_s32            (GUI_GPU_DAVE2D_SETBLENDMODE_FUNC)       (d2_device * handle, d2_u32 srcfactor, d2_u32 dstfactor);

/*********************************************************************
*
*       API struct for Dave2D
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_DAVE2D_API_STRUCT
*
*  Description
*    Maps the required functions of the TES Dave2D API.
*
*    A structure of this type can be set with GUI_GPU_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the Dave2D code (meaning \c{GUI_GPU_HAS_DAVE2D} was not defined.
*
*    For more details about what each function does, please refer to the Dave2D
*    API documentation.
*/
typedef struct {
  GUI_GPU_DAVE2D_OPENDEVICE_FUNC          * pfOpenDevice;            // Pointer to Dave2D function \c{d2_opendevice()}.
  GUI_GPU_DAVE2D_CLOSEDEVICE_FUNC         * pfCloseDevice;           // Pointer to Dave2D function \c{d2_closedevice()}.
  GUI_GPU_DAVE2D_INITHW_FUNC              * pfInitHW;                // Pointer to Dave2D function \c{d2_inithw()}.
  GUI_GPU_DAVE2D_DEINITHW                 * pfDeinitHW;              // Pointer to Dave2D function \c{d2_deinithw()}.
  GUI_GPU_DAVE2D_NEWRENDERBUFFER_FUNC     * pfNewRenderbuffer;       // Pointer to Dave2D function \c{d2_newrenderbuffer()}.
  GUI_GPU_DAVE2D_FREERENDERBUFFER_FUNC    * pfFreeRenderbuffer;      // Pointer to Dave2D function \c{d2_freerenderbuffer()}.
  GUI_GPU_DAVE2D_FBBLITCOPY_FUNC          * pfFbBlitCopy;            // Pointer to Dave2D function \c{d2_utility_fbblitcopy()}.
  GUI_GPU_DAVE2D_FRAMEBUFFER_FUNC         * pfFramebuffer;           // Pointer to Dave2D function \c{d2_framebuffer()}.
  GUI_GPU_DAVE2D_SELECTRENDERBUFFER_FUNC  * pfSelectRenderbuffer;    // Pointer to Dave2D function \c{d2_selectrenderbuffer()}.
  GUI_GPU_DAVE2D_EXECUTERENDERBUFFER_FUNC * pfExecuteRenderbuffer;   // Pointer to Dave2D function \c{d2_executerenderbuffer()}.
  GUI_GPU_DAVE2D_FLUSHFRAME_FUNC          * pfFlushFrame;            // Pointer to Dave2D function \c{d2_flushframe()}.
  GUI_GPU_DAVE2D_SETCOLOR_FUNC            * pfSetColor;              // Pointer to Dave2D function \c{d2_setcolor()}.
  GUI_GPU_DAVE2D_CLIPRECT_FUNC            * pfClipRect;              // Pointer to Dave2D function \c{d2_cliprect()}.
  GUI_GPU_DAVE2D_BLITCOPY_FUNC            * pfBlitCopy;              // Pointer to Dave2D function \c{d2_blitcopy()}.
  GUI_GPU_DAVE2D_SETBLITSRC_FUNC          * pfSetBlitSrc;            // Pointer to Dave2D function \c{d2_setblitsrc()}.
  GUI_GPU_DAVE2D_RENDERBOX_FUNC           * pfRenderbox;             // Pointer to Dave2D function \c{d2_renderbox()}.
  GUI_GPU_DAVE2D_RENDERCIRCLE_FUNC        * pfRenderCircle;          // Pointer to Dave2D function \c{d2_rendercircle()}.
  GUI_GPU_DAVE2D_RENDERPOLYGON_FUNC       * pfRenderPolygon;         // Pointer to Dave2D function \c{d2_renderpolygon()}.
  GUI_GPU_DAVE2D_RENDERPOLYLINE_FUNC      * pfRenderPolyline;        // Pointer to Dave2D function \c{d2_renderpolyline()}.
  GUI_GPU_DAVE2D_RENDERLINE_FUNC          * pfRenderLine;            // Pointer to Dave2D function \c{d2_renderline()}.
  GUI_GPU_DAVE2D_RENDERWEDGE_FUNC         * pfRenderWedge;           // Pointer to Dave2D function \c{d2_renderwedge()}.
  GUI_GPU_DAVE2D_SELECTRENDERMODE_FUNC    * pfSelectRenderMode;      // Pointer to Dave2D function \c{d2_selectrendermode()}.
  GUI_GPU_DAVE2D_SETALPHABLENDMODEEX_FUNC * pfSetAlphaBlendModeEx;   // Pointer to Dave2D function \c{d2_setalphablendmodeex()}.
  GUI_GPU_DAVE2D_SETTEXOPPARAM_FUNC       * pfSetTexOpParam;         // Pointer to Dave2D function \c{d2_settexopparam()}.
  GUI_GPU_DAVE2D_SETTEXTUREOPERATION_FUNC * pfSetTextureOperation;   // Pointer to Dave2D function \c{d2_settextureoperation()}.
  GUI_GPU_DAVE2D_SETFILLMODE_FUNC         * pfSetFillMode;           // Pointer to Dave2D function \c{d2_setfillmode()}.
  GUI_GPU_DAVE2D_GETFILLMODE_FUNC         * pfGetFillMode;           // Pointer to Dave2D function \c{d2_getfillmode()}.
  GUI_GPU_DAVE2D_SETTEXCLUTFORMAT_FUNC    * pfSetTexClutFormat;      // Pointer to Dave2D function \c{d2_settexclut_format()}.
  GUI_GPU_DAVE2D_SETTEXTURE_FUNC          * pfSetTexture;            // Pointer to Dave2D function \c{d2_settexture()}.
  GUI_GPU_DAVE2D_SETTEXCLUT_FUNC          * pfSetTexClut;            // Pointer to Dave2D function \c{d2_settexclut()}.
  GUI_GPU_DAVE2D_SETTEXTUREMAPPING_FUNC   * pfSetTextureMapping;     // Pointer to Dave2D function \c{d2_settexturemapping()}.
  GUI_GPU_DAVE2D_SETALPHA_FUNC            * pfSetAlpha;              // Pointer to Dave2D function \c{d2_setalpha()}.
  GUI_GPU_DAVE2D_SETBLENDMODE_FUNC        * pfSetBlendMode;          // Pointer to Dave2D function \c{d2_setblendmode()}.
} GUI_GPU_DAVE2D_API_STRUCT;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_DAVE2D_API
* 
*  Description
*   Macro to fill a structure of the type GUI_GPU_DAVE2D_API_STRUCT
*    filled with the correct function pointers.
*/
#define GUI_GPU_DAVE2D_API    \
  d2_opendevice,              \
  d2_closedevice,             \
  d2_inithw,                  \
  d2_deinithw,                \
  d2_newrenderbuffer,         \
  d2_freerenderbuffer,        \
  d2_utility_fbblitcopy,      \
  d2_framebuffer,             \
  d2_selectrenderbuffer,      \
  d2_executerenderbuffer,     \
  d2_flushframe,              \
  d2_setcolor,                \
  d2_cliprect,                \
  d2_blitcopy,                \
  d2_setblitsrc,              \
  d2_renderbox,               \
  d2_rendercircle,            \
  d2_renderpolygon,           \
  d2_renderpolyline,          \
  d2_renderline,              \
  d2_renderwedge,             \
  d2_selectrendermode,        \
  d2_setalphablendmodeex,     \
  d2_settexopparam,           \
  d2_settextureoperation,     \
  d2_setfillmode,             \
  d2_getfillmode,             \
  d2_settexclut_format,       \
  d2_settexture,              \
  d2_settexclut,              \
  d2_settexturemapping,       \
  d2_setalpha,                \
  d2_setblendmode

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_DAVE2D_API_CONTEXT
* 
*  Description
*    Structure that holds the contexts created by the Dave2D API.
*    This structure type is used as the return type when calling
*    GUI_GPU_AcquireContext(GUI_GPU_DAVE2D).
*/
typedef struct {
  d2_device       * d2_handle;      // Main Dave2D device instance.
  d2_renderbuffer * renderbuffer;   // Instance of type d2_renderbuffer.
} GUI_GPU_DAVE2D_API_CONTEXT;

#if defined(__cplusplus)
}
#endif

#endif /* GUI_GPU_DAVE2D_H */

/*************************** End of file ****************************/
