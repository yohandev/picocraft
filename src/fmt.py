"""
[fmt.py] Deferred formating, console-side
"""

import lief     # dep: https://pypi.org/project/lief/
import serial   # dep: https://pypi.org/project/pyserial/
import re

ELF_PATH = "build/picocraft.elf"
PORT = [
    "/dev/tty.usbmodem14202",
    "/dev/tty.usbmodem14201",
    "/dev/tty.usbmodem14101",
    "/dev/tty.usbmodem14102",
]
FMT = {
    'chr': lambda mem: chr(next(mem)),
    'u8': lambda mem: int.from_bytes(take(mem, 1), 'little', signed=False),
    'i8': lambda mem: int.from_bytes(take(mem, 1), 'little', signed=True),
    'u16': lambda mem: int.from_bytes(take(mem, 2), 'little', signed=False),
    'i16': lambda mem: int.from_bytes(take(mem, 2), 'little', signed=True),
    'u32': lambda mem: int.from_bytes(take(mem, 4), 'little', signed=False),
    'i32': lambda mem: int.from_bytes(take(mem, 4), 'little', signed=True),
    'u64': lambda mem: int.from_bytes(take(mem, 8), 'little', signed=False),
    'i64': lambda mem: int.from_bytes(take(mem, 8), 'little', signed=True),
    'str': lambda mem: ''.join(iter(lambda: FMT['chr'](mem), '\0')),
}
FMT_PAT = re.compile(r"{[A-za-z0-9]*}")


binary = lief.parse(ELF_PATH)
rodata = binary.get_section('.rodata')


def find_port():
    """
    Finds an existing port in the filesystem
    """
    for port in PORT:
        try: return serial.Serial(port, baudrate=115200)
        except serial.SerialException: continue


def take(iter, n):
    """
    Returns the next `n` elements from the iterator
    """
    return [next(iter) for _ in range(n)]


def drain(source):
    """
    Repeteadly calls `source` for an iterator, and steps through its
    items until drained and calls `source` again(ad infinitum)
    """
    while True: yield from source()


def fmt_sub(fmt, cb):
    """
    Replaces the format string with its actual contents by calling `cb`
    """
    return re.sub(FMT_PAT, lambda m: cb(m.group().strip('{ }')), fmt)


with find_port() as ser:
    """
    Awaits packets from serial, then formats them and prints
    """
    while True:
        # [addr(fmt) | arg1 | arg2 | arg3 | (...)]
        ln = drain(ser.readline)
        # Reads the address from packet, then reads the string
        # at that address from the elf file.
        fmt = FMT['u32'](ln) - rodata.virtual_address
        fmt = FMT['str'](iter(rodata.content[fmt:]))

        print(fmt_sub(fmt, lambda t: str(FMT[t](ln))))