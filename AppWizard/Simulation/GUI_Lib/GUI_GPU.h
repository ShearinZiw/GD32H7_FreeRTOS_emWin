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
File        : GUI_GPU.h
Purpose     : Main module of GPU driver devices.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GPU_H
#define GUI_GPU_H

#include "GUI.h"
#include "GUI_VG.h"

/*********************************************************************
*
*       Standard library fixed size types
*
**********************************************************************
*/
#ifdef WIN32
  #if (_MSC_VER <= 1900)                      // Older compiler version do not know about the header
    typedef char                 int8_t;
    typedef unsigned char        uint8_t;
    typedef short                int16_t;
    typedef unsigned short       uint16_t;
    typedef int                  int32_t;
    typedef unsigned int         uint32_t;
    #if (_MSC_VER > 1200)                     // VC6 does not support typedef (unsigned) long long 
      typedef long long          int64_t;
      typedef unsigned long long uint64_t;
    #endif
  #else
    #include <stdint.h>
  #endif
#else
  #include <stdint.h>
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Redirect 3rd party typename to the same type defined as GUI_...
//
#define REDIRECT_TYPE(TYPE_NAME)    typedef GUI_##TYPE_NAME TYPE_NAME
//
// Used to define a given 3rd party type with public members,
// required when the named members are used in the source file.
//
#define DEFINE_STRUCT_TYPE(TYPE_NAME, MEMBERS)   \
  typedef struct {                               \
    MEMBERS                                      \
  } GUI_##TYPE_NAME;                             \
  REDIRECT_TYPE(TYPE_NAME)
//
// Make sure to rename the enum values!
//
#define DEFINE_ENUM_TYPE(TYPE_NAME) GUI_##TYPE_NAME;          \
                                    REDIRECT_TYPE(TYPE_NAME)
//
// Stringify defines
//
#define GUI_STRINGIFY(x)   #x
#define GUI_MAKE_STR(x)    GUI_STRINGIFY(x)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_HOOK_INDEX
*
*  Description
*    Index values for association of hook functions set to a GPU driver.
*    See the function GUI_GPU_RegisterHook().
*/
typedef enum {
  GUI_GPU_HOOK_PREINITDRIVER,    // Hook to be called before the GPU driver is initialized.
                                 // If it returns 1, the initialization is cancelled.
  GUI_GPU_HOOK_INITDRIVER,       // Hook to be called during the GPU driver is initialized.
                                 // If it returns 1, the initialization is cancelled.
  GUI_GPU_HOOK_POSTINITDRIVER,   // Hook to be called after the GPU driver is initialized.
                                 // If it returns 1, the initialization is cancelled.
  GUI_GPU_HOOK_EXITDRIVER,       // Hook to be called when the GPU driver is de-initialized.
  GUI_GPU_HOOK_BINDAPI,          // Hook to be called to bind the API to the driver before it is initialized.
  GUI_GPU_HOOK_SWITCHBUFFER,     // Hook to be called after the frame has been completely drawn.
  GUI_GPU_HOOK_BEGINDRAW,        // Hook to be called before a drawing operation.
                                 // If it returns 1, the draw is cancelled.
  GUI_GPU_HOOK_ENDDRAW,          // Hook to be called after a drawing operation has finished.
  GUI_GPU_HOOK_CALLHOOK,         // Hook called before another GPU hook is called.
                                 // If it returns 1, calling the hook will be skipped.
  GUI_GPU_HOOK_COUNT,            // Number of hooks.
} GUI_GPU_HOOK_INDEX;

/*********************************************************************
*
*       GUI_VOID_FUNCTION
*
*  Description
*    A void function without parameters. It is often used as an abstract
*    type for generic function pointers.
*/
typedef void (GUI_VOID_FUNCTION)(void);

