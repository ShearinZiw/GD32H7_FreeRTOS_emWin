/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2026  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_00.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_00.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _aCreate
*/
static APPW_CREATE_ITEM _aCreate[] = {
  { WM_OBJECT_WINDOW_Create,
    ID_SCREEN_00, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, -1, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 1, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 2, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, -1, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy1, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 2, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 102, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy2, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 157, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy3, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 218, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy4, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 294, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy5, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 2, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 365, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy6, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 426, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy8, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 254, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 112, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy9, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 252, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 168, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy10, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 252, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 229, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy11, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 252, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 305, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy12, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 254, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 375, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy13, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 252, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 436, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy14, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 501, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 58, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy15, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 499, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 114, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00_Copy19, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 525, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 426, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      271, 43, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BUTTON_Create,
    ID_BUTTON_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 587, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 21, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 30, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_00,
    { { { DISPOSE_MODE_REL_PARENT, 122, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, -1, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      480, 480, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_BOX_00,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_00,        APPW_SET_PROP_COLOR,        { ARG_V(0xff00aa00) } },
  { ID_TEXT_00,        APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00,        APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00,        APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00,        APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy,   APPW_SET_PROP_COLOR,        { ARG_V(GUI_YELLOW) } },
  { ID_TEXT_00_Copy,   APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy,   APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy,   APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy,   APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy1,  APPW_SET_PROP_COLOR,        { ARG_V(GUI_GREEN) } },
  { ID_TEXT_00_Copy1,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy1,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy1,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy1,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy2,  APPW_SET_PROP_COLOR,        { ARG_V(0xff55ffff) } },
  { ID_TEXT_00_Copy2,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy2,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy2,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy2,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy3,  APPW_SET_PROP_COLOR,        { ARG_V(0xffff55ff) } },
  { ID_TEXT_00_Copy3,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy3,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy3,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy3,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy4,  APPW_SET_PROP_COLOR,        { ARG_V(GUI_RED) } },
  { ID_TEXT_00_Copy4,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy4,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy4,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy4,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_WHITE) } },
  { ID_TEXT_00_Copy5,  APPW_SET_PROP_COLOR,        { ARG_V(0xff00aaff) } },
  { ID_TEXT_00_Copy5,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy5,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy5,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy5,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy6,  APPW_SET_PROP_COLOR,        { ARG_V(0xff55007f) } },
  { ID_TEXT_00_Copy6,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy6,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy6,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy6,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy8,  APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLUE) } },
  { ID_TEXT_00_Copy8,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy8,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy8,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy8,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy9,  APPW_SET_PROP_COLOR,        { ARG_V(0xffaa00ff) } },
  { ID_TEXT_00_Copy9,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy9,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy9,  APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy9,  APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy10, APPW_SET_PROP_COLOR,        { ARG_V(0xffaa0000) } },
  { ID_TEXT_00_Copy10, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy10, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy10, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy10, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy11, APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_00_Copy11, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy11, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy11, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy11, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy12, APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLUE) } },
  { ID_TEXT_00_Copy12, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy12, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy12, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy12, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy13, APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLUE) } },
  { ID_TEXT_00_Copy13, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy13, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy13, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy13, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy14, APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLUE) } },
  { ID_TEXT_00_Copy14, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy14, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy14, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy14, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy15, APPW_SET_PROP_COLOR,        { ARG_V(0xffaa007f) } },
  { ID_TEXT_00_Copy15, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy15, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy15, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy15, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_00_Copy19, APPW_SET_PROP_COLOR,        { ARG_V(0xff005500) } },
  { ID_TEXT_00_Copy19, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00_Copy19, APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_0) } },
  { ID_TEXT_00_Copy19, APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_00_Copy19, APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_BUTTON_00,      APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acDARK_Button_Up_100x30),
                                                     ARG_VP(0, acDARK_Button_Down_100x30), } },
  { ID_BUTTON_00,      APPW_SET_PROP_COLORS,       { ARG_V(0xffc0c0c0),
                                                     ARG_V(0xffc0c0c0),
                                                     ARG_V(GUI_INVALID_COLOR) } },
#if 0  /* IMAGE needs file system — disabled */
  { ID_IMAGE_00,       APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_00,       APPW_SET_PROP_SBITMAP,      { ARG_VP(1, "DARK_Image_DiagLinesGray_40x40.dta"),
                                                     ARG_V(170), } },
#endif
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_BUTTON_00,      WM_NOTIFICATION_SET,              ID_TEXT_00_Copy,   APPW_JOB_SETCOLOR,       ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_TEXT_00_Copy__APPW_JOB_SETCOLOR,
    { ARG_V(GUI_BLUE),
      ARG_V(0),
    }, 0, NULL
  },
#if 0  /* IMAGE disabled — skip bitmap action */
  { ID_BUTTON_00,      WM_NOTIFICATION_SET,              ID_IMAGE_00,       APPW_JOB_SETBITMAP,      ID_SCREEN_00__ID_BUTTON_00__WM_NOTIFICATION_SET__ID_IMAGE_00__APPW_JOB_SETBITMAP,
    { ARG_V(0),
      ARG_VP(1, "test.dta"),
      ARG_V(25439),
    }, 0, NULL
  },
#endif
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_00_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_00_RootInfo = {
  ID_SCREEN_00,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_00,
  0
};

/*************************** End of file ****************************/
