/**
 *********************************************************************
 * @file    GUI_Demo.c
 * @brief   Multimedia demo using SD card files via FatFS
 *
 * Usage:
 *   Put demo.png, demo.gif, font.ttf on SD card.
 *   Call GUI_Demo_PNG/FIF/TTF from AppWizard slot or anywhere.
 *********************************************************************
 */

#include "GUI_Demo.h"
#include "ff.h"
#include <string.h>

#define DEMO_BUF_SIZE  (128 * 1024)   /* 128KB shared buffer for media loading */
static U8 _demo_buf[DEMO_BUF_SIZE] __attribute__((aligned(32)));

/*===========================================================================
 * Helper: load file from SD into static buffer. Returns size, 0 on error.
 *===========================================================================*/

static U32 _LoadFile(const char *path)
{
    FIL file;
    FRESULT res;
    U32 size;

    res = f_open(&file, path, FA_READ);
    if (res != FR_OK) return 0;

    size = (U32)f_size(&file);
    if (size == 0 || size > DEMO_BUF_SIZE) {
        f_close(&file);
        return 0;
    }

    UINT br;
    res = f_read(&file, _demo_buf, size, &br);
    f_close(&file);

    return (res == FR_OK && br == size) ? size : 0;
}

/*===========================================================================
 * GUI_Demo_JPEG — load and display JPEG from SD card
 *===========================================================================*/

void GUI_Demo_JPEG(const char *path, int x, int y)
{
    U32 size = _LoadFile(path);
    if (size) GUI_JPEG_Draw(_demo_buf, size, x, y);
}

void GUI_Demo_JPEG_Image(const char *path, IMAGE_Handle hImage)
{
    U32 size = _LoadFile(path);
    if (size && hImage) IMAGE_SetJPEG(hImage, _demo_buf, size);
}

/*===========================================================================
 * GUI_Demo_GIF — load and display GIF from SD card
 *===========================================================================*/

void GUI_Demo_GIF(const char *path, int x, int y)
{
    U32 size = _LoadFile(path);
    if (size) GUI_GIF_Draw(_demo_buf, size, x, y);
}

/*===========================================================================
 * GUI_Demo_BMP — load and display BMP from SD card
 *===========================================================================*/

void GUI_Demo_BMP(const char *path, int x, int y)
{
    U32 size = _LoadFile(path);
    if (size) GUI_BMP_Draw(_demo_buf, x, y);
}

/*===========================================================================
 * GUI_Demo_MOVIE — play emWin MOVIE from SD card
 * (Use JPEG2Movie.exe to convert AVI/JPEG sequence to .mv format)
 *===========================================================================*/

void GUI_Demo_MOVIE(const char *path, int x, int y, int w, int h)
{
    GUI_MOVIE_HANDLE hMovie;
    U32 size = _LoadFile(path);
    if (!size) return;

    hMovie = GUI_MOVIE_Create(_demo_buf, size, NULL);
    if (hMovie) {
        GUI_MOVIE_SetPos(hMovie, x, y);
        GUI_MOVIE_Show(hMovie, x, y, 1);  /* 1 = loop */
    }
}
