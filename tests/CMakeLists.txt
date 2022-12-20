# require CMake 2.8 or greater
cmake_minimum_required(VERSION 2.8)

project(driveless-control-module)

set(CMAKE_CXX_STANDARD 14)
include_directories(${PROJECT_SOURCE_DIR})

#find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED) 
#pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
#pkg_search_module(GLIB REQUIRED glib-2.0) 

find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

SET(TARGET_H
    ../serialcomm/serial_comm_pi.h
    ../serialcomm/serial_sync_link.h
    ../serialcomm/serial_link.h
    ../lib/include/crawler_hal.h
    ../lib/include/gps_data.h
    ../lib/include/imu_data.h
    ../lib/include/bin_codec.h
    ../lib/include/comm_types.h
)

add_executable(
    unittest
    ../serialcomm/serial_link.cpp
    ../serialcomm/serial_comm_pi.cpp
    ../lib/crawler_hal.cpp
    ../lib/gps_data.cpp
    ../lib/imu_data.cpp

    control_tests.cpp
    tstinclude/gps_data_test.cpp
    tstinclude/imu_data_test.cpp
    serialcomm/serial_comm_pi_test.cpp
    serialcomm/serial_link_test.cpp
)

# link my-recognition to jetson-inference library
target_link_libraries(unittest ${GTEST_LIBRARIES} ${WIRINGPI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
