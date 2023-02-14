#include "app_cfg.h"

#include <stdint.h>

void int_array_to_string(size_t num, DATA_TYPE* src, char* dst)
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