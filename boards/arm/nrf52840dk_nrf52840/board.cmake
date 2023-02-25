# The nrf52840dk board is supported by default in Zephyr

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    list(APPEND CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/conf/debug_uart.conf)
endif()