/*********************************************************************
*
*       GUI_GPU_FUNC_DRAWBITMAP
*
*  Description
*    Function prototype for any kind of function that renders
*    a bitmap onto the screen.
*
*  Parameters
*    x:            X position where to render the bitmap in the current destination buffer.
*    y:            Y position where to render the bitmap in the current destination buffer.
*    p:       [IN] Pointer to bitmap data.
*    xSize:        X size of the bitmap.
*    ySize:        Y size of the bitmap.
*    BytesPerLine: Bytes per line (stride) of the bitmap.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_DRAWBITMAP)(int x, int y, const void * p, int xSize, int ySize, int BytesPerLine);

/*********************************************************************
*
*       GUI_GPU_FUNC_DRAWBITMAPEX
*
*  Description
*    Drawing a bitmap scaled and/or mirrored with GUI_DrawBitmapEx().
* 
*  Parameters
*    x:            X position where to render the bitmap in the current destination buffer.
*    y:            Y position where to render the bitmap in the current destination buffer.
*    p:       [IN] Pointer to bitmap data.
*    xSize:        X size of the bitmap.
*    ySize:        Y size of the bitmap.
*    BytesPerLine: Bytes per line (stride) of the bitmap.
*    xMag:         Scale factor of X-direction in the unit 1/1000. A negative value mirrors the x-axis.
*    yMag:         Scale factor of Y-direction in the unit 1/1000. A negative value mirrors the y-axis.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_DRAWBITMAPEX)(int x, int y, const void * p, int xSize, int ySize, int BytesPerLine, int xMag, int yMag);

/*********************************************************************
*
*       GUI_GPU_FUNC_ALPHABLENDING
*
*  Description
*    Function prototype for alpha blending color buffers.
* 
*  Parameters
*    pFG:     [IN]  Pointer to foreground buffer to be mixed.
*    pBG:     [IN]  Pointer to background buffer to be mixed.
*    pDst:    [OUT] Pointer to destination buffer for the result.
*    NumItems:      Number of colors to be blended.
*/
typedef void (GUI_GPU_FUNC_ALPHABLENDING)(LCD_COLOR * pFG, LCD_COLOR * pBG, LCD_COLOR * pDest, U32 NumItems);

/*********************************************************************
*
*       GUI_GPU_FUNC_MIXCOLORSBULK
*
*  Description
*    Function prototype for bulk color mixing. It is mainly used
*    for blending memory devices.
* 
*  Parameters
*    pFG:     [IN]  Pointer to foreground buffer to be mixed.
*    pBG:     [IN]  Pointer to background buffer to be mixed.
*    pDst:    [OUT] Pointer to destination buffer for the result.
*    OffFG:   Pixel offset to pFG.
*    OffBG:   Pixel offset to pBG.
*    OffDest: Pixel offset to pDst.
*    xSize:   X size of the memory device.
*    ySize:   Y size of the memory device.
*    Intens:  Blending intensity.
*/
typedef void (GUI_GPU_FUNC_MIXCOLORSBULK)(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);

