/*
 * Copyright (c) 2023 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HDLCD_HELPER_H__
#define __HDLCD_HELPER_H__

#include "hdlcd_drv.h"
#include <stdint.h>

#define RGB565_YELLOW      0xFFE0U
#define RGB565_RED         0xF800U
#define RGB32_YELLOW       0xFFFF00U
#define RGB32_RED          0xFF0000U
#define A2R10G10B10_YELLOW 0x3FFFFC00U
#define A2R10G10B10_RED    0x3FF00000U

typedef struct hdlcd_mode {
    uint32_t pixel_format;
    uint32_t bytes_per_pixel;
    uint32_t default_highlight_color;
    uint32_t default_mask_color;
    const struct hdlcd_pixel_cfg_t *const pixel_cfg;
} hdlcd_mode_t;

enum hdlcd_pixel_format {
    HDLCD_PIXEL_FORMAT_RGB565 = 0,
    HDLCD_PIXEL_FORMAT_RGB32 = 1,
    HDLCD_PIXEL_FORMAT_A2R10G10B10 = 2,
    HDLCD_PIXEL_FORMAT_NOT_SUPPORTED
};

extern const hdlcd_mode_t HDLCD_MODES[HDLCD_PIXEL_FORMAT_NOT_SUPPORTED];

#endif /* __HDLCD_HELPER_H__ */
