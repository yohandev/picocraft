cmake_minimum_required(VERSION 3.13)

# Import RP Pico SDK
include(pico_sdk_import.cmake)

project(picocraft C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize RP Pico SDK
pico_sdk_init()

# Sources
add_executable(picocraft
    "src/main.c"
    "src/st7789.c"
    "src/texture.c"
)
include_directories("src")

# Enable usb output; Disable uart output
pico_enable_stdio_usb(picocraft 1)
pico_enable_stdio_uart(picocraft 0)
# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(picocraft)

# Dependencies
target_link_libraries(picocraft
    pico_stdlib
    hardware_spi
    hardware_dma
    hardware_pio
)