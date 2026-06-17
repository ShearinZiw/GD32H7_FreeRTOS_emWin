/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_00_Slots.c
Purpose     : AppWizard managed file, function content could be changed
---------------------------END-OF-HEADER------------------------------
*/

#include "Application.h"
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_00.h"
#include "GUI_Demo.h"
#include "BUTTON.h"
#include "IMAGE.h"
#include "TEXT.h"
#ifndef WIN32
  #include "FreeRTOS.h"
  #include "task.h"
  #include "portable.h"
  #include "led/bsp_buzzer.h"
  #include "key/bsp_key.h"
  #include "adc/bsp_speed_adc.h"
  #include "usart/bsp_usart.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** Begin of user code area ***/

#ifndef WIN32
extern const void * IP_FS_Open(const char * sPath, U32 * pSize);
extern void         IP_FS_Close(const void * pHandle);
extern int          IP_FS_Read(void * pBuffer, int ElementSize, int NumElements, const void * pHandle);
extern int          IP_FS_Seek(const void * pHandle, U32 Off);
#endif

typedef struct {
  int Id;
  int x;
  int y;
  int w;
  int h;
  const char * pText;
} HMI_TEXT_ITEM;

typedef struct {
  const char * pTitle;
  const char * pNav;
  const char * pAudio;
  const char * pVideo;
  const char * pVehicle;
  const char * pPhone;
  const char * pSettings;
  const char * pOff;
} HMI_LANGUAGE;

typedef enum {
  HMI_PAGE_NAVIGATION = 0,
  HMI_PAGE_AUDIO,
  HMI_PAGE_VIDEO,
  HMI_PAGE_VEHICLE,
  HMI_PAGE_PHONE,
  HMI_PAGE_SETTINGS,
  HMI_PAGE_OFF
} HMI_PAGE;

typedef struct {
  HMI_PAGE Page;
  int x0;
  int x1;
  const char * pName;
} HMI_NAV_ITEM;

typedef struct {
  const char * pMain;
  const char * pSub;
  const char * pSystem;
  const char * pStatus;
  const char * pTemp;
  const char * pWeather;
  const char * pRange;
} HMI_PAGE_TEXT;

static const HMI_TEXT_ITEM _aTextItem[] = {
  { ID_TEXT_00,        206, 126, 390, 76, "10:32" },
  { ID_TEXT_00_Copy,   236,  90, 330, 30, "Wednesday, May 27" },
  { ID_TEXT_00_Copy1,   18,  20, 190, 26, "GUX vehicle terminal" },
  { ID_TEXT_00_Copy2,  200, 310, 160, 24, "All Systems" },
  { ID_TEXT_00_Copy3,  198, 340, 170, 38, "READY" },
  { ID_TEXT_00_Copy4,  574, 306, 150, 52, "75 F" },
  { ID_TEXT_00_Copy5,  420, 372, 170, 24, "Partly Cloudy" },
  { ID_TEXT_00_Copy6,  590, 372, 160, 24, "Hi 82   Low 70" },
  { ID_TEXT_00_Copy8,   20, 442, 100, 24, "Navigation" },
  { ID_TEXT_00_Copy9,  130, 442, 100, 24, "Audio" },
  { ID_TEXT_00_Copy10, 350, 442, 100, 24, "Vehicle" },
  { ID_TEXT_00_Copy11, 460, 442, 100, 24, "Phone" },
  { ID_TEXT_00_Copy12, 570, 442, 100, 24, "Settings" },
  { ID_TEXT_00_Copy13, 680, 442, 100, 24, "Off" },
  { ID_TEXT_00_Copy14, 548,  76, 230, 22, "" },
  { ID_TEXT_00_Copy15, 548, 100, 230, 22, "" },
  { ID_TEXT_00_Copy19, 548, 124, 230, 22, "" },
};

static const HMI_LANGUAGE _aLanguage[] = {
  { "GUX vehicle terminal", "Navigation", "Audio", "Video",  "Vehicle",  "Phone",   "Settings", "Calendar" },
  { "GUX Fahrzeugterminal", "Navigation", "Audio", "Medien", "Fahrzeug", "Telefon", "Setup",    "Kalender" },
  { "GUX vehicle terminal",
    "\xE5\xAF\xBC\xE8\x88\xAA",
    "\xE9\x9F\xB3\xE9\xA2\x91",
    "\xE8\xA7\x86\xE9\xA2\x91",
    "\xE8\xBD\xA6\xE8\xBE\x86",
    "\xE7\x94\xB5\xE8\xAF\x9D",
    "\xE8\xAE\xBE\xE7\xBD\xAE",
    "\xE6\x97\xA5\xE5\x8E\x86" }
};

static const HMI_NAV_ITEM _aNavItem[] = {
  { HMI_PAGE_NAVIGATION,  18, 122, "NAV" },
  { HMI_PAGE_AUDIO,      128, 232, "AUDIO" },
  { HMI_PAGE_VIDEO,      238, 342, "VIDEO" },
  { HMI_PAGE_VEHICLE,    348, 452, "VEHICLE" },
  { HMI_PAGE_PHONE,      458, 562, "PHONE" },
  { HMI_PAGE_SETTINGS,   568, 672, "SETTINGS" },
  { HMI_PAGE_OFF,        678, 782, "CAL" }
};

static const HMI_PAGE_TEXT _aPageText[] = {
  { "NAV",     "Route guidance",  "Destination", "HOME",     "45 km",   "Next: Main St", "ETA 13:08" },
  { "AUDIO",   "Buzzer Music",    "Now Playing", "DI-DI-DI",  "Vol 62%", "Tap center",    "KEY1 - KEY2 +" },
  { "VIDEO",   "SD Media Center", "Media",       "JPEG",     "GIF/BMP", "Movie ready",   "Use MEDIA=..." },
  { "10:32",   "Wednesday, May 27","All Systems", "READY",    "75 F",    "Partly Cloudy", "000 km/h" },
  { "",        "Enter number",    "",            "NO CALL",  "",        "",              "" },
  { "",        "",                "",            "EN",       "",        "",              "" },
  { "",        "",                "",            "",         "",        "",              "" }
};

static int _ScreenReady;
static char _acRxLine[80];
static unsigned _RxLen;

static char _acTime[16];
static char _acDate[40];
static char _acSystem[32];
static char _acStatus[24];
static char _acTemp[24];
static char _acWeather[40];
static char _acRange[40];
static char _acPerf0[48];
static char _acPerf1[48];
static char _acPerf2[48];
static char _acPhoneNumber[24];
static WM_HWIN _hScreen;
static int _PerfEnabled;
static unsigned _PaintCount;
static unsigned _PerfLoopCount;
static unsigned _LastPerfLoopCount;
static GUI_TIMER_TIME _LastPerfTime;
static HMI_PAGE _ActivePage = HMI_PAGE_VEHICLE;
static unsigned _ActiveLang;
static unsigned _VolumePercent = 62U;
static unsigned _SpeedKmh;
static unsigned _SpeedAdcRaw;
#ifndef WIN32
static GUI_FONT _FontZh24;
static GUI_XBF_DATA _FontZh24Data;
static const void * _hZhFontFile;
static U32 _ZhFontSize;
static int _ZhFontReady;
#endif
#ifndef WIN32
static int _LastTouchPressed;
static GUI_TIMER_TIME _LastInputTime;
static GUI_TIMER_TIME _LastAdcTime;
#endif

