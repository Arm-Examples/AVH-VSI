/* Copyright 2022, 2025 Arm Limited. All Rights Reserved.

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
#ifdef LCD_OUT
#include "hal.h"              // Device HAL, here for LCD access
#endif

/* Video input characteristics */
#define COLOR_BLACK  0
#define IMAGE_WIDTH (192U)
#define IMAGE_HEIGHT (192U)
#define CHANNELS_IMAGE_DISPLAYED (3U)
#define IMAGE_DATA_SIZE (IMAGE_WIDTH*IMAGE_HEIGHT*CHANNELS_IMAGE_DISPLAYED)
#define FRAME_RATE (30U)

#define INPUT_IMAGE "./samples/typing.mp4"  // Input file path

//#define INPUT_IMAGE "./samples/couple.bmp"   // Input file path


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

/*---------------------------------------------------------------------------
 * User application run
 *---------------------------------------------------------------------------*/
void app_run()
{
  void* imgFrame = NULL;

#ifdef LCD_OUT
  /* Video coordinates on LCD */
  uint32_t dataPsnImgDownscaleFactor = 1;
  uint32_t dataPsnImgStartX          = 10;
  uint32_t dataPsnImgStartY          = 35;

  hal_lcd_clear(COLOR_BLACK);
#endif

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
   if (VideoDrv_StreamStart(VIDEO_DRV_IN0, VIDEO_DRV_MODE_CONTINUOS) != VIDEO_DRV_OK) {
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

#ifdef LCD_OUT
     /* Display image on the LCD. */
     hal_lcd_display_image(
       ImageBuf,
       IMAGE_HEIGHT,
       IMAGE_WIDTH,
       CHANNELS_IMAGE_DISPLAYED,
       dataPsnImgStartX,
       dataPsnImgStartY,
       dataPsnImgDownscaleFactor);
#endif

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