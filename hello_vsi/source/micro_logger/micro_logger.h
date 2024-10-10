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

#ifndef _MICRO_LOGGER_H_
#define _MICRO_LOGGER_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern void log_error(const char* format, ...);

extern void log_warning(const char* format, ...);

extern void log_info(const char* format, ...);

extern void log_debug(const char* format, ...);

#endif