static int _ShowMediaImage(const char * pName, const char * pPath, int Format, int Report);
static int _ShowMediaMovie(int Report);
static void _HideMedia(void);
static void _SelectPage(HMI_PAGE Page, int Report);
static void _RefreshControlTelemetry(void);
static void _RefreshPhonePage(void);
static const char * _GetLangCode(unsigned Lang);
static int _ApplyLanguage(unsigned Lang, int Report);

static void _SetWidgetText(int Id, const char * pText) {
  WM_HWIN hItem;

  APPW_SetText(ID_SCREEN_00, Id, (char *)pText);
  if (_hScreen == 0) {
    return;
  }
  hItem = WM_GetDialogItem(_hScreen, Id);
  if (hItem == 0) {
    return;
  }
  if (Id == ID_BUTTON_00) {
    BUTTON_SetText(hItem, pText);
  } else {
    TEXT_SetText(hItem, pText);
  }
  WM_InvalidateWindow(hItem);
}

static void _CopyText(char * pDst, unsigned Size, const char * pSrc) {
  if (Size == 0) {
    return;
  }
  strncpy(pDst, pSrc, Size - 1);
  pDst[Size - 1] = 0;
}

static const char * _GetPageName(HMI_PAGE Page) {
  unsigned i;

  for (i = 0; i < GUI_COUNTOF(_aNavItem); i++) {
    if (_aNavItem[i].Page == Page) {
      return _aNavItem[i].pName;
    }
  }
  return "UNKNOWN";
}

static const char * _GetNavLabel(HMI_PAGE Page) {
  const HMI_LANGUAGE * pLang;

  pLang = &_aLanguage[_ActiveLang];
  switch (Page) {
  case HMI_PAGE_NAVIGATION:
    return pLang->pNav;
  case HMI_PAGE_AUDIO:
    return pLang->pAudio;
  case HMI_PAGE_VIDEO:
    return pLang->pVideo;
  case HMI_PAGE_VEHICLE:
    return pLang->pVehicle;
  case HMI_PAGE_PHONE:
    return pLang->pPhone;
  case HMI_PAGE_SETTINGS:
    return pLang->pSettings;
  case HMI_PAGE_OFF:
    return pLang->pOff;
  default:
    return "";
  }
}

static const char * _GetLangCode(unsigned Lang) {
  switch (Lang) {
  case 1:
    return "DE";
  case 2:
    return "CN";
  default:
    return "EN";
  }
}

#ifndef WIN32
static int _cbGetZhFontData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) {
  const void * hFile;

  hFile = (const void *)pVoid;
  if ((hFile == NULL) || (pBuffer == NULL)) {
    return 1;
  }
  if (IP_FS_Seek(hFile, Off) != 0) {
    return 1;
  }
  return (IP_FS_Read(pBuffer, 1, NumBytes, hFile) == NumBytes) ? 0 : 1;
}

static int _EnsureChineseFont(int Report) {
  int Result;

  if (_ZhFontReady) {
    return 1;
  }
  if (_hZhFontFile == NULL) {
    _hZhFontFile = IP_FS_Open("0:/font.xbf", &_ZhFontSize);
    if (_hZhFontFile == NULL) {
      if (Report) {
        printf("[VehicleUI] LANG=CN failed: missing 0:/font.xbf\r\n");
      }
      return 0;
    }
  }
  _FontZh24Data.pVoid = (void *)_hZhFontFile;
  _FontZh24Data.pfGetData = _cbGetZhFontData;
  Result = GUI_XBF_CreateFont(&_FontZh24, &_FontZh24Data,
                              GUI_XBF_TYPE_PROP_AA4_EXT,
                              _cbGetZhFontData,
                              (void *)_hZhFontFile);
  if (Result != 0) {
    IP_FS_Close(_hZhFontFile);
    _hZhFontFile = NULL;
    _ZhFontSize = 0;
    if (Report) {
      printf("[VehicleUI] LANG=CN failed: invalid XBF font (err=%d)\r\n", Result);
    }
    return 0;
  }
  _ZhFontReady = 1;
  if (Report) {
    printf("[VehicleUI] Loaded Chinese font: 0:/font.xbf (%lu bytes)\r\n", (unsigned long)_ZhFontSize);
  }
  return 1;
}
#else
static int _EnsureChineseFont(int Report) {
  GUI_USE_PARA(Report);
  return 0;
}
#endif

static int _GetBottomPageAt(int x, int y, HMI_PAGE * pPage) {
  unsigned i;

  if ((y < 418) || (y > 472)) {
    return 0;
  }
  for (i = 0; i < GUI_COUNTOF(_aNavItem); i++) {
    if ((x >= _aNavItem[i].x0) && (x <= _aNavItem[i].x1)) {
      *pPage = _aNavItem[i].Page;
      return 1;
    }
  }
  return 0;
}

static void _SetWidgetVisible(int Id, int Visible) {
  WM_HWIN hItem;

  if (_hScreen == 0) {
    return;
  }
  hItem = WM_GetDialogItem(_hScreen, Id);
  if (hItem == 0) {
    return;
  }
  if (Visible) {
    WM_ShowWindow(hItem);
    WM_BringToTop(hItem);
  } else {
    WM_HideWindow(hItem);
  }
}

static void _SetPerfVisible(int Visible) {
  _SetWidgetVisible(ID_TEXT_00_Copy14, Visible);
  _SetWidgetVisible(ID_TEXT_00_Copy15, Visible);
  _SetWidgetVisible(ID_TEXT_00_Copy19, Visible);
  if (_hScreen) {
    WM_InvalidateWindow(_hScreen);
  }
}

