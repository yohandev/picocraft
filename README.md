<div align="center">
    <h1>
        <code><span style="color:rgb(205, 35, 85)">Pico</span>craft</code>
    </h1>
    A Minecraft clone for the $4 Raspberry Pi Pico
</div>

## ⚠️ Work in progress ⚠️
Only the application "framework" is implemented right now, that is: get a screenbuffer on an LCD screen and
console messages on my laptop, but do so very fast while producing a very slim binary(~25kb).

## Features
### TFT Display Drivers using DMA (`st7789.c`)
- Using 1-bit SPI interface at its fastest, the screen refresh is still ~9ms
    - This is a problem! At 60FPS, that leaves 7ms to actually render and meanwhile the CPU is busy bit-banging!
- Solution: Using RP Pico's DMA driver, I can feed a screenbuffer directly to the SPI data-in bus
    - This blocks the CPU for only a few cycles(nanoseconds!) and it's free to do anything else in the meantime.
    - Note: no measures against screenbuffer aliasing implemented yet, but I'm thinking of moving to Rust.

### Deferred Formating Host/Client(`fmt.h`/`fmt.py`)
- String formatting is expensive in both time and binary size, since it must be done on the microcontroller.
- Solution: Have the microcontroller send the raw data, then let the laptop do the formatting!
    - String literals(including the one used for formatting) are sent as pointers, since they're in the binary anyways.
    - Support for any serial interface, though I've only tested UART via a debugging probe.
