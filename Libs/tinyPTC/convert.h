//
// This file is part of tinyPTC, UA version 2019
// Based on TinyPTC-X11-0.7.3 Pixelformat converters
// Copyright (C) 2000-2002 Alessandro Gatti <a.gatti@tiscali.it>
// Copyright (C) 2000-2001 Glenn Fiedler <gaffer@gaffer.org>
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

#ifndef __TINYPTC_CONVERT_H
#define __TINYPTC_CONVERT_H

/* include files */
#include "tinyptc.h"

#define __PTC_CONVERTER_32_TO_32_RGB888
#define __PTC_CONVERTER_32_TO_32_BGR888
#define __PTC_CONVERTER_32_TO_24_RGB888
#define __PTC_CONVERTER_32_TO_24_BGR888
#define __PTC_CONVERTER_32_TO_16_RGB565
#define __PTC_CONVERTER_32_TO_16_BGR565
#define __PTC_CONVERTER_32_TO_16_RGB555
#define __PTC_CONVERTER_32_TO_16_BGR555

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* converter function type */
typedef void (*PTC_CONVERTER)(void *src, void *dst, int pixels);

/* converter request */
PTC_CONVERTER ptc_request_converter(int bits, int32 r, int32 g, int32 b);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TINYPTC_CONVERT_H */