static void _UpdatePerfOverlay(int Force) {
  GUI_TIMER_TIME Now;
  GUI_TIMER_TIME Delta;
  unsigned Fps;
  unsigned LoopDelta;
  unsigned GuiUsedKB;
  unsigned GuiFreeKB;
  unsigned GuiPeakKB;
  unsigned RtosHeapKB;
  unsigned TaskCount;

  if ((_PerfEnabled == 0) || (_hScreen == 0)) {
    return;
  }
  Now = GUI_GetTime();
  Delta = Now - _LastPerfTime;
  if ((Force == 0) && (Delta < 1000)) {
    return;
  }
  if (Delta == 0) {
    Delta = 1;
  }
  LoopDelta = _PerfLoopCount - _LastPerfLoopCount;
  Fps = (LoopDelta * 1000U) / (unsigned)Delta;
  GuiUsedKB = (unsigned)GUI_ALLOC_GetNumUsedBytes() / 1024U;
  GuiFreeKB = (unsigned)GUI_ALLOC_GetNumFreeBytes() / 1024U;
  GuiPeakKB = (unsigned)GUI_ALLOC_GetMaxUsedBytes() / 1024U;
#ifndef WIN32
  RtosHeapKB = (unsigned)xPortGetFreeHeapSize() / 1024U;
  TaskCount = (unsigned)uxTaskGetNumberOfTasks();
#else
  RtosHeapKB = 0;
  TaskCount = 0;
#endif
  snprintf(_acPerf0, sizeof(_acPerf0), "FPS %u  Paint %u", Fps, _PaintCount);
  snprintf(_acPerf1, sizeof(_acPerf1), "GUI %uK/%uK peak %uK", GuiUsedKB, GuiFreeKB, GuiPeakKB);
  snprintf(_acPerf2, sizeof(_acPerf2), "Heap %uK  Tasks %u", RtosHeapKB, TaskCount);
  _SetWidgetText(ID_TEXT_00_Copy14, _acPerf0);
  _SetWidgetText(ID_TEXT_00_Copy15, _acPerf1);
  _SetWidgetText(ID_TEXT_00_Copy19, _acPerf2);
  _SetPerfVisible(1);
  _LastPerfTime = Now;
  _LastPerfLoopCount = _PerfLoopCount;
}

static void _SetPerfEnabled(int Enabled, int Report) {
  _PerfEnabled = Enabled ? 1 : 0;
  if (_PerfEnabled) {
    _LastPerfTime = GUI_GetTime();
    _LastPerfLoopCount = _PerfLoopCount;
    _UpdatePerfOverlay(1);
    if (Report) {
      printf("[VehicleUI] Applied PERF=ON\r\n");
    }
  } else {
    _SetPerfVisible(0);
    if (Report) {
      printf("[VehicleUI] Applied PERF=OFF\r\n");
    }
  }
}

static int _ApplyLanguage(unsigned Lang, int Report) {
  const HMI_LANGUAGE * pLang;

  if (Lang >= GUI_COUNTOF(_aLanguage)) {
    return 0;
  }
  if ((Lang == 2U) && (_EnsureChineseFont(Report) == 0)) {
    _SetWidgetText(ID_TEXT_00_Copy3, "CN FONT ERR");
    return 0;
  }
  _ActiveLang = Lang;
  pLang = &_aLanguage[Lang];
  _SetWidgetText(ID_TEXT_00_Copy1,  pLang->pTitle);
  _SetWidgetText(ID_TEXT_00_Copy8,  pLang->pNav);
  _SetWidgetText(ID_TEXT_00_Copy9,  pLang->pAudio);
  _SetWidgetText(ID_BUTTON_00,      pLang->pVideo);
  _SetWidgetText(ID_TEXT_00_Copy10, pLang->pVehicle);
  _SetWidgetText(ID_TEXT_00_Copy11, pLang->pPhone);
  _SetWidgetText(ID_TEXT_00_Copy12, pLang->pSettings);
  _SetWidgetText(ID_TEXT_00_Copy13, pLang->pOff);
  if (_ActivePage == HMI_PAGE_SETTINGS) {
    _SetWidgetText(ID_TEXT_00_Copy3, _GetLangCode(Lang));
  }
  if (_hScreen) {
    WM_InvalidateWindow(_hScreen);
  }
  if (Report) {
    printf("[VehicleUI] Applied LANG=%s\r\n", _GetLangCode(Lang));
  }
  return 1;
}

static void _SetWidgetPos(WM_HWIN hScreen, int Id, int x, int y, int w, int h) {
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(hScreen, Id);
  if (hItem) {
    WM_SetWindowPos(hItem, x, y, w, h);
    WM_BringToTop(hItem);
  }
}

static void _ConfigureText(WM_HWIN hScreen, int Id) {
  WM_HWIN hItem;

  hItem = WM_GetDialogItem(hScreen, Id);
  if (hItem == 0) {
    return;
  }

  TEXT_SetBkColor(hItem, GUI_INVALID_COLOR);
  if (Id == ID_TEXT_00) {
    TEXT_SetFont(hItem, GUI_FONT_64_ASCII_AA4);
    TEXT_SetTextColor(hItem, 0xfff0f6ff);
  } else if (Id == ID_TEXT_00_Copy) {
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII_AA4);
    TEXT_SetTextColor(hItem, 0xffbfc8d4);
  } else if ((Id == ID_TEXT_00_Copy3) ||
             (Id == ID_TEXT_00_Copy4)) {
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII_AA4);
    TEXT_SetTextColor(hItem, 0xfff6fbff);
  } else if ((Id == ID_TEXT_00_Copy14) ||
             (Id == ID_TEXT_00_Copy15) ||
             (Id == ID_TEXT_00_Copy19)) {
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextColor(hItem, 0xffffffff);
    TEXT_SetBkColor(hItem, 0xdd101820);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
  } else if ((Id == ID_TEXT_00_Copy2) ||
             (Id == ID_TEXT_00_Copy5) ||
             (Id == ID_TEXT_00_Copy6)) {
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextColor(hItem, 0xffcfd8e4);
  } else if (Id == ID_TEXT_00_Copy19) {
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextColor(hItem, 0xffffffff);
  } else {
    TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
    TEXT_SetTextColor(hItem, 0xffedf3fa);
  }
}

static void _DrawRoadScene(void) {
  GUI_POINT road[] = {
    { 330, 214 }, { 470, 214 }, { 650, 479 }, { 150, 479 }
  };
  GUI_POINT leftField[] = {
    { 0, 232 }, { 330, 214 }, { 150, 479 }, { 0, 479 }
  };
  GUI_POINT rightField[] = {
    { 470, 214 }, { 799, 232 }, { 799, 479 }, { 650, 479 }
  };
  GUI_POINT leftShoulder[] = {
    { 314, 224 }, { 330, 214 }, { 150, 479 }, { 118, 479 }
  };
  GUI_POINT rightShoulder[] = {
    { 470, 214 }, { 486, 224 }, { 682, 479 }, { 650, 479 }
  };

  GUI_DrawGradientV(0, 0, 799, 238, 0xff0f334b, 0xff7fa1b0);
  GUI_SetColor(0xff2c5e3c);
  GUI_FillPolygon(leftField, GUI_COUNTOF(leftField), 0, 0);
  GUI_SetColor(0xff3d6b3a);
  GUI_FillPolygon(rightField, GUI_COUNTOF(rightField), 0, 0);
  GUI_SetColor(0xff2a2d31);
  GUI_FillPolygon(road, GUI_COUNTOF(road), 0, 0);
  GUI_SetColor(0xffd8c66e);
  GUI_FillPolygon(leftShoulder, GUI_COUNTOF(leftShoulder), 0, 0);
  GUI_FillPolygon(rightShoulder, GUI_COUNTOF(rightShoulder), 0, 0);

  GUI_SetColor(0xffc7ced5);
  GUI_FillRect(395, 232, 405, 274);
  GUI_FillRect(393, 300, 407, 360);
  GUI_FillRect(390, 390, 410, 468);

}

