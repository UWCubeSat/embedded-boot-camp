/*
 * SHT30 Sensor Driver
 *
 * Copyright © 2022 Eli Reed, Husky Satellite Lab
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <avr/io.h>

#include "sht30.hpp"

using namespace EmbeddedBootCamp;

#define SHT30_I2C_ADDR                  0x44

#define SHT30_CMD_SS_MSR_CLK_STR_DIS    0x2400

#define SHT30_CMD_SS_HIGH_CLK_STR_RPT   0x0006
#define SHT30_CMD_SS_MED_CLK_STR_RPT    0x0006
#define SHT30_CMD_SS_LOW_CLK_STR_RPT    0x0006

