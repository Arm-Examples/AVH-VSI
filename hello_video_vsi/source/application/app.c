/* Copyright 2022 Arm Limited. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#ifdef _RTE_
#include "RTE_Components.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#else
#define EventStartCv(slot, v1, v2)
#define EventStopCv(slot, v1, v2)
#endif
#endif

#include CMSIS_device_header  // Device-specific defines and CMSIS-Core
#include "cmsis_os2.h"        // CMSIS-RTOS2 API
#include "micro_logger.h"     // Application logging engine to UART
#include "video_drv.h"        // Video Driver API
#include "device_definition.h"
#include "device_cfg.h"
#include "hdlcd_drv.h"
#include "hdlcd_helper.h"
//#include "hal.h"              // Device HAL, here for LCD access

/* Video input characteristics */
#define COLOR_BLACK  0
#define IMAGE_WIDTH (192U)
#define IMAGE_HEIGHT (192U)
#define CHANNELS_IMAGE_DISPLAYED (3U)
#define IMAGE_DATA_SIZE (IMAGE_WIDTH*IMAGE_HEIGHT*CHANNELS_IMAGE_DISPLAYED)
#define FRAME_RATE (30U)

//#define INPUT_IMAGE "./samples/typing.mp4"  // Input file path
#define INPUT_IMAGE "./samples/couple.bmp"   // Input file path

__attribute__((section(".ARM.__at_0x60000000")))
 __attribute__((aligned(4)))
static uint8_t ImageBuf[IMAGE_DATA_SIZE];   // Buffer for holding an input frame


#define HRES                    192
#define VRES                    192
#define BYTESPERPIXEL           3
#define FRAME_BUFFER_1_BASE     0x60000000


#define TEST_CYCLE_COUNT_8  8
#define TEST_CYCLE_COUNT_12 12
#define TEST_CYCLE_COUNT_16 16


/*---------------------------------------------------------------------------
 * User application initialization
 *---------------------------------------------------------------------------*/
void app_init()
{
  /* Initializing video driver */
  if (VideoDrv_Initialize(NULL) != VIDEO_DRV_OK) {
    log_error("Failed to initialise video driver\n");
  }
}

volatile static uint8_t flag = 0;
volatile static uint8_t irq_counter = 0;
volatile static uint8_t counter = 0;

void HDLCD_Handler(void)
{
    /* Clear IRQ */
    hdlcd_clear_irq(&HDLCD_DEV, INT_DMA_END_Msk);
    __NVIC_ClearPendingIRQ(HDLCD_IRQn);
    if (counter < 0x40) {
        counter++;
    } else {
        counter = 0;
        flag = 1;
    }
}

void enable_hdlcd_irq(void)
{
    NVIC_ClearPendingIRQ(HDLCD_IRQn);
    NVIC_SetVector(HDLCD_IRQn, (uint32_t)HDLCD_Handler);
    NVIC_EnableIRQ(HDLCD_IRQn);
    hdlcd_enable_irq(&HDLCD_DEV, INT_DMA_END_Msk);
}

void disable_hdlcd_irq(void)
{
    NVIC_DisableIRQ(HDLCD_IRQn);
    hdlcd_disable_irq(&HDLCD_DEV, INT_DMA_END_Msk);
}

enum hdlcd_error_t hdlcd_err;

/*---------------------------------------------------------------------------
 * User application run
 *---------------------------------------------------------------------------*/
