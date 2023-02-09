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
#include "micro_logger.h"
#include "data_sensor_provider.h"

extern void int_array_to_string(size_t num, DATA_TYPE* src, char* dst);

DATA_TYPE* sensor_data;

uint32_t previous_data_pos;

void init()
{
    previous_data_pos = 0;
    sensor_data = (DATA_TYPE*)malloc(DATA_NUM_ELEMENTS * sizeof(DATA_TYPE));
}

void run()
{
    uint32_t current_data_pos = get_total_fetched_sensor_data();
    log_debug("current_data_pos: %d", current_data_pos);

    EventStartCv(0, current_data_pos, previous_data_pos);

    uint32_t sensor_data_size = 0;

    // Fetch the data from the sensor.
    uint32_t sensor_status = get_sensor_data(DATA_NUM_ELEMENTS, &sensor_data_size, &sensor_data);
    log_debug("sensor_status: %d", sensor_status);
    log_debug("sensor_data_size: %d", sensor_data_size);

    EventStopCv(0, sensor_status, sensor_data_size);

    if (sensor_status != 0)
    {
        log_error("Something wrong with the sensor. Sensor status: %d", sensor_status);
        return;
    }
    
    if (sensor_data_size == 0)
    {
        return;
    }

    char printing_text[265];
    int_array_to_string(DATA_NUM_ELEMENTS, sensor_data, printing_text);

    log_info("Received data: %s", printing_text);

    previous_data_pos = current_data_pos;
    
}
