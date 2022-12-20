#include "include/gps_data.h"
#include "include/bin_codec.h"
#include <sstream>
#include <cstring>

GPSData *GPSData::clone()
{
    GPSData *p = new GPSData();
    p->lat = lat;
    p->lon = lon;
    p->age = age;
    p->numSatellites = numSatellites;
    p->hdop = hdop;
    p->altitude = altitude;
    p->speedKmh = speedKmh;

    return p;
}

std::string GPSData::toJson()
{
    std::stringstream *ss = new std::stringstream();
    *ss << "{";
    *ss << " \"lat\": " << lat << ",";
    *ss << " \"lon\": " << lon << ",";
    *ss << " \"age\": " << age << ",";
    *ss << " \"numSatellites\": " << numSatellites << ",";
    *ss << " \"hdop\": " << hdop << ",";
    *ss << " \"altitude\": " << altitude << ",";
    *ss << " \"speedKmh\": " << speedKmh << " ";
    *ss << "}";
    return ss->str();
}

GPSData *GPSData::decode(char *data)
{
    return decode(data, 0);
}

GPSData *GPSData::decode(char *data, uint16_t initPos)
{
    GPSData *outp = new GPSData();
    uint16_t pos = initPos;
    uint8_t size = data[pos] + 1;

    if (size < 25)
        return nullptr;
    pos++;

    outp->lat = BinCodec::read_float(data, pos);
    pos += 4;

    outp->lon = BinCodec::read_float(data, pos);
    pos += 4;

    outp->age = BinCodec::read_long(data, pos);
    pos += 4;

    outp->numSatellites = (int)data[pos];
    pos++;

    outp->hdop = BinCodec::read_long(data, pos);
    pos += 4;

    outp->altitude = BinCodec::read_long(data, pos);
    pos += 4;

    outp->speedKmh = BinCodec::read_float(data, pos);

    return outp;
}

char *GPSData::encode()
{
    //uint8_t p = 25; // total size
    uint8_t p = 25; // total size
    int size = sizeof(char) + (p + 2);
    char *outp = (char *)malloc(size);
    outp[0] = p;

    p = 1;
    p += BinCodec::write(outp, p, lat);
    p += BinCodec::write(outp, p, lon);
    p += BinCodec::write(outp, p, age);
    outp[p++] = numSatellites;
    p += BinCodec::write(outp, p, hdop);
    p += BinCodec::write(outp, p, altitude);
    p += BinCodec::write(outp, p, speedKmh);

    outp[p] = 0;
    return outp;
}

void GPSData::printVector(char *value)
{
    uint8_t size = value[0];
    printf("[ ");
    while (size-- > 0)
    {
        printf(" %d", *value);
        value++;
    }
    printf("]\n");
}