void app_run()
{
  void* imgFrame = NULL;

  /* Video coordinates on LCD */
  uint32_t dataPsnImgDownscaleFactor = 1;
  uint32_t dataPsnImgStartX          = 10;
  uint32_t dataPsnImgStartY          = 35;

  //hal_lcd_clear(COLOR_BLACK);



     log_info("HDLCD init");
     hdlcd_err = hdlcd_init( &HDLCD_DEV );
     if( hdlcd_err != HDLCD_ERR_NONE )
     {
         log_error( "HDLCD init error!");
         return;
     }

     log_info("hdlcd_static_config");
     hdlcd_err = hdlcd_static_config( &HDLCD_DEV );
     if( hdlcd_err != HDLCD_ERR_NONE )
     {
         log_error( "HDLCD static config error!");
         return;
     }

     struct hdlcd_resolution_cfg_t custom_res = {.h_data = 192, .h_front_porch = 0, .h_sync = 0, .h_back_porch = 0,
                       .v_data = 192, .v_front_porch = 0, .v_sync = 0, .v_back_porch = 0};


     log_info("hdlcd_set_resolution");
     hdlcd_err = hdlcd_set_custom_resolution(&HDLCD_DEV, &custom_res);
     if (hdlcd_err != HDLCD_ERR_NONE) {
         log_error("HDLCD resolution error!");
         return;
     }


     struct hdlcd_buffer_cfg_t hdlcd_buff32 = {
         .base_address = FRAME_BUFFER_1_BASE,
         .line_length = (HRES) * HDLCD_MODES[HDLCD_PIXEL_FORMAT_RGB32].bytes_per_pixel,
         .line_count = VRES-1,
         .line_pitch = HRES * HDLCD_MODES[HDLCD_PIXEL_FORMAT_RGB32].bytes_per_pixel,
         .pixel_format = HDLCD_MODES[HDLCD_PIXEL_FORMAT_RGB32].pixel_format};


     hdlcd_err = hdlcd_buffer_config(&HDLCD_DEV, &hdlcd_buff32);
     if (hdlcd_err != HDLCD_ERR_NONE) {
         log_error("HDLCD buffer config error!");
         return;
     }

    hdlcd_err = hdlcd_pixel_config(&HDLCD_DEV, HDLCD_MODES[HDLCD_PIXEL_FORMAT_RGB32].pixel_cfg);
    if (hdlcd_err != HDLCD_ERR_NONE) {
         log_error("HDLCD buffer config error!");
         return;
    }

     log_info("hdlcd_enable");
     hdlcd_err = hdlcd_enable(&HDLCD_DEV);
     if (hdlcd_err != HDLCD_ERR_NONE) {
         log_error("HDLCD error!");
         return;
     }



  /* Configure video driver for input */
  if (VideoDrv_Configure(VIDEO_DRV_IN0,  IMAGE_WIDTH, IMAGE_HEIGHT, VIDEO_DRV_COLOR_RGB888, FRAME_RATE) != VIDEO_DRV_OK) {
    log_error("Failed to configure video input");
    return;
  }

   /* Set input video buffer */
   if (VideoDrv_SetBuf(VIDEO_DRV_IN0,  ImageBuf, IMAGE_DATA_SIZE) != VIDEO_DRV_OK) {
     log_error("Failed to set buffer for video input");
     return;
   }

   /* Set input file */
   if (VideoDrv_SetFile(VIDEO_DRV_IN0, INPUT_IMAGE) != VIDEO_DRV_OK) {
     log_error("Failed to set filename for video input");
     return;
   }

   /* Start video capture */
   if (VideoDrv_StreamStart(VIDEO_DRV_IN0, VIDEO_DRV_MODE_SINGLE) != VIDEO_DRV_OK) {
     log_error("Failed to start frame capture");
     return;
   }

   /* Loop for obtaining video frames */
   while (1) {

     VideoDrv_Status_t status;

     /* Wait for video input frame */
     do {
       status = VideoDrv_GetStatus(VIDEO_DRV_IN0);
       if (status.overflow != 0U) {
         log_info("Overflow");
       }
     } while (status.buf_empty != 0U);

     /* Stop video stream upon end of stream status */
     if (status.eos != 0U) {
       VideoDrv_StreamStop(VIDEO_DRV_IN0);
       break;
     }

     /* Get input video frame buffer */
     imgFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_IN0);

     if (imgFrame == NULL ) {
       log_error("Invalid frame.");
       break;
     }

     enable_hdlcd_irq();



     hdlcd_err = hdlcd_set_frame_buffer_base_address(&HDLCD_DEV, (uint32_t)imgFrame);


     // while (irq_counter != TEST_CYCLE_COUNT_16) {
         // if (flag) {
             // flag = 0;
             // irq_counter++;
         // }
     // }

    disable_hdlcd_irq();

    // /* Display image on the LCD. */
//   hal_lcd_display_image(
//     ImageBuf,
//     IMAGE_HEIGHT,
//     IMAGE_WIDTH,
//     CHANNELS_IMAGE_DISPLAYED,
//     dataPsnImgStartX,
//     dataPsnImgStartY,
//    dataPsnImgDownscaleFactor);

    /* Release input frame */
    VideoDrv_ReleaseFrame(VIDEO_DRV_IN0);

    /* Exit the loop when reaching end of stream */
    if (status.eos != 0U) {
      VideoDrv_StreamStop(VIDEO_DRV_IN0);
      break;
    }
  }

  log_info("Video Stream stopped");

  /* De-initialize video interface */

  VideoDrv_Uninitialize();

}

/*---------------------------------------------------------------------------
 * User application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main(void *argument)
{
  (void)argument;

  app_init(); // Initialization step
  app_run();  // Application run step

  log_info("Application run ended");

  for (;;){;}
}