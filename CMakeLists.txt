cmake_minimum_required(VERSION 3.13)

# Import RP Pico SDK
include(pico_sdk_import.cmake)

project(picocraft C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Initialize Pico SDK
pico_sdk_init()

# Sources
add_executable(picocraft
    "src/main.c"
    "src/st7789.c"
    "src/draw.c"
)
include_directories("src")

# Disable usb & uart output
pico_enable_stdio_usb(picocraft 0)
pico_enable_stdio_uart(picocraft 0)
# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(picocraft)

# Sleep -> busy wait, saves 4kb
target_compile_definitions(hardware_timer INTERFACE PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)
# Panic -> loopm saves 6kb
target_compile_definitions(pico_runtime INTERFACE PICO_PANIC_FUNCTION=)

add_compile_options(-Wall)

# Dependencies
target_link_libraries(picocraft
    # pico_standard_link
    # pico_runtime
    pico_stdlib
    hardware_gpio
    hardware_spi
    hardware_dma
)