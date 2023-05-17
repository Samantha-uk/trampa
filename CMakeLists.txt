cmake_minimum_required(VERSION 3.5)

# Ensure that PICO_SDK_PATH env variable is set
include(pico_sdk_import.cmake)

# Ensure that FREERTOS_KERNEL_PATH env variable is set
include(FreeRTOS_Kernel_import.cmake)

project(trampa)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

pico_sdk_init()

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/usb_descriptors.cpp
        src/button.cpp
        src/midibutton.cpp
        src/midi.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        include/
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap3
        pico_multicore
        pico_stdlib
        tinyusb_board
        tinyusb_device
)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
