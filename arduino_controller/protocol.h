#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define PROTOCOL_FRAME_TYPE_DATA 1
#define PROTOCOL_FRAME_TYPE_ACK 2
#define PROTOCOL_FRAME_TYPE_DATA_LIST 3
#define PROTOCOL_ACK 1
#define PROTOCOL_NACK 2

#define MSG_ACK 1
#define MSG_ERR 2
#define MSG_START 32
#define MSG_END 31


typedef union {
  float fval;
  char bval[4];
} float_pack;

typedef union {
  uint16_t val;
  char bval[2];
} uint16p;

#endif