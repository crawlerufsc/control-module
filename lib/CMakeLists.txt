cmake_minimum_required(VERSION 3.9)
project(crawler_control VERSION 1.0.0 DESCRIPTION "crawler hardware control library")

include(GNUInstallDirs)

SET(TARGET_H
    ../serialcomm/serial_comm_pi.h
    ../serialcomm/serial_sync_link.h
    ../hal/crawler_hal.h
)

add_library(crawler_control SHARED
    crawler_hal.cpp
    ../serialcomm/serial_comm_pi.cpp
    ../serialcomm/serial_link.cpp
)

set_target_properties(crawler_control PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER 
        include/crawler_hal.h 
        include/gps_data.h
        include/imu_data.h
)

#configure_file(crawler_control.pc.in crawler_control.pc @ONLY)

target_include_directories(crawler_control PRIVATE .)

install(TARGETS crawler_control
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/crawler_control.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)