/*********************************************************************
*
*       GUI_GPU_FUNC_ARC
*
*  Description
*    Function prototype for drawing an arc.
* 
*  Parameters
*    x0: X position of center point of the arc.
*    y0: Y position of center point of the arc.
*    rx: Horizontal radius of the arc.
*    ry: Vertical radius of the arc.
*    a0: Starting angle (1000 = 1\deg).
*    a1: Ending angle (1000 = 1\deg).
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_ARC)(int x0, int y0, int rx, int ry, I32 a0, I32 a1);

/*********************************************************************
*
*       GUI_GPU_FUNC_CIRCLE
*
*  Description
*    Function prototype for drawing a circle.
* 
*  Parameters
*    x0: X position of center point of the circle.
*    y0: Y position of center point of the circle.
*    r:  Radius of the circle.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_CIRCLE)(int x0, int y0, int r);

/*********************************************************************
*
*       GUI_GPU_FUNC_LINE
*
*  Description
*    Function prototype for drawing a line.
* 
*  Parameters
*    x0: X position of start point of the line.
*    y0: Y position of start point of the line.
*    x1: X position of end point of the line.
*    y1: Y position of end point of the line.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_LINE)(int x0, int y0, int x1, int y1);

/*********************************************************************
*
*       GUI_GPU_FUNC_RECT
*
*  Description
*    Function prototype for drawing a rectangle.
* 
*  Parameters
*    x0: X position of upper-left corner of the rectangle.
*    y0: Y position of upper-left corner of the rectangle.
*    x1: X position of bottom-right corner of the rectangle.
*    y1: Y position of bottom-right corner of the rectangle.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_RECT)(int x0, int y0, int x1, int y1);

/*********************************************************************
*
*       GUI_GPU_FUNC_ROUNDEDRECT
*
*  Description
*    Function prototype for drawing a rounded rectangle.
* 
*  Parameters
*    x0: X position of upper-left corner of the rectangle.
*    y0: Y position of upper-left corner of the rectangle.
*    x1: X position of bottom-right corner of the rectangle.
*    y1: Y position of bottom-right corner of the rectangle.
*    r:  Radius of rounded rectangle.
*  
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_ROUNDEDRECT)(int x0, int y0, int x1, int y1, int r);

/*********************************************************************
*
*       GUI_GPU_FUNC_POLYGON
*
*  Description
*    Function prototype for drawing a polygon.
* 
*  Parameters
*    pSrc:      [IN] Pointer to points of the polygon.
*    NumPoints: Number of points on array pointed to by pSrc.
*    x:         X position to draw the polygon at.
*    y:         Y position to draw the polygon at.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_POLYGON)(const GUI_POINT * pSrc, int NumPoints, int x, int y);

/*********************************************************************
*
*       GUI_GPU_FUNC_MEMDEV_BLEND
*
*  Description
*    Function prototype for blending a color into a memory device.
* 
*  Parameters
*    hMem:        Memory device to be blended.
*    Color:       Color to be used for blending.
*    BlendIntens: Alpha value used for blending the color into hMem.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_MEMDEV_BLEND)(GUI_MEMDEV_Handle hMem, GUI_COLOR Color, U8 BlendIntens);

/*********************************************************************
*
*       GUI_GPU_FUNC_MEMDEV_ROTATE
*
*  Description
*    Function prototype for rotating and scaling a memory device.
* 
*  Parameters
*    hSrc: Source memory device to be rotated and scaled.
*    hDst: Destination memory device.
*    dx:   X translation offset.
*    dy:   Y translation offset.
*    a:    Rotation angle (1000 = 1\deg).
*    Mag:  Scaling value (1000 = 1x)
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_MEMDEV_ROTATE)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, float dx, float dy, int a, int Mag);

/*********************************************************************
*
*       GUI_GPU_FUNC_COLORTOINDEX
*
*  Description
*    Function prototype for bulk conversion of colors to a given index.
* 
*  Parameters
*    pColor:      [IN]  Pointer to buffer of colors to be converted.
*    pIndex:      [OUT] Pointer to buffer of converted indices.
*    NumItems:    Number of colors to be converted.
*    SizeOfIndex: Byte size of one color index.
*/
typedef void (GUI_GPU_FUNC_COLORTOINDEX)(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex);

/*********************************************************************
*
*       GUI_GPU_FUNC_INDEXTOCOLOR
*
*  Description
*    Function prototype for bulk conversion of a given index to color.
*
*  Parameters
*    pIndex:      [IN]  Pointer to buffer of indices to be converted.
*    pColor:      [OUT] Pointer to buffer of converted colors.
*    NumItems:    Number of indices to be converted.
*    SizeOfIndex: Byte size of one color index.
*/
typedef void (GUI_GPU_FUNC_INDEXTOCOLOR)(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex);

/*********************************************************************
*
*       GUI_GPU_FUNC_GETPALCONVTABLE
* 
*  Description
*     Function prototype to calculate the index values to be used for the given
*     palette when drawing a palette-based bitmap.
* 
*  Parameters
*    pLogPal:    [IN]  Pointer to palette to be converted to indices.
*    pBitmap:    [IN]  Pointer to bitmap to be drawn.
*    ppReturn:   [OUT] Resulting pointer to converted indices. Returns NULL on fail.
*/
typedef void (GUI_GPU_FUNC_GETPALCONVTABLE)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, LCD_PIXELINDEX ** ppReturn);

