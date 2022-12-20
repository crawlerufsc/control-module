#include <gtest/gtest.h>
#include "../../serialcomm/serial_comm_pi.h"
#include <stdlib.h>
#include <set>
#include <nlohmann/json.hpp>
#include "../../lib/include/gps_data.h"

using nlohmann::json;

TEST(GpsData, ToJsonTest)
{
    GPSData *data = new GPSData();
    data->lat = 12.2;
    data->lon = 11.1;
    data->age = 1234;
    data->numSatellites = 23;
    data->hdop = 2345;
    data->altitude = 3456;
    data->speedKmh = 123.12;

    std::string json_str = data->toJson();

    auto data2 = json::parse(json_str);

    ASSERT_EQ(data->lat, data2["lat"].get<float>());
    ASSERT_EQ(data->lon, data2["lon"].get<float>());
    ASSERT_EQ(data->age, data2["age"].get<long>());
    ASSERT_EQ(data->numSatellites, data2["numSatellites"].get<int>());
    ASSERT_EQ(data->hdop, data2["hdop"].get<long>());
    ASSERT_EQ(data->altitude, data2["altitude"].get<long>());
    ASSERT_EQ(data->speedKmh, data2["speedKmh"].get<float>());

}

TEST(GpsData, EncodeDecode)
{
    GPSData *data = new GPSData();
    data->lat = 12.2;
    data->lon = 11.1;
    data->age = 1234;
    data->numSatellites = 23;
    data->hdop = 2345;
    data->altitude = 3456;
    data->speedKmh = 123.12;

    char *p = data->encode();

    GPSData *data2 = GPSData::decode(p);

    printf ("%s\n", data2->toJson().c_str());

    ASSERT_EQ(data->lat, data2->lat);
    ASSERT_EQ(data->lon, data2->lon);
    ASSERT_EQ(data->age, data2->age);
    ASSERT_EQ(data->numSatellites, data2->numSatellites);
    ASSERT_EQ(data->hdop, data2->hdop);
    ASSERT_EQ(data->altitude, data2->altitude);
    ASSERT_EQ(data->speedKmh, data2->speedKmh);
}