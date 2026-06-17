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
  #include "usart/bsp_usart.h"
#endif
#include <stdio.h>
#include <string.h>

/*** Begin of user code area ***/

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
  { ID_TEXT_00_Copy14,   0,   0,   1,  1, "" },
  { ID_TEXT_00_Copy15,   0,   0,   1,  1, "" },
  { ID_TEXT_00_Copy19,   0,   0,   1,  1, "" },
};

static const HMI_LANGUAGE _aLanguage[] = {
  { "GUX vehicle terminal", "Navigation", "Audio", "Video", "Vehicle",  "Phone",   "Settings", "Off" },
  { "GUX Fahrzeugterminal", "Navigation", "Audio", "Medien", "Fahrzeug", "Telefon", "Setup",    "Aus" }
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
static WM_HWIN _hScreen;

static int _ShowMediaImage(const char * pName, const char * pPath, int Format, int Report);

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

static void _ApplyLanguage(unsigned Lang) {
  const HMI_LANGUAGE * pLang;

  if (Lang >= GUI_COUNTOF(_aLanguage)) {
    return;
  }
  pLang = &_aLanguage[Lang];
  _SetWidgetText(ID_TEXT_00_Copy1,  pLang->pTitle);
  _SetWidgetText(ID_TEXT_00_Copy8,  pLang->pNav);
  _SetWidgetText(ID_TEXT_00_Copy9,  pLang->pAudio);
  _SetWidgetText(ID_BUTTON_00,      pLang->pVideo);
  _SetWidgetText(ID_TEXT_00_Copy10, pLang->pVehicle);
  _SetWidgetText(ID_TEXT_00_Copy11, pLang->pPhone);
  _SetWidgetText(ID_TEXT_00_Copy12, pLang->pSettings);
  _SetWidgetText(ID_TEXT_00_Copy13, pLang->pOff);
  if (_hScreen) {
    WM_InvalidateWindow(_hScreen);
  }
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
  } else if ((Id == ID_TEXT_00_Copy2) ||
             (Id == ID_TEXT_00_Copy5) ||
             (Id == ID_TEXT_00_Copy6) ||
             (Id == ID_TEXT_00_Copy14) ||
             (Id == ID_TEXT_00_Copy15)) {
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
  int x;
  int i;

  GUI_SetColor(0xdd101820);
  GUI_FillRect(0, 404, 799, 479);
  for (i = 0, x = 18; i < 7; i++, x += 110) {
    _DrawNavItem(x, x + 104, i == 2);
  }
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
  _DrawRoadScene();
  _DrawCarStatusIcon();
  _DrawWeatherIcon();
  _DrawTopIcons();
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

  _CopyText(_acTime,    sizeof(_acTime),    "10:32");
  _CopyText(_acDate,    sizeof(_acDate),    "Wednesday, May 27");
  _CopyText(_acSystem,  sizeof(_acSystem),  "All Systems");
  _CopyText(_acStatus,  sizeof(_acStatus),  "READY");
  _CopyText(_acTemp,    sizeof(_acTemp),    "75 F");
  _CopyText(_acWeather, sizeof(_acWeather), "Partly Cloudy");
  _CopyText(_acRange,   sizeof(_acRange),   "Hi 82   Low 70");

  WM_InvalidateWindow(hScreen);
}

static void _OnButtonClick(WM_HWIN hScreen) {
  GUI_USE_PARA(hScreen);
  (void)_ShowMediaImage("JPEG", "0:/demo.jpg", GUI_DEMO_IMAGE_JPEG, 1);
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

static int _ShowMediaImage(const char * pName, const char * pPath, int Format, int Report) {
  WM_HWIN hImage;
  int Result;

  if (_hScreen == 0) {
    return 0;
  }
  hImage = WM_GetDialogItem(_hScreen, ID_IMAGE_00);
  Result = (hImage != 0) ? GUI_Demo_ImageFile(pPath, hImage, Format) : 0;
  if (Result == 1) {
    WM_ShowWindow(hImage);
    WM_BringToTop(hImage);
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), pName);
    if (Report) {
      printf("[VehicleUI] Applied MEDIA=%s\r\n", pName);
    }
    return 1;
  }
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

  if (_hScreen) {
    hImage = WM_GetDialogItem(_hScreen, ID_IMAGE_00);
    if (hImage) {
      WM_HideWindow(hImage);
    }
  }
  if (GUI_Demo_MOVIE("0:/demo.emf", 220, 96, 360, 260)) {
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "Media");
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), "MOVIE");
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
    _SetTextFromCommand(ID_TEXT_00_Copy2, _acSystem, sizeof(_acSystem), "All Systems");
    _SetTextFromCommand(ID_TEXT_00_Copy3, _acStatus, sizeof(_acStatus), "READY");
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

static const char *_FindValue(const char * pLine, const char * pKey) {
  const char * pValue;

  pValue = strstr(pLine, pKey);
  return pValue ? pValue + strlen(pKey) : NULL;
}

static int _ApplySerialCommand(const char * pLine, int Report) {
  const char * pValue;

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
  } else if ((pValue = _FindValue(pLine, "LANG=")) != NULL) {
    if (strstr(pValue, "DE") == pValue) {
      _ApplyLanguage(1);
      if (Report) {
        printf("[VehicleUI] Applied LANG=DE\r\n");
      }
      return 1;
    } else if (strstr(pValue, "EN") == pValue) {
      _ApplyLanguage(0);
      if (Report) {
        printf("[VehicleUI] Applied LANG=EN\r\n");
      }
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
    if (Report) {
      printf("[VehicleUI] Applied MEDIA=HIDE\r\n");
    }
    return 1;
  }
  return 0;
}

void VehicleUI_Exec(void) {
#ifndef WIN32
  int Count;
  uint8_t Byte;
  char c;

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
    break;
  case WM_NOTIFY_PARENT: {
    int NCode = pMsg->Data.v;
    int Id = WM_GetId(pMsg->hWinSrc);
    if (Id == ID_BUTTON_00 && NCode == WM_NOTIFICATION_CLICKED) {
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
