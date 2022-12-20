#ifndef _GPS_DATA_H
#define _GPS_DATA_H

#include <string>
#include "bin_codec.h"

class GPSData
{
private:
    /* data */
public:
    float lat;
    float lon;
    long age;
    int numSatellites;
    long hdop;
    long altitude;
    float speedKmh;
    

    GPSData *clone();
    std::string toJson();
    static GPSData * decode(char *data);
    static GPSData * decode(char *data, uint16_t initPos);
    char * encode();
    static void printVector(char *value);
};

#endif