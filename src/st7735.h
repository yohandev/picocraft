/**
 * [st7735.h] Drivers for the ST7735 LCD display.
 * 
 * - Initializes ST7735
 * - Sends a framebuffer to the display
 * - Makes no use of chip select(CS) to receive data
 */
#pragma once

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

#include "num.h"

const static u32 PIN_MISO = 4; // [Unused] GPIO 4(pin 6) In -> CS on ST7735
const static u32 PIN_SCK  = 6; // GPIO 6(pin 9) Clock -> SCK on ST7735
const static u32 PIN_MOSI = 7; // GPIO 7(pin 10) Out -> SDA on ST7735
const static u32 PIN_DC  = 13; // GPIO 13(pin 17) Data/Command -> AO on ST7735
const static u32 PIN_RST = 14; // GPIO 14(pin 19) Reset -> RESET on ST7735

#define SPI_PORT spi0

/** ST7735 Instruction set. */
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01 // Software reset
#define ST7735_RDDID   0x04 // Read display ID
#define ST7735_RDDST   0x09 // Read display status
#define ST7735_SLPIN   0x10 // Sleep in and booster off
#define ST7735_SLPOUT  0x11 // Sleep out and booster on
#define ST7735_PTLON   0x12 // Partial mode on
#define ST7735_NORON   0x13 // Partial off(normal)
#define ST7735_INVOFF  0x20 // Display inversion off
#define ST7735_INVON   0x21 // Display inversion on
#define ST7735_DISPOFF 0x28 // Display off
#define ST7735_DISPON  0x29 // Display on
#define ST7735_CASET   0x2A // Column address set
#define ST7735_RASET   0x2B // Row address set
#define ST7735_RAMWR   0x2C // Memory write
#define ST7735_RAMRD   0x2E // Memory read
#define ST7735_PTLAR   0x30 // Partial start/end address set
#define ST7735_COLMOD  0x3A // Interface pixel format
#define ST7735_MADCTL  0x36 // Memory data access control
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_PWCTR6  0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

/** ST7735 display size */
#define ST7735_WIDTH   160
#define ST7735_HEIGHT  128

typedef struct {
    u32 dma;                    // DMA channel
    dma_channel_config dma_cfg; // DMA configuration
} LCD;

void lcd_init(LCD* self);
void lcd_drop(LCD* self);

void lcd_write_command(LCD* self, const u8* cmd, usize argc);
void lcd_reset(LCD* self);

/**
 * Submits and draws the given framebuffer(160 * 128 * RGB565)
 */
void lcd_draw(LCD* self, const u16* buf);