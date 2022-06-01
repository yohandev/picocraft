/**
 * [st7739.h] Drivers for the ST7739 LCD display
 */
#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "num.h"

#define ST7789_NOP          0x00
#define ST7789_SWRESET      0x01 // Software reset
#define ST7789_RDDID        0x04 // Read display ID
#define ST7789_RDDST        0x09 // Read display status
#define ST7789_SLPIN        0x10 // Sleep in and booster off
#define ST7789_SLPOUT       0x11 // Sleep out and booster on
#define ST7789_PTLON        0x12 // Partial mode on
#define ST7789_NORON        0x13 // Partial off(normal)
#define ST7789_INVOFF       0x20 // Display inversion off
#define ST7789_INVON        0x21 // Display inversion on
#define ST7789_DISPOFF      0x28 // Display off
#define ST7789_DISPON       0x29 // Display on
#define ST7789_CASET        0x2A // Column address set
#define ST7789_RASET        0x2B // Row address set
#define ST7789_RAMWR        0x2C // Memory write
#define ST7789_RAMRD        0x2E // Memory read
#define ST7789_PTLAR        0x30 // Partial start/end address set
#define ST7789_VSCRDEF      0x33
#define ST7789_COLMOD       0x3A // Interface pixel format
#define ST7789_MADCTL       0x36 // Memory data access control
#define ST7789_VSCSAD       0x37
#define ST7789_MADCTL_MY    0x80 // Page Address Order
#define ST7789_MADCTL_MX    0x40 // Column Address Order
#define ST7789_MADCTL_MV    0x20 // Page/Column Order
#define ST7789_MADCTL_ML    0x10 // Line Address Order
#define ST7789_MADCTL_MH    0x04 // Display Data Latch Order
#define ST7789_MADCTL_RGB   0x00
#define ST7789_MADCTL_BGR   0x08
#define ST7789_RDID1        0xDA
#define ST7789_RDID2        0xDB
#define ST7789_RDID3        0xDC
#define ST7789_RDID4        0xDD
#define ST7789_COLMOD_65K   0x50
#define ST7789_COLMOD_16BIT 0x05

typedef struct {
    usize width;            // Width(px, display height)
    usize height;           // Height(px, display width)

    u32 sck;                // Clock pin
    u32 mosi;               // Peripheral-in pin
    u32 dc;                 // Data/Command pin
    u32 rst;                // Reset pin

    spi_inst_t *const spi;  // SPI port to use
    u32 baudrate;           // SPI baudrate(Hz)
} st7789;

void st7789_init(st7789* self);
void st7789_drop(st7789* self);

void st7789_write(st7789* self, const u8* cmd, usize argc);
void st7789_reset(st7789* self);
void st7789_draw(st7789* self, const rgb16* buf);