static void _DrawCarStatusIcon(void) {
  GUI_SetColor(0xffeef4fb);
  GUI_SetPenSize(4);
  GUI_DrawLine(42, 314, 62, 314);
  GUI_DrawLine(62, 314, 86, 278);
  GUI_DrawLine(86, 278, 154, 278);
  GUI_DrawLine(154, 278, 180, 314);
  GUI_DrawLine(180, 314, 206, 314);
  GUI_DrawLine(42, 314, 42, 360);
  GUI_DrawLine(206, 314, 206, 360);
  GUI_DrawLine(42, 360, 206, 360);
  GUI_DrawLine(78, 294, 164, 294);
  GUI_DrawCircle(78, 374, 18);
  GUI_DrawCircle(170, 374, 18);
  GUI_SetPenSize(1);
}

static void _DrawWeatherIcon(void) {
  GUI_SetColor(0xffe8f0f8);
  GUI_SetPenSize(3);
  GUI_DrawCircle(488, 324, 24);
  GUI_DrawLine(488, 284, 488, 296);
  GUI_DrawLine(448, 324, 460, 324);
  GUI_DrawLine(516, 296, 526, 286);
  GUI_DrawLine(460, 296, 450, 286);
  GUI_DrawArc(522, 340, 28, 20, 0, 180);
  GUI_DrawCircle(500, 340, 18);
  GUI_DrawCircle(542, 340, 18);
  GUI_DrawLine(498, 358, 562, 358);
  GUI_SetPenSize(1);
}

static void _DrawNavItem(int x0, int x1, int Selected) {
  GUI_COLOR Color0;
  GUI_COLOR Color1;

  if (Selected) {
    Color0 = 0xff00b4e8;
    Color1 = 0xff008ec2;
  } else {
    Color0 = 0xff8592a2;
    Color1 = 0xff606c78;
  }
  GUI_DrawGradientRoundedV(x0, 418, x1, 472, 5, Color0, Color1);
  GUI_SetColor(0xff2c333a);
  GUI_DrawRoundedRect(x0, 418, x1, 472, 5);
}

