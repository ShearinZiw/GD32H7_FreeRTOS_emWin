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
  { ID_TEXT_00,         24,  14, 340, 36, "GD32H7 Industrial HMI" },
  { ID_TEXT_00_Copy,    24,  58, 220, 28, "Line A / Auto / Running" },
  { ID_TEXT_00_Copy1,   38, 116, 150, 28, "TEMP" },
  { ID_TEXT_00_Copy2,   38, 150, 150, 34, "36.5 C" },
  { ID_TEXT_00_Copy3,  218, 116, 150, 28, "PRESSURE" },
  { ID_TEXT_00_Copy4,  218, 150, 150, 34, "0.82 MPa" },
  { ID_TEXT_00_Copy5,  398, 116, 150, 28, "MOTOR" },
  { ID_TEXT_00_Copy6,  398, 150, 150, 34, "1280 RPM" },
  { ID_TEXT_00_Copy8,  578, 116, 150, 28, "OUTPUT" },
  { ID_TEXT_00_Copy9,  578, 150, 150, 34, "72 %" },
  { ID_TEXT_00_Copy10,  40, 258, 210, 28, "Trend: temp / pressure" },
  { ID_TEXT_00_Copy11, 456, 258, 180, 28, "Alarm queue" },
  { ID_TEXT_00_Copy12, 456, 296, 250, 28, "No active alarm" },
  { ID_TEXT_00_Copy13, 456, 334, 250, 28, "Touch: ready" },
  { ID_TEXT_00_Copy14, 456, 372, 250, 28, "SD: 0:/demo.jpg" },
  { ID_TEXT_00_Copy15, 456, 410, 250, 28, "FPS/CPU: pending" },
  { ID_TEXT_00_Copy19, 612,  22, 150, 28, "Load SD image" },
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
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextColor(hItem, 0xffd8f2ff);
  } else if ((Id == ID_TEXT_00_Copy2) ||
             (Id == ID_TEXT_00_Copy4) ||
             (Id == ID_TEXT_00_Copy6) ||
             (Id == ID_TEXT_00_Copy9)) {
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextColor(hItem, 0xffffffff);
  } else {
    TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
    TEXT_SetTextColor(hItem, 0xffc8d0d8);
  }
}

static void _DrawCard(int x0, int y0, int x1, int y1, GUI_COLOR Color) {
  GUI_SetColor(Color);
  GUI_FillRoundedRect(x0, y0, x1, y1, 6);
  GUI_SetColor(0xff404040);
  GUI_DrawRoundedRect(x0, y0, x1, y1, 6);
}

static void _DrawBar(int x0, int y0, int x1, int y1, int Percent, GUI_COLOR Color) {
  int Fill;

  if (Percent < 0) {
    Percent = 0;
  } else if (Percent > 100) {
    Percent = 100;
  }
  Fill = x0 + ((x1 - x0) * Percent) / 100;
  GUI_SetColor(0xff303030);
  GUI_FillRoundedRect(x0, y0, x1, y1, 4);
  GUI_SetColor(Color);
  GUI_FillRoundedRect(x0, y0, Fill, y1, 4);
}

static void _DrawTrend(void) {
  int i;
  const int ax = 42;
  const int ay = 405;
  const int w = 330;
  const int h = 112;
  const int yTemp[]  = { 65, 58, 62, 48, 54, 38, 44, 30, 36, 28, 32 };
  const int yPress[] = { 88, 82, 78, 84, 72, 76, 64, 70, 58, 62, 52 };

  GUI_SetColor(0xff3a3a3a);
  for (i = 0; i <= 4; i++) {
    GUI_DrawLine(ax, ay - i * (h / 4), ax + w, ay - i * (h / 4));
  }
  GUI_SetPenSize(3);
  GUI_SetColor(0xff35c46a);
  for (i = 0; i < 10; i++) {
    GUI_DrawLine(ax + i * 32, ay - yTemp[i], ax + (i + 1) * 32, ay - yTemp[i + 1]);
  }
  GUI_SetColor(0xff2aa8ff);
  for (i = 0; i < 10; i++) {
    GUI_DrawLine(ax + i * 32, ay - yPress[i], ax + (i + 1) * 32, ay - yPress[i + 1]);
  }
  GUI_SetPenSize(1);
}

static void _PaintDashboard(void) {
  GUI_SetBkColor(0xff101418);
  GUI_Clear();

  GUI_SetColor(0xff181f26);
  GUI_FillRect(0, 0, 799, 479);
  GUI_SetColor(0xff222a32);
  GUI_FillRect(0, 0, 799, 64);

  _DrawCard( 24,  96, 188, 212, 0xff1e2b24);
  _DrawCard(204,  96, 368, 212, 0xff202935);
  _DrawCard(384,  96, 548, 212, 0xff2a2520);
  _DrawCard(564,  96, 740, 212, 0xff242431);

  _DrawBar( 38, 192, 174, 202, 58, 0xff35c46a);
  _DrawBar(218, 192, 354, 202, 82, 0xff2aa8ff);
  _DrawBar(398, 192, 534, 202, 64, 0xffffb347);
  _DrawBar(578, 192, 726, 202, 72, 0xffb68cff);

  _DrawCard( 24, 244, 396, 446, 0xff18222b);
  _DrawCard(436, 244, 740, 446, 0xff201e24);
  _DrawTrend();

  GUI_SetColor(0xff35c46a);
  GUI_FillRoundedRect(698, 28, 724, 54, 13);
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
    WM_SetWindowPos(hImage, 378, 84, 400, 300);
    WM_HideWindow(hImage);
  }

  for (i = 0; i < GUI_COUNTOF(_aTextItem); i++) {
    _SetWidgetPos(hScreen, _aTextItem[i].Id, _aTextItem[i].x, _aTextItem[i].y,
                  _aTextItem[i].w, _aTextItem[i].h);
    _ConfigureText(hScreen, _aTextItem[i].Id);
    _SetWidgetText(_aTextItem[i].Id, _aTextItem[i].pText);
  }

  _SetWidgetPos(hScreen, ID_BUTTON_00, 586, 22, 110, 32);
  _SetWidgetText(ID_BUTTON_00, "Load");
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
