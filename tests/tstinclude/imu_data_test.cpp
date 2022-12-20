#include <gtest/gtest.h>
#include "../../serialcomm/serial_comm_pi.h"
#include <stdlib.h>
#include <set>
#include <nlohmann/json.hpp>
#include "../../lib/include/imu_data.h"

using nlohmann::json;

TEST(IMUData, ToJsonTest)
{
    IMUData *data = new IMUData();
    data->temperature = 23.1;
    data->accX = 1.234;
    data->accY = 2.341;
    data->accZ = 3.412;
    data->gyroX = 4.123;
    data->gyroY = 1.234;
    data->gyroZ = 2.341;
    data->angleX = 3.412;
    data->angleY = 4.123;
    data->angleZ = 1.234;
    data->accAngleX = 2.341;
    data->accAngleY = 3.412;

    std::string json_str = data->toJson();

    auto data2 = json::parse(json_str);

    ASSERT_EQ(data->temperature, data2["temperature"].get<float>());
    ASSERT_EQ(data->accX, data2["accX"].get<float>());
    ASSERT_EQ(data->accY, data2["accY"].get<float>());
    ASSERT_EQ(data->accZ, data2["accZ"].get<float>());
    ASSERT_EQ(data->gyroX, data2["gyroX"].get<float>());
    ASSERT_EQ(data->gyroY, data2["gyroY"].get<float>());
    ASSERT_EQ(data->gyroZ, data2["gyroZ"].get<float>());
    ASSERT_EQ(data->angleX, data2["angleX"].get<float>());
    ASSERT_EQ(data->angleY, data2["angleY"].get<float>());
    ASSERT_EQ(data->angleZ, data2["angleZ"].get<float>());
    ASSERT_EQ(data->accAngleX, data2["accAngleX"].get<float>());
    ASSERT_EQ(data->accAngleY, data2["accAngleY"].get<float>());

}

TEST(IMUData, EncodeDecode)
{
    IMUData *data = new IMUData();
    data->temperature = 23.1;
    data->accX = 1.234;
    data->accY = 2.341;
    data->accZ = 3.412;
    data->gyroX = 4.123;
    data->gyroY = 1.234;
    data->gyroZ = 2.341;
    data->angleX = 3.412;
    data->angleY = 4.123;
    data->angleZ = 1.234;
    data->accAngleX = 2.341;
    data->accAngleY = 3.412;

    char *p = data->encode();

    IMUData *data2 = IMUData::decode(p);

    ASSERT_EQ(data->temperature, data2->temperature);
    ASSERT_EQ(data->accX, data2->accX);
    ASSERT_EQ(data->accY, data2->accY);
    ASSERT_EQ(data->accZ, data2->accZ);
    ASSERT_EQ(data->gyroX, data2->gyroX);
    ASSERT_EQ(data->gyroY, data2->gyroY);
    ASSERT_EQ(data->gyroZ, data2->gyroZ);
    ASSERT_EQ(data->angleX, data2->angleX);
    ASSERT_EQ(data->angleY, data2->angleY);
    ASSERT_EQ(data->angleZ, data2->angleZ);
    ASSERT_EQ(data->accAngleX, data2->accAngleX);
    ASSERT_EQ(data->accAngleY, data2->accAngleY);
}