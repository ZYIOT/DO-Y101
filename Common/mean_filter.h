
#ifndef __COMMON_MEAN_FILTER_H__
#define __COMMON_MEAN_FILTER_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "stdlib.h"
#include "string.h"
    typedef struct
    {
        double *buffer;
       volatile size_t position;
       volatile size_t size;
       volatile size_t capacity;
    } mean_filter_t, *mean_filter_pt;

    int mean_filter_init(mean_filter_pt filter, double *buffer, size_t max_capacity);
    int mean_filter_add(mean_filter_pt filter, double data);
    int mean_filter_out(mean_filter_pt filter);
    int mean_filter_get(mean_filter_pt filter, double *r_mean);
    int mean_filter_count(mean_filter_pt filter);
    int mean_filter_reset(mean_filter_pt filter);
#ifdef __cplusplus
}
#endif
#endif
