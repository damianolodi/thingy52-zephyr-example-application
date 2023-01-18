# Set a defualt board in case none is selected
if (NOT BOARD_ID)
    set(BOARD_ID "thingy52" CACHE
        STRING "Choose target board."
        FORCE)
endif()

# Set the possible board names based based on project support
set_property(CACHE BOARD_ID
    PROPERTY STRINGS "thingy52" "myboard"
)

# Select the correct directory based on the board name
if (BOARD_ID STREQUAL "thingy52")
    add_subdirectory(arm/thingy52)
elseif(BOARD_ID STREQUAL "myboard")
    add_subdirectory(arm/myboard)
endif()
