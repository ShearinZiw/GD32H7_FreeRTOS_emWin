/*********************************************************************
*                        SEGGER Software GmbH                        *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*               (c) 1996 - 2023  SEGGER Software GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.52 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  GigaDevice Semiconductor Inc. whose
registered office is situated at  Building No.8, IC Park, No.9 Fenghao
East Road, Haidian, Beijing, China solely for the purposes of creating
libraries  for  ARM Cortex-M processor-based  devices, sublicensed and
distributed  under  the  terms  and conditions of the End User License
Agreement supplied by GigaDevice Semiconductor Inc.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              GigaDevice Semiconductor Inc., Building No.8, IC Park, No.9 Fenghao East Road, Beijing Haidian, , CHINA
Licensed SEGGER software: emWin
License number:           GUI-00918
License model:            Buyout SRC [Buyout Source Code License], according to Buyout Agreement signed Sep. 12th 2023 by Ivo Geilenbruegge and Aug. 31th 2023 by Guangyi Jin
Licensed product:         Any
Licensed platform:        Cortex-M
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2023-08-11 - 2026-08-26
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "LCDConf.h"
#include "PIDConf.h"

#include "MIMXRT1176_cm7.h"
#include "pin_mux.h"

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_clock.h"
#include "fsl_mipi_dsi.h"
#include "fsl_pxp.h"
#include "clock_config.h"
#include "display_support.h"

#include "fsl_mipi_dsi.h"
#include "fsl_rm68200.h"

#include "fsl_dc_fb_lcdifv2.h"

#include <math.h>

#include "vg_lite.h"
#include "vg_lite_platform.h"
#include "vg_lite_kernel.h"

/*********************************************************************
*
*       OS configuration
*
**********************************************************************
*/
#define USE_OS  (1)

#if (USE_OS == 1)
  #define EMBOS     (1)
  #define FREERTOS  (2)
  #define OS EMBOS

  #if   (OS == EMBOS)
    #include "RTOS.h"
    #define  SEMAPHORE_TYPE                OS_SEMAPHORE
    #define  SEMAPHORE_HANDLE_TYPE         OS_SEMAPHORE *
    #define  MAKE_SEMAPHORE_HANDLE(sema)   &sema
  #elif (OS == FREERTOS)
    #include "FreeRTOS.h"
    #include "semphr.h"
    #define  SEMAPHORE_TYPE                SemaphoreHandle_t
    #define  SEMAPHORE_HANDLE_TYPE         SemaphoreHandle_t
    #define  MAKE_SEMAPHORE_HANDLE(sema)   sema
  #else
    #error "No RTOS selected"
  #endif
#endif

/*********************************************************************
*
*       LCD configuration (TO BE MODIFIED)
*
**********************************************************************
*/
//
// 16 or 32 bpp supported.
//
#define LCD_BITS_PER_PIXEL   32

//
// Buffers / VScreens
//
#define NUM_BUFFERS  2 // Number of multiple buffers to be used (at least 1 buffer)
#define NUM_VSCREENS 1 // Number of virtual  screens to be used (at least 1 screen)

//
// Define number of layers for this configuration file. Must be equal or less than in GUIConf.h!
//
#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 1

//
// Default display orientation. Should be one of the above defined display orientations.
//
#define ORIENTATION     GUI_ROTATION_CCW

//
// Allow run-time display rotation. Necessary for AppWizard ROTATEDISPLAY job.
//
#define ENABLE_DISPLAY_ROTATION   1

//
// Some emWin operations require running the PXP multiple times. For those cases,
// a temporary buffer is needed. Set the below define to 1 to use a persistent buffer
// as the temporary buffer without the need for dynamic memory allocations (improves performance).
// In case enough RAM is available, we would definitely recommend to keep this define enabled.
//
#define USE_PXP_BUFFER    1


/*********************************************************************
*
*       LCD configuration (NOT TO BE MODIFIED)
*
**********************************************************************
*/
#define LCD_BYTES_PER_PIXEL    (LCD_BITS_PER_PIXEL / 8)
//
// Physical display size (not to be changed).
//
#define XSIZE_PHYS 720
#define YSIZE_PHYS 1280
//
// Porch values
//
#define HSW    8
#define HFP    32
#define HBP    32
#define VSW    2
#define VFP    16
#define VBP    14

#define LCDIF_POL_FLAGS  (kLCDIFV2_DataEnableActiveHigh | kLCDIFV2_VsyncActiveLow | kLCDIFV2_HsyncActiveLow | \
                               kLCDIFV2_DriveDataOnFallingClkEdge)

#define MIPI_DSI          (&g_mipiDsi)
#define MIPI_DSI_LANE_NUM 2
//
// Mipi pins
//
#define MIPI_PANEL_RST_GPIO   GPIO9
#define MIPI_PANEL_RST_PIN    1
#define MIPI_PANEL_POWER_GPIO GPIO11
#define MIPI_PANEL_POWER_PIN  16
//
// Backlight
//
#define MIPI_PANEL_BL_GPIO GPIO9
#define MIPI_PANEL_BL_PIN  29

#define MIPI_DPHY_BIT_CLK_ENLARGE(origin) (((origin) / 8) * 9)
//
// Master read priority
//
#define PXP_READ_QOS    *((U32 *)0x41043100)

/*********************************************************************
*
*       Automatic selection of driver and color conversion
*/
#if LCD_BITS_PER_PIXEL == 16
  #define COLOR_CONVERSION    GUICC_M565
  #define PIXEL_FORMAT        kVIDEO_PixelFormatRGB565
  #define LCD_DATA_BUS_WIDTH  kLCDIFV2_DataBus16Bit
  #define PXP_OUT_FORMAT      kPXP_OutputPixelFormatRGB565
  #define PXP_PS_FORMAT       kPXP_PsPixelFormatRGB565
  #define PXP_AS_FORMAT       kPXP_AsPixelFormatRGB565
  #define VGLITE_DST_FORMAT   VG_LITE_BGR565
  //
  #if   (ORIENTATION == GUI_ROTATION_0)
    #define DISPLAY_DRIVER      GUIDRV_LIN_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSY_16
  #elif (ORIENTATION == GUI_ROTATION_CW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_16
  #elif (ORIENTATION == GUI_ROTATION_180)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OXY_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSX_16
  #elif (ORIENTATION == GUI_ROTATION_CCW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSY_16
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_16
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSX_16
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OXY_16
  #endif
#elif LCD_BITS_PER_PIXEL == 32
  #define COLOR_CONVERSION    GUICC_M8888I
  #define PIXEL_FORMAT        kVIDEO_PixelFormatXRGB8888
  #define LCD_DATA_BUS_WIDTH  kLCDIFV2_DataBus16Bit
  #define PXP_OUT_FORMAT      kPXP_OutputPixelFormatARGB8888
  #define PXP_PS_FORMAT       kPXP_PsPixelFormatRGB888
  #define PXP_AS_FORMAT       kPXP_AsPixelFormatARGB8888
  #define VGLITE_DST_FORMAT   VG_LITE_BGRX8888
  //
  #if   (ORIENTATION == GUI_ROTATION_0)
    #define DISPLAY_DRIVER      GUIDRV_LIN_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSY_32
  #elif (ORIENTATION == GUI_ROTATION_CW)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_32
  #elif (ORIENTATION == GUI_ROTATION_180)
    #define DISPLAY_DRIVER      GUIDRV_LIN_OXY_32
    #define DISPLAY_DRIVER_CW   GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_180  GUIDRV_LIN_32
    #define DISPLAY_DRIVER_CCW  GUIDRV_LIN_OSX_32
  #elif (ORIENTATION == GUI_ROTATION_CCW)
    #define DISPLAY_DRIVER     GUIDRV_LIN_OSY_32
    #define DISPLAY_DRIVER_CW  GUIDRV_LIN_32
    #define DISPLAY_DRIVER_180 GUIDRV_LIN_OSX_32
    #define DISPLAY_DRIVER_CCW GUIDRV_LIN_OXY_32
  #endif
#else
  #error "Selected color depth is not supported in this configuration!"
#endif

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR (U32)(&_aVRAM0[0])
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#if ((LCD_BITS_PER_PIXEL != 16) && (LCD_BITS_PER_PIXEL != 32))
  #error Invalid display bit depth defined!
#endif

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
static void _LCD_PXP_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat);
static void _LCD_PXP_Index2ColorBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat);

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Buffer related
//
#define LCD_XY2BufferPtr(x, y)     (U32)((U8 *)_aBufferAddress[_BackBuffer] + ((y * XSIZE_PHYS) + x) * LCD_BYTES_PER_PIXEL)
//
// PXP macros
//
#define PXP_PS_InvalidFormat           kPXP_PsPixelFormatY4     // Unused format, used here as invalid format flag
#define PXP_DisableSurface(surf)       PXP_Set##surf##SurfacePosition(PXP, 0xFFFFu, 0xFFFFu, 0u, 0u)
#define PXP_DisableProcessSurface()    PXP_DisableSurface(Process)
#define PXP_DisableAlphaSurface()      PXP_DisableSurface(Alpha)

#define DEFINE_PXP_COLOR2INDEX(SUFFIX, FORMAT)                                                                     \
  static void _LCD_PXP_Color2IndexBulk_##SUFFIX(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) { \
    _LCD_PXP_Color2IndexBulk(pColor, pIndex, NumItems, SizeOfIndex, FORMAT);                                       \
  }

#define DEFINE_PXP_INDEX2COLOR(SUFFIX, FORMAT)                                                                     \
  static void _LCD_PXP_Index2ColorBulk_##SUFFIX(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) { \
    _LCD_PXP_Index2ColorBulk(pIndex, pColor, NumItems, SizeOfIndex, FORMAT);                                       \
  }

#define DEFINE_PXP_COLORCONV(SUFFIX, FORMAT)  \
  DEFINE_PXP_COLOR2INDEX(SUFFIX, FORMAT)      \
  DEFINE_PXP_INDEX2COLOR(SUFFIX, FORMAT)

#define GUI_PXP_SETCOLORCONV(FORMAT)   GUICC_##FORMAT##_SetCustColorConv((Enable) ? _LCD_PXP_Color2IndexBulk_##FORMAT : NULL, \
                                                                         (Enable) ? _LCD_PXP_Index2ColorBulk_##FORMAT : NULL)
#ifndef GUI_CEIL
  #define GUI_CEIL ceilf
#endif
//
// VGLITE macros
//
#define F32 float
#define F64 double

#define VG_LITE_PATH_BUFFER_SIZE   (1024 * 4)
#define VG_LITE_MINIMUM_IMAGE_SIZE 16

#define CONVERT_HIRES(x)                  ( (GUI_AA_IsHiResEnabled()) ? ( (F32)x / (F32)GUI_AA_GetFactor() ) : (F32)x)
#define CONVERT_HIRES_COORD(xPixel, yPixel, FloatCoord)  FloatCoord.x = CONVERT_HIRES(xPixel); \
                                                         FloatCoord.y = CONVERT_HIRES(yPixel)

#define WRITE_F32(p, Payload)    *((F32 *)p) = Payload; \
                                 p += sizeof(F32)
#define WRITE_I8(p, Payload)     *((I32 *)p) = 0;       \
                                 *((I8  *)p) = Payload; \
                                 p += sizeof(I32)
//
// Writing VGLITE operations to a buffer.
//
#define WRITE_MOVE(p, x, y)                      WRITE_I8 (p, VLC_OP_MOVE); \
                                                 WRITE_F32(p, x);           \
                                                 WRITE_F32(p, y)
#define WRITE_LINE(p, x, y)                      WRITE_I8 (p, VLC_OP_LINE); \
                                                 WRITE_F32(p, x);           \
                                                 WRITE_F32(p, y)
#define WRITE_ARC(p, OpCode, rh, rv, rot, x, y)  WRITE_I8 (p, OpCode);      \
                                                 WRITE_F32(p, rh);          \
                                                 WRITE_F32(p, rv);          \
                                                 WRITE_F32(p, rot);         \
                                                 WRITE_F32(p, x);           \
                                                 WRITE_F32(p, y)
#define WRITE_END(p)                             WRITE_I8 (p, VLC_OP_END)

#define VG_RET_ON_ERR(Fx)         \
  err = Fx;                       \
  if (err) {                      \
    return 1;                     \
  }

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef enum {
  BUFFER_TYPE_FRAMEBUFFER = 0,
  BUFFER_TYPE_PROCESS_SURFACE,
  BUFFER_TYPE_ALPHA_SURFACE,
} BUFFER_TYPE;

typedef enum {
  BLEND_MODE_NORMAL     = (1 << 0),
  BLEND_MODE_MULTIPLY   = (1 << 1),
  BLEND_MODE_PORTERDUFF = (1 << 2),
} BLEND_MODE;

typedef struct {
  void      * p;
  int         xSize, ySize;
  int         BytesPerLine;
  bool        OverwriteAlpha;
  BUFFER_TYPE Type;
  union {
    pxp_output_pixel_format_t Out;
    pxp_ps_pixel_format_t     Ps;
    pxp_as_pixel_format_t     As;
  } Format;
} BUFFER_OBJ;

typedef struct {
  BLEND_MODE                   Mode;
  U8                           Alpha;
  pxp_porter_duff_blend_mode_t PorterDuffMode;
} PXP_BLEND_INFO;

typedef struct {
  int              x, y;
  int              xSize, ySize;
  BUFFER_OBJ     * pOutputBuffer;
  BUFFER_OBJ     * pProcessSurface;
  BUFFER_OBJ     * pAlphaSurface;
  PXP_BLEND_INFO * pBlend;
} LCD_PXP_DRAW_CONTEXT;

typedef struct {
  int * pxSizeRoundUp;
  int * pySizeRoundUp;
  int * pxSizeRoundDown;
  int * pySizeRoundDown;
} LCD_PXP_SIZE_CONTEXT;

//
// VGLITE related
//
typedef struct {
  float min_x, min_y, max_x, max_y;
} vg_lite_bbox_t;

typedef struct {
  float x, y;
} vg_lite_coord_t;

typedef enum {
  VG_PATH_MODE_FILL    = (1 << 0),
  VG_PATH_MODE_STROKE  = (1 << 1),
  VG_PATH_MODE_AA      = (1 << 2),
  VG_PATH_MODE_BLIT    = (1 << 3),
  VG_PATH_MODE_PATTERN = (1 << 4),
} VG_PATH_MODE;

#define VG_DRAW_AA  (VG_PATH_MODE_STROKE | VG_PATH_MODE_AA)
#define VG_FILL_AA  (VG_PATH_MODE_FILL   | VG_PATH_MODE_AA)
#define VG_DRAW     (VG_PATH_MODE_STROKE)
#define VG_FILL     (VG_PATH_MODE_FILL)

