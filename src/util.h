#pragma once

#include "num.h"

#define len(_x) (sizeof(_x) / sizeof(_x[0]))
#define swap(_a, _b) { i32 t = _a; _a = _b; _b = t; }