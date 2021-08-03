cmake_minimum_required(VERSION 3.6)
project(h264_sei)

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

set(SOURCE_FILES
    h264_sei.cc
    webrtc/src/rtc_base/checks.cc
    webrtc/src/rtc_base/zero_memory.cc
    webrtc/src/common_video/h264/h264_common.cc
)

# Create target.
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    .
    webrtc/src
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    RTC_DISABLE_CHECK_MSG=1
)

# Compile definitions for WebRTC
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WEBRTC_WIN)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC WEBRTC_POSIX)
endif()

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WEBRTC_MAC)
    if(IOS)
        target_compile_definitions(${PROJECT_NAME} PUBLIC WEBRTC_IOS)
    endif()
elseif(ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WEBRTC_LINUX WEBRTC_ANDROID)
endif()

# Demo
set(PROJECT_DEMO "${PROJECT_NAME}_test")

set(SOURCE_FILES_DEMO
    sei_test.cc
)

add_executable(${PROJECT_DEMO} ${SOURCE_FILES_DEMO})

target_link_libraries(${PROJECT_DEMO} PRIVATE ${PROJECT_NAME})
