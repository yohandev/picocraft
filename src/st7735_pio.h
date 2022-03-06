/**
 * [st7735_pio.h] Drivers for the ST7735 LCD display using PIO
 * 
 * Initialization happens via bit-banging on system core 0
 *  - Blocks CPU; who cares, happens once and fast "enough"
 * Screen upload/refresh triggered by IRQ (?)
 *  - Read via DMA, no word-by-word feeding (?)
 * Rendering:
 *  - CLK: Side-set
 *  - 
 */