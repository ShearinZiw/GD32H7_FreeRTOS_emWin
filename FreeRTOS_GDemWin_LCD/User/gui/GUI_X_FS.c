/**
 *********************************************************************
 * @file    GUI_X_FS.c
 * @brief   emWin file system bridge — FatFS backend
 *
 * Implements the emWin IP_FS_* callbacks using FatFS f_* functions.
 * This enables emWin to load images (PNG/GIF/JPEG), fonts (TTF/XBF),
 * and video (MOVIE) directly from files on SD card / OSPI Flash.
 *
 * Usage:
 *   1. Call FS_Init() once to mount all drives.
 *   2. emWin API like GUI_PNG_Draw("0:/image.png", ...) work
 *      directly with FatFS paths.
 *********************************************************************
 */

#include "GUI.h"
#include "ff.h"
#include <stdio.h>

/*===========================================================================
 * Drive configuration
 *===========================================================================*/
#define FS_DRIVE_SD         "0:"    /* SD card (FatFS drive 0) */
#define FS_DRIVE_OSPI       "1:"    /* OSPI Flash (FatFS drive 1) */
#define FS_MAX_OPEN_FILES   8

static FATFS _g_sd_fs;
static FATFS _g_ospi_fs;
static int    _g_fs_ready = 0;

typedef struct {
    FIL File;
    U32 Size;
    U8  InUse;
} FS_FILE_HANDLE;

static FS_FILE_HANDLE _aFileHandle[FS_MAX_OPEN_FILES];

static FS_FILE_HANDLE *_AllocFileHandle(void)
{
    int i;

    for (i = 0; i < FS_MAX_OPEN_FILES; i++) {
        if (_aFileHandle[i].InUse == 0) {
            _aFileHandle[i].InUse = 1;
            _aFileHandle[i].Size = 0;
            return &_aFileHandle[i];
        }
    }
    return NULL;
}

static void _ReleaseFileHandle(FS_FILE_HANDLE *pHandle)
{
    if (pHandle) {
        pHandle->Size = 0;
        pHandle->InUse = 0;
    }
}

/*===========================================================================
 * FS_Init — mount all file systems
 *===========================================================================*/

int FS_Init(void)
{
    FRESULT res;

    printf("[FS] Mounting SD card (%s)...\r\n", FS_DRIVE_SD);
    res = f_mount(&_g_sd_fs, FS_DRIVE_SD, 1);
    if (res == FR_OK) {
        printf("[FS] SD card mounted OK\r\n");
        _g_fs_ready = 1;
    } else {
        printf("[FS] SD card mount failed (err=%d)\r\n", res);
    }

    printf("[FS] Mounting OSPI Flash (%s)...\r\n", FS_DRIVE_OSPI);
    res = f_mount(&_g_ospi_fs, FS_DRIVE_OSPI, 1);
    if (res == FR_OK) {
        printf("[FS] OSPI Flash mounted OK\r\n");
    } else {
        printf("[FS] OSPI Flash mount failed (err=%d), trying f_mkfs...\r\n", res);
        /* First-time use: format the OSPI flash */
        res = f_mkfs(FS_DRIVE_OSPI, 0, 0);
        if (res == FR_OK) {
            res = f_mount(&_g_ospi_fs, FS_DRIVE_OSPI, 1);
            if (res == FR_OK) {
                printf("[FS] OSPI Flash formatted and mounted OK\r\n");
            }
        }
    }

    return _g_fs_ready;
}

/*===========================================================================
 * emWin IP_FS Interface
 *
 * emWin calls these functions with DOS-style paths like "0:/image.png".
 * Keep one FatFS FIL open per emWin handle so streamed media, GIF
 * animation and external fonts can use sequential reads and seeks.
 *===========================================================================*/

/**
 * IP_FS_Open — open a file for reading.
 */
const void *IP_FS_Open(const char *sPath, U32 *pSize)
{
    FS_FILE_HANDLE *pHandle;
    FRESULT res;

    if (pSize) {
        *pSize = 0;
    }

    pHandle = _AllocFileHandle();
    if (pHandle == NULL) {
        printf("[FS] No free file handles opening: %s\r\n", sPath);
        return NULL;
    }

    res = f_open(&pHandle->File, sPath, FA_READ);
    if (res != FR_OK) {
        printf("[FS] Open failed: %s (err=%d)\r\n", sPath, res);
        _ReleaseFileHandle(pHandle);
        return NULL;
    }

    pHandle->Size = (U32)f_size(&pHandle->File);
    if (pSize) {
        *pSize = pHandle->Size;
    }
    return pHandle;
}

/**
 * IP_FS_Close — close a previously opened file.
 */
void IP_FS_Close(const void *pHandle)
{
    FS_FILE_HANDLE *pFile = (FS_FILE_HANDLE *)pHandle;

    if (pFile) {
        f_close(&pFile->File);
        _ReleaseFileHandle(pFile);
    }
}

/**
 * IP_FS_Read — read data from a file.
 */
int IP_FS_Read(void *pBuffer, int ElementSize, int NumElements, const void *pHandle)
{
    FS_FILE_HANDLE *pFile = (FS_FILE_HANDLE *)pHandle;
    FRESULT res;
    UINT NumBytesRead = 0;
    UINT NumBytesToRead;

    if ((pFile == NULL) || (pBuffer == NULL) || (ElementSize <= 0) || (NumElements <= 0)) {
        return 0;
    }

    NumBytesToRead = (UINT)(ElementSize * NumElements);
    res = f_read(&pFile->File, pBuffer, NumBytesToRead, &NumBytesRead);

    return (res == FR_OK) ? (int)(NumBytesRead / (UINT)ElementSize) : 0;
}

/**
 * IP_FS_Seek — seek within a file.
 */
int IP_FS_Seek(const void *pHandle, U32 Off)
{
    FS_FILE_HANDLE *pFile = (FS_FILE_HANDLE *)pHandle;
    FRESULT res;

    if (pFile == NULL) {
        return 1;
    }

    res = f_lseek(&pFile->File, Off);
    return (res == FR_OK) ? 0 : 1;
}

/**
 * IP_FS_GetLen — get file size.
 */
U32 IP_FS_GetLen(const void *pHandle)
{
    FS_FILE_HANDLE *pFile = (FS_FILE_HANDLE *)pHandle;

    return (pFile != NULL) ? pFile->Size : 0;
}
