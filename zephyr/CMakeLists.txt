cmake_minimum_required(VERSION 3.13.1)
set(DTC_OVERLAY_FILE "dfr0009.overlay")
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zephyr-hd44780)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE ${app_sources})
