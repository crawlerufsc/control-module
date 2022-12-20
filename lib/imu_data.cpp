#include "include/imu_data.h"
#include "include/bin_codec.h"
#include <sstream>

IMUData *IMUData::clone()
{
    IMUData *p = new IMUData();
    p->temperature = temperature;
    p->accX = accX;
    p->accY = accY;
    p->accZ = accZ;
    p->gyroX = gyroX;
    p->gyroY = gyroY;
    p->gyroZ = gyroZ;
    p->angleX = angleX;
    p->angleY = angleY;
    p->angleZ = angleZ;
    p->accAngleX = accAngleX;
    p->accAngleY = accAngleY;
    return p;
}

std::string IMUData::toJson()
{
    std::stringstream *ss = new std::stringstream();
    *ss << "{ ";
    *ss << "\"temperature\": " << temperature << ",";
    *ss << "\"accX\": " << accX << ",";
    *ss << "\"accY\": " << accY << ",";
    *ss << "\"accZ\": " << accZ << ",";
    *ss << "\"gyroX\": " << gyroX << ",";
    *ss << "\"gyroY\": " << gyroY << ",";
    *ss << "\"gyroZ\": " << gyroZ << ",";
    *ss << "\"angleX\": " << angleX << ",";
    *ss << "\"angleY\": " << angleY << ",";
    *ss << "\"angleZ\": " << angleZ << ",";
    *ss << "\"accAngleX\": " << accAngleX << ",";
    *ss << "\"accAngleY\": " << accAngleY;
    *ss << " }";
    return ss->str();
}

IMUData *IMUData::decode(char *data) {
    return decode(data, 0);
}

IMUData *IMUData::decode(char *data, uint16_t initPos = 0)
{
    IMUData *outp = new IMUData();

    uint16_t pos = initPos;
    uint8_t size = data[pos] + 1;
    pos++;

    if (pos + 4 > size)
        return nullptr;
    outp->temperature = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->accX = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->accY = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->accZ = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->gyroX = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->gyroY = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->gyroZ = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->angleX = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->angleY = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->angleZ = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->accAngleX = BinCodec::read_float(data, pos);
    pos += 4;

    if (pos + 4 > size)
        return nullptr;
    outp->accAngleY = BinCodec::read_float(data, pos);

    return outp;
}

char *IMUData::encode()
{
    uint8_t p = 49; // total size
    char *outp = (char *)malloc(sizeof(char) + p);
    outp[0] = p;

    p = 1;
    p += BinCodec::write(outp, p, temperature);
    p += BinCodec::write(outp, p, accX);
    p += BinCodec::write(outp, p, accY);
    p += BinCodec::write(outp, p, accZ);
    p += BinCodec::write(outp, p, gyroX);
    p += BinCodec::write(outp, p, gyroY);
    p += BinCodec::write(outp, p, gyroZ);
    p += BinCodec::write(outp, p, angleX);
    p += BinCodec::write(outp, p, angleY);
    p += BinCodec::write(outp, p, angleZ);
    p += BinCodec::write(outp, p, accAngleX);
    BinCodec::write(outp, p, accAngleY);

    return outp;
}