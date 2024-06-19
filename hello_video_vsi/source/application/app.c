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

#include <stdlib.h>

#include "app.h"
#include "app_cfg.h"
#include "micro_logger.h" /* Logging engine to UART */
#include "video_drv.h"    /* Video Driver API */

#include "hal.h"

/* Video input characteristics */
#define COLOR_BLACK  0
#define IMAGE_WIDTH (192U)
#define IMAGE_HEIGHT (192U)
#define IMAGE_DATA_SIZE (150528U)
#define CHANNELS_IMAGE_DISPLAYED (3U)

/* Input file */
#define INPUT_IMAGE "./samples/couple.bmp"

/* Buffer for holding an input frame */
static uint8_t ImageBuf[IMAGE_DATA_SIZE];

void init()
{
  if (VideoDrv_Initialize(NULL) != VIDEO_DRV_OK) {
    log_error("Failed to initialise video driver\n");
  }
}

void run()
{
  void* imgFrame = NULL;

  /* Video coordinates on LCD */
  uint32_t dataPsnImgDownscaleFactor = 1;
  uint32_t dataPsnImgStartX          = 10;
  uint32_t dataPsnImgStartY          = 35;
  uint32_t dataPsnTxtInfStartX       = 20;
  uint32_t dataPsnTxtInfStartY       = 28;

  hal_lcd_clear(COLOR_BLACK);

  /* Configure Video Driver for Input */
  if (VideoDrv_Configure(VIDEO_DRV_IN0,  IMAGE_WIDTH, IMAGE_HEIGHT, VIDEO_DRV_COLOR_RGB888, 30U) != VIDEO_DRV_OK) {
    log_error("Failed to configure video input\n");
    return;
  }

  /* Set Input Video buffer */
  if (VideoDrv_SetBuf(VIDEO_DRV_IN0,  ImageBuf, IMAGE_DATA_SIZE) != VIDEO_DRV_OK) {
    log_error("Failed to set buffer for video input\n");
    return;
  }

  /* Set input file */
  if (VideoDrv_SetFile(VIDEO_DRV_IN0, INPUT_IMAGE) != VIDEO_DRV_OK) {
    log_error("Failed to set filename for video input\n");
    return;
  }

  /* Start video capture (single frame) */
  if (VideoDrv_StreamStart(VIDEO_DRV_IN0, VIDEO_DRV_MODE_CONTINUOS) != VIDEO_DRV_OK) {
    log_error("Failed to start frame capture");
    return;
  }

  /* loop for obtaining video frames */
  while (1) {

    VideoDrv_Status_t status;

    /* Wait for video input frame */
    do {
      status = VideoDrv_GetStatus(VIDEO_DRV_IN0);
      if (status.overflow != 0U) {
        log_info("Overflow\n");
      }
    } while (status.buf_empty != 0U);

    if (status.eos != 0U) {
      /* Stop Video Stream */
      VideoDrv_StreamStop(VIDEO_DRV_IN0);
      // log_info("Video Stream stopped");
    }

    /* Get input video frame buffer */
    imgFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_IN0);

    if (imgFrame == NULL ) {
      log_error("Invalid frame.\n");
      break;
    }

    /* Display image on the LCD. */
    hal_lcd_display_image(
      ImageBuf,
      IMAGE_HEIGHT,
      IMAGE_WIDTH,
      CHANNELS_IMAGE_DISPLAYED,
      dataPsnImgStartX,
      dataPsnImgStartY,
      dataPsnImgDownscaleFactor);

    /* Release input frame */
    VideoDrv_ReleaseFrame(VIDEO_DRV_IN0);

      /* Exit the loop when reaching end of stream */
    if (status.eos) {
      break;
    }
  }

  log_info("Video Stream stopped");


  /* De-initialize Video Interface */
  VideoDrv_Uninitialize();

  return;
}
