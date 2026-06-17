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
#include <stdio.h>

/*** Begin of user code area ***/

typedef struct {
  int Id;
  int x;
  int y;
  int w;
  int h;
  const char * pText;
} HMI_TEXT_ITEM;

static const HMI_TEXT_ITEM _aTextItem[] = {
  { ID_TEXT_00,        206, 126, 390, 76, "10:32" },
  { ID_TEXT_00_Copy,   236,  90, 330, 30, "Wednesday, May 27" },
  { ID_TEXT_00_Copy1,   18,  20, 190, 26, "GUX vehicle terminal" },
  { ID_TEXT_00_Copy2,  200, 310, 160, 24, "All Systems" },
  { ID_TEXT_00_Copy3,  198, 340, 170, 38, "READY" },
  { ID_TEXT_00_Copy4,  574, 306, 150, 52, "75 F" },
  { ID_TEXT_00_Copy5,  420, 372, 170, 24, "Partly Cloudy" },
  { ID_TEXT_00_Copy6,  590, 372, 160, 24, "Hi 82   Low 70" },
  { ID_TEXT_00_Copy8,   24, 442,  82, 24, "Navigation" },
  { ID_TEXT_00_Copy9,  118, 442,  82, 24, "Audio" },
  { ID_TEXT_00_Copy10, 304, 442,  82, 24, "Vehicle" },
  { ID_TEXT_00_Copy11, 398, 442,  82, 24, "Phone" },
  { ID_TEXT_00_Copy12, 492, 442,  82, 24, "Settings" },
  { ID_TEXT_00_Copy13, 586, 442,  82, 24, "Off" },
  { ID_TEXT_00_Copy14,  42, 382, 210, 24, "Touch media to load SD image" },
  { ID_TEXT_00_Copy15, 544,  20, 170, 26, "Vol  Cam  Home" },
  { ID_TEXT_00_Copy19,   0,   0,   1,  1, "" },
};

static int _ScreenReady;

static void _SetWidgetText(int Id, const char * pText) {
  APPW_SetText(ID_SCREEN_00, Id, (char *)pText);
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
    { 330, 214 }, { 470, 214 }, { 628, 430 }, { 172, 430 }
  };
  GUI_POINT leftField[] = {
    { 0, 232 }, { 330, 214 }, { 172, 430 }, { 0, 430 }
  };
  GUI_POINT rightField[] = {
    { 470, 214 }, { 799, 232 }, { 799, 430 }, { 628, 430 }
  };
  GUI_POINT leftShoulder[] = {
    { 314, 224 }, { 330, 214 }, { 172, 430 }, { 144, 430 }
  };
  GUI_POINT rightShoulder[] = {
    { 470, 214 }, { 486, 224 }, { 656, 430 }, { 628, 430 }
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
  GUI_FillRect(390, 390, 410, 430);

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
  for (i = 0, x = 18; i < 7; i++, x += 94) {
    _DrawNavItem(x, x + 88, i == 2);
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

  _SetWidgetPos(hScreen, ID_BUTTON_00, 206, 418, 90, 56);
  _SetWidgetText(ID_BUTTON_00, "Video");
  WM_InvalidateWindow(hScreen);
}

static void _OnButtonClick(WM_HWIN hScreen) {
  static int click_cnt = 0;
  char buf[32];
  WM_HWIN hImage;

  click_cnt++;
  sprintf(buf, "SD image loaded #%d", click_cnt);
  _SetWidgetText(ID_TEXT_00_Copy14, buf);

  hImage = WM_GetDialogItem(hScreen, ID_IMAGE_00);
  GUI_Demo_JPEG_Image("0:/demo.jpg", hImage);
  if (hImage) {
    WM_ShowWindow(hImage);
    WM_BringToTop(hImage);
  }
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
