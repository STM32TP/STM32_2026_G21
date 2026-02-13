#include "sht31.h"

HAL_StatusTypeDef SHT31_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t cmd[2] = {0x30, 0xA2}; // Soft reset command for SHT31 sensor
    
    // Send soft reset command via I2C
    return HAL_I2C_Master_Transmit(hi2c, SHT31_ADDR, cmd, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SHT31_Read(I2C_HandleTypeDef *hi2c, float *temperature, float *humidity)
{
    uint8_t cmd[2]  = {0x24, 0x00}; // Measurement command: high repeatability, clock stretching disabled
    uint8_t data[6];               // Buffer to store received data (6 bytes: T[2] + CRC + H[2] + CRC)

    // Send measurement command
    if (HAL_I2C_Master_Transmit(hi2c, SHT31_ADDR, cmd, 2, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(15); // Wait for measurement to complete (typical max measurement time)

    // Read 6 bytes from sensor
    if (HAL_I2C_Master_Receive(hi2c, SHT31_ADDR, data, 6, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    // Combine MSB and LSB for raw temperature and humidity values
    uint16_t rawT = (data[0] << 8) | data[1];
    uint16_t rawH = (data[3] << 8) | data[4];

    // Convert raw temperature value to degrees Celsius
    // Formula from datasheet: T = -45 + 175 * (rawT / 65535)
    *temperature = -45.0f + 175.0f * ((float)rawT / 65535.0f);

    // Convert raw humidity value to relative humidity percentage
    // Formula from datasheet: RH = 100 * (rawH / 65535)
    *humidity = 100.0f * ((float)rawH / 65535.0f);

    return HAL_OK; // Return success
}
