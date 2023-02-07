/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2021 Arm Limited (or its affiliates). All rights reserved.
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

#include "main.h"
#include "main_functions.h"


#ifdef __USE_GUI
#include "gui.h"
#include "arm_2d_helper.h"

osEventFlagsId_t s_evt2DTaskAvailable = NULL;
osEventFlagsId_t s_evt2DResourceAvailable = NULL;

const osThreadAttr_t gui_main_attr = {
    .stack_size = 4096U };

osThreadId_t gui_main_tid;

#endif

static const osThreadAttr_t app_main_attr = {
    .stack_size = 4096U };

osThreadId_t app_main_tid;

#ifdef __EVENT_DRIVEN
void sensor_rx_event(void)
{
    osThreadFlagsSet(app_main_tid, 0x00000001U);
}
#endif

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN
void app_main(void *argument)
{
    (void)argument;

    setup();

    for (;;)
    {
#ifdef __EVENT_DRIVEN
        osThreadFlagsWait(0x00000001U, osFlagsWaitAny, osWaitForever);
#endif
        loop();
        
#ifdef __USE_GUI
        osThreadFlagsSet(gui_main_tid, 0x00000001U);
#endif
    }
}

/*---------------------------------------------------------------------------
 * Gui thread
 *---------------------------------------------------------------------------*/
#ifdef __USE_GUI
__NO_RETURN
void gui_main(void *argument)
{
    (void)argument;

    setup_gui(argument);

    for (;;)
    {
        loop_gui();
    }
}
#endif

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
void app_initialize(void)
{
    app_main_tid = osThreadNew(app_main, NULL, &app_main_attr);
    osThreadFlagsSet(app_main_tid, 0x00000001U); // Set app main as active thread

#ifdef __USE_GUI
    gui_main_tid = osThreadNew(gui_main, NULL, &gui_main_attr);
#endif

}