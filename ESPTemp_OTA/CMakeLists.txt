cmake_minimum_required(VERSION 3.24)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_mqtt)
set(EXTRA_PARTITION_TABLE_CSV partitions.csv)