/*********************************************************************
*
*       GUI_GPU_FUNC_COPYRECT
*
*  Description
*    Function prototype for copying a rectangular area on the screen
*    to a given position on the screen.
* 
*  Parameters
*    x0:    X position of upper-left corner of rectangle on the screen to be copied.
*    y0:    Y position of upper-left corner of rectangle on the screen to be copied.
*    x1:    X position of the destination where the rectangle should be copied to.
*    y1:    Y position of the destination where the rectangle should be copied to.
*    xSize: X size of the rectangle to be copied.
*    ySize: Y size of the rectangle to be copied.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_COPYRECT)(int x0, int y0, int x1, int y1, int xSize, int ySize);

/*********************************************************************
*
*       GUI_GPU_FUNC_COPYBUFFERRECT
*
*  Description
*    Function prototype for copying rectangular areas of frame buffers.
* 
*  Parameters
*    pSrc:    [IN] Source address of pixels to be copied.
*    pDst:    [IN] Destination address where pixels should be copied.
*    xSize:        X size of area to be copied.
*    ySize:        Y size of area to be copied.
*    BytesPerLine: Stride value (number of bytes per line) of pSrc and pDst.
* 
*  Return value
*    == 1: If hardware operation failed.
*    == 0: On success.
*/
typedef int (GUI_GPU_FUNC_COPYBUFFERRECT)(const void * pSrc, void * pDst, int xSize, int ySize, int BytesPerLine);

/*********************************************************************
*
*       GUI_GPU_FUNC_GRADIENT
*
*  Description
*    Function prototype for drawing linear gradients.
* 
*  Parameters
*    x0:     X position of upper-left corner of the gradient.
*    y0:     Y position of upper-left corner of the gradient.
*    x1:     X position of lower-right corner of the gradient.
*    y1:     Y position of lower-right corner of the gradient.
*    Color0: Starting color of the gradient.
*    Color1: Ending color of the gradient.
*    Axis:   GUI_HORIZONTAL for horizontal gradients, GUI_VERTICAL for
*            vertical gradients.
*    r:      Radius of the gradient, if it has rounded corners.
* 
*  Return value
*    == 1: If hardware failed to render.
*    == 0: On successful rendering.
*/
typedef int (GUI_GPU_FUNC_GRADIENT)(int x0, int y0, int x1, int y1, GUI_COLOR Color0, GUI_COLOR Color1, int Axis, int r);

/*********************************************************************
*
*       GUI_GPU_LOAD_API_CALLBACK
*
*  Description
*    Callback used to load a given routine. The callback receives the
*    name of the routine to be loaded as a string and should return its
*    address as a void function pointer.
* 
*    The main use case of this callback is to load a function from a DLL.
* 
*  Parameters
*    sFunction: [IN] Name of the routine to be loaded as a zero-terminated
*                    string.
*  
*  Return value
*    Address of the function casted to a void function pointer.
*/
typedef GUI_VOID_FUNCTION * (GUI_GPU_LOAD_API_CALLBACK)(const char * sFunction);

