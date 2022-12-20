#ifndef _BIN_CODEC_H
#define _BIN_CODEC_H

#include "comm_types.h"

class BinCodec
{
public:
    static inline float read_float(char *data, uint16_t pos)
    {
        floatp pkt;
        for (uint8_t i = 0; i < sizeof(float); i++)
            pkt.bval[i] = data[pos + i];
        return pkt.fval;
    }

    static inline long read_long(char *data, uint16_t pos)
    {
        longp pkt;
        for (uint8_t i = 0; i < sizeof(long); i++)
            pkt.bval[i] = data[pos + i];
        return pkt.val;
    }

    static inline uint16_t read_uint16(char *data, uint16_t pos)
    {
        uint16p pkt;
        for (uint8_t i = 0; i < sizeof(uint16_t); i++)
            pkt.bval[i] = data[pos + i];
        return pkt.val;
    }

    static inline uint8_t write(char *data, uint16_t pos, float value)
    {
        floatp pkt;
        pkt.fval = value;

        for (uint8_t i = 0; i < sizeof(float); i++)
            data[pos + i] = pkt.bval[i];

        return sizeof(float);
    }

    static inline uint8_t write(char *data, uint16_t pos, uint16_t value)
    {
        uint16p pkt;
        pkt.val = value;

        for (uint8_t i = 0; i < sizeof(uint16_t); i++)
            data[pos + i] = pkt.bval[i];

        return sizeof(uint16_t);
    }

    static inline uint8_t write(char *data, uint16_t pos, long value)
    {
        longp pkt;
        pkt.val = value;

        for (uint8_t i = 0; i < sizeof(long); i++)
            data[pos + i] = pkt.bval[i];

        return sizeof(long);
    }
};

#endif