typedef struct {
  void               * pPathData;
  U32                  NumBytes;
  vg_lite_bbox_t       BBox;
  vg_lite_path_t       Path;
  vg_lite_matrix_t     PatternMatrix;
  vg_lite_buffer_t     SrcBuffer;
  vg_lite_error_t   (* pfInitPath)(vg_lite_path_t*, vg_lite_format_t,
                                   vg_lite_quality_t, uint32_t, void*,
                                   vg_lite_float_t, vg_lite_float_t,
                                   vg_lite_float_t, vg_lite_float_t);
} LCD_VGLITE_PATH_DATA;

#define BUFFER_ALIGNMENT     __attribute__((aligned(32)))

//
// Framebuffer
//
U8                  _aVRAM0[XSIZE_PHYS * YSIZE_PHYS * NUM_BUFFERS * NUM_VSCREENS * LCD_BYTES_PER_PIXEL] BUFFER_ALIGNMENT;
static volatile int _PendingBuffer = -1;
static volatile int _BackBuffer    =  0;
//
// Buffer used for various PXP operations requiring two buffers, as well as CopyRect().
//
#if USE_PXP_BUFFER
  U8 _aBufferPXP[XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL] BUFFER_ALIGNMENT;
#endif
//
// Temporarily allocated buffer
//
static void * _pBuffer;
//
// Config objects used for PXP operations 
//
static LCD_PXP_DRAW_CONTEXT _PXP_Context;
static PXP_BLEND_INFO       _PXP_Blend;
static BUFFER_OBJ           _PXP_aBuffer[3]; // Output buffer, process surface, alpha surface
//
// Currently used display rotation for PXP.
//
static pxp_rotate_degree_t _PXP_Rotation;
static int                 _CurrentRotation;

//
// VGLITE
//
#define VGLITE_BUFFER_SIZE  0x1000000
static U8 _aBufferVGLITE[VGLITE_BUFFER_SIZE] BUFFER_ALIGNMENT;

static U32 _aBufferAddress[NUM_BUFFERS] = {
  VRAM_ADDR,
  #if (NUM_BUFFERS > 1)
    VRAM_ADDR + XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL,
  #endif
  #if (NUM_BUFFERS > 2)
    VRAM_ADDR + XSIZE_PHYS * YSIZE_PHYS * LCD_BYTES_PER_PIXEL * 2
  #endif
};

//
// Semaphores
//
#if (USE_OS == 1)
  //
  // Semaphore definitions.
  //
  static SEMAPHORE_TYPE _Sema_WaitForPXP;
  static SEMAPHORE_TYPE _Sema_WaitForVSYNC;
  //
  // Handles/Pointers to above semaphores.
  //
  static SEMAPHORE_HANDLE_TYPE _hSema_WaitForPXP    = MAKE_SEMAPHORE_HANDLE(_Sema_WaitForPXP);
  static SEMAPHORE_HANDLE_TYPE _hSema_WaitForVSYNC  = MAKE_SEMAPHORE_HANDLE(_Sema_WaitForVSYNC);
#endif

//
// VGLITE: Path data
//
static F32                  _aPathBuffer[VG_LITE_PATH_BUFFER_SIZE / sizeof(F32)];
static LCD_VGLITE_PATH_DATA _PathData;

/*********************************************************************
*
*       Static code: General helper functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Malloc
*/
static void * _Malloc(U32 NumBytes) {
#if USE_PXP_BUFFER
  if (NumBytes > sizeof(_aBufferPXP)) {
    _pBuffer = NULL;
  }
  _pBuffer = _aBufferPXP;
#else
  GUI_HMEM hMem;

  hMem = GUI_ALLOC_AllocNoInit(NumBytes);
  if (hMem == 0) {
    return NULL;
  }
  GUI_LOCK();
  _pBuffer = GUI_ALLOC_LockH(hMem);
#endif
  return _pBuffer;
}

/*********************************************************************
*
*       _Free
*/
static void _Free(void * p) {
#if USE_PXP_BUFFER
  GUI_USE_PARA(p);
#else
  GUI_HMEM hMem;
  
  if (p) {
    hMem = GUI_ALLOC_p2h(p);
    GUI_ALLOC_UnlockH(p);
    GUI_UNLOCK();
    GUI_ALLOC_Free(hMem);
  }
#endif
}

/*********************************************************************
*
*       _GetMemdevParams
*/
static void _GetMemdevParams(GUI_MEMDEV_Handle hMem, void ** pp, int * pxSize, int * pySize, int * pBPP) {
  *pp     = GUI_MEMDEV_GetDataPtr(hMem);
  *pxSize = GUI_MEMDEV_GetXSize(hMem);
  *pySize = GUI_MEMDEV_GetYSize(hMem);
  *pBPP   = GUI_MEMDEV_GetBitsPerPixel(hMem);
}

/*********************************************************************
*
*       Static code: Semaphore handling
*
**********************************************************************
*/
#if (USE_OS == 1)
/*********************************************************************
*
*       _Semaphore_Take
*/
static void _Semaphore_Take(SEMAPHORE_HANDLE_TYPE hSema) {
#if (OS == EMBOS)
  OS_SEMAPHORE_TakeBlocked(hSema);
#endif
#if (OS == FREERTOS)
  xSemaphoreTake(hSema, portMAX_DELAY);
#endif
}

/*********************************************************************
*
*       _Semaphore_Create
*/
static void _Semaphore_Create(SEMAPHORE_HANDLE_TYPE * phSema) {
#if (OS == EMBOS)
  OS_SEMAPHORE_Create(*phSema, 0);
#endif
#if (OS == FREERTOS)
  *phSema = xSemaphoreCreateBinary();
#endif
}

/*********************************************************************
*
*       _Semaphore_Give
*/
static void _Semaphore_Give(SEMAPHORE_HANDLE_TYPE hSema) {
#if (OS == EMBOS)
  OS_EnterInterrupt();
  OS_SEMAPHORE_Give(hSema);
  OS_LeaveInterrupt();
#endif
#if (OS == FREERTOS)
  xSemaphoreGive(hSema);
#endif
}
#endif

/*********************************************************************
*
*       Static code: PXP helper functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Swap
*/
static void _Swap(int * p0, int * p1) {
  int Temp;

  Temp = *p0;
  *p0  = *p1;
  *p1  = Temp;
}

/*********************************************************************
*
*       _SwapSizesOnDemand
*/
static void _SwapSizesOnDemand(int * pxSize, int * pySize) {
  if (LCD_GetSwapXY()) {
    _Swap(pxSize, pySize);
  }
}

/*********************************************************************
*
*       _PXP_InitOperation
*/
static void _PXP_InitOperation(void) {
  //
  // Clear data cache before operation to avoid artefacts on the screen.
  //
  GUI_DCACHE_Clear(0);
  PXP_Reset(PXP);
  //
  // Use interrupt with semaphores to decrease CPU load.
  //
#if (USE_OS == 1)
  PXP_EnableInterrupts(PXP, kPXP_CompleteInterruptEnable);
#endif
  //
  // Disable CSC module, enabled by default.
  // 16x16 blocks for better performance.
  //
  PXP_EnableCsc1(PXP, false);
  PXP_SetProcessBlockSize(PXP, kPXP_BlockSize16);
}

/*********************************************************************
*
*       _PXP_StartOperation
*/
static void _PXP_StartOperation(void) {
  //
  // Start and wait for process to complete.
  //
  PXP_Start(PXP);
#if (USE_OS == 1)
  _Semaphore_Take(_hSema_WaitForPXP);
#else
  while (!(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP)));
  PXP_ClearStatusFlags(PXP, kPXP_CompleteFlag);
#endif
  //
  // Free temporary buffer if there is one.
  //
  if (_pBuffer) {
    _Free(_pBuffer);
    _pBuffer = NULL;
  }
}

/*********************************************************************
*
*       _PXP_ScalingParamValid
*/
static bool _PXP_ScalingParamValid(int inputDimension, int outputDimension) {
  U32 scaleFact = ((U32)inputDimension << 12U) / outputDimension;
  
  if ((scaleFact >= (16UL << 12U)) || scaleFact == 0) {
    return false;
  }
  return true;
}

/*********************************************************************
*
*       _PXP_SetPorterDuffMode
*/
static void _PXP_SetPorterDuffMode(pxp_porter_duff_blend_mode_t Mode) {
  pxp_porter_duff_config_t pd_cfg;

  PXP_GetPorterDuffConfig(Mode, &pd_cfg);
  PXP_SetPorterDuffConfig(PXP, &pd_cfg);
}

/*********************************************************************
*
*       _Log2PhysCoords
*/
static void _Log2PhysCoords(int x, int y, int xSize, int ySize, int * pxPhys, int * pyPhys) {
  *pxPhys = 0;
  *pyPhys = 0;
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    *pxPhys = x;
    *pyPhys = y;
    break;
  case kPXP_Rotate90:
    *pxPhys = XSIZE_PHYS - y - ySize;
    *pyPhys = x;
    break;
  case kPXP_Rotate180:
    *pxPhys = XSIZE_PHYS - x - xSize;
    *pyPhys = YSIZE_PHYS - y - ySize;
    break;
  case kPXP_Rotate270:
    *pxPhys = y;
    *pyPhys = YSIZE_PHYS - x - xSize;
    break;
  }
}

/*********************************************************************
*
*       _Phys2LogCoords
*/
static void _Phys2LogCoords(int * px0, int * py0, int * px1, int * py1) {
  int Temp;

  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    break;
  case kPXP_Rotate90:
    Temp = *px0;
    *px0 = XSIZE_PHYS - 1 - *px1;
    *px1 = XSIZE_PHYS - 1 - Temp;
    _Swap(px0, py0);
    _Swap(px1, py1);
    break;
  case kPXP_Rotate180:
    *px0 = XSIZE_PHYS - 1 - *px0;
    *px1 = XSIZE_PHYS - 1 - *px1;
    *py0 = YSIZE_PHYS - 1 - *py0;
    *py1 = YSIZE_PHYS - 1 - *py1;
    if (*px0 > *px1) {
      _Swap(px0, px1);
    }
    if (*py0 > *py1) {
      _Swap(py0, py1);
    }
    break;
  case kPXP_Rotate270:
    Temp = *py1;
    *py1 = (YSIZE_PHYS - 1 - *py0);
    *py0 = (YSIZE_PHYS - 1 - Temp);
    _Swap(px0, py0);
    _Swap(px1, py1);
    break;
  }
}

/*********************************************************************
*
*       _Phys2LogCoordsEx
*/
static void _Phys2LogCoordsEx(GUI_RECT * pRect) {
  int x0 = pRect->x0;
  int y0 = pRect->y0;
  int x1 = pRect->x1;
  int y1 = pRect->y1;
  _Phys2LogCoords(&x0, &y0, &x1, &y1);
  pRect->x0 = x0;
  pRect->y0 = y0;
  pRect->x1 = x1;
  pRect->y1 = y1;
}

/*********************************************************************
*
*       _GetBufferPtr
*/
static U32 _GetBufferPtr(int x, int y, int xSize, int ySize) {
  U8 * pBuffer;
  int  xPhys, yPhys;

  //
  // Convert logical coordinates to physical coordinates
  //
  _Log2PhysCoords(x, y, xSize, ySize, &xPhys, &yPhys);
  pBuffer  = (U8 *)_aBufferAddress[_BackBuffer];
  pBuffer += ((yPhys * XSIZE_PHYS) + xPhys) * LCD_BYTES_PER_PIXEL;
  return (U32)pBuffer;
}

/*********************************************************************
*
*       _PXP_ConfigOutBufferEx
*/
static void _PXP_ConfigOutBufferEx(void * pAddress, U32 Pitch, int xSize, int ySize, bool OverwriteAlpha, pxp_output_pixel_format_t PixelFmt) {
  pxp_output_buffer_config_t out_cfg;

  out_cfg.pixelFormat    = PixelFmt;
  out_cfg.interlacedMode = kPXP_OutputProgressive;
  out_cfg.buffer0Addr    = (U32)pAddress;
  out_cfg.buffer1Addr    = 0U;
  out_cfg.pitchBytes     = Pitch;
  out_cfg.width          = xSize;
  out_cfg.height         = ySize;
  PXP_SetOutputBufferConfig(PXP, &out_cfg);
  //
  // Make sure alpha channel is overwritten with 0xFF after the composite has been written to frame buffer.
  //
  if (OverwriteAlpha) {
    PXP->OUT_CTRL |= (0xFFu << 24) | (0x1u << 23);
  }
}

