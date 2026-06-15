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
-------------------------- END-OF-HEADER -----------------------------

File    : GUI_VNC_X_StartServer.c
Purpose : Starts the VNC server using plain BSD socket API.
*/

#include "GUI_VNC.h"
#include "GUI_Debug.h"

#include "IP_FS.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
//
// Define maximum number of VNC clients that can connect
//
#define VNC_MAX_CLIENTS  3

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

enum {
  SOCKET_TYPE_BSD = 0,
  SOCKET_TYPE_WEBSOCKET
};

#define LOCK(hLock)    pthread_mutex_lock(&hLock)
#define UNLOCK(hLock)  pthread_mutex_unlock(&hLock)

#define NUM_THREADS              1
//
// Internal thread ID
//
#define ID_THREAD_SERVER         0

/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/

typedef void* SYS_HANDLE;

typedef struct {
  pthread_mutex_t      hLock;
  GUI_VNC_CONTEXT      Context;
  void               * pConnection;
  U8                   acBuffer [GUI_VNC_BUFFER_SIZE];
  pthread_t            aThreadId[NUM_THREADS];     // [0]:     Server thread ID.
                                                   // [1]:     Parent webserver thread ID.
                                                   // [2,...]: Child webserver thread ID. Can be more than one.
} GUI_VNC_CONNECTION;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GUI_VNC_CONNECTION _aConnections[VNC_MAX_CLIENTS];
static GUI_VNC_CONTEXT  * _pFirstContext;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetConnection()
*
*  Function description
*    Returns a pointer to the current connection struct based on the thread ID.
*
*  Return value
*    == NULL: Error.
*    != NULL: Current connection.
*/
static GUI_VNC_CONNECTION * _GetConnection(void) {
  int   i, j;

  pthread_t   tid;
  tid = pthread_self();
  //
  // Look for index of this task.
  //
  for (j = 0; j < VNC_MAX_CLIENTS; j++) {
    for (i = 0; i < NUM_THREADS; i++) {
      if (_aConnections[j].aThreadId[i] == tid) {
        return &_aConnections[j];
      }
    }
  }
  return NULL;
}

