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

#include "gui.h"

#include "platform.h"
#include "arm_2d_helper.h"
#include "example_gui.h"

#include "draw_line.h"

#include <stdint.h>

/*============================ MACROS ========================================*/
#ifndef __STR
#   define __STR(__A)      #__A
#endif

#ifndef STR
#   define STR(__A)         __STR(__A)
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__OVERRIDE_WEAK
arm_2d_runtime_feature_t ARM_2D_RUNTIME_FEATURE = {
    .TREAT_OUT_OF_RANGE_AS_COMPLETE         = 1,
    .HAS_DEDICATED_THREAD_FOR_2D_TASK       = 1,
};

/*============================ PROTOTYPES ====================================*/
__attribute__((nothrow)) 
extern int64_t clock(void);

/*============================ LOCAL VARIABLES ===============================*/

static char s_chPerformanceInfo[MAX(((GLCD_WIDTH/6)+1), 54)] = {0};

extern osEventFlagsId_t s_evt2DTaskAvailable;
extern osEventFlagsId_t s_evt2DResourceAvailable;

static ARM_NOINIT arm_2d_helper_pfb_t s_tExamplePFB;

/*============================ IMPLEMENTATION ================================*/
void display_task(void) 
{  
    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static const)
        
        /* a region for the busy wheel */
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {(APP_SCREEN_WIDTH - 100) / 2,
                          (APP_SCREEN_HEIGHT - 100) / 2},
            .tSize = {
                .iWidth = 100,
                .iHeight = 100,  
            },
        ),
        
        /* a region for the status bar on the bottom of the screen */
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {0,APP_SCREEN_HEIGHT - 8*2},
            .tSize = {
                .iWidth = APP_SCREEN_WIDTH,
                .iHeight = 8*2,  
            },
        ),

    END_IMPL_ARM_2D_REGION_LIST()
            

/*! define the partial-flushing area */

    example_gui_do_events();

    //! call partial framebuffer helper service
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task( 
                                &s_tExamplePFB, NULL));
                                //(arm_2d_region_list_item_t *)s_tDirtyRegions));
    
    //! update performance info
    do {
        
        int32_t nTotalCyclCount = s_tExamplePFB.Statistics.nTotalCycle;
        int32_t nTotalLCDCycCount = s_tExamplePFB.Statistics.nRenderingCycle;
        int64_t lTemp = clock();
        static int64_t s_lLast = 0;
        int32_t nElapsed = lTemp - s_lLast;
        s_lLast = lTemp;

        snprintf(s_chPerformanceInfo, 
                 sizeof(s_chPerformanceInfo),
                "UPS %d\tCPU Usage %2.1f%% (LCD Latency %2dms)", 
                (int32_t)SystemCoreClock / nElapsed, 
                (float)nTotalCyclCount / (float)nElapsed * 100.0f,
                nTotalLCDCycCount / ((int32_t)SystemCoreClock / 1000));

    } while(0);

}

extern DATA_TYPE* sensor_samples;
void int_array_to_string(size_t num, DATA_TYPE* src, char* dst)
{
    int num_chars = 0;
    
    for(int x = 0; x < num; x++)
    {
        uint32_t val = (uint32_t)src[x];
        sprintf(&dst[num_chars], "%d ", val);
        num_chars += 2;

        while(val > 10)
        {
            num_chars += 1;
            val /= 10;
        }
    }
}

__OVERRIDE_WEAK 
void example_gui_on_refresh_evt_handler(const arm_2d_tile_t *ptFrameBuffer)
{
    ARM_2D_UNUSED(ptFrameBuffer);

#if !defined(__USE_FVP__)
    //! print performance info
    arm_lcd_text_location( GLCD_HEIGHT / 8 - 2, 0);
    
    arm_lcd_printf( "Screen: " STR(APP_SCREEN_WIDTH) "*"
                STR(APP_SCREEN_HEIGHT) 
                " PFB: " STR(PFB_BLOCK_WIDTH) "*"
                STR(PFB_BLOCK_HEIGHT)
                " System Freq: %dMHz\r\n",
                (int32_t)SystemCoreClock / 1000000);       
    arm_lcd_puts(s_chPerformanceInfo);

    // print data samples on the top of the screen
    char string_buf[265];
    int_array_to_string(DATA_NUM_ELEMENTS, sensor_samples, string_buf);
    
    arm_lcd_text_location(1, 1);
    arm_lcd_printf(string_buf);

    // draw box for graph
    arm_2d_region_t r = {{50,40}, {200,170}};
    arm_2d_draw_box(ptFrameBuffer, &r, 1, GLCD_COLOR_MAROON, 255);

    // Draw data as graph
    GLCD_SetForegroundColor(GLCD_COLOR_MAGENTA);

    arm_2d_location_t start = { r.tLocation.iX, r.tLocation.iY + r.tSize.iHeight };
    arm_2d_location_t end;

    float height_step = r.tSize.iHeight / 255.f;
    for(int i = 0; i < DATA_NUM_ELEMENTS; i++)
    {
        end.iX = start.iX + (r.tSize.iWidth / DATA_NUM_ELEMENTS);
        end.iY = (r.tLocation.iY + r.tSize.iHeight) - (int)(height_step * sensor_samples[i]);

        draw_line_between_points(start, end);

        start = end;
    }

#endif
}