/*********************************************************************
*
*       _PXP_ConfigOutBuffer
*/
static void _PXP_ConfigOutBuffer(void * pAddress, U32 Pitch, int xSize, int ySize, bool OverwriteAlpha) {
  _PXP_ConfigOutBufferEx(pAddress, Pitch, xSize, ySize, OverwriteAlpha, PXP_OUT_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigOutBufferAuto
*/
static int _PXP_ConfigOutBufferAuto(int x0, int y0, int xSize, int ySize) {
  GUI_MEMDEV_Handle hMem;
  U32               Addr;
  U32               AddrMem;
  void            * pData;
  int               xSizeDev, ySizeDev, BppDev;
  int               xPosDev, yPosDev;

  //
  // Check if there currently is a MEMDEV selected so that we can write into the device.
  //
  hMem = GUI_MEMDEV_GetSelMemdev();
  if (hMem) {
    _GetMemdevParams(hMem, &pData, &xSizeDev, &ySizeDev, &BppDev);
    if (BppDev != 32) {
      return 1;
    }
    xPosDev = GUI_MEMDEV_GetXPos(hMem);
    yPosDev = GUI_MEMDEV_GetYPos(hMem);
    x0     -= xPosDev;
    y0     -= yPosDev;
    AddrMem = ((U32)pData) + (((y0 * xSizeDev) + x0) * (BppDev / 8));
    _PXP_ConfigOutBuffer((void *)AddrMem, xSizeDev * (BppDev / 8), xSizeDev, ySizeDev, true);
  } else {
    Addr = _GetBufferPtr(x0, y0, xSize, ySize);
    _SwapSizesOnDemand(&xSize, &ySize);
    _PXP_ConfigOutBuffer((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize, true);
  }
  return 0;
}

/*********************************************************************
*
*       _PXP_ConfigProcessSurfaceEx
*/
static void _PXP_ConfigProcessSurfaceEx(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_ps_pixel_format_t PixelFmt) {
  pxp_ps_buffer_config_t ps_cfg;

  ps_cfg.bufferAddr  = (U32)pAddress;
  ps_cfg.bufferAddrU = 0;
  ps_cfg.bufferAddrV = 0;
  ps_cfg.pitchBytes  = Pitch;
  ps_cfg.pixelFormat = PixelFmt;
  ps_cfg.swapByte    = 0;
  PXP_SetProcessSurfaceBufferConfig(PXP, &ps_cfg);
  PXP_SetProcessSurfacePosition(PXP, 0, 0, xSize - 1, ySize - 1);
}

/*********************************************************************
*
*       _PXP_ConfigProcessSurface
*/
static void _PXP_ConfigProcessSurface(void * pAddress, U32 Pitch, int xSize, int ySize) {
  _PXP_ConfigProcessSurfaceEx(pAddress, Pitch, xSize, ySize, PXP_PS_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaSurfaceEx
*/
static void _PXP_ConfigAlphaSurfaceEx(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_as_pixel_format_t PixelFmt) {
  pxp_as_buffer_config_t as_cfg;

  as_cfg.pixelFormat = PixelFmt;
  as_cfg.bufferAddr  = (U32)pAddress;
  as_cfg.pitchBytes  = Pitch;
  PXP_SetAlphaSurfaceBufferConfig(PXP, &as_cfg);
  PXP_SetAlphaSurfacePosition(PXP, 0, 0, xSize - 1, ySize - 1);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaSurface
*/
static void _PXP_ConfigAlphaSurface(void * pAddress, U32 Pitch, int xSize, int ySize) {
  _PXP_ConfigAlphaSurfaceEx(pAddress, Pitch, xSize, ySize, PXP_AS_FORMAT);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlendNormal
*/
static void _PXP_ConfigAlphaBlendNormal(void) {
  pxp_as_blend_config_t blend_cfg;

  blend_cfg.alpha       = 0;
  blend_cfg.invertAlpha = 0;
  blend_cfg.alphaMode   = kPXP_AlphaEmbedded;
  PXP_SetAlphaSurfaceBlendConfig(PXP, &blend_cfg);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlendMultiply
*/
static void _PXP_ConfigAlphaBlendMultiply(U8 Alpha) {
  pxp_as_blend_config_t blend_cfg;

  blend_cfg.alpha       = Alpha;
  blend_cfg.invertAlpha = 0;
  blend_cfg.alphaMode   = kPXP_AlphaMultiply;
  PXP_SetAlphaSurfaceBlendConfig(PXP, &blend_cfg);
}

/*********************************************************************
*
*       Static code: PXP related emWin functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _LCD_PXP_SetProcessSurface
*/
static void _LCD_PXP_SetProcessSurface(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_ps_pixel_format_t Fmt) {
  BUFFER_OBJ * pPS;

  _PXP_Context.pProcessSurface = pPS = &_PXP_aBuffer[BUFFER_TYPE_PROCESS_SURFACE];
  pPS->p            = pAddress;
  pPS->Type         = BUFFER_TYPE_PROCESS_SURFACE;
  pPS->xSize        = xSize;
  pPS->ySize        = ySize;
  pPS->BytesPerLine = Pitch;
  pPS->Format.Ps    = Fmt;
}

/*********************************************************************
*
*       _LCD_PXP_SetAlphaSurface
*/
static void _LCD_PXP_SetAlphaSurface(void * pAddress, U32 Pitch, int xSize, int ySize, pxp_as_pixel_format_t Fmt) {
  BUFFER_OBJ * pAS;

  _PXP_Context.pAlphaSurface = pAS = &_PXP_aBuffer[BUFFER_TYPE_ALPHA_SURFACE];
  pAS->p            = pAddress;
  pAS->Type         = BUFFER_TYPE_ALPHA_SURFACE;
  pAS->xSize        = xSize;
  pAS->ySize        = ySize;
  pAS->BytesPerLine = Pitch;
  pAS->Format.As    = Fmt;
}

/*********************************************************************
*
*       _LCD_PXP_GetFrameBufferConfig
*/
static BUFFER_OBJ * _LCD_PXP_GetFrameBufferConfig(LCD_PXP_DRAW_CONTEXT * pCtx) {
  BUFFER_OBJ * p = &_PXP_aBuffer[BUFFER_TYPE_FRAMEBUFFER];

  p->p              = (void *)_GetBufferPtr(pCtx->x, pCtx->y, pCtx->xSize, pCtx->ySize);
  p->xSize          = pCtx->xSize;
  p->ySize          = pCtx->ySize;
  p->Type           = BUFFER_TYPE_FRAMEBUFFER;
  p->Format.Out     = PXP_OUT_FORMAT;
  p->BytesPerLine   = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
  p->OverwriteAlpha = true;
  return p;
}

/*********************************************************************
*
*       _LCD_PXP_AlphaBlending
*/
static PXP_BLEND_INFO * _LCD_PXP_AlphaBlending(void) {
  LCD_PXP_DRAW_CONTEXT * pCtx   = &_PXP_Context;
  PXP_BLEND_INFO       * pBlend = &_PXP_Blend;

  pCtx->pBlend = pBlend;
  return pBlend;
}

/*********************************************************************
*
*       _LCD_PXP_InitContext
*/
static void _LCD_PXP_InitContext(int x, int y, int xSize, int ySize) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;
  BUFFER_OBJ           * pOB;

  //
  // Clear all previous parameters.
  //
  memset(pCtx, 0, sizeof(LCD_PXP_DRAW_CONTEXT));
  pCtx->x     = x;
  pCtx->y     = y;
  pCtx->xSize = xSize;
  pCtx->ySize = ySize;
  //
  // Set frame buffer as output buffer.
  //
  pOB                 = _LCD_PXP_GetFrameBufferConfig(pCtx);
  pCtx->pOutputBuffer = pOB;
}

#define ROUND_UP_BY(div, n)    ((n % div) ? (n + (div - (n % div))) : n)
#define ROUND_DOWN_BY(div, n)  ((n % div) ? (n -        (n % div) ) : n)
/*********************************************************************
*
*       _PXP_CheckSizes
*/
static void _PXP_CheckSizes(int xSize, int ySize, LCD_PXP_SIZE_CONTEXT * pSize) {
  //
  // In case a size is not divisible by 16 (the block size set to the PXP),
  // we need the sizes rounded up and rounded down.
  // The rounded down size is the part of the bitmap that is drawn by the PXP.
  // The rounded up size is to incorporate the background of the bitmap will be
  // blended into. It is crucial for rotated configs that all sizes are divisible by 16.
  //
  *pSize->pxSizeRoundUp   = ROUND_UP_BY(16, xSize);
  *pSize->pySizeRoundUp   = ROUND_UP_BY(16, ySize);
  *pSize->pxSizeRoundDown = ROUND_DOWN_BY(16, xSize);
  *pSize->pySizeRoundDown = ROUND_DOWN_BY(16, ySize);
}

/*********************************************************************
*
*       _PXP_ClipSizes
*/
static void _PXP_ClipSizes(int x, int y, LCD_PXP_SIZE_CONTEXT * pSize) {
  int * pxSize = pSize->pxSizeRoundUp;
  int * pySize = pSize->pySizeRoundUp;

  if (x + *pxSize > (LCD_GetXSize())) {
    *pxSize = LCD_GetXSize() - x;
  }
  if (y + *pySize > (LCD_GetYSize())) {
    *pySize = LCD_GetYSize() - y;
  }
}

/*********************************************************************
*
*       _PXP_InvertRotation
*/
static pxp_rotate_degree_t _PXP_InvertRotation(void) {
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
    return kPXP_Rotate0;
  case kPXP_Rotate90:
    return kPXP_Rotate270;
  case kPXP_Rotate180:
    return kPXP_Rotate180;
  case kPXP_Rotate270:
    return kPXP_Rotate90;
  }
  return kPXP_Rotate0;
}

/*********************************************************************
*
*       _LCD_PXP_ConfigOutputBuffer
*/
static void _LCD_PXP_ConfigOutputBuffer(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigOutBufferEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->OverwriteAlpha, pBuffer->Format.Out);
}

/*********************************************************************
*
*       _LCD_PXP_ConfigProcessSurface
*/
static void _LCD_PXP_ConfigProcessSurface(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigProcessSurfaceEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->Format.Ps);
}

/*********************************************************************
*
*       _LCD_PXP_ConfigAlphaSurface
*/
static void _LCD_PXP_ConfigAlphaSurface(BUFFER_OBJ * pBuffer) {
  _PXP_ConfigAlphaSurfaceEx(pBuffer->p, pBuffer->BytesPerLine, pBuffer->xSize, pBuffer->ySize, pBuffer->Format.As);
}

/*********************************************************************
*
*       _PXP_ConfigAlphaBlend
*/
static void _LCD_PXP_ConfigAlphaBlend(PXP_BLEND_INFO * pBlend) {
  if (!pBlend) {
    return;
  }
  if (pBlend->Mode & BLEND_MODE_NORMAL) {
    _PXP_ConfigAlphaBlendNormal();
  }
  if (pBlend->Mode & BLEND_MODE_MULTIPLY) {
    _PXP_ConfigAlphaBlendMultiply(pBlend->Alpha);
  }
  if (pBlend->Mode & BLEND_MODE_PORTERDUFF) {
    _PXP_SetPorterDuffMode(pBlend->PorterDuffMode);
  }
}

/*********************************************************************
*
*       _GetSizeUnmirrored
*/
static void _GetSizeUnmirrored(int xSize, int ySize, int * pOutXSize, int * pOutYSize) {
  switch (_PXP_Rotation) {
  case kPXP_Rotate0:
  case kPXP_Rotate180:
    *pOutXSize = xSize;
    *pOutYSize = ySize;
    break;
  case kPXP_Rotate90:
  case kPXP_Rotate270:
    *pOutXSize = ySize;
    *pOutYSize = xSize;
    break;
  }
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmap
*
*  Used by all functions which need to observe the current rotation
*  (i.e. most bitmap functions that output directly to the screen).
*  Functions drawing into memory devices do not need to call this function.
*/
static int _LCD_PXP_DrawBitmap(LCD_PXP_DRAW_CONTEXT * pCtx) {
  U32                  Addr;
  int                  xSizeRD, ySizeRD, xSizeRU, ySizeRU; // x/ySize rounded down and up
  LCD_PXP_SIZE_CONTEXT Sizes =  { &xSizeRU, &ySizeRU, &xSizeRD, &ySizeRD };
  void               * pBuffer = NULL;
  int                  x = pCtx->x, y = pCtx->y;
  bool                 IsRotated = (_PXP_Rotation != kPXP_Rotate0);
  BUFFER_OBJ         * pOut, *pAS;
  int                  xSizeUM, ySizeUM;
  int                  xDiff = 0, yDiff = 0;

  _PXP_InitOperation();
  //
  // Non-rotated config: entire operation is done with one PXP iteration.
  //
  if (!IsRotated  || !pCtx->pAlphaSurface) {
    //
    // Set rotation if required.
    //
    if (IsRotated) {
      PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_Rotation, kPXP_FlipDisable);
    }
    //
    // Configure output buffer. If a specific buffer has been set, use it (e.g. memdev).
    // If not, use the default which is the backbuffer.
    //
    pOut = (pCtx->pOutputBuffer) ? pCtx->pOutputBuffer : _LCD_PXP_GetFrameBufferConfig(pCtx);
    _LCD_PXP_ConfigOutputBuffer(pOut);
    //
    // Enable/disable PS.
    //
    if (!pCtx->pProcessSurface) {
      PXP_DisableProcessSurface();
    } else {
      _LCD_PXP_ConfigProcessSurface(pCtx->pProcessSurface);
    }
    //
    // Enable/disable AS and alpha blending.
    //
    if (!pCtx->pAlphaSurface) {
      PXP_DisableAlphaSurface();
    } else {
      _LCD_PXP_ConfigAlphaSurface(pCtx->pAlphaSurface);
      _LCD_PXP_ConfigAlphaBlend(pCtx->pBlend);
    }
    _PXP_StartOperation();
  } else { // In rotated configs with semi-transparent bitmaps, two steps are required.
    //
    // Step 1: Copy affected frame buffer area and rotate it against the current angle into the temp. buffer.
    // We rotate here in the opposite direction to have the frame buffer content unrotated, just like the bitmap is.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_InvertRotation(), kPXP_FlipDisable);
    _PXP_CheckSizes(pCtx->xSize, pCtx->ySize, &Sizes);
    xDiff = xSizeRU;
    yDiff = ySizeRU;
    _PXP_ClipSizes(x, y, &Sizes);
    xDiff -= xSizeRU;
    yDiff -= ySizeRU;
    pBuffer = _Malloc(ySizeRU * xSizeRU * LCD_BYTES_PER_PIXEL);
    if (!pBuffer) {
      return 1;
    }
    _GetSizeUnmirrored(xSizeRU, ySizeRU, &xSizeUM, &ySizeUM);
    _PXP_ConfigOutBuffer(pBuffer, xSizeRU * LCD_BYTES_PER_PIXEL, xSizeUM, ySizeUM, true);
    PXP_DisableAlphaSurface();
    Addr = _GetBufferPtr(x, y, xSizeRU, ySizeRU);
    _PXP_ConfigProcessSurface((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSizeUM, ySizeUM);
    _PXP_StartOperation();
    //
    // Step 2: Blend alpha bitmap over copied area in temp. buffer into the frame buffer.
    //
    _PXP_InitOperation();
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, _PXP_Rotation, kPXP_FlipDisable);
    //
    // Make sure size of output buffer is aligned.
    //
    pOut = (pCtx->pOutputBuffer) ? pCtx->pOutputBuffer : _LCD_PXP_GetFrameBufferConfig(pCtx);
    pOut->xSize = xSizeRU;
    pOut->ySize = ySizeRU;
    pOut->p     = (void *)Addr;
    _LCD_PXP_ConfigOutputBuffer(pOut);
    //
    // Blend bitmap as AS over PS into frame buffer.
    //
    _PXP_ConfigProcessSurface(pBuffer, xSizeRU * LCD_BYTES_PER_PIXEL, xSizeRU, ySizeRU);
    pAS = pCtx->pAlphaSurface;
    _LCD_PXP_ConfigAlphaSurface(pAS);
    _LCD_PXP_ConfigAlphaBlend(pCtx->pBlend);
    if (_PXP_Rotation == kPXP_Rotate180) {
      PXP_SetAlphaSurfacePosition(PXP, xDiff, yDiff, pAS->xSize + xDiff - 1, pAS->ySize + yDiff - 1);
    }
    _PXP_StartOperation();
  }
  return 0;
}

/*********************************************************************
*
*       _LCD_PXP_CopyBuffer
*/
static void _LCD_PXP_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  GUI_USE_PARA(LayerIndex);

  _PXP_InitOperation();
  //
  // Configure destination buffer as output buffer.
  //
  _PXP_ConfigOutBuffer((void *)_aBufferAddress[IndexDst], XSIZE_PHYS * LCD_BYTES_PER_PIXEL, XSIZE_PHYS, YSIZE_PHYS, true);
  //
  // Configure source buffer as PS.
  //
  _PXP_ConfigProcessSurface((void *)_aBufferAddress[IndexSrc], XSIZE_PHYS * LCD_BYTES_PER_PIXEL, XSIZE_PHYS, YSIZE_PHYS);
  //
  // AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
  //
  // Buffer switch done.
  //
  _BackBuffer = IndexDst;
}

#if 0 // Disabled due to slow speed.
/*********************************************************************
*
*       _LCD_PXP_FillRect
*/
static void _LCD_PXP_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  pxp_porter_duff_config_t pd_cfg;
  U8                       Alpha;
  U32                      Addr;
  U32                      w, h;
  GUI_COLOR                Color;

  GUI_USE_PARA(LayerIndex);
  //
  // Do XOR filling by software.
  //
  if (GUI_GetDrawMode() & GUI_DM_XOR) {
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))NULL);
    _Phys2LogCoords(&x0, &y0, &x1, &y1);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_PXP_FillRect);
    return;
  }
  //
  // Non-XOR filling done by PXP.
  //
  _PXP_InitOperation();
  w = x1 - x0 + 1;
  h = y1 - y0 + 1;
  //
  // Configure output buffer.
  // This is a low-level driver function, coordinates are physical not logical.
  //
  Addr = LCD_XY2BufferPtr(x0, y0);
  _PXP_ConfigOutBuffer((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, w, h, true);
  //
  Color = GUI_Index2Color(PixelIndex);
  Alpha = GUI_GetAlpha();
  if (Alpha == 0xFF) {
    //
    // Opaque fill, AS not needed.
    //
    PXP_DisableAlphaSurface();
  } else {
    //
    // Transparency fill, output buffer is used as AS.
    //
    _PXP_ConfigAlphaSurface((void *)Addr, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, w, h);
  }
  //
  // Disable PS, instead generate color.
  //
  PXP_DisableProcessSurface();
  PXP_SetProcessSurfaceBackGroundColor(PXP, Color);
  //
  // Configure Porter-Duff blending.
  //
  pd_cfg.enable             = 1;
  pd_cfg.dstColorMode       = kPXP_PorterDuffColorNoAlpha;
  pd_cfg.srcColorMode       = kPXP_PorterDuffColorNoAlpha;
  pd_cfg.dstGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
  pd_cfg.srcGlobalAlphaMode = kPXP_PorterDuffGlobalAlpha;
  pd_cfg.dstFactorMode      = kPXP_PorterDuffFactorStraight;
  pd_cfg.srcFactorMode      = (Alpha == 0xFF) ? kPXP_PorterDuffFactorStraight : kPXP_PorterDuffFactorInversed;
  pd_cfg.dstGlobalAlpha     = Alpha;
  pd_cfg.srcGlobalAlpha     = Alpha;
  pd_cfg.dstAlphaMode       = kPXP_PorterDuffAlphaStraight;
  pd_cfg.srcAlphaMode       = kPXP_PorterDuffAlphaStraight;
  PXP_SetPorterDuffConfig(PXP, &pd_cfg);
  _PXP_StartOperation();
}
#endif

