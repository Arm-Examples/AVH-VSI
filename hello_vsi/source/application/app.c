/* Copyright 2022-2024 Arm Limited. All Rights Reserved.

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
#include <stdint.h>

#include CMSIS_device_header    // Device-specific defines and CMSIS-Core
#include "cmsis_os2.h"          // CMSIS-RTOS2 API
#include "micro_logger.h"       // Application logging engine to UART
#include "sensor_drv.h"         // Sensor Driver API

#define DATA_NUM_TYPE uint8_t   // Data type of a numeric element in sensor reading (sample)
#define DATA_SAMPLE_RATE  (20)  // Amount of samples per second
#define DATA_NUM_ELEMENTS (20)  // Amount of samples in a DMA block

#define SENSOR_BLOCK_NUM (1)    // Amount of DMA blocks in DMA buffer,(must be 2^n)
#define SENSOR_BLOCK_SIZE (DATA_NUM_ELEMENTS * sizeof(DATA_NUM_TYPE)) // Size of DMA block in bytes, ( must be multiple of 4)

#define SENSOR_BUFFER_SIZE (SENSOR_BLOCK_NUM * SENSOR_BLOCK_SIZE)     // Size of DMA buffer

osThreadId_t app_main_tid;

__attribute__((aligned(4)))
DATA_NUM_TYPE sensor_data[SENSOR_BLOCK_SIZE];

__attribute__((aligned(4)))
DATA_NUM_TYPE sensor_dma_buffer[SENSOR_BUFFER_SIZE];

extern void int_array_to_string(size_t num, DATA_NUM_TYPE* src, char* dst);

void sensor_event(uint32_t event);

uint8_t   is_sensor_ready  = 0;
uint32_t  sensor_data_size = 0;




/*---------------------------------------------------------------------------
 * User application initialization
 *---------------------------------------------------------------------------*/
void app_init()
{
  // sensor_data = (DATA_NUM_TYPE*)malloc(DATA_NUM_ELEMENTS * sizeof(DATA_NUM_TYPE));  // Allocating memory for storing sensor data

  /* Initializing sensor driver with callback */
  if (SensorDrv_Initialize(sensor_event)) {
    log_error("Failed to initialise sensor driver");
  }
}


/*---------------------------------------------------------------------------
 * User application run
 *---------------------------------------------------------------------------*/
void app_run()
{
  /* Configure Video Driver for Input */
  if (SensorDrv_Configure(SENSOR_DRV_INTERFACE_RX,
                          1U,                        // single channel
                          sizeof(DATA_NUM_TYPE) * 8, // sample bits
                          DATA_SAMPLE_RATE) ) {      // sample rate
    log_error("Failed to configure sensor input");
    return;
  }

  /* Set sensor buffer */
  if (SensorDrv_SetBuf(SENSOR_DRV_INTERFACE_RX, sensor_dma_buffer, SENSOR_BLOCK_NUM, SENSOR_BLOCK_SIZE)){
    log_error("Failed to configure sensor input");
    return;
  }

  /* Enable Sensor operation in receive mode */
  if (SensorDrv_Control(SENSOR_DRV_CONTROL_RX_ENABLE)) {
    log_error("Failed to configure sensor input");
    return;
  }

#ifdef __GATED_FETCH
  /* Puase sensor rx operation */
  if (SensorDrv_Control(SENSOR_DRV_CONTROL_RX_PAUSE)) {
    log_error("Failed to configure sensor input");
    return;
  }
  
  uint32_t rx_count = 0;
#endif

  is_sensor_ready = 1;
  sensor_data_size = SENSOR_BLOCK_SIZE;

  /* Loop for obtaining samples */
  while (1) {

#ifdef __EVENT_DRIVEN
    /* waiting for the flag indicating data reception event;
       on timeout/error exit from the loop */
    uint32_t timeout = 2*((DATA_NUM_ELEMENTS*1000)/DATA_SAMPLE_RATE); //double the time expected for sensor reading in ms
    if (0 > (int)osThreadFlagsWait(0x1U, osFlagsWaitAny, timeout)) {break;}
#endif

#ifdef __GATED_FETCH
    osDelay(10000U);                                                  // delay between fetching sensor data

    SensorDrv_Control(SENSOR_DRV_CONTROL_RX_RESUME);                  // resume sensor rx operation
    rx_count = SensorDrv_GetRxCount();

    /* wait until number of received blocks is incremented */
    while ((SensorDrv_GetRxCount() <= rx_count)){
      osDelay(1U);
    }

    SensorDrv_Control(SENSOR_DRV_CONTROL_RX_PAUSE);                   // pause sensor rx operation

    memcpy(sensor_data, sensor_dma_buffer, sensor_data_size);         // copy from sensor DMA buffer

    if(SensorDrv_GetStatus().rx_active == 0U) {break;}                // exit if sensor rx operation is disabled (end of data)
#endif

    /* Print out received sensor samples */
    char printing_text[265];
    int_array_to_string(DATA_NUM_ELEMENTS, sensor_data, printing_text);
    log_info("Received data: %s", printing_text);
  }

  log_info("Sensor Stream stopped");
  is_sensor_ready = 0;
  SensorDrv_Uninitialize();

  return;
}

/*---------------------------------------------------------------------------
 * Utility function for converting array to string 
 *---------------------------------------------------------------------------*/
void int_array_to_string(size_t num, DATA_NUM_TYPE* src, char* dst)
{
  int num_chars = 0;

  for(int x = 0; x < num; x++)
  {
    uint32_t val = (uint32_t)src[x];
    sprintf(&dst[num_chars], "%d ", val);
    num_chars += 2;

    while(val >= 10)
    {
       num_chars += 1;
       val /= 10;
    }
  }
}

/*---------------------------------------------------------------------------
 * Sensor event callback
 *---------------------------------------------------------------------------*/
void sensor_event(uint32_t event)
{
#ifdef __EVENT_DRIVEN
  if (is_sensor_ready)
  {
    /* Received sensor data */
    if (event & SENSOR_DRV_EVENT_RX_DATA)
    {
      memcpy(sensor_data, sensor_dma_buffer, sensor_data_size);  // copy from sensor DMA buffer
      osThreadFlagsSet(app_main_tid, 0x1U);                      // issue thread flag to process data
    }
  }
#endif
}

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument)
{
  (void)argument;

  app_init(); // Initialization step
  app_run();  // Application run step

  log_info("Application run ended");

  for (;;){;}
}

/*-----------------------------------------------------------------------------
 * Application initialization
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();                         // Initialize CMSIS-RTOS2
  app_main_tid = osThreadNew(app_main_thread, NULL, NULL);
  osKernelStart();                              // Start thread execution
  return 0;
}
