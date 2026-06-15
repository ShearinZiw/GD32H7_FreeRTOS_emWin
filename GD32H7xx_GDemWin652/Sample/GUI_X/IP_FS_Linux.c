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
Purpose     : Implementation of file system on Linux OS
---------------------------END-OF-HEADER------------------------------
*/

#include <stdio.h>
#include <string.h>
#include "IP_FS.h"

#ifdef __linux
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>

/*********************************************************************
*
*       defines, configurable
*
**********************************************************************
*/
#ifndef   IP_FS_BASEDIR
  #define IP_FS_BASEDIR  "/srv/"
#endif
/*********************************************************************
*
*       static data
*
**********************************************************************
*/
static char*  _sBaseDir;
static char   _acBaseDir[256] = IP_FS_BASEDIR;
static int    _IsInited       = 0;

/*********************************************************************
*
*       static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitIfRequired
*/
static void _InitIfRequired(){
  struct passwd* pPW;
  int            HomeLen;
  int            TotalLen;

  if (_IsInited == 0) {
    if (_acBaseDir[0] == '~') {
      pPW          = getpwuid(getuid());
      if (pPW != NULL) {
        HomeLen   = strlen(pPW->pw_dir);
        TotalLen  = HomeLen + strlen(&_acBaseDir[1]) + 1;
        _sBaseDir = (char*)malloc(TotalLen);
        if (_sBaseDir != NULL) {
          strncpy(_sBaseDir, pPW->pw_dir, TotalLen);
          strncat(_sBaseDir, &_acBaseDir[1], TotalLen);
        }
      }
    } else {
      _sBaseDir = &_acBaseDir[0];
    }
    _IsInited = 1;
  }
}

/*********************************************************************
*
*       _ConvertFileName
*/
static void _ConvertFileName(char* sDest, const char* sSrc, unsigned BufferSize) {
  char c;
  const char* s;
  U32 i;
  _InitIfRequired();
  //
  // If sSrc starts with a '/' skip it.
  //
  if (sSrc[0] == '/') {
    s = sSrc + 1;
  } else {
    s = sSrc;
  }
  //
  // Convert given filename into Linux filename.
  //
  strncpy(sDest, _sBaseDir, BufferSize);
  *(sDest + BufferSize - 1) = '\0';
  i = strlen(sDest);
  while(1) {
    c = *s++;
    *(sDest + i++) = c;
    if (c == 0) {
      break;
    }
  }
}

/*********************************************************************
*
*       _IP_FS_Linux_Open
*/
static void* _IP_FS_Linux_Open(const char* sFileName) {
  char acFilename[256];
  int hFile;

  _ConvertFileName(acFilename, sFileName, sizeof(acFilename));
  hFile = open(acFilename, O_RDONLY);
  if (hFile == -1) {
    hFile = 0;
  }
  return (void*)(intptr_t)hFile;
}

/*********************************************************************
*
*       _IP_FS_Linux_Close
*/
static int _IP_FS_Linux_Close(void* hFile) {
  close((intptr_t)hFile);
  return 0;
}

/*********************************************************************
*
*       _IP_FS_Linux_ReadAt
*/
static int _IP_FS_Linux_ReadAt(void* hFile, void* pDest, U32 Pos, U32 NumBytes) {
  ssize_t NumBytesRead;

  lseek((intptr_t)hFile, Pos, SEEK_SET);
  NumBytesRead = read((intptr_t)hFile, pDest, NumBytes);
  if (NumBytesRead == -1) {
    return -1;
  }
  return NumBytesRead;
}

/*********************************************************************
*
*       _IP_FS_Linux_GetLen
*/
static long _IP_FS_Linux_GetLen(void* hFile) {
  struct stat buf;
  int r;

  r = fstat((intptr_t) hFile, &buf);
  if (r == -1) {
    return -1;
  }
  return buf.st_size;
}

/*********************************************************************
*
*       _IP_FS_Linux_ForEachDirEntryEx
*/
static int _IP_FS_Linux_ForEachDirEntryEx(void* pContext, const char* sDir, void (*pf)(void* pContext, void* pFileEntry)) {
  DIR* hDir;
  struct dirent* pFd;
  int r;
  char acFilter[256];

  r = 0;
  _InitIfRequired();
  strcpy(acFilter, _sBaseDir);
  if (sDir[0] == '/') {
    strcat(acFilter, sDir + 1);
  } else {
    strcat(acFilter, sDir);
  }

  hDir = opendir(acFilter);
  if (hDir != NULL) {
    while ((pFd = readdir(hDir))) {
      pf(pContext, pFd);
    }
  } else {
    r = -1;
  }
  closedir(hDir);
  return r;
}

/*********************************************************************
*
*       _IP_FS_Linux_ForEachDirEntry
*/
static void _IP_FS_Linux_ForEachDirEntry(void* pContext, const char* sDir, void (*pf)(void* pContext, void* pFileEntry)) {
	_IP_FS_Linux_ForEachDirEntryEx(pContext, sDir, pf);
}

