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

#ifndef _SHT30_H
#define _SHT30_H

#include <stdint.h>

namespace EmbeddedBootCamp {
    class SHT30 {
        public:
            struct Status {
                uint16_t write_checksum : 1;
                uint16_t command : 1;
                uint16_t __res0__ : 2;
                uint16_t sys_reset : 1;
                uint16_t __res1__ : 5;
                uint16_t t_tracking_alert : 1;
                uint16_t rh_tracking_alert : 1;
                uint16_t __res2__ : 1;
                uint16_t heater : 1;
                uint16_t __res3__ : 1;
                uint16_t alert_pending : 1;
            };

            struct Data {
                float temperature;
                float humidity;
            };

            struct RawData {
                uint16_t temperature;
                uint16_t humidity;
                uint8_t t_crc;
                uint8_t h_crc;
            };

            enum class TemperatureUnits {
                CELSIUS,
                FAHRENHEIT,
            };

            enum class DataMode {
                SINGLE_SHOT,
                CS_SINGLE_SHOT,
                CONTINUOUS,
            };

            enum class MeasurementFrequency {
                MPS_0_5,
                MPS_1,
                MPS_2,
                MPS_4,
                MPS_10,
            };

            enum class Resolution {
                LOW,
                MEDIUM,
                HIGH,
            };

            enum class Result {
                NONE,
                ERR_CONFIG,
                ERR_NO_DATA,
                ERR_CRC,
                ERR_NACK,
                ERR_PARAM,
            };
            
            // Construct sensor object instance
            SHT30();

            // Initialize peripheral resources (I2C, GPIO, etc.)
            void Begin();

            // Set units for temperature
            inline void SetTemperatureUnits(TemperatureUnits units) {
                m_units = units;
            }

            // Set data acquisition mode
            inline void SetDataMode(DataMode mode) {
                m_data_mode = mode;
            }

            // Set measurement frequency (measurements per second)
            inline void SetMeasurementFrequency(MeasurementFrequency mfq) {
                m_measurement_frequency = mfq;
            }

            // Set the measurement resolution
            inline void SetResolution(Resolution res) {
                m_resolution = res;
            }

            // Read and return a processed set of data
            Data Read();

            // Read and return an unprocessed set of data 
            RawData ReadRaw();

            // Read the device status register
            Status ReadStatus();

            // Clear the status register
            Result ClearStatus();

            // Soft reset the device
            Result SoftReset();

            // Get the result of the last command
            inline Result GetLastResult() const { return m_cached_result; }

        private:
            Result GenericWrite(uint8_t *buf, uint16_t n_bytes);

            Result GenericRead(uint8_t *buf, uint16_t n_bytes);

            Result GenericWriteRead(
                uint8_t *wr_buf,
                uint16_t wr_bytes,
                uint8_t *r_buf,
                uint16_t r_bytes
            );

            bool CheckCRC(uint8_t crc);

            Result m_cached_result;

            TemperatureUnits m_units;

            DataMode m_data_mode;

            MeasurementFrequency m_measurement_frequency;

            Resolution m_resolution;
    };  // class SHT30
}   // namespace EmbeddedBootCamp

#endif  // _SHT32_H