/*********************************************************************
*
*       _LCD_PXP_DrawMemdevAlpha
*/
static void _LCD_PXP_DrawMemdevAlpha(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  _PXP_InitOperation();
  //
  // Destination device is output buffer.
  //
  _PXP_ConfigOutBuffer(pDst, BytesPerLineDst, xSize, ySize, false);
  //
  // Configure destination device as PS.
  //
  _PXP_ConfigProcessSurface(pDst, BytesPerLineDst, xSize, ySize);
  //
  // Configure bitmap as AS.
  //
  _PXP_ConfigAlphaSurfaceEx((void *)pSrc, BytesPerLineSrc, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  _PXP_ConfigAlphaBlendNormal();
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmap32bpp
*/
static void _LCD_PXP_DrawBitmap32bpp(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;
  BUFFER_OBJ           * pPS;

  GUI_USE_PARA(LayerIndex);
  //
  // Use actual xSize according to pitch of buffer.
  //
  _LCD_PXP_InitContext(x, y, xSize, ySize);
  //
  // Set bitmap as AS and blend normally.
  //
  _LCD_PXP_SetAlphaSurface((void *)p, BytesPerLine, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  _LCD_PXP_AlphaBlending()->Mode = BLEND_MODE_NORMAL;
  //
  // Use frame buffer as PS, copy output buffer config to PS config.
  //
  pCtx->pProcessSurface = pPS = &_PXP_aBuffer[BUFFER_TYPE_PROCESS_SURFACE];
  *pPS                  = *pCtx->pOutputBuffer;
  pPS->Format.Ps        = kPXP_PsPixelFormatRGB888;
  //
  // Draw 32bpp alpha bitmap into frame buffer.
  //
  _LCD_PXP_DrawBitmap(pCtx);
}

/*********************************************************************
*
*       _LCD_PXP_CopyRect
*/
static void _LCD_PXP_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32      AddrSrc, AddrDst;
  void   * pBuffer = NULL;
  GUI_RECT rSrc    = { x0, y0, x0 + xSize - 1, y0 + ySize - 1 };
  GUI_RECT rDst    = { x1, y1, x1 + xSize - 1, y1 + ySize - 1 };
  int      PitchSrc;

  GUI_USE_PARA(LayerIndex);
  _PXP_InitOperation();
  AddrSrc = _GetBufferPtr(x0, y0, xSize, ySize);
  AddrDst = _GetBufferPtr(x1, y1, xSize, ySize);
  //
  // Swap if X/Y axis are mirrored.
  //
  _SwapSizesOnDemand(&xSize, &ySize);
  //
  // Use buffer if the rectangles intersect
  //
  PitchSrc = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
  if (GUI_RectsIntersect(&rSrc, &rDst)) {
    pBuffer = _Malloc(LCD_GetXSize() * LCD_GetYSize() * LCD_BYTES_PER_PIXEL);
    if (!pBuffer) {
      //
      // Fallback to software.
      // Coordinates are physical, they need to be converted to logical coords.
      //
      _Phys2LogCoordsEx(&rSrc);
      _Phys2LogCoordsEx(&rDst);
      LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))NULL);
      GUI_CopyRect(rSrc.x0, rSrc.y0, rDst.x0, rDst.y0, rSrc.x1 - rSrc.x0 + 1, rSrc.y1 - rSrc.y0 + 1);
      LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_LCD_PXP_CopyBuffer);
      return;
    }
    PitchSrc = xSize * LCD_BYTES_PER_PIXEL;
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBuffer(pBuffer, PitchSrc, xSize, ySize, true);
    //
    // Configure rect to be copied as processing surface.
    //
    _PXP_ConfigProcessSurface((void *)AddrSrc, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize);
    //
    // AS not needed.
    //
    PXP_DisableAlphaSurface();
    _PXP_StartOperation();
    //
    // Configure address for final copy.
    //
    AddrSrc = (U32)pBuffer;
  }
  //
  // Configure output buffer.
  //
  _PXP_ConfigOutBuffer((void *)AddrDst, XSIZE_PHYS * LCD_BYTES_PER_PIXEL, xSize, ySize, true);
  //
  // Configure rect to be copied as processing surface.
  //
  _PXP_ConfigProcessSurface((void *)AddrSrc, PitchSrc, xSize, ySize);
  //
  // AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_MixColorsBulk
*/
static void _LCD_PXP_MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens) {
  GUI_USE_PARA(OffFG);
  GUI_USE_PARA(OffDest);
  _PXP_InitOperation();
  //
  // Configure output buffer.
  //
  _PXP_ConfigOutBuffer(pDst, xSize * LCD_BYTES_PER_PIXEL, xSize, ySize, false);
  //
  // Configure BG as PS.
  //
  _PXP_ConfigProcessSurface(pBG, (xSize + OffBG) * LCD_BYTES_PER_PIXEL, xSize, ySize);
  //
  // Configure FG as AS.
  //
  _PXP_ConfigAlphaSurface(pFG, xSize * LCD_BYTES_PER_PIXEL, xSize, ySize);
  _PXP_ConfigAlphaBlendMultiply(Intens);
  //
  // Configure Porter-Duff mode.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawMemdevM565
*/
static void _LCD_PXP_DrawMemdevM565(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  _PXP_InitOperation();
  //
  // 32bpp memdev is output buffer.
  //
  _PXP_ConfigOutBufferEx(pDst, BytesPerLineDst, xSize, ySize, false, kPXP_OutputPixelFormatARGB8888);
  //
  // Destination memdev is configured as PS.
  //
  _PXP_ConfigProcessSurfaceEx(pDst, BytesPerLineDst, xSize, ySize, kPXP_PsPixelFormatRGB888);
  //
  // Configure source device as AS.
  //
  _PXP_ConfigAlphaSurfaceEx((void *)pSrc, BytesPerLineSrc, xSize, ySize, kPXP_AsPixelFormatRGB565);
  _PXP_ConfigAlphaBlendNormal();
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawMemdev16bpp
*/
static void _LCD_PXP_DrawMemdev16bpp(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  GUI_USE_PARA(BytesPerLineSrc);
  _PXP_InitOperation();
  //
  // Output buffer is 16bpp memdev.
  //
  _PXP_ConfigOutBufferEx(pDst, BytesPerLineDst, xSize, ySize, false, kPXP_OutputPixelFormatRGB565);
  //
  // Source memdev is configured as PS.
  //
  _PXP_ConfigProcessSurfaceEx((void *)pSrc, BytesPerLineDst, xSize, ySize, kPXP_PsPixelFormatRGB565);
  //
  // Bitmap does not have alpha, AS not needed.
  //
  PXP_DisableAlphaSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmapM565
*/
static void _LCD_PXP_DrawBitmapM565(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  LCD_PXP_DRAW_CONTEXT * pCtx = &_PXP_Context;

  GUI_USE_PARA(LayerIndex);
  //
  // Use actual xSize according to pitch of buffer.
  //
  _LCD_PXP_InitContext(x, y, xSize, ySize);
  //
  // Set 16bpp bitmap as PS.
  //
  _LCD_PXP_SetProcessSurface((void *)p, BytesPerLine, xSize, ySize, kPXP_PsPixelFormatRGB565);
  //
  // Draw 16bpp opaque bitmap into frame buffer.
  //
  _LCD_PXP_DrawBitmap(pCtx);
}

/*********************************************************************
*
*       _LCD_PXP_BlendMemdev
*/
static int _LCD_PXP_BlendMemdev(GUI_MEMDEV_Handle hMem, GUI_COLOR Color, U8 BlendIntens) {
  U32  * pBuffer;
  U32    NumBytes;
  void * pDst;
  int    BitsPerPixel;
  int    BytesPerPixel;
  int    xSize, ySize;
  
  _GetMemdevParams(hMem, &pDst, &xSize, &ySize, &BitsPerPixel);
  BytesPerPixel = BitsPerPixel / 8;
  if (BitsPerPixel != 32) {
    return 1;
  }
  //
  // Allocate temp. buffer
  //
  NumBytes = xSize * ySize * BytesPerPixel;
  pBuffer  = (U32 *)_Malloc(NumBytes);
  if (pBuffer == NULL) {
    return 1;
  }
  //
  // Step 1: Fill buffer with given color and given alpha value
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBuffer(pBuffer, xSize * BytesPerPixel, xSize, ySize, true);
  //
  // No AS needed.
  //
  PXP_DisableAlphaSurface();
  //
  // Disable PS, generate the color instead.
  //
  PXP_DisableProcessSurface();
  PXP_SetProcessSurfaceBackGroundColor(PXP, Color);
  //
  _PXP_StartOperation();
  //
  // Step 2: Blend temp. buffer over memory device.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBufferEx(pDst, xSize * BytesPerPixel, xSize, ySize, false, kPXP_OutputPixelFormatARGB8888);
  //
  // Configure memdev as PS.
  //
  _PXP_ConfigProcessSurfaceEx(pDst, xSize * BytesPerPixel, xSize, ySize, kPXP_PsPixelFormatRGB888);
  //
  // Use temp. buffer as AS.
  //
  _PXP_ConfigAlphaSurfaceEx(pBuffer, xSize * BytesPerPixel, xSize, ySize, kPXP_AsPixelFormatARGB8888);
  //
  // Set Porter-Duff blending to SrcOver.
  //
  _PXP_ConfigAlphaBlendMultiply(BlendIntens);
  _PXP_SetPorterDuffMode(kPXP_PorterDuffOver);
  _PXP_StartOperation();
  return 0;
}

/*********************************************************************
*
*       _LCD_PXP_GetPsFormatFromBitmap
*/
static pxp_ps_pixel_format_t _LCD_PXP_GetPsFormatFromBitmap(const GUI_BITMAP * pBitmap) {
  if (pBitmap->pMethods == GUI_DRAW_BMPM8888I) {
    return kPXP_PsPixelFormatRGB888;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMPM565) {
    return kPXP_PsPixelFormatRGB565;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMPM444_12) {
    return kPXP_PsPixelFormatRGB444;
  }
  if (pBitmap->pMethods == GUI_DRAW_BMP555) {
    return kPXP_PsPixelFormatRGB555;
  }
  return PXP_PS_InvalidFormat;
}

/*********************************************************************
*
*       _LCD_PXP_DrawBitmapEx
*/
static int _LCD_PXP_DrawBitmapEx(const GUI_BITMAP * pBitmap, int x0, int y0, int xMag, int yMag) {
  pxp_ps_pixel_format_t Fmt;
  pxp_flip_mode_t       FlipMode;
  int                   xSizeScaled;
  int                   ySizeScaled;
  int                   DevClass;
  bool                  TwoSteps;
  U32                   NumBytes;
  void                * pBuffer;
  pxp_rotate_degree_t   Rotation = _PXP_Rotation;
  bool                  Scaling;
  int                   xSizeRD, ySizeRD, xSizeRU, ySizeRU;
  LCD_PXP_SIZE_CONTEXT  Ctx = { &xSizeRU, &ySizeRU, &xSizeRD, &ySizeRD };
  int                   Pitch;

  //
  // We only want to write into the frame buffer or the current MEMDEV.
  //
  DevClass = GUI_DEVICE_GetDeviceClass();
  if ((DevClass != DEVICE_CLASS_DRIVER) && (DevClass != DEVICE_CLASS_MEMDEV)) {
    return 1;
  }
  //
  // Convert bitmap format to processing surface format. If the format is not supported by PXP we abort.
  //
  Fmt = _LCD_PXP_GetPsFormatFromBitmap(pBitmap);
  if (Fmt == PXP_PS_InvalidFormat) {
    return 1;
  }
  //
  // Retrieve flip mode to be used.
  // We also always need positive scaling factors.
  //
  FlipMode  = kPXP_FlipDisable;
  if (xMag < 0) {
    FlipMode |= kPXP_FlipHorizontal;
    xMag      = -xMag;
  }
  if (yMag < 0) {
    FlipMode |= kPXP_FlipVertical;
    yMag      = -yMag;
  }
  xSizeScaled = (pBitmap->XSize * xMag) / 1000;
  ySizeScaled = (pBitmap->YSize * yMag) / 1000;
  //
  // Do operation by software if desired size is too small or too large.
  //
  if (!(_PXP_ScalingParamValid(pBitmap->XSize, xSizeScaled) && _PXP_ScalingParamValid(pBitmap->YSize, ySizeScaled))) {
    return 1;
  }
  //
  // In case we output into a memory device, it must not be rotated.
  //
  if (DevClass == DEVICE_CLASS_MEMDEV) {
    Rotation = kPXP_Rotate0;
  }
  //
  // Vertical flipping (same as rotation) and scaling cannot be done in one step.
  //
  Scaling  = ((xMag != 1000) || (yMag != 1000)) ? true : false;
  TwoSteps = ((Scaling && (FlipMode & kPXP_FlipVertical)) || (Rotation != kPXP_Rotate0)) ? true : false;
  _PXP_InitOperation();
  if (!TwoSteps) {
    //
    // Set flipping mode and rotation.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, Rotation, FlipMode);
    //
    // Config scaling params for PS.
    //
    PXP_SetProcessSurfaceScaler(PXP, pBitmap->XSize, pBitmap->YSize, xSizeScaled, ySizeScaled);
    //
    // Configure bitmap as PS.
    //
    PXP_DisableAlphaSurface();
    _PXP_ConfigProcessSurfaceEx((void *)pBitmap->pData, pBitmap->BytesPerLine, xSizeScaled, ySizeScaled, Fmt);
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBufferAuto(x0, y0, xSizeScaled, ySizeScaled);
    //
    // Execute and done.
    //
    _PXP_StartOperation();
  } else {
    _PXP_CheckSizes(pBitmap->XSize, pBitmap->YSize, &Ctx);
    _PXP_ClipSizes(x0, y0, &Ctx);
    //
    // Step 1: Do flipping and/or rotation operation to temp. buffer.
    // Output will be converted into 32bpp pixels.
    //
    NumBytes = (sizeof(U32) * xSizeRU * ySizeRU);
    pBuffer  = _Malloc(NumBytes);
    if (pBuffer == NULL) {
      return 1;
    }
    Pitch = (LCD_GetSwapXY()) ? (sizeof(U32) * ySizeRU) : (sizeof(U32) * xSizeRU);
    //
    // Set flipping mode but no rotation.
    //
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, Rotation, FlipMode);
    //
    // Use temp. buffer as output buffer for this step.
    //
    _PXP_ConfigOutBufferEx(pBuffer, Pitch, pBitmap->XSize, pBitmap->YSize, true, kPXP_OutputPixelFormatARGB8888);
    //
    // Configure bitmap as PS.
    //
    PXP_DisableAlphaSurface();
    _PXP_ConfigProcessSurfaceEx((void *)pBitmap->pData, pBitmap->BytesPerLine, pBitmap->XSize, pBitmap->YSize, Fmt);
    _PXP_StartOperation();
    //
    // Step 2: Scale the flipped bitmap.
    //
    _PXP_InitOperation();
    PXP_SetProcessSurfaceScaler(PXP, pBitmap->XSize, pBitmap->YSize, xSizeScaled, ySizeScaled);
    PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, kPXP_Rotate0, kPXP_FlipDisable);
    //
    // Configure output buffer.
    //
    _PXP_ConfigOutBufferAuto(x0, y0, xSizeScaled, ySizeScaled);
    //
    // Configure temp. buffer as PS.
    //
    PXP_DisableAlphaSurface();
    _SwapSizesOnDemand(&xSizeScaled, &ySizeScaled); // Swap sizes if required for PS.
    _PXP_ConfigProcessSurfaceEx(pBuffer, Pitch, xSizeScaled, ySizeScaled, kPXP_PsPixelFormatRGB888);
    //
    // We are done, start PXP and free memory.
    //
    _PXP_StartOperation();
  }
  return 0;
}

/*********************************************************************
*
*       Custom color conversions
*/
DEFINE_PXP_COLORCONV(M1555I, kPXP_OutputPixelFormatARGB1555)
DEFINE_PXP_COLORCONV(M565,   kPXP_OutputPixelFormatRGB565)
DEFINE_PXP_COLORCONV(M4444I, kPXP_OutputPixelFormatARGB4444)
DEFINE_PXP_COLORCONV(M8888I, kPXP_OutputPixelFormatARGB8888)

/*********************************************************************
*
*       _LCD_PXP_Color2IndexBulk
*/
static void _LCD_PXP_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat) {
  //
  // Set input color buffer as AS so that alpha values are also converted.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBufferEx(pIndex, NumItems * SizeOfIndex, NumItems, 1, false, PixelFormat);
  _PXP_ConfigAlphaSurface(pColor, NumItems * LCD_BYTES_PER_PIXEL, NumItems, 1);
  _PXP_ConfigAlphaBlendNormal();
  PXP_DisableProcessSurface();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       _LCD_PXP_Index2ColorBulk
*/
static void _LCD_PXP_Index2ColorBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex, pxp_output_pixel_format_t PixelFormat) {
  pxp_as_pixel_format_t InputFormat;

  //
  // Convert given parameter to input format.
  //
  switch (PixelFormat) {
  case kPXP_OutputPixelFormatARGB1555:
    InputFormat = kPXP_AsPixelFormatARGB1555;
    break;
  case kPXP_OutputPixelFormatRGB565:
    InputFormat = kPXP_AsPixelFormatRGB565;
    break;
  case kPXP_OutputPixelFormatARGB4444:
    InputFormat = kPXP_AsPixelFormatARGB4444;
    break;
  default:
  case kPXP_OutputPixelFormatARGB8888:
    InputFormat = kPXP_AsPixelFormatARGB8888;
    break;
  }
  //
  // No AS needed.
  // Configure input colors as PS.
  //
  _PXP_InitOperation();
  _PXP_ConfigOutBuffer(pColor, NumItems * LCD_BYTES_PER_PIXEL, NumItems, 1, true);
  PXP_DisableProcessSurface();
  _PXP_ConfigAlphaSurfaceEx(pIndex, NumItems * SizeOfIndex, NumItems, 1, InputFormat);
  _PXP_ConfigAlphaBlendNormal();
  _PXP_StartOperation();
}

