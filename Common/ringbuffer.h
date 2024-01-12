#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <string.h>
#define RING_BUFFER_LENGTH 1000

typedef struct
{
    uint8_t *buffer;
    uint32_t position;
    uint32_t size;
    uint32_t capacity;
} ring_buffer_t, *ring_buffer_pt;

#define RING_BUFFER_OK 0
#define RING_BUFFER_FULL -1
#define RING_BUFFER_NO_SUFFICIENT_SPACE -2

int RingBuffer_init(ring_buffer_pt ring_buffer, uint8_t *buffer, uint32_t max_capacity);
uint32_t RingBuffer_count(ring_buffer_pt ring_buffer);
uint32_t RingBuffer_get_free_space(ring_buffer_pt ring_buffer);
uint32_t RingBuffer_read(ring_buffer_pt ring_buffer, uint8_t *data, uint32_t len);
int RingBuffer_write(ring_buffer_pt ring_buffer, uint8_t *data, uint32_t len);
int RingBuffer_reset(ring_buffer_pt ring_buffer);

#endif
