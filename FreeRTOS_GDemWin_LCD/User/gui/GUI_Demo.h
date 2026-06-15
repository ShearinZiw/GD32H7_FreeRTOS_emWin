/**
 *********************************************************************
 * @file    GUI_Demo.h
 * @brief   Multimedia demo: PNG/GIF/TTF loading from SD card
 *********************************************************************
 */

#ifndef __GUI_DEMO_H
#define __GUI_DEMO_H

#include "GUI.h"

void GUI_Demo_JPEG(const char *path, int x, int y);
void GUI_Demo_GIF(const char *path, int x, int y);
void GUI_Demo_BMP(const char *path, int x, int y);
void GUI_Demo_MOVIE(const char *path, int x, int y, int w, int h);

#endif /* __GUI_DEMO_H */