/*********************************************************************
*
*       Static code: VGLITE related
*
**********************************************************************
*/
/*********************************************************************
*
*       _VGLITE_MemdevToVgBuffer
*/
static void _VGLITE_MemdevToVgBuffer(GUI_MEMDEV_Handle hMem, vg_lite_buffer_t * pBuffer) {
  void * p;
  int    xSize, ySize, Bpp;

  if (!hMem || !pBuffer) {
    return;
  }
  _GetMemdevParams(hMem, &p, &xSize, &ySize, &Bpp);
  pBuffer->width   = xSize;
  pBuffer->height  = ySize;
  pBuffer->stride  = xSize * (Bpp / 8);
  pBuffer->tiled   = VG_LITE_LINEAR;
  pBuffer->format  = VG_LITE_BGRA8888;
  pBuffer->memory  = p;
  pBuffer->address = (U32)p;
  //
  // Determine format.
  //
  switch (Bpp) {
  case 16:
    pBuffer->format = VG_LITE_BGR565;
    break;
  case 32:
    pBuffer->format = VG_LITE_BGRA8888;
    break;
  }
}

#define CHECK_FORMAT(BMP_FMT, VG_FMT)   if (pBM->pMethods == BMP_FMT) { pBuffer->format = VG_FMT; return 0; }
/*********************************************************************
*
*       _VGLITE_BitmapToVgBuffer
*/
static int _VGLITE_BitmapToVgBuffer(const GUI_BITMAP * pBM, vg_lite_buffer_t * pBuffer) {
  if (!pBM || !pBuffer) {
    return 1;
  }
  pBuffer->width   = pBM->XSize;
  pBuffer->height  = pBM->YSize;
  pBuffer->stride  = pBM->BytesPerLine;
  pBuffer->tiled   = VG_LITE_LINEAR;
  pBuffer->address = (U32)pBM->pData;
  pBuffer->memory  = (void *)pBuffer->address;
  //
  // Determine format.
  //
  if (pBM->pPal && !pBM->pPal->HasTrans) {
    switch (pBM->BitsPerPixel) {
    case 1: pBuffer->format = VG_LITE_INDEX_1; break;
    case 2: pBuffer->format = VG_LITE_INDEX_2; break;
    case 4: pBuffer->format = VG_LITE_INDEX_4; break;
    case 8: pBuffer->format = VG_LITE_INDEX_8; break;
    }
    vg_lite_set_CLUT(pBM->pPal->NumEntries, (uint32_t *)pBM->pPal->pPalEntries);
    return 0;
  } else if (pBM->pMethods) {
    CHECK_FORMAT(GUI_DRAW_BMPM8888I, VG_LITE_BGRA8888);
    CHECK_FORMAT(GUI_DRAW_BMPM565,   VG_LITE_BGR565);
    CHECK_FORMAT(GUI_DRAW_BMP565,    VG_LITE_RGB565);
    CHECK_FORMAT(GUI_DRAW_A8,        VG_LITE_A8);
  }
  return 1; // Format not supported.
}

/*********************************************************************
*
*       _VGLITE_GetRenderTarget
*/
static vg_lite_buffer_t * _VGLITE_GetRenderTarget(void) {
  static vg_lite_buffer_t rt;
  GUI_MEMDEV_Handle hMem;

  hMem = GUI_MEMDEV_GetSelMemdev();
  if (hMem) {
    _VGLITE_MemdevToVgBuffer(hMem, &rt);
  } else {
    rt.width   = XSIZE_PHYS;
    rt.height  = YSIZE_PHYS;
    rt.stride  = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
    rt.tiled   = VG_LITE_LINEAR;
    rt.format  = VGLITE_DST_FORMAT;
    rt.address = _aBufferAddress[_BackBuffer];
    rt.memory  = (void *)rt.address;
  }
  return &rt;
}

/*********************************************************************
*
*       _VGLITE_GetColor
*/
static vg_lite_color_t _VGLITE_GetColor(void) {
  GUI_COLOR       Color;
  U8              Alpha;
  vg_lite_color_t res;

  //
  // Use currently set alpha component.
  //
  Alpha = GUI_GetAlpha();
  Color = (Alpha << 24) | (GUI_GetColor() & 0xFFFFFF);
  //
  // Convert to the format that VGLITE uses.
  //
  res = (vg_lite_color_t)(GUICC_8888I)->pfColor2Index(Color);
  return res;
}

#define WRITE_DASH_PATTERN(ITEM)  aDashPattern[DashCnt++] = ITEM; \
                                  aDashPattern[DashCnt++] = ITEM
/*********************************************************************
*
*       _VGLITE_EnableDraw
*/
static int _VGLITE_EnableDraw(int PenSize) {
  LCD_VGLITE_PATH_DATA * p = &_PathData;
  vg_lite_path_t       * path = &p->Path;
  vg_lite_error_t        err;
  vg_lite_color_t        color;
  vg_lite_cap_style_t    cap_style;
  F32                    thickness;
  U8                     PenShape;
  vg_lite_float_t      * pDashPattern;
  U32                    DashCnt;

  color     = _VGLITE_GetColor();
  thickness = (!PenSize) ? (F32)GUI_GetPenSize() : (F32)PenSize;
  //
  // Determine cap style according to pen shape.
  //
  PenShape  = GUI_GetPenShape();
  switch (PenShape) {
  case GUI_PS_ROUND:  cap_style = VG_LITE_CAP_ROUND;  break;
  case GUI_PS_SQUARE: cap_style = VG_LITE_CAP_SQUARE; break;
  default:
  case GUI_PS_FLAT:   cap_style = VG_LITE_CAP_BUTT;   break;
  }
  //
  // Set up line pattern if line style is set.
  //
  DashCnt      = 0;
  pDashPattern = NULL;
#if 0 // Results of dashed lines are unsatisfactory.
  {
    U8                     LineStyle;
    vg_lite_float_t        Dash, Dot;
    static vg_lite_float_t aDashPattern[6];
    LineStyle = GUI_GetLineStyle();
    if (LineStyle != GUI_LS_SOLID) {
      pDashPattern = aDashPattern;
      Dash         = thickness * 5;
      Dot          = thickness;
      switch (LineStyle) {
      case GUI_LS_DASH:
        WRITE_DASH_PATTERN(Dash);
        break;
      case GUI_LS_DOT:
        WRITE_DASH_PATTERN(Dot);
        break;
      case GUI_LS_DASHDOT:
        WRITE_DASH_PATTERN(Dash);
        WRITE_DASH_PATTERN(Dot);
        break;
      case GUI_LS_DASHDOTDOT:
        WRITE_DASH_PATTERN(Dash);
        WRITE_DASH_PATTERN(Dot);
        WRITE_DASH_PATTERN(Dot);
        break;
      }
    }
  }
#endif
  //
  // Enable stroker for the path.
  //
  VG_RET_ON_ERR(vg_lite_set_draw_path_type(path, VG_LITE_DRAW_STROKE_PATH));
  VG_RET_ON_ERR(vg_lite_set_stroke(path, cap_style, VG_LITE_JOIN_ROUND, thickness, .0f, pDashPattern, DashCnt, .0f, color));
  VG_RET_ON_ERR(vg_lite_update_stroke(path));
  return 0;
}

/*********************************************************************
*
*       _VGLITE_EnableClipping
*/
static int _VGLITE_EnableClipping(void) {
  vg_lite_error_t  err;
  const GUI_RECT * pClip;
  int              x0, y0, x1, y1;
  int              w, h;

  //
  // Convert clip rect to physical coordinates.
  //
  pClip = GUI__GetClipRect();
  x0    = pClip->x0;
  y0    = pClip->y0;
  x1    = pClip->x1;
  y1    = pClip->y1;
  if (_CurrentRotation != GUI_ROTATE_0) {
    w = x1 - x0 + 1;
    h = y1 - y0 + 1;
    _Log2PhysCoords(x0, y0, w, h, &x0, &y0);
    _Log2PhysCoords(x1, y1, w, h, &x1, &y1);
    x0 = GUI_ABS(x0);
    x1 = GUI_ABS(x1);
    y0 = GUI_ABS(y0);
    y1 = GUI_ABS(y1);
    _SwapSizesOnDemand(&w, &h);
  }
  VG_RET_ON_ERR(vg_lite_enable_scissor());
  VG_RET_ON_ERR(vg_lite_set_scissor(x0, y0, w, h) );
  return 0;
}