static void _DrawBottomBar(void) {
  GUI_RECT Rect;
  unsigned i;

  GUI_SetColor(0xdd101820);
  GUI_FillRect(0, 404, 799, 479);
  GUI_SetTextMode(GUI_TM_TRANS);
#ifndef WIN32
  if ((_ActiveLang == 2U) && _ZhFontReady) {
    GUI_SetFont(&_FontZh24);
  } else
#endif
  GUI_SetFont(GUI_FONT_16B_ASCII);
  for (i = 0; i < GUI_COUNTOF(_aNavItem); i++) {
    _DrawNavItem(_aNavItem[i].x0, _aNavItem[i].x1, _aNavItem[i].Page == _ActivePage);
    Rect.x0 = _aNavItem[i].x0 + 2;
    Rect.y0 = 438;
    Rect.x1 = _aNavItem[i].x1 - 2;
    Rect.y1 = 466;
    GUI_SetColor(0xfff4f8fc);
    GUI_DispStringInRect((char *)_GetNavLabel(_aNavItem[i].Page), &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }
}

static void _DrawPhoneKeypad(void) {
  static const char * aLabels[12] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "CLR", "0", "DEL" };
  GUI_RECT Rect;
  int Row;
  int Col;
  int Index;
  int x;
  int y;

  GUI_SetFont(GUI_FONT_20B_ASCII);
  GUI_SetTextMode(GUI_TM_TRANS);
  for (Row = 0; Row < 4; Row++) {
    for (Col = 0; Col < 3; Col++) {
      Index = Row * 3 + Col;
      x = 286 + Col * 76;
      y = 140 + Row * 48;
      GUI_DrawGradientRoundedV(x, y, x + 64, y + 38, 6, 0xffeef3f7, 0xffaab7c2);
      GUI_SetColor(0xff2b333d);
      GUI_DrawRoundedRect(x, y, x + 64, y + 38, 6);
      Rect.x0 = x;
      Rect.y0 = y;
      Rect.x1 = x + 64;
      Rect.y1 = y + 38;
      GUI_SetColor(0xff16202a);
      GUI_DispStringInRect((char *)aLabels[Index], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
  }
}

static void _DrawSettingsLanguage(void) {
  GUI_RECT Rect;
  const char * aLang[] = { "EN", "DE", "CN" };
  int i;
  int x;

  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetFont(GUI_FONT_20B_ASCII);
  for (i = 0; i < 3; i++) {
    x = 290 + i * 78;
    if ((unsigned)i == _ActiveLang) {
      GUI_DrawGradientRoundedV(x, 148, x + 64, 196, 8, 0xff00b4e8, 0xff008ec2);
    } else {
      GUI_DrawGradientRoundedV(x, 148, x + 64, 196, 8, 0xffeef3f7, 0xffaab7c2);
    }
    GUI_SetColor(0xff26313b);
    GUI_DrawRoundedRect(x, 148, x + 64, 196, 8);
    Rect.x0 = x;
    Rect.y0 = 148;
    Rect.x1 = x + 64;
    Rect.y1 = 196;
    GUI_SetColor((unsigned)i == _ActiveLang ? 0xffffffff : 0xff16202a);
    GUI_DispStringInRect((char *)aLang[i], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }
  GUI_SetFont(GUI_FONT_16_ASCII);
  GUI_SetColor(0xffcfd8e4);
  GUI_DispStringAt("CN font: N/A", 350, 226);
}

static void _DrawCalendarMonth(void) {
  static const char * aWeek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  GUI_RECT Rect;
  char acDay[4];
  int i;
  int Day;
  int Row;
  int Col;
  int x;
  int y;

  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetFont(GUI_FONT_16B_ASCII);
  GUI_SetColor(0xfff4f8fc);
  GUI_DispStringAt("June 2026", 348, 106);
  for (i = 0; i < 7; i++) {
    Rect.x0 = 230 + i * 50;
    Rect.y0 = 132;
    Rect.x1 = Rect.x0 + 46;
    Rect.y1 = 154;
    GUI_SetColor(0xff9fc7d8);
    GUI_DispStringInRect((char *)aWeek[i], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }
  for (Day = 1; Day <= 30; Day++) {
    Row = (Day) / 7;
    Col = (Day) % 7;
    x = 230 + Col * 50;
    y = 162 + Row * 38;
    if (Day == 17) {
      GUI_DrawGradientRoundedV(x + 4, y, x + 42, y + 30, 5, 0xff00b4e8, 0xff008ec2);
      GUI_SetColor(0xffffffff);
    } else {
      GUI_SetColor(0x3316202a);
      GUI_FillRoundedRect(x + 4, y, x + 42, y + 30, 5);
      GUI_SetColor(0xffedf3fa);
    }
    Rect.x0 = x + 4;
    Rect.y0 = y;
    Rect.x1 = x + 42;
    Rect.y1 = y + 30;
    snprintf(acDay, sizeof(acDay), "%d", Day);
    GUI_DispStringInRect(acDay, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }
}

static void _DrawFeaturePanel(void) {
  GUI_POINT play[] = { { 374, 178 }, { 374, 258 }, { 454, 218 } };

  GUI_SetColor(0xaa101820);
  GUI_FillRoundedRect(184, 78, 616, 390, 8);
  GUI_SetColor(0x55364755);
  GUI_DrawRoundedRect(184, 78, 616, 390, 8);
  GUI_SetColor(0xffe8f0f8);
  GUI_SetPenSize(4);

  switch (_ActivePage) {
  case HMI_PAGE_NAVIGATION:
    GUI_DrawLine(296, 310, 378, 230);
    GUI_DrawLine(378, 230, 470, 266);
    GUI_DrawLine(470, 266, 548, 162);
    GUI_FillCircle(296, 310, 9);
    GUI_FillCircle(548, 162, 12);
    GUI_SetPenSize(2);
    GUI_DrawLine(532, 166, 548, 146);
    GUI_DrawLine(548, 146, 564, 166);
    break;
  case HMI_PAGE_AUDIO:
    GUI_DrawCircle(400, 226, 46);
    GUI_DrawCircle(400, 226, 18);
    GUI_DrawLine(456, 180, 456, 292);
    GUI_DrawLine(456, 180, 522, 164);
    GUI_DrawLine(522, 164, 522, 270);
    GUI_FillCircle(442, 292, 18);
    GUI_FillCircle(508, 270, 18);
    GUI_SetFont(GUI_FONT_16B_ASCII);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetColor(0xffcfd8e4);
    GUI_DispStringAt("Tap panel to play", 340, 324);
    break;
  case HMI_PAGE_VIDEO:
    GUI_DrawRoundedRect(280, 132, 520, 302, 10);
    GUI_FillPolygon(play, GUI_COUNTOF(play), 0, 0);
    break;
  case HMI_PAGE_PHONE:
    _DrawPhoneKeypad();
    break;
  case HMI_PAGE_SETTINGS:
    _DrawSettingsLanguage();
    break;
  case HMI_PAGE_OFF:
    _DrawCalendarMonth();
    break;
  case HMI_PAGE_VEHICLE:
  default:
    break;
  }
  GUI_SetPenSize(1);
}

static void _DrawTopIcons(void) {
  GUI_SetColor(0xffd8e4ee);
  GUI_SetPenSize(3);
  GUI_DrawArc(558, 39, 18, 18, 310, 50);
  GUI_DrawArc(558, 39, 11, 11, 310, 50);
  GUI_DrawLine(626, 30, 658, 30);
  GUI_DrawLine(626, 30, 626, 54);
  GUI_DrawLine(626, 54, 658, 54);
  GUI_DrawLine(658, 30, 658, 54);
  GUI_DrawLine(658, 36, 676, 28);
  GUI_DrawLine(658, 48, 676, 56);
  GUI_DrawLine(722, 44, 744, 24);
  GUI_DrawLine(744, 24, 766, 44);
  GUI_DrawLine(728, 44, 728, 58);
  GUI_DrawLine(760, 44, 760, 58);
  GUI_DrawLine(728, 58, 760, 58);
  GUI_SetPenSize(1);
}

static void _PaintDashboard(void) {
  GUI_SetBkColor(0xff101418);
  GUI_Clear();
  if (_ActivePage == HMI_PAGE_OFF) {
    GUI_DrawGradientV(0, 0, 799, 479, 0xff05080c, 0xff14191f);
    _DrawFeaturePanel();
  } else {
    _DrawRoadScene();
    if (_ActivePage == HMI_PAGE_VEHICLE) {
      _DrawCarStatusIcon();
      _DrawWeatherIcon();
    } else {
      _DrawFeaturePanel();
    }
    _DrawTopIcons();
  }
  _DrawBottomBar();
}

static void _InitDashboard(WM_HWIN hScreen) {
  unsigned i;
  WM_HWIN hImage;
  WM_HWIN hBox;

  if (_ScreenReady) {
    return;
  }
  _ScreenReady = 1;
  _hScreen = hScreen;
  GUI_UC_SetEncodeUTF8();

  hBox = WM_GetDialogItem(hScreen, ID_BOX_00);
  if (hBox) {
    WM_HideWindow(hBox);
  }

  hImage = WM_GetDialogItem(hScreen, ID_IMAGE_00);
  if (hImage) {
    WM_SetWindowPos(hImage, 220, 96, 360, 260);
    WM_HideWindow(hImage);
  }

  for (i = 0; i < GUI_COUNTOF(_aTextItem); i++) {
    _SetWidgetPos(hScreen, _aTextItem[i].Id, _aTextItem[i].x, _aTextItem[i].y,
                  _aTextItem[i].w, _aTextItem[i].h);
    _ConfigureText(hScreen, _aTextItem[i].Id);
    _SetWidgetText(_aTextItem[i].Id, _aTextItem[i].pText);
  }

  _SetWidgetPos(hScreen, ID_BUTTON_00, 238, 418, 106, 56);
  _SetWidgetText(ID_BUTTON_00, "Video");
  _SetWidgetVisible(ID_TEXT_00_Copy8, 0);
  _SetWidgetVisible(ID_TEXT_00_Copy9, 0);
  _SetWidgetVisible(ID_BUTTON_00, 0);
  _SetWidgetVisible(ID_TEXT_00_Copy10, 0);
  _SetWidgetVisible(ID_TEXT_00_Copy11, 0);
  _SetWidgetVisible(ID_TEXT_00_Copy12, 0);
  _SetWidgetVisible(ID_TEXT_00_Copy13, 0);

  _CopyText(_acTime,    sizeof(_acTime),    "10:32");
  _CopyText(_acDate,    sizeof(_acDate),    "Wednesday, May 27");
  _CopyText(_acSystem,  sizeof(_acSystem),  "All Systems");
  _CopyText(_acStatus,  sizeof(_acStatus),  "READY");
  _CopyText(_acTemp,    sizeof(_acTemp),    "75 F");
  _CopyText(_acWeather, sizeof(_acWeather), "Partly Cloudy");
  _CopyText(_acRange,   sizeof(_acRange),   "000 km/h");

  _SetPerfVisible(0);
  _RefreshControlTelemetry();
  WM_InvalidateWindow(hScreen);
}

static void _OnButtonClick(WM_HWIN hScreen) {
  GUI_USE_PARA(hScreen);
  _SelectPage(HMI_PAGE_VIDEO, 1);
}

static void _RefreshVehicleScreen(void) {
  if (_hScreen) {
    WM_InvalidateWindow(_hScreen);
  }
}

static void _SetTextFromCommand(int Id, char * pStore, unsigned StoreSize, const char * pValue) {
  _CopyText(pStore, StoreSize, pValue);
  _SetWidgetText(Id, pStore);
  _RefreshVehicleScreen();
}

static void _SetTemperature(const char * pValue) {
  if ((strchr(pValue, 'F') == NULL) && (strchr(pValue, 'C') == NULL)) {
    snprintf(_acTemp, sizeof(_acTemp), "%s F", pValue);
  } else {
    _CopyText(_acTemp, sizeof(_acTemp), pValue);
  }
  _SetWidgetText(ID_TEXT_00_Copy4, _acTemp);
  _RefreshVehicleScreen();
}

static void _RefreshControlTelemetry(void) {
  if (_ActivePage == HMI_PAGE_VEHICLE) {
    snprintf(_acRange, sizeof(_acRange), "%03u km/h", _SpeedKmh);
    _SetWidgetText(ID_TEXT_00_Copy6, _acRange);
  } else if (_ActivePage == HMI_PAGE_AUDIO) {
    snprintf(_acTemp, sizeof(_acTemp), "Vol %u%%", _VolumePercent);
    snprintf(_acRange, sizeof(_acRange), "KEY1 -  KEY2 +");
    _SetWidgetText(ID_TEXT_00_Copy4, _acTemp);
    _SetWidgetText(ID_TEXT_00_Copy6, _acRange);
  } else if (_ActivePage == HMI_PAGE_SETTINGS) {
    snprintf(_acTemp, sizeof(_acTemp), "Vol %u%%", _VolumePercent);
    snprintf(_acWeather, sizeof(_acWeather), "Speed %u km/h", _SpeedKmh);
    snprintf(_acRange, sizeof(_acRange), "%03u km/h", _SpeedKmh);
    _SetWidgetText(ID_TEXT_00_Copy4, _acTemp);
    _SetWidgetText(ID_TEXT_00_Copy5, _acWeather);
    _SetWidgetText(ID_TEXT_00_Copy6, _acRange);
  }
}

static void _SetVolumePercent(unsigned Value, int Report) {
  if (Value > 100U) {
    Value = 100U;
  }
  _VolumePercent = Value;
  _RefreshControlTelemetry();
#ifndef WIN32
  Buzzer_Play(BUZZER_SOUND_CLICK);
#endif
  if (Report) {
    printf("[VehicleUI] Applied VOLUME=%u\r\n", _VolumePercent);
  }
}

static void _SetSpeedSample(unsigned Raw, unsigned Speed, int Report) {
  _SpeedAdcRaw = Raw & 0x0FFFU;
  _SpeedKmh = Speed;
  _RefreshControlTelemetry();
  if (Report) {
    printf("[VehicleUI] ADC speed raw=0x%03X, speed=%u km/h\r\n", _SpeedAdcRaw, _SpeedKmh);
  }
}

static void _RefreshPhonePage(void) {
  if (_ActivePage != HMI_PAGE_PHONE) {
    return;
  }
  if (_acPhoneNumber[0] == 0) {
    _SetWidgetText(ID_TEXT_00_Copy, "Enter number");
    _SetWidgetText(ID_TEXT_00_Copy3, "NO CALL");
  } else {
    _SetWidgetText(ID_TEXT_00_Copy, _acPhoneNumber);
    _SetWidgetText(ID_TEXT_00_Copy3, "DIAL READY");
  }
}

static int _GetPhoneKeyAt(int x, int y, char * pKey) {
  static const char Keys[12] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#' };
  int Col;
  int Row;
  int x0;
  int y0;

  if ((x < 286) || (x > 502) || (y < 140) || (y > 326)) {
    return 0;
  }
  Col = (x - 286) / 76;
  Row = (y - 140) / 48;
  x0 = 286 + Col * 76;
  y0 = 140 + Row * 48;
  if ((Col < 0) || (Col > 2) || (Row < 0) || (Row > 3)) {
    return 0;
  }
  if ((x > (x0 + 64)) || (y > (y0 + 38))) {
    return 0;
  }
  *pKey = Keys[Row * 3 + Col];
  return 1;
}

static void _InputPhoneKey(char Key, int Report) {
  unsigned Len;

  Len = (unsigned)strlen(_acPhoneNumber);
  if (Key == '*') {
    _acPhoneNumber[0] = 0;
  } else if (Key == '#') {
    if (Len > 0U) {
      _acPhoneNumber[Len - 1U] = 0;
    }
  } else if (Len < (sizeof(_acPhoneNumber) - 1U)) {
    _acPhoneNumber[Len] = Key;
    _acPhoneNumber[Len + 1U] = 0;
  }
#ifndef WIN32
  Buzzer_Play(BUZZER_SOUND_CLICK);
#endif
  _RefreshPhonePage();
  _RefreshVehicleScreen();
  if (Report) {
    printf("[VehicleUI] PHONE=%s\r\n", _acPhoneNumber[0] ? _acPhoneNumber : "(empty)");
  }
}

static void _PlayAudioTune(int Report) {
#ifndef WIN32
  Buzzer_Play(BUZZER_SOUND_TUNE);
#endif
  _SetWidgetText(ID_TEXT_00_Copy3, "PLAYING");
  _RefreshVehicleScreen();
  if (Report) {
    printf("[VehicleUI] Applied AUDIO=TUNE\r\n");
  }
}

static void _PlayPageSound(HMI_PAGE Page) {
#ifndef WIN32
  if (Page == HMI_PAGE_OFF) {
    Buzzer_Play(BUZZER_SOUND_POWER);
  } else if (Page == HMI_PAGE_VIDEO) {
    Buzzer_Play(BUZZER_SOUND_MEDIA);
  } else {
    Buzzer_Play(BUZZER_SOUND_PAGE);
  }
#else
  GUI_USE_PARA(Page);
#endif
}

static void _ApplyPageText(HMI_PAGE Page) {
  const HMI_PAGE_TEXT * pText;

  pText = &_aPageText[(unsigned)Page];
  _SetTextFromCommand(ID_TEXT_00,       _acTime,    sizeof(_acTime),    pText->pMain);
  _SetTextFromCommand(ID_TEXT_00_Copy,  _acDate,    sizeof(_acDate),    pText->pSub);
  _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem,  sizeof(_acSystem),  pText->pSystem);
  _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus,  sizeof(_acStatus),  pText->pStatus);
  _SetTextFromCommand(ID_TEXT_00_Copy4, _acTemp,    sizeof(_acTemp),    pText->pTemp);
  _SetTextFromCommand(ID_TEXT_00_Copy5, _acWeather, sizeof(_acWeather), pText->pWeather);
  _SetTextFromCommand(ID_TEXT_00_Copy6, _acRange,   sizeof(_acRange),   pText->pRange);
}

static void _SelectPage(HMI_PAGE Page, int Report) {
  if ((unsigned)Page >= GUI_COUNTOF(_aPageText)) {
    return;
  }
  _ActivePage = Page;
  if (Page != HMI_PAGE_VIDEO) {
    _HideMedia();
    _ApplyPageText(Page);
  } else {
    _ApplyPageText(Page);
    (void)_ShowMediaMovie(Report);
  }
  _RefreshControlTelemetry();
  _RefreshPhonePage();
  if (Page == HMI_PAGE_SETTINGS) {
    _SetWidgetText(ID_TEXT_00_Copy3, _GetLangCode(_ActiveLang));
  }
  _PlayPageSound(Page);
  if (Report) {
    printf("[VehicleUI] Applied PAGE=%s\r\n", _GetPageName(Page));
  }
  _RefreshVehicleScreen();
}

static int _ShowMediaImage(const char * pName, const char * pPath, int Format, int Report) {
  WM_HWIN hImage;
  int Result;

  if (_hScreen == 0) {
    return 0;
  }
  hImage = WM_GetDialogItem(_hScreen, ID_IMAGE_00);
  Result = (hImage != 0) ? GUI_Demo_ImageFile(pPath, hImage, Format) : 0;
  if (Result == 1) {
    _ActivePage = HMI_PAGE_VIDEO;
    WM_ShowWindow(hImage);
    WM_BringToTop(hImage);
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), pName);
#ifndef WIN32
    Buzzer_Play(BUZZER_SOUND_MEDIA);
#endif
    if (Report) {
      printf("[VehicleUI] Applied MEDIA=%s\r\n", pName);
    }
    return 1;
  }
  _ActivePage = HMI_PAGE_VIDEO;
  _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
  _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus),
                      (Result == GUI_DEMO_IMAGE_UNSUPPORTED) ? "PNG LIB" : "NO FILE");
  if (Report) {
    if (Result == GUI_DEMO_IMAGE_UNSUPPORTED) {
      printf("[VehicleUI] MEDIA=%s unsupported by current GDemWin library\r\n", pName);
    } else {
      printf("[VehicleUI] Media file missing: %s\r\n", pPath);
    }
  }
  return 0;
}