/*********************************************************************
*
*       _IP_FS_Linux_GetDirEntryFileName
*/
static void _IP_FS_Linux_GetDirEntryFileName(void* pFileEntry, char* sFileName, U32 SizeOfBuffer) {
  struct dirent* pFd;

  pFd = (struct dirent*)pFileEntry;
  strncpy(sFileName, pFd->d_name, SizeOfBuffer);
  *(sFileName + SizeOfBuffer - 1) = 0;
}

/*********************************************************************
*
*       _IP_FS_Linux_GetDirEntryFileSize
*/
static U32 _IP_FS_Linux_GetDirEntryFileSize(void* pFileEntry, U32* pFileSizeHigh) {
  struct dirent* pFd;

  pFd = (struct dirent*)pFileEntry;
  return pFd->d_reclen;
}

/*********************************************************************
*
*       _IP_FS_Linux_GetDirEntryAttributes
*
*  Return value
*    bit 0   - 0: File, 1:Directory
*/
static int _IP_FS_Linux_GetDirEntryAttributes(void* pFileEntry) {
  struct dirent* pFd;

  pFd = (struct dirent*)pFileEntry;
  return (pFd->d_type & DT_DIR) ? 1 : 0;
}

/*********************************************************************
*
*       _IP_FS_Linux_Create
*/
static void* _IP_FS_Linux_Create(const char* sFileName) {
  char acFilename[256];
  int hFile;

  _ConvertFileName(acFilename, sFileName, sizeof(acFilename));
  hFile = open(acFilename, O_RDWR | O_CREAT, 0644);
  if (hFile == -1) {
    hFile = 0;
  }
  return (void*)(intptr_t)hFile;
}

/*********************************************************************
*
*       _IP_FS_Linux_WriteAt
*/
static int _IP_FS_Linux_WriteAt(void* hFile, void* pBuffer, U32 Pos, U32 NumBytes) {
  lseek((intptr_t)hFile, Pos, SEEK_SET);
  return write((intptr_t)hFile, pBuffer, NumBytes);
}

/*********************************************************************
*
*       _IP_FS_Linux_DeleteFile
*/
static void* _IP_FS_Linux_DeleteFile(const char* sFilename) {
  char acFilename[256];

  _ConvertFileName(acFilename, sFilename, sizeof(acFilename));
  printf("%s", acFilename);
  return (void *)(intptr_t)unlink(acFilename);
}

/*********************************************************************
*
*       _IP_FS_Linux_MoveFile
*/
static int _IP_FS_Linux_MoveFile(const char* sOldFilename, const char* sNewFilename) {
  char acOldFilename[256];
  char acNewFilename[256];

  _ConvertFileName(acOldFilename, sOldFilename, sizeof(acOldFilename));
  _ConvertFileName(acNewFilename, sNewFilename, sizeof(acNewFilename));
  return rename(acOldFilename, acNewFilename);
}

/*********************************************************************
*
*       _IP_FS_Linux_MakeDir
*/
static int _IP_FS_Linux_MakeDir(const char* sDirname) {
  char acDirname[256];

  _ConvertFileName(acDirname, sDirname, sizeof(acDirname));
  return mkdir(acDirname, 0775);
}

/*********************************************************************
*
*       _IP_FS_Linux_RemoveDir
*/
static int _IP_FS_Linux_RemoveDir(const char* sDirname) {
  char acDirname[256];

  _ConvertFileName(acDirname, sDirname, sizeof(acDirname));
  return rmdir(acDirname);
}

/*********************************************************************
*
*       _IP_FS_Linux_RemoveDir
*/
int _IP_FS_Linux_IsFolder(const char* sDirname) {
  struct stat statbuf;

  if (stat(sDirname, &statbuf) != 0) {
    return 0;
  }
  return S_ISDIR(statbuf.st_mode);
}

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
const IP_FS_API IP_FS_Linux = {
  //
  // Read only file operations.
  //
  _IP_FS_Linux_Open,
  _IP_FS_Linux_Close,
  _IP_FS_Linux_ReadAt,
  _IP_FS_Linux_GetLen,
  //
  // Simple directory operations.
  //
  _IP_FS_Linux_ForEachDirEntry,
  _IP_FS_Linux_GetDirEntryFileName,
  _IP_FS_Linux_GetDirEntryFileSize,
  NULL,
  _IP_FS_Linux_GetDirEntryAttributes,
  //
  // Simple write type file operations.
  //
  _IP_FS_Linux_Create,
  _IP_FS_Linux_DeleteFile,
  _IP_FS_Linux_MoveFile,
  _IP_FS_Linux_WriteAt,
  //
  // Additional directory operations
  //
  _IP_FS_Linux_MakeDir,
  _IP_FS_Linux_RemoveDir,
  //
  // Additional operations
  //
  _IP_FS_Linux_IsFolder,
  _IP_FS_Linux_MoveFile,
  _IP_FS_Linux_ForEachDirEntryEx,
};

#endif  // __linux
/*************************** End of file ****************************/