/*********************************************************************
*
*       _VGLITE_SetupMatrix
*/
static void _VGLITE_SetupMatrix(vg_lite_matrix_t * matrix) {
  vg_lite_coord_t  tr  = { 0.0f, 0.0f };
  F32              deg = 0.0f;
  U8               MemdevSel;

  //
  // Default: just assume identity matrix, vector paths already contain
  // all necessary coordinates.
  //
  vg_lite_identity(matrix);
  //
  // Rotate matrix if display has a rotation config enabled.
  // Only necessary if frame buffer is the rendering target.
  //
  MemdevSel = (GUI_MEMDEV_GetSelMemdev()) ? 1 : 0;
  if ((_CurrentRotation != GUI_ROTATION_0) && (MemdevSel == 0)) {
    switch (_CurrentRotation) {
    case GUI_ROTATION_CW:
      deg  = 90.0f;
      tr.y = -XSIZE_PHYS;
      break;
    case GUI_ROTATION_180:
      deg  = 180.0f;
      tr.x = (F32)-XSIZE_PHYS;
      tr.y = (F32)-YSIZE_PHYS;
      break;
    case GUI_ROTATION_CCW:
      deg  = 270.0f;
      tr.x = (F32)-YSIZE_PHYS;
      break;
    }
    vg_lite_rotate(deg, matrix);
    vg_lite_translate(tr.x, tr.y, matrix);
  }
}

/*********************************************************************
*
*       _VGLITE_Exec
*/
static int _VGLITE_Exec(VG_PATH_MODE Mode, int PenSize) {
  vg_lite_buffer_t     * rt;
  vg_lite_color_t        Color;
  LCD_VGLITE_PATH_DATA * p    = &_PathData;
  vg_lite_bbox_t       * bbox = &p->BBox;
  vg_lite_error_t        err;
  vg_lite_matrix_t       mat;
  vg_lite_quality_t      aa_quality;
  
  //
  // Get current rendering target (frame buffer or memdev).
  //
  rt = _VGLITE_GetRenderTarget();
  //
  // Init given path with previously set parameters.
  //
  aa_quality = (Mode & VG_PATH_MODE_AA) ? VG_LITE_HIGH : VG_LITE_LOW;
  VG_RET_ON_ERR(p->pfInitPath(&p->Path, VG_LITE_FP32, aa_quality, p->NumBytes, p->pPathData,
                              bbox->min_x, bbox->min_y, bbox->max_x, bbox->max_y));
  //
  // Enable stroker if required.
  //
  if (Mode & VG_PATH_MODE_STROKE) {
    VG_RET_ON_ERR(_VGLITE_EnableDraw(PenSize));
  }
  //
  // Set clip rect.
  //
  VG_RET_ON_ERR(_VGLITE_EnableClipping());
  //
  // Setup matrix according to display rotation.
  //
  _VGLITE_SetupMatrix(&mat);
  //
  // Setup operation to be done...
  //
  Color = _VGLITE_GetColor();
  if        (Mode & (VG_PATH_MODE_STROKE | VG_PATH_MODE_FILL)) { // Handle fill/stroke operation.
    VG_RET_ON_ERR(vg_lite_draw(rt, &p->Path, VG_LITE_FILL_EVEN_ODD, &mat, VG_LITE_BLEND_SRC_OVER, Color));
  } else if (Mode & VG_PATH_MODE_BLIT) {                         // Handle blit operation.
    VG_RET_ON_ERR(vg_lite_blit(rt, &p->SrcBuffer, &mat, VG_LITE_BLEND_SRC_OVER, Color, VG_LITE_FILTER_POINT));
  } else if (Mode & VG_PATH_MODE_PATTERN) {                      // Handle pattern operation.
    VG_RET_ON_ERR(vg_lite_draw_pattern(rt, &p->Path, VG_LITE_FILL_EVEN_ODD, &mat, &p->SrcBuffer,
                                       &p->PatternMatrix, VG_LITE_BLEND_SRC_OVER, VG_LITE_PATTERN_PAD,
                                       Color, VG_LITE_FILTER_POINT));
  }
  //
  // Clear cache to avoid artefacts on the screen.
  //
  GUI_DCACHE_Clear(0);
  //
  // Execute buffered commands.
  //
  VG_RET_ON_ERR(vg_lite_finish());
  //
  // Cleanup, if we're here and the below functions fail, we still return 0 because the engine
  // already drew something onto the screen.
  //
  vg_lite_disable_scissor();
  vg_lite_clear_path(&p->Path);
  return 0;
}

/*********************************************************************
*
*       _VGLITE_GetNumParas
*/
static int _VGLITE_GetNumParas(int OpCode) {
  int NumParas = 0;

  switch (OpCode) {
  case VLC_OP_END:
  case VLC_OP_CLOSE:
    NumParas = 0;
    break;
  case VLC_OP_MOVE:
  case VLC_OP_MOVE_REL:
  case VLC_OP_LINE:
  case VLC_OP_LINE_REL:
    NumParas = 2;
    break;
  case VLC_OP_QUAD:
  case VLC_OP_QUAD_REL:
    NumParas = 4;
    break;
  case VLC_OP_CUBIC:
  case VLC_OP_CUBIC_REL:
    NumParas = 6;
    break;
  case VLC_OP_SCCWARC:
  case VLC_OP_SCCWARC_REL:
  case VLC_OP_SCWARC:
  case VLC_OP_SCWARC_REL:
  case VLC_OP_LCCWARC:
  case VLC_OP_LCCWARC_REL:
  case VLC_OP_LCWARC:
  case VLC_OP_LCWARC_REL:
    NumParas = 5;
    break;
  }
  return NumParas;
}

/*********************************************************************
*
*       _VGLITE_OperationSize
*/
static int _VGLITE_OperationSize(int OpCode) {
  int Size     = sizeof(F32); // Size of single value in path array.
  int NumParas = _VGLITE_GetNumParas(OpCode);
  return (Size * 1) + // The operation code
         (Size * NumParas);
}

/*********************************************************************
*
*       _VGLITE_SetupRectPath
*/
static void _VGLITE_SetupRectPath(int x0, int y0, int x1, int y1, int r) {
  LCD_VGLITE_PATH_DATA * pRes = &_PathData;
  vg_lite_coord_t        p0, p1;
  F32                    rad;
  U8                   * pPath = pRes->pPathData;

  if (r < 0) {
    return;
  }
  //
  // Convert from HiRes to floating point.
  //
  CONVERT_HIRES_COORD(x0, y0, p0);
  CONVERT_HIRES_COORD(x1, y1, p1);
  rad = CONVERT_HIRES(r);
  //
  // Otherwise rectangle path does not cover entire given area.
  //
  p1.x += 1.0f;
  p1.y += 1.0f;
  //
  // Normal rect path, no rounded corners.
  //
  if (r == 0) {
    WRITE_MOVE(pPath, p0.x, p0.y);
    WRITE_LINE(pPath, p1.x, p0.y);
    WRITE_LINE(pPath, p1.x, p1.y);
    WRITE_LINE(pPath, p0.x, p1.y);
    WRITE_LINE(pPath, p0.x, p0.y);
    WRITE_END (pPath);
    //
    // Path does not include arcs.
    //
    pRes->pfInitPath = vg_lite_init_path;
  } else { // Rounded corners.
    WRITE_MOVE(pPath, p0.x + rad, p0.y);
    WRITE_LINE(pPath, p1.x - rad, p0.y);
    WRITE_ARC (pPath, VLC_OP_SCWARC, rad, rad, 0, p1.x, p0.y + rad);
    WRITE_LINE(pPath, p1.x, p1.y - rad);
    WRITE_ARC (pPath, VLC_OP_SCWARC, rad, rad, 0, p1.x - rad, p1.y);
    WRITE_LINE(pPath, p0.x + rad, p1.y);
    WRITE_ARC (pPath, VLC_OP_SCWARC, rad, rad, 0, p0.x, p1.y - rad);
    WRITE_LINE(pPath, p0.x, p0.y + rad);
    WRITE_ARC (pPath, VLC_OP_SCWARC, rad, rad, 0, p0.x + rad, p0.y);
    WRITE_END (pPath);
    //
    // Path includes arcs.
    //
    pRes->pfInitPath = vg_lite_init_arc_path;
  }
  //
  // Setup boundary box.
  //
  pRes->BBox.min_x = p0.x;
  pRes->BBox.min_y = p0.y;
  pRes->BBox.max_x = p1.x;
  pRes->BBox.max_y = p1.y;
  //
  // Get path size.
  //
  pRes->NumBytes = (U32)pPath - (U32)pRes->pPathData;
}

/*********************************************************************
*
*       _VGLITE_SetupEllipsePath
*/
static void _VGLITE_SetupEllipsePath(int x0, int y0, int rx, int ry) {
  vg_lite_coord_t c;
  F32 frx, fry;
  U8 * pPath;
  LCD_VGLITE_PATH_DATA * pRes = &_PathData;
  vg_lite_bbox_t * bbox = &pRes->BBox;

  pPath = (U8 *)pRes->pPathData;
  //
  // Convert HiRes coordinates.
  //
  CONVERT_HIRES_COORD(x0, y0, c);
  frx = CONVERT_HIRES(rx);
  fry = CONVERT_HIRES(ry);
  //
  // Draw lower half
  //
  WRITE_MOVE(pPath, c.x + frx, c.y);
  WRITE_ARC (pPath, VLC_OP_SCWARC, frx, fry, 0, c.x - frx, c.y);
  //
  // Draw upper half
  //
  WRITE_ARC(pPath, VLC_OP_SCWARC, frx, fry, 0, c.x + frx, c.y);
  WRITE_END(pPath);
  //
  // Set up boundary box as the size of the ellipse.
  //
  bbox->min_x = c.x - frx;
  bbox->min_y = c.y - fry;
  bbox->max_x = c.x + frx;
  bbox->max_y = c.y + fry;
  //
  // Get path size.
  //
  pRes->NumBytes = (U32)pPath - (U32)pRes->pPathData;
  //
  // Path includes arcs.
  //
  pRes->pfInitPath = vg_lite_init_arc_path;
}

/*********************************************************************
*
*       _VGLITE_SetupPolygonPath
*/
static int _VGLITE_SetupPolygonPath(const GUI_POINT * pSrc, int NumPoints, int x, int y) {
  int                    NumPathItems, n;
  LCD_VGLITE_PATH_DATA * p = &_PathData;
  vg_lite_bbox_t       * bbox = &p->BBox;
  vg_lite_coord_t        coord, pos;
  U8                   * pPath = (U8 *)p->pPathData;

  //
  // Check if buffer size is sufficient for the amount of points.
  //
  NumPathItems = _VGLITE_OperationSize(VLC_OP_MOVE) + _VGLITE_OperationSize(VLC_OP_END) +
                (_VGLITE_OperationSize(VLC_OP_LINE) * NumPoints);
  if (NumPathItems > VG_LITE_PATH_BUFFER_SIZE) {
    return 1;
  }
  bbox->min_x = (F32)LCD_GetXSize();
  bbox->min_y = (F32)LCD_GetYSize();
  for (n = 0; n < NumPoints; n++) {
    //
    // Set up bounding box.
    //
    CONVERT_HIRES_COORD(pSrc[n].x, pSrc[n].y, coord);
    if (coord.x > bbox->max_x) {
      bbox->max_x = coord.x;
    }
    if (coord.y > bbox->max_y) {
      bbox->max_y = coord.y;
    }
    if (coord.x < bbox->min_x) {
      bbox->min_x = coord.x;
    }
    if (coord.y < bbox->min_y) {
      bbox->min_y = coord.y;
    }
    //
    // And now the path data.
    //
    CONVERT_HIRES_COORD(x, y, pos);
    coord.x += pos.x;
    coord.y += pos.y;
    if (n == 0) {
      WRITE_MOVE(pPath, coord.x, coord.y);
    } else {
      WRITE_LINE(pPath, coord.x, coord.y);
    }
  }
  WRITE_END(pPath); // Close path
  //
  // Init the rest of the parameters.
  //
  p->NumBytes   = (U32)pPath - (U32)p->pPathData;
  p->pfInitPath = vg_lite_init_path;
  return 0;
}

/*********************************************************************
*
*       _VGLITE_EllipseOperation
*
*  Generic routine for drawing/filling ellipses or circles.
*/
static int _VGLITE_EllipseOperation(int x0, int y0, int rx, int ry, VG_PATH_MODE Mode) {  
  vg_lite_error_t err;

  _VGLITE_SetupEllipsePath(x0, y0, rx, ry);
  VG_RET_ON_ERR(_VGLITE_Exec(Mode, 0));
  return 0;
}

/*********************************************************************
*
*       Ellipse and circle functions...
*/
static int _VGLITE_DrawCircle   (int x0, int y0, int r)          { return _VGLITE_EllipseOperation(x0, y0, r,  r,  VG_DRAW);    }
static int _VGLITE_DrawCircleAA (int x0, int y0, int r)          { return _VGLITE_EllipseOperation(x0, y0, r,  r,  VG_DRAW_AA); }
static int _VGLITE_FillCircle   (int x0, int y0, int r)          { return _VGLITE_EllipseOperation(x0, y0, r,  r,  VG_FILL);    }
static int _VGLITE_FillCircleAA (int x0, int y0, int r)          { return _VGLITE_EllipseOperation(x0, y0, r,  r,  VG_FILL_AA); }

/*********************************************************************
*
*       _VGLITE_RectOperation
*
*  Generic routine for drawing/filling rectangles, with or without radius.
*/
static int _VGLITE_RectOperation(int x0, int y0, int x1, int y1, int r, VG_PATH_MODE Mode) {
  vg_lite_error_t err;

  _VGLITE_SetupRectPath(x0, y0, x1, y1, r);
  VG_RET_ON_ERR(_VGLITE_Exec(Mode, 0));
  return 0;
}

/*********************************************************************
*
*       Rectangle functions...
*/
static int _VGLITE_DrawRect         (int x0, int y0, int x1, int y1)        { return _VGLITE_RectOperation(x0, y0, x1, y1, 0, VG_DRAW);    }
static int _VGLITE_DrawRoundedRect  (int x0, int y0, int x1, int y1, int r) { return _VGLITE_RectOperation(x0, y0, x1, y1, r, VG_DRAW);    }
static int _VGLITE_FillRoundedRect  (int x0, int y0, int x1, int y1, int r) { return _VGLITE_RectOperation(x0, y0, x1, y1, r, VG_FILL);    }

