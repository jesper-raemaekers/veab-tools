cmake_minimum_required(VERSION 3.1...3.27)

project(
  VeabTools
  VERSION 1.0
  LANGUAGES CXX)

add_executable(veabwrite veab_write.cpp)
add_executable(veabread veab_read.cpp)
add_executable(veabled veab_led.cpp)
add_executable(veabswitch veab_switch.cpp)

install(TARGETS veabwrite veabread veabled veabswitch DESTINATION bin)
