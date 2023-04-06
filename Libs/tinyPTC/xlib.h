//
// This file is part of tinyPTC, UA version 2019
// Based on TinyPTC-X11-0.7.3 Raw XLib target
// Copyright (C) 2002 by Alessandro Gatti (a.gatti@tiscali.it)
// Copyright (C) 2019 by Francisco J. Gallego-Durán (@FranGallegoBR)
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#ifndef __PTC_XLIB_H
#define __PTC_XLIB_H

#include "tinyptc.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __PTC_XLIB__

#ifdef __TINYPTC_INCLUDED
#error You chose more than one target. Please make your mind and try again.
#endif /* __TINYPTC_INCLUDED */

#define __TINYPTC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ptc_set_on_keypress  ( void (*onkeypress)  (KeySym) );
void ptc_set_on_keyrelease( void (*onkeyrelease)(KeySym) );
int ptc_open(const char *title, int width, int height);
int ptc_update(void *buffer);
void ptc_close(void);
int ptc_process_events(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PTC_XLIB__ */

#endif /* __PTC_XLIB_H */

#ifdef __PTC_FROM_SOURCE

static void (*ptc_onkeypress)  (KeySym);
static void (*ptc_onkeyrelease)(KeySym);
static Display *ptc_display;
static int ptc_screen;
static int ptc_screen_width;
static int ptc_screen_height;
static int ptc_viewport_width;
static int ptc_viewport_height;
static int ptc_depth;
static Window ptc_root_window;
static Visual *ptc_visual;
static char *ptc_buffer;
static int ptc_pixmap_format_count;
static int ptc_pixmap_counter;
static int ptc_converter_depth;
static XPixmapFormatValues *ptc_pixmap_formats;

static Window ptc_window;
static int ptc_x_position;
static int ptc_y_position;
static XSetWindowAttributes ptc_window_attributes;
static XSizeHints ptc_window_sizehints;
static GC ptc_window_gc;
static XImage *ptc_ximage;

#ifdef __PTC_ENABLE_CONVERSIONS__
static PTC_CONVERTER ptc_convert;
static int ptc_output_pitch;
static int ptc_blitcounter;
static int ptc_source_index;
static int ptc_destination_index;
#endif /* __PTC_ENABLE_CONVERSIONS__ */

#endif /* __PTC_FROM_SOURCE */