static int _ShowMediaMovie(int Report) {
  WM_HWIN hImage;

  _ActivePage = HMI_PAGE_VIDEO;
  if (_hScreen) {
    hImage = WM_GetDialogItem(_hScreen, ID_IMAGE_00);
    if (hImage) {
      WM_HideWindow(hImage);
    }
  }
  if (GUI_Demo_MOVIE("0:/demo.emf", 220, 96, 360, 260)) {
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), "MOVIE");
#ifndef WIN32
    Buzzer_Play(BUZZER_SOUND_MEDIA);
#endif
    if (Report) {
      printf("[VehicleUI] Applied MEDIA=MOVIE\r\n");
    }
    return 1;
  }
  _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
  _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), "NO FILE");
  if (Report) {
    printf("[VehicleUI] Media file missing: 0:/demo.emf\r\n");
  }
  return 0;
}

static void _HideMedia(void) {
  WM_HWIN hImage;

  GUI_Demo_StopMovie();
  if (_hScreen) {
    hImage = WM_GetDialogItem(_hScreen, ID_IMAGE_00);
    if (hImage) {
      WM_HideWindow(hImage);
    }
    WM_InvalidateWindow(_hScreen);
  }
}

static int _IsImmediateMediaCommand(const char * pLine) {
  return (strstr(pLine, "MEDIA=JPEG")  != NULL) ||
         (strstr(pLine, "MEDIA=PNG")   != NULL) ||
         (strstr(pLine, "MEDIA=GIF")   != NULL) ||
         (strstr(pLine, "MEDIA=BMP")   != NULL) ||
         (strstr(pLine, "MEDIA=MOVIE") != NULL) ||
         (strstr(pLine, "MEDIA=HIDE")  != NULL);
}

