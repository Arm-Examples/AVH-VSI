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

#ifdef LCD_OUT
#include "hal.h"              // Device HAL, here for LCD access
#endif

#ifdef VSI_OUT
#define VIDEO_OUTPUT_CHANNELS 1
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
__attribute__((aligned(16)))
static uint8_t ImageInBuf[IMAGE_DATA_SIZE];   // Buffer for holding an input frame
__attribute__((section(".ARM.__at_0x60000000")))
__attribute__((aligned(16)))
static uint8_t ImageOutBuf[IMAGE_DATA_SIZE];  // Buffer for holding an output frame

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
  uint32_t imgCount = 0;
  void* imgFrame = NULL;
  void* outFrame = NULL;
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
  if (VideoDrv_SetBuf(VIDEO_DRV_IN0,  ImageInBuf, IMAGE_DATA_SIZE) != VIDEO_DRV_OK) {
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

#ifdef VSI_OUT
  /* Set Output Video file (only when using AVH - default: Display) */
  if (VideoDrv_SetFile(VIDEO_DRV_OUT0, "output_image.mp4") != VIDEO_DRV_OK) {
    log_error("Failed to set filename for video output\n");
    return;
  }
  
  /* Configure Output Video */
  if (VideoDrv_Configure(VIDEO_DRV_OUT0, IMAGE_WIDTH, IMAGE_HEIGHT, VIDEO_DRV_COLOR_RGB888, FRAME_RATE) != VIDEO_DRV_OK) {
    log_error("Failed to configure video output\n");
    return;
  }

  /* Set Output Video buffer */
  if (VideoDrv_SetBuf(VIDEO_DRV_OUT0, ImageOutBuf, IMAGE_DATA_SIZE) != VIDEO_DRV_OK) {
    log_error("Failed to set buffer for video output\n");
    return;
  }

#endif


   /* Loop for obtaining video frames */
   while (1) {

     VideoDrv_Status_t statusIn;
     VideoDrv_Status_t statusOut;

     /* Wait for video input frame */
     do {
       statusIn = VideoDrv_GetStatus(VIDEO_DRV_IN0);
       if (statusIn.overflow != 0U) {
         log_info("Input Overflow");
       }
     } while (statusIn.buf_empty != 0U);

     /* Get input video frame buffer */
     imgFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_IN0);

     if (imgFrame == NULL ) {
       log_error("Invalid frame.");
       break;
     }

     /* Stop video stream upon end of stream status */
//     if (statusIn.eos != 0U) {
//       VideoDrv_StreamStop(VIDEO_DRV_IN0);
//       break;
//     }

      /* Release output frame */
      VideoDrv_ReleaseFrame(VIDEO_DRV_IN0);

#ifdef VSI_OUT
      /* Get output video frame buffer */
      outFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_OUT0);

      if (outFrame == NULL ) {
        log_error("Invalid frame.");
        break;
      }

      /* Copy image frame with detection boxes to output frame buffer */
      memcpy(outFrame, imgFrame, IMAGE_DATA_SIZE);

      /* Release output frame */
      VideoDrv_ReleaseFrame(VIDEO_DRV_OUT0);

      /* Start video output (single frame) */
      VideoDrv_StreamStart(VIDEO_DRV_OUT0, VIDEO_DRV_MODE_CONTINUOS);

      /* Wait for video input frame */
      do {
        statusOut = VideoDrv_GetStatus(VIDEO_DRV_OUT0);
        if (statusOut.overflow != 0U) {
          log_info("Overflow");
        }
      } while (statusOut.buf_full != 0U);

      /* Check for end of stream (when using AVH with file as Video input) */
      VideoDrv_StreamStop(VIDEO_DRV_OUT0);

#endif

#ifdef LCD_OUT
     /* Display image on the LCD. */
     hal_lcd_display_image(
       ImageInBuf,
       IMAGE_HEIGHT,
       IMAGE_WIDTH,
       CHANNELS_IMAGE_DISPLAYED,
       dataPsnImgStartX,
       dataPsnImgStartY,
       dataPsnImgDownscaleFactor);
#endif

      /* Exit the loop when reaching end of stream */
      if (statusIn.eos != 0U) {
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