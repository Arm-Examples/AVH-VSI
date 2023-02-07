/* Copyright 2022 Tobias Andersson @Arm Ltd. All Rights Reserved.

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

#include "micro_logger.hpp"

#include <cstdarg>
#include <cstdio>
#include <string>

namespace logger
{
    void add_log_string_to_each_line(std::string &log_message, const std::string log_type)
    {
        log_message.insert(0, log_type.c_str());
        int pos = 0;
        while(pos < log_message.size())
        {
            pos = log_message.find_first_of('\n', pos);

            if(pos == std::string::npos)
            {
                break;
            }

            log_message.insert(pos + 1, log_type.c_str());

            pos += log_type.size() + 1; 
        }
    }

    void error(const char* format, ...)
    {
        if (log_level < LOG_LEVEL::ERROR)
            return;
        char *log_buffer = 0;
        va_list args;
        va_start(args, format);

        int how_many = vsprintf(log_buffer, format, args);

        std::string log_message(log_buffer);

        add_log_string_to_each_line(log_message, "ERROR: ");

        printf("%s\n", log_message.c_str());

        va_end(args);
    }

    void warning(const char* format, ...)
    {
        if (log_level < LOG_LEVEL::WARNING)
            return;
        char *log_buffer = 0;
        va_list args;
        va_start(args, format);

        int how_many = vsprintf(log_buffer, format, args);

        std::string log_message(log_buffer);

        add_log_string_to_each_line(log_message, "WARNING: ");

        printf("%s\n", log_message.c_str());

        va_end(args);
    }

    void info(const char* format, ...)
    {
        if (log_level < LOG_LEVEL::INFO)
            return;
        char *log_buffer = 0;
        va_list args;
        va_start(args, format);

        int how_many = vsprintf(log_buffer, format, args);

        std::string log_message(log_buffer);

        add_log_string_to_each_line(log_message, "INFO: ");

        printf("%s\n", log_message.c_str());

        va_end(args);
    }

    void debug(const char* format, ...)
    {
        if (log_level < LOG_LEVEL::DEBUG)
            return;
        char *log_buffer = 0;
        va_list args;
        va_start(args, format);

        int how_many = vsprintf(log_buffer, format, args);

        std::string log_message(log_buffer);

        add_log_string_to_each_line(log_message, "DEBUG: ");

        printf("%s\n", log_message.c_str());

        va_end(args);
    }

} // namespace logger
