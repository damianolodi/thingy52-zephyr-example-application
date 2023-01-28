# Thingy52 board is supported by default in Zephyr

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    list(APPEND CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/conf/debug_rtt.conf)
endif()

list(APPEND DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/boards/arm/thingy52_nrf52832/thingy52_nrf52832.overlay)