static int _FindPage(const char * pValue, HMI_PAGE * pPage) {
  unsigned i;

  if (strstr(pValue, "NAVIGATION") == pValue) {
    *pPage = HMI_PAGE_NAVIGATION;
    return 1;
  }
  if (strstr(pValue, "SET") == pValue) {
    *pPage = HMI_PAGE_SETTINGS;
    return 1;
  }
  if ((strstr(pValue, "CALENDAR") == pValue) || (strstr(pValue, "OFF") == pValue)) {
    *pPage = HMI_PAGE_OFF;
    return 1;
  }
  for (i = 0; i < GUI_COUNTOF(_aNavItem); i++) {
    if (strstr(pValue, _aNavItem[i].pName) == pValue) {
      *pPage = _aNavItem[i].Page;
      return 1;
    }
  }
  return 0;
}

static const char *_FindValue(const char * pLine, const char * pKey) {
  const char * pValue;

  pValue = strstr(pLine, pKey);
  return pValue ? pValue + strlen(pKey) : NULL;
}

static int _ApplySerialCommand(const char * pLine, int Report) {
  const char * pValue;
  HMI_PAGE Page;

  if ((pValue = _FindValue(pLine, "TIME=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00, _acTime, sizeof(_acTime), pValue);
    if (Report) {
      printf("[VehicleUI] Applied TIME=%s\r\n", _acTime);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "DATE=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00_Copy, _acDate, sizeof(_acDate), pValue);
    if (Report) {
      printf("[VehicleUI] Applied DATE=%s\r\n", _acDate);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "SYSTEM=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), pValue);
    if (Report) {
      printf("[VehicleUI] Applied SYSTEM=%s\r\n", _acSystem);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "STATUS=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), pValue);
    if (Report) {
      printf("[VehicleUI] Applied STATUS=%s\r\n", _acStatus);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "TEMP=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTemperature(pValue);
    if (Report) {
      printf("[VehicleUI] Applied TEMP=%s\r\n", _acTemp);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "WEATHER=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00_Copy5, _acWeather, sizeof(_acWeather), pValue);
    if (Report) {
      printf("[VehicleUI] Applied WEATHER=%s\r\n", _acWeather);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "RANGE=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetTextFromCommand(ID_TEXT_00_Copy6, _acRange, sizeof(_acRange), pValue);
    if (Report) {
      printf("[VehicleUI] Applied RANGE=%s\r\n", _acRange);
    }
    return 1;
  } else if ((pValue = _FindValue(pLine, "PERF=")) != NULL) {
    if (strstr(pValue, "ON") == pValue) {
      _SetPerfEnabled(1, Report);
      return 1;
    } else if (strstr(pValue, "OFF") == pValue) {
      _SetPerfEnabled(0, Report);
      return 1;
    }
  } else if ((pValue = _FindValue(pLine, "VOLUME=")) != NULL) {
    if (*pValue == 0) {
      return 0;
    }
    _SetVolumePercent((unsigned)atoi(pValue), Report);
    return 1;
  } else if ((pValue = _FindValue(pLine, "PHONE=")) != NULL) {
    _CopyText(_acPhoneNumber, sizeof(_acPhoneNumber), pValue);
    _RefreshPhonePage();
    if (Report) {
      printf("[VehicleUI] PHONE=%s\r\n", _acPhoneNumber[0] ? _acPhoneNumber : "(empty)");
    }
    return 1;
  } else if (strstr(pLine, "AUDIO=TUNE") != NULL) {
    _PlayAudioTune(Report);
    return 1;
  } else if ((pValue = _FindValue(pLine, "PAGE=")) != NULL) {
    if (_FindPage(pValue, &Page)) {
      _SelectPage(Page, Report);
      return 1;
    }
  } else if ((pValue = _FindValue(pLine, "LANG=")) != NULL) {
    if (strstr(pValue, "DE") == pValue) {
      (void)_ApplyLanguage(1, Report);
      return 1;
    } else if (strstr(pValue, "EN") == pValue) {
      (void)_ApplyLanguage(0, Report);
      return 1;
    } else if (strstr(pValue, "CN") == pValue) {
      (void)_ApplyLanguage(2, Report);
      return 1;
    }
  } else if (strstr(pLine, "MEDIA=JPEG") != NULL) {
    return _ShowMediaImage("JPEG", "0:/demo.jpg", GUI_DEMO_IMAGE_JPEG, Report);
  } else if (strstr(pLine, "MEDIA=PNG") != NULL) {
    return _ShowMediaImage("PNG", "0:/demo.png", GUI_DEMO_IMAGE_PNG, Report);
  } else if (strstr(pLine, "MEDIA=GIF") != NULL) {
    return _ShowMediaImage("GIF", "0:/demo.gif", GUI_DEMO_IMAGE_GIF, Report);
  } else if (strstr(pLine, "MEDIA=BMP") != NULL) {
    return _ShowMediaImage("BMP", "0:/demo.bmp", GUI_DEMO_IMAGE_BMP, Report);
  } else if (strstr(pLine, "MEDIA=MOVIE") != NULL) {
    return _ShowMediaMovie(Report);
  } else if (strstr(pLine, "MEDIA=HIDE") != NULL) {
    _HideMedia();
    _ApplyPageText(_ActivePage);
    if (Report) {
      printf("[VehicleUI] Applied MEDIA=HIDE\r\n");
    }
    return 1;
  }
  return 0;
}

#ifndef WIN32
static int _GetSettingsLangAt(int x, int y, unsigned * pLang) {
  int i;
  int x0;

  if ((y < 148) || (y > 196)) {
    return 0;
  }
  for (i = 0; i < 3; i++) {
    x0 = 290 + i * 78;
    if ((x >= x0) && (x <= (x0 + 64))) {
      *pLang = (unsigned)i;
      return 1;
    }
  }
  return 0;
}

static void _HandleFeatureTouch(int x, int y) {
  char Key;
  unsigned Lang;

  if (_ActivePage == HMI_PAGE_PHONE) {
    if (_GetPhoneKeyAt(x, y, &Key)) {
      _InputPhoneKey(Key, 1);
    }
  } else if (_ActivePage == HMI_PAGE_AUDIO) {
    if ((x >= 184) && (x <= 616) && (y >= 78) && (y <= 390)) {
      _PlayAudioTune(1);
    }
  } else if (_ActivePage == HMI_PAGE_SETTINGS) {
    if (_GetSettingsLangAt(x, y, &Lang)) {
      (void)_ApplyLanguage(Lang, 1);
      _RefreshVehicleScreen();
    }
  }
}

static void _PollBottomTouch(void) {
  GUI_PID_STATE State;
  HMI_PAGE Page;

  State.x = 0;
  State.y = 0;
  State.Pressed = 0;
  State.Layer = 0;
  GUI_PID_GetState(&State);
  if (State.Pressed && (_LastTouchPressed == 0)) {
    if (_GetBottomPageAt(State.x, State.y, &Page)) {
      _SelectPage(Page, 1);
    } else {
      _HandleFeatureTouch(State.x, State.y);
    }
  }
  _LastTouchPressed = State.Pressed ? 1 : 0;
}

static void _PollBoardControls(void) {
  GUI_TIMER_TIME Now;
  uint8_t KeyEvent;
  uint16_t Raw;
  uint16_t Speed;
  static unsigned LastReportedSpeed;

  Now = GUI_GetTime();
  if ((Now - _LastInputTime) >= 50) {
    _LastInputTime = Now;
    KeyEvent = BoardKey_Scan();
    if (KeyEvent & BOARD_KEY1_MASK) {
      if (_VolumePercent >= 5U) {
        _SetVolumePercent(_VolumePercent - 5U, 1);
      } else {
        _SetVolumePercent(0U, 1);
      }
    }
    if (KeyEvent & BOARD_KEY2_MASK) {
      _SetVolumePercent(_VolumePercent + 5U, 1);
    }
  }

  if ((Now - _LastAdcTime) >= 200) {
    _LastAdcTime = Now;
    Raw = SpeedADC_ReadRaw();
    Speed = SpeedADC_RawToKmh(Raw);
    _SetSpeedSample((unsigned)Raw, (unsigned)Speed, 0);
    if ((Speed > (LastReportedSpeed + 4U)) ||
        ((LastReportedSpeed > 4U) && (Speed < (LastReportedSpeed - 4U)))) {
      LastReportedSpeed = Speed;
      printf("[VehicleUI] ADC speed raw=0x%03X, speed=%u km/h\r\n", (unsigned)Raw, (unsigned)Speed);
    }
  }
}
#endif

void VehicleUI_Exec(void) {
#ifndef WIN32
  int Count;
  uint8_t Byte;
  char c;

  _PerfLoopCount++;
  for (Count = 0; Count < 16; Count++) {
    if (Debug_USART_ReadByte(&Byte) == 0) {
      break;
    }
    c = (char)Byte;
    if ((c == '\r') || (c == '\n')) {
      if (_RxLen > 0) {
        _acRxLine[_RxLen] = 0;
        if (_ApplySerialCommand(_acRxLine, 1) == 0) {
          printf("[VehicleUI] Ignored RX line: %s\r\n", _acRxLine);
        }
        _RxLen = 0;
      }
    } else if ((c < 32) || (c > 126)) {
      _RxLen = 0;
    } else if (_RxLen < (sizeof(_acRxLine) - 1)) {
      _acRxLine[_RxLen++] = c;
      _acRxLine[_RxLen] = 0;
      if (_IsImmediateMediaCommand(_acRxLine)) {
        (void)_ApplySerialCommand(_acRxLine, 1);
        _RxLen = 0;
      } else {
        (void)_ApplySerialCommand(_acRxLine, 0);
      }
    } else {
      _RxLen = 0;
    }
  }
  _PollBottomTouch();
  _PollBoardControls();
  Buzzer_Service();
  _UpdatePerfOverlay(0);
#endif
}

/*** End of user code area ***/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       cbID_SCREEN_00
*/
void cbID_SCREEN_00(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    _InitDashboard(pMsg->hWin);
    break;
  case WM_PAINT:
    if (_ScreenReady == 0) {
      _InitDashboard(pMsg->hWin);
    }
    _PaintDashboard();
    _PaintCount++;
    break;
  case WM_NOTIFY_PARENT: {
    int NCode = pMsg->Data.v;
    int Id = WM_GetId(pMsg->hWinSrc);
    if ((Id == ID_BUTTON_00) &&
        ((NCode == WM_NOTIFICATION_CLICKED) || (NCode == WM_NOTIFICATION_RELEASED))) {
      _OnButtonClick(pMsg->hWin);
    }
    break;
  }
  default:
    break;
  }
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_TEXT_00_Copy__APPW_JOB_SETCOLOR
*/
void ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_TEXT_00_Copy__APPW_JOB_SETCOLOR(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_IMAGE_00__APPW_JOB_SETBITMAP
*/
void ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_IMAGE_00__APPW_JOB_SETBITMAP(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*************************** End of file ****************************/