ARM_NOINIT
static task_cycle_info_t s_tArm2DTaskInfo;
ARM_NOINIT
static task_cycle_info_agent_t s_tArm2DTaskInfoAgent;

__OVERRIDE_WEAK 
void arm_2d_helper_perf_counter_start(void)
{
    start_task_cycle_counter(&s_tArm2DTaskInfo);
}

__OVERRIDE_WEAK 
int32_t arm_2d_helper_perf_counter_stop(void)
{
    return stop_task_cycle_counter(&s_tArm2DTaskInfo);
}


static 
IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    
    example_gui_refresh(ptTile, bIsNewFrame);
        
    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_NAVY);
    arm_2d_op_wait_async(NULL);
    return arm_fsm_rt_cpl;
}

static 
IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);
    ARM_2D_UNUSED(bIsNewFrame);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    ptTile->pchBuffer);
                    
    arm_2d_helper_pfb_report_rendering_complete(&s_tExamplePFB, 
                                                (arm_2d_pfb_t *)ptPFB);
}


/*----------------------------------------------------------------------------*
 * RTOS Port                                                                  *
 *----------------------------------------------------------------------------*/

static volatile bool s_bWaitForOPCPL = false;

__OVERRIDE_WEAK
void arm_2d_notif_aync_op_cpl(uintptr_t pUserParam)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pUserParam;
    assert (NULL != evtFlag) ;
    osEventFlagsSet(evtFlag, 0x01); 
}

__OVERRIDE_WEAK
bool arm_2d_port_wait_for_async(uintptr_t pUserParam)
{
    osEventFlagsId_t evtFlag = (osEventFlagsId_t)pUserParam;
    assert (NULL != evtFlag) ;

    osEventFlagsWait(evtFlag, 0x01, osFlagsWaitAny, osWaitForever );
    return true;
}


__OVERRIDE_WEAK
void arm_2d_notif_new_op_arrive(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    
    assert (NULL != s_evt2DTaskAvailable) ;
    
    osEventFlagsSet(s_evt2DTaskAvailable, 0x01); 
}


__OVERRIDE_WEAK 
void arm_2d_notif_aync_sub_task_cpl(uintptr_t pUserParam)
{
    ARM_2D_UNUSED(pUserParam);
    assert (NULL != s_evt2DResourceAvailable) ;
    
    osEventFlagsSet(s_evt2DResourceAvailable, 0x01); 
}


static arm_2d_task_t tTaskCB = {0};

void setup_gui(void* argument)
{    
    init_task_cycle_counter();
    register_task_cycle_agent(&s_tArm2DTaskInfo, &s_tArm2DTaskInfoAgent);

    ARM_2D_UNUSED(argument);
    //! draw background first
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tExamplePFB,NULL));
    
    //! update draw function
    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   &s_tExamplePFB, 
                                                &__pfb_draw_handler);


}

void loop_gui(void)
{
    //! retrieve the number of system ticks
    uint32_t wTick = osKernelGetTickCount();        
    display_task();
    
    //! lock frame rate
    osDelayUntil(wTick + (1000 / APP_TARGET_FPS));
}

void init_gui()
{
    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT( 
        &s_tExamplePFB,                 //!< FPB Helper object
        APP_SCREEN_WIDTH,               //!< screen width
        APP_SCREEN_HEIGHT,              //!< screen height
        uint16_t,                       //!< colour date type
        PFB_BLOCK_WIDTH,                //!< PFB block width
        PFB_BLOCK_HEIGHT,               //!< PFB block height
        1,                              //!< number of PFB in the PFB pool
        {
            .evtOnLowLevelRendering = {
                //! callback for low level rendering 
                .fnHandler = &__pfb_render_handler,                         
            },
            .evtOnDrawing = {
                //! callback for drawing GUI 
                .fnHandler = &__pfb_draw_background_handler, 
            },
        }
    ) < 0) {
        //! error detected
        assert(false);
    }
    
    init_task_cycle_info(&s_tArm2DTaskInfo);
}