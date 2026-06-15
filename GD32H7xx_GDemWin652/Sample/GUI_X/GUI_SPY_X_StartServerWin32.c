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
File        : GUI_SPY_X_StartServer.c
Purpose     : Start of the emWinSPY server in WIN32 simulation.
---------------------------END-OF-HEADER------------------------------
*/

#include <windows.h>

#include "GUI.h"
#include "GUI_Debug.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PORT 2468 // emWinSPY-Port

static HINSTANCE _hLib;

static void _LoadWSock32(void) {
  if (_hLib == 0) {
    _hLib = LoadLibrary("WSock32.dll");
  }
}

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
static int _ListenAtTcpAddr(unsigned short port) {
  #define _TRY(x) if ((x) < 0) { closesocket(sock); return -1; }
  
  int sock;
  struct sockaddr_in addr;
  int one = 1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  _TRY(sock = socket(AF_INET, SOCK_STREAM, 0));
  _TRY(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &one, sizeof(one)));
  _TRY(bind(sock, (struct sockaddr *)&addr, sizeof(addr)));
  _TRY(listen(sock, 5));
  return sock;
}

/*********************************************************************
*
*       _Send / _Recv
*
*  Function description:
*    Functions for sending and receiving data used by emWinSPY
*
*  Return value:
*    In case of success both functions should return the number 
*    of successfully transmitted bytes, otherwise -1.
*/
static int _Send(const U8 * buf, int len, void * pConnectInfo) {
  int r;

  if (len <= 0) {
    return -1;
  }
  r = send((SOCKET)pConnectInfo, buf, len, 0);
  if (r == SOCKET_ERROR) {
    return -1;
  }
  return r;
}

static int _Recv(U8 * buf, int len, void * pConnectInfo) {
  int r;

  if (len <= 0) {
    return -1;
  }
  r = recv((SOCKET)pConnectInfo, buf, len, 0);
  if (r == SOCKET_ERROR) {
    return -1;
  }
  return r;
}

/*********************************************************************
*
*       _ServerThread
*
*  Function description:
*    The server thread waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should return
*    if the connection is closed.
*/
static UINT __stdcall _ServerThread(void * p) {
  WORD wVersionRequested;
  WSADATA wsaData;
  int r, s, sock, addrlen;
  struct sockaddr addr;
  
  GUI_USE_PARA(p);
  //
  // Initialize WinSock
  //
  wVersionRequested = MAKEWORD(2, 0);
  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    return 1;
  }
  s = _ListenAtTcpAddr(PORT);
  if (s < 0) {
    return 1; // Error
  }
  while (1) {
    //
    // Wait for an incoming connection
    //
    addrlen = sizeof(addr);
    if ((sock = accept(s, &addr, &addrlen)) < 0) {
      return 1; // Error
    }
    //
    // Execute emWinSPY
    //
    r = GUI_SPY_Process(_Send, _Recv, (void *)sock);
    //
    // Close connection
    //
    closesocket(sock);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SPY_X_StartServer()
*
*  Function description
*    That function actually is responsible for creating the server thread and establishing a
*    connection. When running the simulation it already contains an implementation of
*    that function. When running on hardware that function has to be supplied by the customer.
*
*  Return value
*    == 0: on success
*    == 1: on error.
*
*  Additional information
*    As already mentioned earlier the sample folder \f{Sample\\GUI_X} contains a sample
*    implementation of a server thread. It is located in the file \f{GUI_SPY_X_StartServer.c}
*    and is written to be used with embOS/IP. In case of using different tools it should be
*    an easy task to adapt that sample.
*/
int GUI_SPY_X_StartServer(void) {
  int ThreadId;
  HANDLE hThread;
  
  hThread = CreateThread(NULL, 0, _ServerThread, NULL, 0, &ThreadId);
  return hThread ? 0 : 1;
}

/*************************** End of file ****************************/
