# Set a defualt board in case none is selected
if (NOT BOARD)
    set(BOARD "thingy52_nrf52832") 
endif()

# Select the correct directory based on the board name
if (BOARD STREQUAL "thingy52_nrf52832")
    message(NOTICE "Select BOARD Thingy52")
    include(boards/arm/thingy52_nrf52832/board.cmake)
elseif(BOARD STREQUAL "nrf52840dk_nrf52840")
    message(NOTICE "Select BOARD nRF52840DK")
    include(boards/arm/nrf52840dk_nrf52840/board.cmake)
else()
    message(FATAL_ERROR "Selected board is not supported. Add your board to boards/CMakeLists.txt")
endif()
