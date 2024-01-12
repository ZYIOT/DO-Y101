#include "ringbuffer.h"
#include "hardware_wrapper.h"
int RingBuffer_init(ring_buffer_pt ring_buffer, uint8_t *buffer, uint32_t max_capacity)
{
    if (max_capacity < 2)
    {
        return -1;
    }
    ring_buffer->buffer = buffer;
    memset(ring_buffer->buffer, 0, max_capacity);
    ring_buffer->capacity = max_capacity;
    RingBuffer_reset(ring_buffer);
    return 0;
}

uint32_t RingBuffer_read(ring_buffer_pt ring_buffer, uint8_t *data, uint32_t len)
{
    if (ring_buffer->size < len)
    {
        len = ring_buffer->size;
    }
    if (len == 0)
    {
        return 0;
    }
    uint32_t end = ring_buffer->position + len;
    if (end <= ring_buffer->capacity)
    {
        memcpy(data, &ring_buffer->buffer[ring_buffer->position], len);
        ring_buffer->position = end;
    }
    else
    {
        uint32_t offset = end % ring_buffer->capacity;
        memcpy(data, &ring_buffer->buffer[ring_buffer->position], len - offset);
        memcpy(&data[len - offset], &ring_buffer->buffer[0], offset);
        ring_buffer->position = offset;
    }
    ring_buffer->size -= len;
    return len;
}

int RingBuffer_write(ring_buffer_pt ring_buffer, uint8_t *data, uint32_t len)
{
    uint32_t free_space = RingBuffer_get_free_space(ring_buffer);
    if (len > free_space)
    {
        len = free_space;
    }
    if (len == 0)
    {
        return 0;
    }
    uint32_t start = ring_buffer->position + ring_buffer->size;
    uint32_t end = start + len;
    if (end <= ring_buffer->capacity)
    {
        memcpy(&ring_buffer->buffer[start], data, len);
    }
    else
    {
        uint32_t offset = end % ring_buffer->capacity;
        memcpy(&ring_buffer->buffer[start], data, len - offset);
        memcpy(&ring_buffer->buffer[0], &data[len - offset], offset);
    }
    ring_buffer->size += len;
    return len;
}

int RingBuffer_reset(ring_buffer_pt ring_buffer)
{
    ring_buffer->position = 0;
    ring_buffer->size = 0;
    return RING_BUFFER_OK;
}

uint32_t RingBuffer_count(ring_buffer_pt ring_buffer)
{
    return ring_buffer->size;
}

uint32_t RingBuffer_get_free_space(ring_buffer_pt ring_buffer)
{
    return ring_buffer->capacity - ring_buffer->size;
}
