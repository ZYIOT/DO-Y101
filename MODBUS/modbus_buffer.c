#include "modbus_buffer.h"
#include "hardware_wrapper.h"
int MODBUS_BUFFER_init(modbus_buffer_pt buffer)
{
    memset(buffer->buffer, 0, MODBUS_PROTOCOL_BUFFER_SIZE);
    MODBUS_BUFFER_reset(buffer);
    return 0;
}

static int MODBUS_BUFFER_write(modbus_buffer_pt buffer, uint8_t *data, uint32_t len)
{
    uint32_t free_space = MODBUS_BUFFER_get_free_space(buffer);
    if (len > free_space)
    {
        len = free_space;
    }
    if (len == 0)
    {
        return 0;
    }
    uint32_t start = buffer->position + buffer->size;
    uint32_t end = start + len;
    if (end < MODBUS_PROTOCOL_BUFFER_SIZE)
    {
        memcpy(&buffer->buffer[start], data, len);
    }
    else
    {
        uint32_t offset = end % MODBUS_PROTOCOL_BUFFER_SIZE;
        memcpy(&buffer->buffer[start], data, len - offset);
        memcpy(&buffer->buffer[0], &data[len - offset], offset);
    }
    buffer->size += len;
    return len;
}

int MODBUS_BUFFER_read(modbus_buffer_pt buffer, uint8_t *data, uint32_t len)
{
    int rc = 0;
    if (len == 0)
    {
        return 0;
    }

    uint32_t remain_size = buffer->size - buffer->read_size;
    if (remain_size < len)
    {
        int read_len = len - remain_size;
        uint8_t buf[MODBUS_PROTOCOL_BUFFER_SIZE];
        if (buffer->port == NULL)
        {
            return PROTOCOL_READ_ERROR;
        }
        rc = buffer->port->read(buf, read_len, MODBUS_READ_TIMEOUT);
        if (rc != read_len)
        {
            return PROTOCOL_READ_ERROR;
        }
        MODBUS_BUFFER_write(buffer, buf, rc);
    }
    uint32_t end = buffer->read_position + len;
    if (end < MODBUS_PROTOCOL_BUFFER_SIZE)
    {
        memcpy(data, &buffer->buffer[buffer->read_position], len);
        buffer->read_position = end;
    }
    else
    {
        uint32_t offset = end % MODBUS_PROTOCOL_BUFFER_SIZE;
        memcpy(data, &buffer->buffer[buffer->read_position], len - offset);
        memcpy(&data[len - offset], &buffer->buffer[0], offset);
        buffer->read_position = offset;
    }
    buffer->read_size += len;
    return len;
}

int MODBUS_BUFFER_reset(modbus_buffer_pt buffer)
{
    buffer->position = 0;
    buffer->read_position = 0;
    buffer->size = 0;
    buffer->read_size = 0;
    return MODBUS_BUFFER_OK;
}

uint32_t MODBUS_BUFFER_count(modbus_buffer_pt buffer)
{
    return buffer->size;
}

uint32_t MODBUS_BUFFER_get_free_space(modbus_buffer_pt buffer)
{
    return MODBUS_PROTOCOL_BUFFER_SIZE - buffer->size;
}

int MODBUS_BUFFER_shift(modbus_buffer_pt buffer)
{
    if (buffer->size >= 1)
    {
        buffer->position = (buffer->position + 1) % MODBUS_PROTOCOL_BUFFER_SIZE;
        buffer->read_position = buffer->position;
        buffer->size--;
        buffer->read_size = 0;
    }
    return MODBUS_BUFFER_OK;
}
