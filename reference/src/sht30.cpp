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

extern "C" {
    #include <stdint.h>
    #include <avr/common.h>
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <util/twi.h>
}

#include "sht30.hpp"

using namespace EmbeddedBootCamp;

#define SHT30_I2C_ADDR                  0x44

#define SHT30_CMD_SS_MSR_CLK_STR_DIS    0x2400

#define SHT30_CMD_SS_HIGH_CLK_STR_RPT   0x0006
#define SHT30_CMD_SS_MED_CLK_STR_RPT    0x0006
#define SHT30_CMD_SS_LOW_CLK_STR_RPT    0x0006

#define TWI_SLA_W(addr)                 (addr << 1)
#define TWI_SLA_R(addr)                 (TWI_SLA_W(addr) | 0x1)

#define TWI_ACK                         0x1
#define TWI_NACK                        0x0

static inline bool check_status_register(uint8_t status) {
    return TW_STATUS == status;
}

static void respond(uint8_t ack) {
    if (ack) {
        TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);
    } else {
        TWCR = _BV(TWEN) | _BV(TWINT);
    }
}

SHT30::SHT30(volatile uint16_t *int_pin_port, uint8_t int_pin_mask)
    : m_int_pin_port(int_pin_port), m_int_pin_mask(int_pin_mask) {
    // enable Two-Wire interface. This:
    //    - sets PC5 to act as the serial clock line
    //    - sets PC4 to act as the serial data line
    // Also clear the interrupt flag and get ready to ACK
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // _BV(bit) is a macro which expands to: (0x1 << bit)
}

SHT30::Result SHT30::ReadRaw(SHT30::RawData *out) {
    Result res = Result::OK;
    
}

SHT30::Result SHT30::Read(SHT30::Data *out) {
    RawData raw{0, 0, 0, 0};
    Result res = ReadRaw(&raw);
    if (res != Result::OK) {
        return res;
    }

    
    return Result::OK;
}

SHT30::Result SHT30::GenericWrite(
    uint8_t *buf,
    uint16_t n_bytes,
    bool autoStop
) {
    uint16_t idx = 0;

    // write a start condition to the bus
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);

    // spin wait while we send start condition
    while (!(TWCR & _BV(TWINT)));

    // Repeated start is okay, basically just a fall-through to regular start
    if (!check_status_register(TW_START) && !check_status_register(TW_REP_START)) {
        m_cached_result = Result::ERR_START;
        if (check_status_register(TW_BUS_ERROR)) {
            m_cached_result = Result::ERR_BUS;
        }
        return m_cached_result;
    }
    
    // transmit slave address in write mode
    TWDR = TWI_SLA_W(SHT30_I2C_ADDR);
    while (!(TWCR & _BV(TWINT)));

    if (!check_status_register(TW_MT_SLA_ACK)) {
        m_cached_result = Result::ERR_NACK;
        if (check_status_register(TW_BUS_ERROR)) {
            m_cached_result = Result::ERR_BUS;
        }
        return m_cached_result;
    }

    // start loading bytes into the data register and sending them off
    while (idx < n_bytes) {
        TWDR = buf[idx++];

        // wait for data to transmit
        while (!(TWCR & _BV(TWINT)));

        if (!check_status_register(TW_MT_DATA_ACK)) {
            if (check_status_register(TW_MT_DATA_NACK)) {
                m_cached_result = Result::ERR_NACK;
            } else {
                m_cached_result = Result::ERR_BUS;
            }
            return m_cached_result;
        }
    }

    // all data sent, should we send stop?
    if (autoStop) {
        // write a stop condition to the bus
        TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);

        // spin wait while we send stop condition
        while (!(TWCR & _BV(TWINT)));
    }

    return Result::OK;
}

SHT30::Result SHT30::GenericRead(
    uint8_t *buf,
    uint16_t n_bytes,
    bool autoStop
) {
    uint16_t idx = 0;

    // write a start condition to the bus
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);

    // spin wait while we send start condition
    while (!(TWCR & _BV(TWINT)));

    // Repeated start is okay, basically just a fall-through to regular start
    if (!check_status_register(TW_START) && !check_status_register(TW_REP_START)) {
        m_cached_result = Result::ERR_START;
        if (check_status_register(TW_BUS_ERROR)) {
            m_cached_result = Result::ERR_BUS;
        }
        return m_cached_result;
    }
    
    // transmit slave address in read mode
    TWDR = TWI_SLA_R(SHT30_I2C_ADDR);

    // wait for bus state to change
    while (!(TWCR & _BV(TWINT)));

    if (!check_status_register(TW_MR_SLA_ACK)) {
        m_cached_result = Result::ERR_NACK;
        if (check_status_register(TW_BUS_ERROR)) {
            m_cached_result = Result::ERR_BUS;
        }
        return m_cached_result;
    }

    if (n_bytes == 0) {
        // assume no buffer, simply NACK and return early
        respond(TWI_NACK);
    }

    // start waiting for bytes to read
    while (idx < n_bytes) {
        // wait to receive data
        while (!(TWCR & _BV(TWINT)));

        buf[idx++] = TWDR;

        // ack if we expect more data, otherwise nack
        respond(idx == n_bytes ? TWI_NACK : TWI_ACK);

        // wait for ACK
        while (!(TWCR & _BV(TWINT)));

        if (!check_status_register(TW_MR_DATA_ACK)) {
            if (check_status_register(TW_MR_DATA_NACK)) {
                m_cached_result = Result::ERR_NACK;
            } else {
                m_cached_result = Result::ERR_BUS;
            }
            return m_cached_result;
        }
    }

    // all data received, should we send stop?
    if (autoStop) {
        // write a stop condition to the bus
        TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);

        // spin wait while we send stop condition
        while (!(TWCR & _BV(TWINT)));
    }

    return Result::OK;
}

SHT30::Result SHT30::GenericWriteRead(
    uint8_t *wr_buf,
    uint16_t wr_bytes,
    uint8_t *r_buf,
    uint16_t r_bytes,
    bool autoStop
) {
    Result res = GenericWrite(wr_buf, wr_bytes, false);

    if (res != Result::OK) {
        return res;
    }
    return GenericRead(r_buf, r_bytes, true);
}



