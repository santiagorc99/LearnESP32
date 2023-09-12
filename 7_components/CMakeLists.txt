# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# This is to point to an external component located in the local DISK.
# set(EXTRA_COMPONENT_DIRS C:/Santiago/Path)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(7_components)
