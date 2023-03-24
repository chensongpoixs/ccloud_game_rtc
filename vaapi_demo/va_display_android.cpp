/*
 * Copyright (c) 2018 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <va/va_android.h>
#include "va_display.h"

static unsigned int fake_display = 0xdeada01d;

static VADisplay
va_open_display_android(void)
{
    return vaGetDisplay(&fake_display);
}

static void
va_close_display_android(VADisplay va_dpy)
{
}

static VAStatus
va_put_surface_android(
    VADisplay          va_dpy,
    VASurfaceID        surface,
    const VARectangle *src_rect,
    const VARectangle *dst_rect
)
{
    return VA_STATUS_ERROR_UNIMPLEMENTED;
}

extern "C"
const VADisplayHooks va_display_hooks_android = {
    "android",
    va_open_display_android,
    va_close_display_android,
    va_put_surface_android
};