/*********************************************************************
*
*       GUI_DTA_GET_DATA_FUNC
* 
*  Description
*    GetData function used for GPU threshold files (.gput), for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef GUI_GET_DATA_FUNC_II   GUI_GPU_GET_DATA_FUNC;


/*********************************************************************
*
*       GUI_GPU_OS_GET_TIME_US
* 
*  Description
*    Returns the current time as microseconds.
* 
*  Return value
*    Current time in microseconds.
*/
typedef U64 (GUI_GPU_OS_GET_TIME_US)(void);

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_ChromART     (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_PixelPipeline(void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_Dave2D       (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_OpenVG       (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_VGLite       (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_NemaVG       (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_NanoVG       (void);
const GUI_GPU_DRIVER_STRUCT * GUI_GPU_Null         (void);

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
GUI_GPU_DRIVER * GUI_GPU_GetFirst              (void);
GUI_GPU_DRIVER * GUI_GPU_GetLast               (void);
GUI_GPU_DRIVER * GUI_GPU_GetNext               (GUI_GPU_DRIVER * pDriver);
GUI_GPU_DRIVER * GUI_GPU_GetPrevious           (GUI_GPU_DRIVER * pDriver);
int              GUI_GPU_Create                (GUI_GPU_DRIVER * pDriver, void * pConfig);
int              GUI_GPU_Delete                (GUI_GPU_DRIVER * pDriver);
int              GUI_GPU_IsEnabled             (GUI_GPU_DRIVER * pDriver);
int              GUI_GPU_IsFunctionEnabled     (GUI_GPU_DRIVER * pDriver, GUI_GPU_FUNCTION_INDEX Index);
void             GUI_GPU_Enable                (GUI_GPU_DRIVER * pDriver, int Enable);
void             GUI_GPU_EnableFunction        (GUI_GPU_DRIVER * pDriver, GUI_GPU_FUNCTION_INDEX Index, int Enable);
int              GUI_GPU_HasFunction           (GUI_GPU_DRIVER * pDriver, GUI_GPU_FUNCTION_INDEX Index);
int              GUI_GPU_SetFunction           (GUI_GPU_DRIVER * pDriver, GUI_GPU_FUNCTION_INDEX Index, GUI_VOID_FUNCTION * pFunc);
void             GUI_GPU_RegisterHook          (GUI_GPU_DRIVER * pDriver, GUI_GPU_HOOK_INDEX Index, GUI_VOID_FUNCTION * pfHook);
void             GUI_GPU_IRQHandler            (GUI_GPU_DRIVER * pDriver, int Index);
void             GUI_GPU_ClearThresholds       (GUI_GPU_DRIVER * pDriver);
void             GUI_GPU_SetThreshold          (GUI_GPU_DRIVER * pDriver, GUI_GPU_FUNCTION_INDEX Index, int Threshold);
int              GUI_GPU_LoadThresholdFile     (GUI_GPU_DRIVER * pDriver, const U8 * pFile, unsigned NumBytes);
int              GUI_GPU_LoadThresholdFileEx   (GUI_GPU_DRIVER * pDriver, GUI_GPU_GET_DATA_FUNC * pfGetData, void * p);
int              GUI_GPU_SerializeThresholdFile(GUI_GPU_DRIVER * pDriver, GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p, GUI_GPU_OS_GET_TIME_US * pfGetTime);
//
// For drivers depending on 3rd party APIs
//
int    GUI_GPU_HasBoundAPI   (GUI_GPU_DRIVER * pDriver);
void   GUI_GPU_BindAPI       (GUI_GPU_DRIVER * pDriver, const void * pAPI);
void   GUI_GPU_BindDynamicAPI(GUI_GPU_DRIVER * pDriver, GUI_GPU_LOAD_API_CALLBACK * cbLoadFunction);
int    GUI_GPU_SetVersion    (GUI_GPU_DRIVER * pDriver, U8 Major, U8 Minor, U8 Bugfix);
void * GUI_GPU_AcquireContext(GUI_GPU_DRIVER * pDriver, void ** ppContext);
int    GUI_GPU_ReleaseContext(GUI_GPU_DRIVER * pDriver);

/*********************************************************************
*
*       Compatability support for old GUI_SVG_DRIVER_... API
*
**********************************************************************
*/
// 
// The pointer passed to identify a driver is a function
// so that only the directly referenced drivers are linked.
//
#define GUI_SVG_DRIVER_OPENVG    GUI_GPU_OPENVG
#define GUI_SVG_DRIVER_VGLITE    GUI_GPU_VGLITE
#define GUI_SVG_DRIVER_NEMAVG    GUI_GPU_NEMAVG
#define GUI_SVG_DRIVER_NANOVG    GUI_GPU_NANOVG

#define GUI_SVG_DRIVER   GUI_GPU_DRIVER

//
// GUI_SVG_DRIVER API
//
GUI_GPU_DRIVER * GUI_SVG_GetSelectedGPU (void);
int              GUI_SVG_SelectGPU      (GUI_GPU_DRIVER * pDriver);
void             GUI_SVG_SetGPUHooks    (GUI_GPU_DRIVER * pDriver, const GUI_SVG_HOOKS * pHooks);

//
// Compatability macros
//
#define GUI_SVG_DRIVER_BindDynamicAPI(pDrv, pAPI, cb)   GUI_GPU_BindDynamicAPI(pDrv, cb)
#define GUI_SVG_DRIVER_BindAPI                          GUI_GPU_BindAPI
#define GUI_SVG_DRIVER_HasBoundAPI                      GUI_GPU_HasBoundAPI
#define GUI_SVG_DRIVER_SetVersion                       GUI_GPU_SetVersion
#define GUI_SVG_DRIVER_GetSelected                      GUI_SVG_GetSelectedGPU
#define GUI_SVG_DRIVER_Select                           GUI_SVG_SelectGPU
#define GUI_SVG_DRIVER_SetHooks                         GUI_SVG_SetGPUHooks

#endif /* GUI_GPU_H */

/*************************** End of file ****************************/
