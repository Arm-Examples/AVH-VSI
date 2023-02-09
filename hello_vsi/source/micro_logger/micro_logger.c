/* Copyright 2022 Tobias Andersson. All Rights Reserved.

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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

enum LOG_LEVEL
{
    ERROR = 0,
    WARNING,
    INFO,
    DEBUG
};
static enum LOG_LEVEL log_level = INFO;

void print_log_string_to_each_line(char* log_message, const char* log_type)
{
    char buffer[255];
    
    strcpy(buffer, log_message);
    char *token;
    const char* delim = "\n";
    token = strtok(buffer, delim);
    while(token != NULL)
    {
        printf("%s%s\n", log_type, token);
        token = strtok(NULL, delim);
    }
}

extern void log_error(const char* format, ...)
{
    if (log_level <  ERROR)
        return;
    char *log_buffer;
    va_list args;
    va_start(args, format);

    int how_many = vsprintf(log_buffer, format, args);

    print_log_string_to_each_line(log_buffer, "ERROR: ");

    va_end(args);
}

extern void log_warning(const char* format, ...)
{
    if (log_level < WARNING)
        return;
    char *log_buffer = 0;
    va_list args;
    va_start(args, format);

    int how_many = vsprintf(log_buffer, format, args);

    print_log_string_to_each_line(log_buffer, "WARNING: ");

    va_end(args);
}

extern void log_info(const char* format, ...)
{
    if (log_level < INFO)
        return;
    char *log_buffer = 0;
    va_list args;
    va_start(args, format);

    int how_many = vsprintf(log_buffer, format, args);

    print_log_string_to_each_line(log_buffer, "INFO: ");

    va_end(args);

}

extern void log_debug(const char* format, ...)
{
    if (log_level < DEBUG)
        return;
    char *log_buffer = 0;
    va_list args;
    va_start(args, format);

    int how_many = vsprintf(log_buffer, format, args);

    print_log_string_to_each_line(log_buffer, "DEBUG: ");

    va_end(args);
}
