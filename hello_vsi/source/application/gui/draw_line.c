/*---------------------------------------------------------------------------
 * Copyright (c) 2022 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include <stdlib.h> 

#include "draw_line.h"
#include "platform.h"

void draw_line_between_points(arm_2d_location_t start_point, arm_2d_location_t end_point)
{

  int   dx = abs(end_point.iX - start_point.iX), 
        sx = start_point.iX < end_point.iX ? 1 : -1;
  int   dy = abs(end_point.iY - start_point.iY), 
        sy = start_point.iY < end_point.iY ? 1 : -1; 
  int   err = (dx > dy ? dx : -dy) / 2, 
        e2 = 0;
  int   xPos = start_point.iX,
        yPos = start_point.iY;

  for(;;){

    GLCD_DrawPixel(xPos,yPos);

    if (xPos == end_point.iX && yPos == end_point.iY) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; xPos += sx; }
    if (e2 < dy) { err += dx; yPos += sy; }
  }
}