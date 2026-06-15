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
#include <string.h>
#include <stdio.h>

/*===========================================================================
 * Drive configuration
 *===========================================================================*/
#define FS_DRIVE_SD         "0:"    /* SD card (FatFS drive 0) */
#define FS_DRIVE_OSPI       "1:"    /* OSPI Flash (FatFS drive 1) */

static FATFS _g_sd_fs;
static FATFS _g_ospi_fs;
static int    _g_fs_ready = 0;

/*===========================================================================
 * FS_Init — mount all file systems
 *===========================================================================*/

int FS_Init(void)
{
    FRESULT res;
    char path[4] = "0:/";

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
 * We strip the drive prefix and pass the rest to FatFS.
 *===========================================================================*/

static FATFS *_GetFS(const char *sPath) {
    if (sPath[0] == '0' && sPath[1] == ':') return &_g_sd_fs;
    if (sPath[0] == '1' && sPath[1] == ':') return &_g_ospi_fs;
    return &_g_sd_fs;  /* Default to SD card */
}

static const char *_GetRelPath(const char *sPath) {
    if (sPath[0] >= '0' && sPath[0] <= '9' && sPath[1] == ':') {
        return sPath + 2;
    }
    return sPath;
}

/**
 * IP_FS_Open — open a file for reading.
 */
const void *IP_FS_Open(const char *sPath, U32 *pSize)
{
    static FIL file;
    FRESULT res;

    res = f_open(&file, sPath, FA_READ);
    if (res != FR_OK) {
        printf("[FS] Open failed: %s (err=%d)\r\n", sPath, res);
        *pSize = 0;
        return NULL;
    }

    *pSize = (U32)f_size(&file);
    f_close(&file);

    /* Return the path as an opaque handle (simplified: re-open on each read) */
    char *handle = malloc(strlen(sPath) + 1);
    if (handle) strcpy(handle, sPath);
    return handle;
}

/**
 * IP_FS_Close — close a previously opened file.
 */
void IP_FS_Close(const void *pHandle)
{
    if (pHandle) free((void *)pHandle);
}

/**
 * IP_FS_Read — read data from a file.
 */
int IP_FS_Read(void *pBuffer, int ElementSize, int NumElements, const void *pHandle)
{
    FIL file;
    FRESULT res;
    UINT br;

    if (!pHandle) return 0;

    res = f_open(&file, (const char *)pHandle, FA_READ);
    if (res != FR_OK) return 0;

    res = f_read(&file, pBuffer, ElementSize * NumElements, &br);
    f_close(&file);

    return (res == FR_OK) ? (int)(br / ElementSize) : 0;
}

/**
 * IP_FS_Seek — seek within a file.
 */
int IP_FS_Seek(const void *pHandle, U32 Off)
{
    FIL file;
    FRESULT res;

    if (!pHandle) return 1;

    res = f_open(&file, (const char *)pHandle, FA_READ);
    if (res != FR_OK) return 1;

    res = f_lseek(&file, Off);
    f_close(&file);

    return (res == FR_OK) ? 0 : 1;
}

/**
 * IP_FS_GetLen — get file size.
 */
U32 IP_FS_GetLen(const void *pHandle)
{
    FIL file;
    FRESULT res;
    U32 size = 0;

    if (!pHandle) return 0;

    res = f_open(&file, (const char *)pHandle, FA_READ);
    if (res != FR_OK) return 0;

    size = (U32)f_size(&file);
    f_close(&file);

    return size;
}
