/**
 *********************************************************************
 * @file    GUI_Demo.h
 * @brief   Multimedia demo: PNG/GIF/TTF loading from SD card
 *********************************************************************
 */

#ifndef __GUI_DEMO_H
#define __GUI_DEMO_H

#include "GUI.h"
#include "IMAGE.h"

void GUI_Demo_JPEG(const char *path, int x, int y);
void GUI_Demo_JPEG_Image(const char *path, IMAGE_Handle hImage);
int  GUI_Demo_ImageFile(const char *path, IMAGE_Handle hImage, int Format);
void GUI_Demo_GIF(const char *path, int x, int y);
void GUI_Demo_BMP(const char *path, int x, int y);
int  GUI_Demo_MOVIE(const char *path, int x, int y, int w, int h);
void GUI_Demo_StopMovie(void);

#define GUI_DEMO_IMAGE_JPEG        0
#define GUI_DEMO_IMAGE_PNG         1
#define GUI_DEMO_IMAGE_GIF         2
#define GUI_DEMO_IMAGE_BMP         3
#define GUI_DEMO_IMAGE_UNSUPPORTED (-1)

#endif /* __GUI_DEMO_H */
