/**
 * [fmt.h] Provides printing utilities for deferred formating
 */
#pragma once

#include "pico/stdlib.h"

#include "num.h"

#define PRINT_UART uart0
#define PRINT_UART_TX 16
#define PRINT_UART_RX 17
#define PRINT_UART_BAUDRATE 115200

#define PRINT_FMT_WRITE(X) ({                       \
    __typeof__(X) _x = (X);                         \
    uart_write_blocking(                            \
        PRINT_UART,                                 \
        (u8*)&(_x),                                 \
        sizeof(_x)                                  \
    );                                              \
})
#define PRINT_FMT(X) {_Generic((X),                 \
    char: PRINT_FMT_WRITE(X),                       \
    char*: PRINT_FMT_WRITE(X),                      \
    u8: PRINT_FMT_WRITE(X),                         \
    i8: PRINT_FMT_WRITE(X),                         \
    u16: PRINT_FMT_WRITE(X),                        \
    i16: PRINT_FMT_WRITE(X),                        \
    u32: PRINT_FMT_WRITE(X),                        \
    i32: PRINT_FMT_WRITE(X),                        \
    u64: PRINT_FMT_WRITE(X),                        \
    i64: PRINT_FMT_WRITE(X),                        \
    fixed: PRINT_FMT_WRITE(X)                       \
);}
#define PRINT_0()
#define PRINT_1(X) PRINT_FMT(X)
#define PRINT_2(X, ...) PRINT_FMT(X) PRINT_1(__VA_ARGS__)
#define PRINT_3(X, ...) PRINT_FMT(X) PRINT_2(__VA_ARGS__)
#define PRINT_4(X, ...) PRINT_FMT(X) PRINT_3(__VA_ARGS__)
#define PRINT_5(X, ...) PRINT_FMT(X) PRINT_4(__VA_ARGS__)

// See: https://stackoverflow.com/a/11763277
#define PRINT_GET_MACRO(_0, _1, _2, _3, _4, _5, NUM, ...) PRINT_##NUM

// Prints to UART using deferred formatting.
// Arguments inside the formatting string must specify
// their type inside brackets, ie. "The time is {u32}"
#define print(fmt, ...) {                           \
    PRINT_FMT((char*)fmt)                           \
    PRINT_GET_MACRO(                                \
        _0, ##__VA_ARGS__,                          \
        5, 4, 3, 2, 1, 0)(__VA_ARGS__)              \
    PRINT_FMT((char)'\n')                           \
}

// Initializes UART stdio
inline void stdout_init(void) {
    stdio_uart_init_full(
        PRINT_UART,
        PRINT_UART_BAUDRATE,
        PRINT_UART_TX,
        PRINT_UART_RX
    );
}