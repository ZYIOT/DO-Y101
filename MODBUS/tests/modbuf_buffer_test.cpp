#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

extern "C"
{
#include "modbus_buffer.c"
  static modbus_buffer_t modbus_buffer;
}

TEST(modbus_buffer_init, pass)
{
  EXPECT_EQ(0, MODBUS_BUFFER_init(&modbus_buffer));
  EXPECT_EQ(MODBUS_PROTOCOL_BUFFER_SIZE, MODBUS_BUFFER_get_free_space(&modbus_buffer));
  EXPECT_EQ(0, MODBUS_BUFFER_count(&modbus_buffer));

  uint8_t b[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};
  MODBUS_BUFFER_write(&modbus_buffer, b, 5);
  MODBUS_BUFFER_write(&modbus_buffer, b, 5);
  EXPECT_EQ(MODBUS_PROTOCOL_BUFFER_SIZE - 10, MODBUS_BUFFER_get_free_space(&modbus_buffer));
  EXPECT_EQ(10, MODBUS_BUFFER_count(&modbus_buffer));
  uint8_t rb[100];
  int len = 0;
  for (int i = 0; i < 10; i++)
  {
    len = MODBUS_BUFFER_read(&modbus_buffer, rb, 1);
    MODBUS_BUFFER_shift(&modbus_buffer);
    EXPECT_EQ(MODBUS_PROTOCOL_BUFFER_SIZE - 9 + i, MODBUS_BUFFER_get_free_space(&modbus_buffer));
    EXPECT_EQ(9 - i, MODBUS_BUFFER_count(&modbus_buffer));
    EXPECT_EQ(1, len);
    EXPECT_EQ(b[i % 5], rb[0]);
  }
  len = MODBUS_BUFFER_read(&modbus_buffer, rb, 1);
  MODBUS_BUFFER_shift(&modbus_buffer);
  EXPECT_EQ(PROTOCOL_READ_ERROR, len);
  EXPECT_EQ(MODBUS_PROTOCOL_BUFFER_SIZE, MODBUS_BUFFER_get_free_space(&modbus_buffer));
  EXPECT_EQ(0, MODBUS_BUFFER_count(&modbus_buffer));

  MODBUS_BUFFER_write(&modbus_buffer, b, 10);
  for (int i = 0; i < 10; i++)
  {
    len = MODBUS_BUFFER_read(&modbus_buffer, rb, 1);
    EXPECT_EQ(MODBUS_PROTOCOL_BUFFER_SIZE - 10, MODBUS_BUFFER_get_free_space(&modbus_buffer));
    EXPECT_EQ(10, MODBUS_BUFFER_count(&modbus_buffer));
    EXPECT_EQ(1, len);
    EXPECT_EQ(b[i], rb[0]);
  }
}