/*********************************************************************
*
*       _VGLITE_PolygonOperation
*
*  Generic routine for drawing/filling polygons.
*/
static int _VGLITE_PolygonOperation(const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y, VG_PATH_MODE Mode) {
  vg_lite_error_t err;

  VG_RET_ON_ERR(_VGLITE_SetupPolygonPath(pSrc, NumPoints, x, y));
  VG_RET_ON_ERR(_VGLITE_Exec(Mode, Thickness));
  return 0;
}

/*********************************************************************
*
*       Polygon functions...
*/
static int _VGLITE_DrawPolygonAA(const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y) { return _VGLITE_PolygonOperation(pSrc, NumPoints, Thickness, x, y, VG_DRAW_AA); }
static int _VGLITE_FillPolygonAA(const GUI_POINT * pSrc, int NumPoints, int x, int y)                { return _VGLITE_PolygonOperation(pSrc, NumPoints, 0,         x, y, VG_FILL_AA); }

/*********************************************************************
*
*       _VGLITE_LineOperation
*/
static int _VGLITE_LineOperation(int x0, int y0, int x1, int y1, VG_PATH_MODE Mode) {
  vg_lite_error_t        err;
  LCD_VGLITE_PATH_DATA * p     = &_PathData;
  vg_lite_bbox_t       * bbox  = &p->BBox;
  U8                   * pPath = (U8 *)p->pPathData;
  
  //
  // Set up path data.
  //
  WRITE_MOVE(pPath, x0, y0);
  WRITE_LINE(pPath, x1, y1);
  WRITE_END (pPath);
  p->pfInitPath = vg_lite_init_path;
  p->NumBytes   = (U32)pPath - (U32)p->pPathData;
  //
  // Set up boundary box as the area of the line.
  //
  bbox->min_x = (F32)x0;
  bbox->min_y = (F32)y0;
  bbox->max_x = (F32)x1;
  bbox->max_y = (F32)y1;
  //
  // Draw it.
  //
  VG_RET_ON_ERR(_VGLITE_Exec(Mode, 0));
  return 0;
}

/*********************************************************************
*
*       Line functions...
*/
static int _VGLITE_DrawLine  (int x0, int y0, int x1, int y1) { return _VGLITE_LineOperation(x0, y0, x1, y1, VG_DRAW);    }
static int _VGLITE_DrawLineAA(int x0, int y0, int x1, int y1) { return _VGLITE_LineOperation(x0, y0, x1, y1, VG_DRAW_AA); }

/*********************************************************************
*
*       _VGLITE_GetCosSin
*/
static void _VGLITE_GetCosSin(I32 a0, I32 a1, int r, F32 * cos_a0, F32 * sin_a0, F32 * cos_a1, F32 * sin_a1) {
  F32 fr;

  fr      = (F32)r;
  *cos_a0 = ((F32)GUI__CosHQ(a0) * fr) / 65536.0f;
  *sin_a0 = ((F32)GUI__SinHQ(a0) * fr) / 65536.0f;
  *cos_a1 = ((F32)GUI__CosHQ(a1) * fr) / 65536.0f;
  *sin_a1 = ((F32)GUI__SinHQ(a1) * fr) / 65536.0f;
}

/*********************************************************************
*
*       _VGLITE_DrawArc
*/
static int _VGLITE_DrawArc(int x0, int y0, int rx, int ry, I32 a0, I32 a1) {
  vg_lite_error_t        err;
  LCD_VGLITE_PATH_DATA * p = &_PathData;
  U8                   * pPath = (U8 *)p->pPathData;
  vg_lite_bbox_t       * bbox = &p->BBox;
  F32                    frx;
  vg_lite_coord_t        start, end, center;
  U8                     OpCode;
  F32                    sin_a0, cos_a0, sin_a1, cos_a1;

  GUI_USE_PARA(ry);
  CONVERT_HIRES_COORD(x0, y0, center);
  frx = (F32)rx;
  //
  // Get start and end points on the arc from the given coordinate and the angles.
  //
  _VGLITE_GetCosSin(a0, a1, rx, &cos_a0, &sin_a0, &cos_a1, &sin_a1);
  start.x = center.x - cos_a0;
  start.y = center.y - sin_a0;
  end.x   = center.x - cos_a1;
  end.y   = center.y - sin_a1;
  //
  // Determine op code for arc, based on size.
  //
  OpCode = ((a1 - a0) >= 180000) ? VLC_OP_LCCWARC : VLC_OP_SCCWARC;
  //
  // Path begins at the start point of the arc.
  //
  WRITE_MOVE(pPath, start.x, start.y);
  WRITE_ARC (pPath, OpCode, frx, frx, 0, end.x, end.y);
  //
  // Set up boundary box as the size of the circle.
  //
  bbox->min_x = (F32)center.x - frx;
  bbox->min_y = (F32)center.y - frx;
  bbox->max_x = (F32)center.x + frx;
  bbox->max_y = (F32)center.y + frx;
  //
  // Set up the rest of the path parameters.
  //
  p->pfInitPath = vg_lite_init_arc_path;
  p->NumBytes   = (U32)pPath - (U32)p->pPathData;
  //
  // Draw it.
  //
  VG_RET_ON_ERR(_VGLITE_Exec(VG_DRAW_AA, 0));
  return 0;
}

/*********************************************************************
*
*       _VGLITE_RotateMatrixCCW
*/
static void _VGLITE_RotateMatrixCCW(vg_lite_float_t degrees, vg_lite_matrix_t * matrix) {
  //
  // Convert degrees into radians.
  //
  F32 angle = degrees / 180.0f * M_PI;
  //
  // Compute cosine and sine values.
  //
  F32 cos_angle = cosf(angle);
  F32 sin_angle = sinf(angle);
  //
  // Setup CCW rotation matrix.
  //
  vg_lite_matrix_t rot_mat = {{
    { cos_angle, sin_angle, 0.0f},
    {-sin_angle, cos_angle, 0.0f},
    { 0.0f,      0.0f,      1.0f}
  }};
  //
  // Multiply with current matrix.
  //
  vg_lite_multiply(matrix, &rot_mat);
}

#if GUI_SUPPORT_MEMDEV
/*********************************************************************
*
*       _VGLITE__RotateMemdev
*/
static int _VGLITE__RotateMemdev(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, float HiResFact) {
  void           * pSrc, * pDst;
  int              xSizeSrc, ySizeSrc, xSizeDst, ySizeDst;
  int              BppSrc, BppDst;
  vg_lite_error_t  err;
  vg_lite_matrix_t mat;
  vg_lite_buffer_t src, dst;
  F32              deg, scale;
  F32              x, y;
  
  _GetMemdevParams(hSrc, &pSrc, &xSizeSrc, &ySizeSrc, &BppSrc);
  _GetMemdevParams(hDst, &pDst, &xSizeDst, &ySizeDst, &BppDst);
  if (xSizeSrc % 16) {
    return 1;
  }
  if ((BppSrc != 32) || (BppDst != 32)) {
    return 1;
  }
  GUI_DCACHE_Clear(0);
  //
  // Set up source and destination buffers.
  //
  _VGLITE_MemdevToVgBuffer(hSrc, &src);
  _VGLITE_MemdevToVgBuffer(hDst, &dst);
  //
  // Translate given bitmap offset.
  //
  vg_lite_identity(&mat);
  vg_lite_translate((F32)dx / HiResFact,
                    (F32)dy / HiResFact, &mat);
  //
  // Translate center of bitmap to origin to rotate CCW around the center point.
  //
  x = (((F32)xSizeSrc) / 2.0f);
  y = (((F32)ySizeSrc) / 2.0f);
  vg_lite_translate(x, y, &mat);
  deg = (F32)a / 1000.f;
  _VGLITE_RotateMatrixCCW(deg, &mat);
  //
  // Scale using the given factor.
  //
  scale = ((F32)Mag / 1000.f);
  vg_lite_scale(scale, scale, &mat);
  //
  // Translate back from center point.
  //
  vg_lite_translate(-x, -y, &mat);
  //
  // Blit and done.
  //
  VG_RET_ON_ERR(vg_lite_blit(&dst, &src, &mat, VG_LITE_BLEND_SRC_OVER, 0, VG_LITE_FILTER_LINEAR));
  VG_RET_ON_ERR(vg_lite_finish());
  return 0;
}

/*********************************************************************
*
*       Memdev rotate & scale functions...
*/
static int _VGLITE_RotateMemdevLR(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag) { return _VGLITE__RotateMemdev(hSrc, hDst, dx, dy, a, Mag, 1.0f); }
static int _VGLITE_RotateMemdevHR(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag) { return _VGLITE__RotateMemdev(hSrc, hDst, dx, dy, a, Mag, 8.0f); }
#endif

/*********************************************************************
*
*       _GetDisplayRotationMatrix()
*/
static void _GetDisplayRotationMatrix(GUI_MATRIX * pMat, int x, int y) {
  int        Orientation;
  float      a = 0.0f;

  //
  // Now setup a matrix according to the current display rotation.
  //
  if (GUI_DEVICE_GetDeviceClass() == DEVICE_CLASS_DRIVER) {
    Orientation = LCD_ROTATE_GetCurrentOrientation();
    switch (Orientation) {
    default:
    case GUI_ROTATION_0:
      a = 0.0f;
      break;
    case GUI_ROTATION_CW:
      y = -XSIZE_PHYS + y;
      a = -90.0f;
      break;
    case GUI_ROTATION_180:
      x = -XSIZE_PHYS + x;
      y = -YSIZE_PHYS + y;
      a = -180.0f;
      break;
    case GUI_ROTATION_CCW:
      x = -YSIZE_PHYS + x;
      a = -270.0f;
      break;
    }
  }
  //
  // Rotate matrix accordingly.
  //
  GUI_MATRIX_Identity(pMat);
  GUI_MATRIX_Rotate(pMat, a);
  GUI_MATRIX_Translate(pMat, x, y);
}

/*********************************************************************
*
*       _ConvertMatrix
*/
static void _ConvertMatrix(const GUI_MATRIX * pMatIn, vg_lite_matrix_t * pMatOut) {
  const float * p;

  p = (const float *)pMatIn;
  //
  // Convert matrix from emWin/OpenVG format into VGLite format.
  //
  GUI_MATRIX_Initialize((GUI_MATRIX *)pMatOut, p[0], p[1], p[2],
                                               p[3], p[4], p[5],
                                               p[6], p[7], p[8]);
}

/*********************************************************************
*
*       _VGLITE_DrawBitmapEx
*
*  This function is a bit slower than PXP, but the advantage is that
*  the bitmap may also be semi-transparent.
*/
static int _VGLITE_DrawBitmapEx(const GUI_BITMAP * pBitmap, int x0, int y0, int xMag, int yMag) {
  vg_lite_buffer_t * rt = _VGLITE_GetRenderTarget();
  vg_lite_buffer_t   src;
  vg_lite_error_t    err;
  vg_lite_matrix_t   m;
  GUI_MATRIX         Mat;
  F32                scale_x, scale_y;
  
  GUI_DCACHE_Clear(0);
  //
  // Set up source buffer.
  //
  VG_RET_ON_ERR(_VGLITE_BitmapToVgBuffer(pBitmap, &src));
  //
  // Translate to given pos.
  //
  _GetDisplayRotationMatrix(&Mat, x0, y0);
  _ConvertMatrix(&Mat, &m);
  //
  // Scale matrix with given scaling factors.
  // Keep the sign to flip the matrix as well.
  //
  scale_x = (F32)xMag / 1000.f;
  scale_y = (F32)yMag / 1000.f;
  vg_lite_scale(scale_x, scale_y, &m);
  VG_RET_ON_ERR(vg_lite_blit(rt, &src, &m, VG_LITE_BLEND_SRC_OVER, 0, VG_LITE_FILTER_LINEAR));
  VG_RET_ON_ERR(vg_lite_finish());
  return 0;
}

/*********************************************************************
*
*       _ConvertA8
*/
static void * _ConvertA8(int * pxSize, int ySize, const void * pBitmap, int * pStride) {
  U32    NumBytes;
  int    xSizeAligned;
  int    xSize;
  void * pAlloc;
  int    y;
  U8   * pDest, *pSrc;

  xSize = *pxSize;
  //
  // xSize of bitmap must be 16 byte aligned so that it can be drawn.
  //
  xSizeAligned = ROUND_UP_BY(16, xSize);
  NumBytes     = xSizeAligned * ySize;
  //
  // Try to allocate temp. buffer
  //
  pAlloc = GUI_ALLOC_calloc(1, NumBytes);
  if (!pAlloc) {
    return NULL;
  }
  //
  // Copy linewise
  //
  for (y = 0; y < ySize; y++) {
    pDest = (U8 *)pAlloc  + (xSizeAligned * y);
    pSrc  = (U8 *)pBitmap + (xSize        * y);
    memcpy(pDest, pSrc, xSize);
  }
  //
  // Save new sizes on success
  //
  *pStride = xSizeAligned;
  *pxSize  = xSizeAligned;
  return pAlloc;
}

/*********************************************************************
*
*       _VGLITE_DrawAlphaBitmap
*/
static void _VGLITE_DrawAlphaBitmap(const void * pSrc, vg_lite_buffer_t * rt, vg_lite_matrix_t * mat, int xSize, int ySize, int BytesPerLine) {
  vg_lite_buffer_t src;
  vg_lite_color_t  color;
  void           * pBuffer = NULL;

  //
  // Bitmap may be unaligned, allocate temp. buffer
  //
  if (xSize % 16) {
    pBuffer     = _ConvertA8(&xSize, ySize, pSrc, &BytesPerLine);
    src.address = (uint32_t)pBuffer;
  } else {
    src.address = (uint32_t)pSrc;
  }
  src.memory            = (void *)src.address;
  src.stride            = BytesPerLine;
  src.width             = xSize;
  src.height            = ySize;
  src.format            = VG_LITE_A8;
  src.tiled             = VG_LITE_LINEAR;
  src.image_mode        = VG_LITE_MULTIPLY_IMAGE_MODE;
  src.transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
  //
  // Do the blitting
  //
  GUI_DCACHE_Clear(0);
  color = _VGLITE_GetColor();
  _VGLITE_EnableClipping();
  vg_lite_blit(rt, &src, mat, VG_LITE_BLEND_SRC_OVER, color, VG_LITE_FILTER_POINT);
  vg_lite_finish();
  vg_lite_disable_scissor();
  //
  // Cleanup temp. buffer
  //
  if (pBuffer) {
    GUI_ALLOC_free(pBuffer);
  }
}

/*********************************************************************
*
*       _VGLITE_DrawBitmapA8
*/
static void _VGLITE_DrawBitmapA8(int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine) {
  vg_lite_buffer_t * rt;
  vg_lite_matrix_t   m;
  GUI_MATRIX         Mat;

  GUI_USE_PARA(LayerIndex);
  rt = _VGLITE_GetRenderTarget();
  //
  // Translate to given pos
  //
  _GetDisplayRotationMatrix(&Mat, x, y);
  _ConvertMatrix(&Mat, &m);
  //
  // Draw alpha bitmap
  //
  _VGLITE_DrawAlphaBitmap(p, rt, &m, xSize, ySize, BytesPerLine);
}

