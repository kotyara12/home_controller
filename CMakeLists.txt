cmake_minimum_required(VERSION 3.16.0)
set(EXCLUDE_COMPONENTS freemodbus)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(LINK_OPTIONS "-Wl,--wrap=esp_panic_handler" APPEND)
project(home_controller)