/*********************************************************************
*
*       _cbVNC_Recv()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Where to store the received data.
*    NumBytes   : Maximum amount of data to receive.
*    pConnection: Connection handle (WebSocket context).
*
*  Return value
*    Amount of data received: >  0
*    Connection closed      : == 0
*    Error                  : <  0
*/
static int _cbVNC_Recv(U8* pData, int NumBytes, void* pConnection) {
  return recv((PTR_ADDR)pConnection, (char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _cbVNC_Send()
*
*  Function description
*    This function is called indirectly by the server; it's address
*    is passed to the actual server code as function pointer. It is
*    needed because the server is independent of the TCP/IP stack
*    implementation, so details for the TCP/IP stack can be placed
*    here.
*
*  Parameters
*    pData      : Data to send.
*    NumBytes   : Amount of data to send.
*    pConnection: Connection handle.
*
*  Return value
*    Amount of data sent: >  0
*    Connection closed  : == 0
*    Error              : <  0
*/
static int _cbVNC_Send(const U8* pData, int NumBytes, void* pConnection) {
  return send((PTR_ADDR)pConnection, (const char*)pData, NumBytes, 0);
}

/*********************************************************************
*
*       _ListenAtTCPAddr()
*
*  Function description
*    Creates a non-blocking socket for accepting a VNC connection.
*
*  Parameters
*    Port: Port to use for accepting connections.
*
*  Return value
*    Parent socket handle.
*/
static PTR_ADDR _ListenAtTCPAddr(U16 Port) {
  PTR_ADDR           hSock;
  struct sockaddr_in InAddr;

  hSock = socket(AF_INET, SOCK_STREAM, 0);
  if (hSock > 0) {
    memset(&InAddr, 0, sizeof(InAddr));
    InAddr.sin_family      = AF_INET;
    InAddr.sin_port        = htons(Port);
    InAddr.sin_addr.s_addr = INADDR_ANY;
    bind(hSock, (struct sockaddr*)&InAddr, sizeof(InAddr));
    listen(hSock, 1);
  }
  return hSock;
}

/*********************************************************************
*
*       _ServerTask()
*
*  Function description
*    This routine is the actual server task.
*    It waits in an endless loop for a new WebSocket connection to be
*    signaled and handles the VNC connection.
*
*  Parameters
*    pPara: Unused.
*
*  Return value
*    Error: 1
*/
static void * _ServerTask(void * pPara) {
  int                  r;
  GUI_VNC_CONNECTION * pConnect;
  PTR_ADDR             hSockParent;
  PTR_ADDR             hSock;

  //
  // Get connection pointer.
  //
  pConnect = _GetConnection();
  if (pConnect == NULL) {
    return NULL; // Error.
  }
  //
  // Start listening on a BSD socket for a connection.
  // WebSocket connections will be dispatched by the Web Server.
  //
  hSockParent = _ListenAtTCPAddr(5900 + pConnect->Context.ServerIndex);  // Default port for VNC is is 590x, where x is the 0-based layer index.
  if (hSockParent == 0) {                                                // 0 is a valid socket for Windows but we reject it.
	close(hSockParent);
  }
  if (hSockParent <= 0) {
    return NULL;  // Error.
  }
  while (1) {
    //
    // Check that the connection handle is actually valid.
    // A handle of NULL is treated as invalid although some
    // TCP/IP stacks like Windows treat a socket handle of
    // 0 as valid. However this occurs very rare and in this
    // situation the peer should simply close the connection
    // after some time due to a timeout.
    // We will first check if a connection handle is present
    // which means checking for a WebSocket handle. If no
    // WebSocket handle is active we will check non-blocking
    // if there is any BSD socket to accept.
    //
    LOCK(pConnect->hLock);  // Make sure that _pConnection does not get set from another task after being read by us.
    if (pConnect->pConnection == NULL) {
      //
      // Check if we have a connection to accept on the regular VNC port.
      //
      hSock = accept(hSockParent, NULL, NULL);
      if (hSock == -1) {
        UNLOCK(pConnect->hLock);
        //
        // Check if this is an IP_ERR_WOULD_BLOCK or a real error.
        //
        sleep(50);  // Grant the system some time between retrying.
        continue;
      }
      //
      // We have a connection on the regular VNC port.
      //
      pConnect->pConnection = (void *)hSock;
    }
    UNLOCK(pConnect->hLock);
    r = 0;
    //
    // Run the actual server using the BSD socket handle as connection handle.
    //
    r = GUI_VNC_Process(&pConnect->Context, _cbVNC_Send, _cbVNC_Recv, (void *)pConnect->pConnection);
    //
    // Close the connection and invalidate the connection handle
    // so we can accept a new client.
    //
    close((PTR_ADDR)pConnect->pConnection);
    pConnect->pConnection = NULL;
    switch (r) {
    case GUI_VNC_ERROR_WRONGFORMAT:
      GUI_DEBUG_ERROROUT("VNC: Client requests unsupported pixel format");
      break;
    }
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
*       GUI_VNC_X_StartServer()
*
*  Function description
*    This function has to start a thread listening for an incoming connection. If a
*    connection is established it has to execute the actual VNC server GUI_VNC_Process().
*    The function has to be supplied by the customer because the implementation
*    depends on the used TCP/IP stack and on the used operating system.
*    The emWin shipment contains an example implementation under
*    \c{Sample\\GUI_X\\GUI_VNC_X_StartServer.c}. It could be used as a starting point for
*    adapting it to other systems.
*
*  Parameters
*    LayerIndex: Layer to be shown by the viewer.
*    ServerIndex: Server index.
*
*  Return value
*    ==  0: OK.
*    == -1: Error.
*
*  Additional information
*    There is no difference to start a VNC server in the simulation or on the target. In
*    both cases you should call this function. The simulation contains an implementation
*    of this function, the hardware implementation has to be done by the customer.
*/
int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {
  GUI_VNC_CONNECTION * pConnection;
  GUI_VNC_CONTEXT    * pContext;
  GUI_VNC_CONTEXT    * pContextTemp;
  int err;

  //
  // Check if maximum number of VNC clients is not exceeded.
  //
  if (ServerIndex >= VNC_MAX_CLIENTS) {
    return -1;
  }
  pContext    = &_aConnections[ServerIndex].Context;
  pConnection = &_aConnections[ServerIndex];
  //
  // Init VNC context and attach to layer (so context is
  // updated if the display-layer-contents change).
  //
  // If we already have a device, set the device pointer
  // to the already created one.
  //
  if (_pFirstContext == NULL) {
    GUI_VNC_AttachToLayer(pContext, LayerIndex);
    _pFirstContext = pContext;
  } else {
    if (_pFirstContext->ServerIndex == ServerIndex) {
      //
      // Return error if server index is already in use.
      //
      return -1;
    }
    //
    // New context makes use of existing device.
    //
    pContext->pDevice      = _pFirstContext->pDevice;
    pContext->xSize        = _pFirstContext->xSize;
    pContext->ySize        = _pFirstContext->ySize;
    pContext->BitsPerPixel = _pFirstContext->BitsPerPixel;
    pContext->pNext        = NULL;
    //
    // Update pNext pointers
    //
    pContextTemp = _pFirstContext;
    while (pContextTemp->pNext) {
      pContextTemp = pContextTemp->pNext;
    }
    pContextTemp->pNext = pContext;
  }
  pContext->ServerIndex = ServerIndex;
  //
  // Set up pointer to RFB buffer.
  //
  pContext->pBuffer      = pConnection->acBuffer;
  pContext->SizeOfBuffer = GUI_VNC_BUFFER_SIZE;
  //
  // Create task for VNC server. It will wait for the signal
  // of a dispatched WebSocket connection.
  //
  err = pthread_create(&pConnection->aThreadId[ID_THREAD_SERVER], NULL, &_ServerTask, NULL);
  return err;
}

/*********************************************************************
*
*       GUI_VNC_X_StartServerFT()
*
*  Function description
*    Function which has to be implemented by the customer to start the VNC server with
*    file transfer support. Additionally to starting a server thread the function has to
*    enable the file transfer extensions by calling GUI_VNC_EnableFileTransfer() and it
*    has to set a function table to be used for file access by GUI_VNC_SetFS_API().
*
*  Parameters
*    LayerIndex: Layer to be shown by the viewer.
*    ServerIndex: Server index.
*
*  Return value
*    Returns 0.
*
*  Additional information
*    Under \c{Sample\\GUI_X\\GUI_VNC_X_StartServer.c} a sample is available which shows
*    a sample implementation using embOS/IP and emFile.
*/
int GUI_VNC_X_StartServerFT(int LayerIndex, int ServerIndex) {
  int r;

  //
  // Ready-to-use-version is currently only available for Windows.
  // For other environments the user has to implement his own FS-IP-API.
  //

//  //
//  // Set file access API (IP_FS_Win32, IP_FS_FS, ...).
//  //
//  GUI_VNC_SetFS_API(&IP_FS_Win32);
//  //
//  // Enable file transfer of VNC-server.
//  //
//  GUI_VNC_EnableFileTransfer(1);
//  //
//  // Start the VNC server.
//  //
//  r = GUI_VNC_X_StartServer(LayerIndex, ServerIndex);
  r = 1;
  return r;
}

/*************************** End of file ****************************/