/*********************************************************************
*
*       _VGLITE_DrawBitmapA8Memdev
*/
static void _VGLITE_DrawBitmapA8Memdev(void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc) {
  vg_lite_matrix_t m;
  vg_lite_buffer_t dst;

  //
  // Init destination buffer.
  //
  dst         = *_VGLITE_GetRenderTarget();
  dst.memory  = pDst;
  dst.address = (uint32_t)pDst;
  dst.width   = xSize;
  dst.height  = ySize;
  dst.stride  = BytesPerLineDst;
  //
  // Draw alpha bitmap
  //
  vg_lite_identity(&m);
  _VGLITE_DrawAlphaBitmap(pSrc, &dst, &m, xSize, ySize, BytesPerLineSrc);
}

/*********************************************************************
*
*       _VGLITE_Init
*/
static void _VGLITE_Init(void) {
  const clock_root_config_t gc355ClockConfig = {
    .clockOff = false,
    .mux      = 4, /*!< PLL_528. */
    .div      = 1,
  };
  //
  // Init clock and interrupt
  //
  CLOCK_SetRootClock(kCLOCK_Root_Gc355, &gc355ClockConfig);
  CLOCK_GetRootClockFreq(kCLOCK_Root_Gc355);
  CLOCK_EnableClock(kCLOCK_Gpu2d);
  NVIC_SetPriority(GPU2D_IRQn, 10);
  EnableIRQ(GPU2D_IRQn);
  //
  // Init VGLITE library
  //
  vg_lite_init_mem(0x41800000, 0, &_aBufferVGLITE[0], sizeof(_aBufferVGLITE));
  vg_lite_set_command_buffer_size((256 << 10));
  //
  // Init tessellation buffer
  //
  vg_lite_init(XSIZE_PHYS, YSIZE_PHYS);
  //
  // Init pointer to path buffer
  //
  _PathData.pPathData = (void *)&_aPathBuffer[0];
  //
  // Set functions pointers to emWin drawing operations
  //
  LCD_VGLITE_Enable(1);
}

/*********************************************************************
*
*       Static code: Display init
*
**********************************************************************
*/
/*********************************************************************
*
*       _VSyncCallback
*/
static void _VSyncCallback(void *p, void *SwitchOffBuffer) {
  GUI_USE_PARA(p);
  GUI_USE_PARA(SwitchOffBuffer);
  if (_PendingBuffer >= 0) {
    GUI_MULTIBUF_Confirm(_PendingBuffer);
#if (USE_OS == 1)
      _Semaphore_Give(_hSema_WaitForVSYNC);
#endif
    _PendingBuffer = -1;
  } else if (_PendingBuffer == -2) {
    _PendingBuffer = -1;
  }
}

/*********************************************************************
*
*       _InitController
*/
static void _InitController(void) {
  static U8    Done;
  int          status;
  dc_fb_info_t fbInfo;

  if (Done == 0) {
#if (USE_OS == 1)
    _Semaphore_Create(&_hSema_WaitForPXP);
    _Semaphore_Create(&_hSema_WaitForVSYNC);
#endif
    //
    // Set up GPIOs
    //
    BOARD_InitMipiPanelPins();
    //
    // Set up LCD controller
    //
    BOARD_PrepareDisplayController();
    //
    // Init controller.
    //
    status = g_dc.ops->init(&g_dc);
    if (kStatus_Success != status) {
      while (1);
    }
    //
    // Set custom IRQ prio.
    //
    NVIC_ClearPendingIRQ(LCDIFv2_IRQn);
    NVIC_SetPriority(LCDIFv2_IRQn, 8);
    EnableIRQ(LCDIFv2_IRQn);
    //
    // Config framebuffer size.
    //
    g_dc.ops->getLayerDefaultConfig(&g_dc, 0, &fbInfo);
    fbInfo.pixelFormat = PIXEL_FORMAT;
    fbInfo.width       = XSIZE_PHYS;
    fbInfo.height      = YSIZE_PHYS;
    fbInfo.strideBytes = XSIZE_PHYS * LCD_BYTES_PER_PIXEL;
    g_dc.ops->setLayerConfig(&g_dc, 0, &fbInfo);
    g_dc.ops->setCallback(&g_dc, 0, _VSyncCallback, NULL);
    //
    // Init PXP
    //
    PXP_Init(PXP);
    PXP_READ_QOS = 15;
    LCD_PXP_Enable(1);
#if (USE_OS == 1)
    NVIC_EnableIRQ(PXP_IRQn);
    NVIC_SetPriority(PXP_IRQn, 0xf);
    PXP_EnableInterrupts( PXP, kPXP_CompleteInterruptEnable );
#endif
    //
    // Init VGLITE
    //
    _VGLITE_Init();
    Done = 1;
  }
}

/*********************************************************************
*
*       _ClearCache
*/
static void _ClearCache(U32 v) {
  GUI_USE_PARA(v);
  SCB_CleanInvalidateDCache();
}

/*********************************************************************
*
*       _ConfigDriver
*
* Purpose:
*   Called from LCD_ROTATE_SetSelEx() after a new driver has been selected in order to
*   configure the new driver.
*/
static void _ConfigDriver(GUI_DEVICE * pDevice, int Index, int LayerIndex) {
  GUI_USE_PARA(Index);
  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void *)_aBufferAddress[0]);
  //
  // Update the current display rotation angle for PXP.
  //
  if      ((pDevice->pDeviceAPI == GUIDRV_LIN_32)     || (pDevice->pDeviceAPI == GUIDRV_LIN_16))     {
    _PXP_Rotation    = kPXP_Rotate0;
    _CurrentRotation = GUI_ROTATION_0;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OSX_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OSX_16)) {
    _PXP_Rotation    = kPXP_Rotate90;
    _CurrentRotation = GUI_ROTATION_CW;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OXY_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OXY_16)) {
    _PXP_Rotation    = kPXP_Rotate180;
    _CurrentRotation = GUI_ROTATION_180;
  }
  else if ((pDevice->pDeviceAPI == GUIDRV_LIN_OSY_32) || (pDevice->pDeviceAPI == GUIDRV_LIN_OSY_16)) {
    _PXP_Rotation    = kPXP_Rotate270;
    _CurrentRotation = GUI_ROTATION_CCW;
  }
  //
  // Also update multitouch module if rotation is changed.
  //
#if GUI_USE_MULTI_TOUCH
  GUI_MTOUCH_SetOrientationEx(_CurrentRotation, LayerIndex);
#else
  GUI_USE_PARA(LayerIndex);
#endif
}

/*********************************************************************
*
*       Interrupt handlers
*
**********************************************************************
*/
#if (USE_OS == 1)
/*********************************************************************
*
*       PXP_IRQHandler
*/
void PXP_IRQHandler(void);
void PXP_IRQHandler(void) {
  if (PXP_GetStatusFlags( PXP ) & kPXP_CompleteFlag ) {
    PXP_ClearStatusFlags( PXP, kPXP_CompleteFlag );
    _Semaphore_Give(_hSema_WaitForPXP);
  }
}
#endif

/*********************************************************************
*
*       GPU2D_IRQHandler
*/
void GPU2D_IRQHandler(void);
void GPU2D_IRQHandler(void) {
  vg_lite_IRQHandler();
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
#define SET_HW_FUNC(Setter, Fx)              Setter((Enable) ? Fx : NULL)
#define SET_HW_FUNCS_2(Setter, Fx1, Fx2)     Setter((Enable) ? Fx1 : NULL, \
                                                    (Enable) ? Fx2 : NULL)
#define SET_DEV_FUNC(Index, Fx)              LCD_SetDevFunc(0, Index, (void(*)(void))((Enable) ? Fx : NULL))

/*********************************************************************
*
*       LCD_PXP_Enable
*/
void LCD_PXP_Enable(int Enable) {
  //
  // Hardware fill rect disabled due to slow speed.
  //
  //GUI_AlphaEnableFillRectHW(Enable);
  //SET_DEV_FUNC  (LCD_DEVFUNC_FILLRECT,     _LCD_PXP_FillRect);
  SET_DEV_FUNC  (LCD_DEVFUNC_COPYBUFFER,   _LCD_PXP_CopyBuffer);
  SET_DEV_FUNC  (LCD_DEVFUNC_COPYRECT,     _LCD_PXP_CopyRect);
  //
  // Set up functions for drawing bitmaps and memory devices with PXP
  //
  SET_HW_FUNC   (GUI_SetFuncDrawBitmapEx,  _LCD_PXP_DrawBitmapEx);
  SET_HW_FUNC   (GUI_SetFuncMixColorsBulk, _LCD_PXP_MixColorsBulk);
  SET_HW_FUNCS_2(GUI_SetFuncDrawAlpha,     _LCD_PXP_DrawMemdevAlpha, _LCD_PXP_DrawBitmap32bpp);
#if GUI_SUPPORT_MEMDEV
#if GUI_MEMDEV_SUPPORT_CUSTOMDRAW
  SET_HW_FUNC   (GUI_MEMDEV_SetDrawMemdev16bppFunc, _LCD_PXP_DrawMemdev16bpp);
#endif
  SET_HW_FUNCS_2(GUI_SetFuncDrawM565, _LCD_PXP_DrawMemdevM565, _LCD_PXP_DrawBitmapM565);
  //
  // Memdev manupulation functions with PXP
  //
  SET_HW_FUNC   (GUI_MEMDEV_SetBlendFunc, _LCD_PXP_BlendMemdev);
#endif
  //
  // Color conversion functions
  //
  GUI_PXP_SETCOLORCONV(M1555I);
  GUI_PXP_SETCOLORCONV(M565);
  GUI_PXP_SETCOLORCONV(M4444I);
  GUI_PXP_SETCOLORCONV(M8888I);
}

/*********************************************************************
*
*       LCD_VGLITE_Enable
*/
void LCD_VGLITE_Enable(int Enable) {
#if GUI_SUPPORT_AA
  //
  // Set AA drawing functions
  //
  SET_HW_FUNC(GUI_AA_SetFuncDrawArc,         _VGLITE_DrawArc);
  SET_HW_FUNC(GUI_AA_SetFuncDrawCircle,      _VGLITE_DrawCircleAA);
  SET_HW_FUNC(GUI_AA_SetFuncDrawLine,        _VGLITE_DrawLineAA);
  SET_HW_FUNC(GUI_AA_SetFuncDrawPolyOutline, _VGLITE_DrawPolygonAA);
  //
  // Set AA filling functions
  //
  SET_HW_FUNC(GUI_AA_SetFuncFillCircle,      _VGLITE_FillCircleAA);
  SET_HW_FUNC(GUI_AA_SetFuncFillPolygon,     _VGLITE_FillPolygonAA);
#endif
  //
  // Set non-AA drawing functions
  //
  SET_HW_FUNC(GUI_SetFuncDrawCircle,         _VGLITE_DrawCircle);
  SET_HW_FUNC(GUI_SetFuncDrawLine,           _VGLITE_DrawLine);
  SET_HW_FUNC(GUI_SetFuncDrawRect,           _VGLITE_DrawRect);
  SET_HW_FUNC(GUI_SetFuncDrawRoundedRect,    _VGLITE_DrawRoundedRect);
  //
  // Set non-AA filling functions
  //
  SET_HW_FUNC(GUI_SetFuncFillRoundedRect,    _VGLITE_FillRoundedRect);
  SET_HW_FUNC(GUI_SetFuncFillCircle,         _VGLITE_FillCircle);
  //
  // Set bitmap drawing functions
  //
  SET_HW_FUNC   (GUI_SetFuncDrawBitmapEx,    _VGLITE_DrawBitmapEx);
  SET_HW_FUNCS_2(GUI_SetFuncDrawA8,          _VGLITE_DrawBitmapA8Memdev, _VGLITE_DrawBitmapA8);
#if GUI_SUPPORT_MEMDEV
  //
  // Set memory device functions
  //
  SET_HW_FUNC(GUI_MEMDEV_SetRotateFuncLR,    _VGLITE_RotateMemdevLR);
  SET_HW_FUNC(GUI_MEMDEV_SetRotateFuncHR,    _VGLITE_RotateMemdevHR);
#endif
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;

#if (NUM_BUFFERS > 1)
  GUI_MULTIBUF_Config(NUM_BUFFERS);
#endif
  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Configure the initial driver
  //
  _ConfigDriver(pDevice, 0, GUI_ROTATION_0);
  //
  // If screen rotation is not required the following section could be switched off to spare ROM
  //
#if ENABLE_DISPLAY_ROTATION
  //
  // Create additional drivers and add them to the display rotation module
  //
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_CCW, 0, GUI_ROTATION_CCW);
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_180, 0, GUI_ROTATION_180);
  LCD_ROTATE_AddDriverExOrientation(DISPLAY_DRIVER_CW,  0, GUI_ROTATION_CW);
  //
  // Set callback function to be used for layer 0
  //
  LCD_ROTATE_SetCallback(_ConfigDriver, 0);
#endif
  //
  // Cache management
  //
  GUI_DCACHE_SetClearCacheHook(_ClearCache);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  LCD_X_SHOWBUFFER_INFO *pInfo;
  GUI_USE_PARA(LayerIndex);
  r = 0;
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    _InitController();
    PID_X_Init();
    break;
  }
  case LCD_X_ON: {
    _PendingBuffer = -2;
    g_dc.ops->setFrameBuffer(&g_dc, 0, (void *)_aBufferAddress[_BackBuffer]);
    //
    // Wait for frame buffer sent to display controller video memory.
    //
    if ((g_dc.ops->getProperty(&g_dc) & kDC_FB_ReserveFrameBuffer) == 0) {
      while (_PendingBuffer == -2);
    }
    g_dc.ops->enableLayer(&g_dc, 0);
    break;
  }
  case LCD_X_SHOWBUFFER: {
    pInfo          = (LCD_X_SHOWBUFFER_INFO *)pData;
    _PendingBuffer = pInfo->Index;
    //
    // Make the given buffer visible
    //
    g_dc.ops->setFrameBuffer(&g_dc, 0, (void *)_aBufferAddress[pInfo->Index]);
    //
    // Wait for VSYNC ISR to restore '_PendingBuffer'
    //
#if (USE_OS == 1)
    _Semaphore_Take(_hSema_WaitForVSYNC);
#else
    while (_PendingBuffer >= 0);
#endif
    return 0;
  }
  default:
    r = -1;
    break;
  }
  return r;
}

/*************************** End of file ****************************/
