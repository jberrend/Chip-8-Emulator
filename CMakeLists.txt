cmake_minimum_required(VERSION 3.7)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/emulator.cpp src/emulator.h src/instruction.h src/display.cpp src/display.h src/InputHandler.cpp src/InputHandler.h)

# DONT FORGET THIS LINE WHATEVER YOU DO
set(EXECUTABLE_NAME "Chip8Emulator")
#######################################

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SFML REQUIRED system window network graphics audio)

if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()