#ifndef CRCUNIT_CRC_HPP
#define CRCUNIT_CRC_HPP
#include <stdint.h>

#include <stddef.h>

namespace crcunit
{
    class  CRC
    {
      public:
        static uint16_t rtuCRC16  (const void * data, size_t len);
        static uint32_t crc32     (const void * data, size_t len);
        static uint8_t  crc8_1wire(const void * data, size_t len);
    };
}

#endif
