cmake_minimum_required(VERSION 3.20.0)

##################
# Process Boards #
##################

list(APPEND CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/conf/prj.conf)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	list(APPEND CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/conf/debug.conf)
	add_compile_definitions(DEBUG)
endif()

include(cmake/select_board.cmake)

#################
# Build Modules #
#################

# Define and Process Project Options
include(BuildOptions.cmake)

##########
# ZEPHYR #
##########

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

################
# Dependencies #
################

include(cmake/CPM.cmake)

#######
# APP #
#######

project(zephyr-template
	VERSION 0.1
	DESCRIPTION "MYDESCRIPTION"
	LANGUAGES C CXX ASM)

set_property(GLOBAL PROPERTY C_STANDARD 11)
set_property(GLOBAL PROPERTY CXX_STANDARD 17)

target_sources(app PRIVATE src/main.c src/hts221/hts221.c)

target_include_directories(app PRIVATE src)
target_include_directories(app PRIVATE src src/hts221)
