#include "ultrasonic.h"

static GPIO_TypeDef* us_gpio = GPIOA;
static uint16_t us_pin = GPIO_PIN_8;

// Internal function for microsecond delay using DWT cycle counter
static void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;  // Get current CPU cycle count
    uint32_t delayTicks = us * (HAL_RCC_GetHCLKFreq() / 1000000); // Convert microseconds to CPU cycles
    while (DWT->CYCCNT - startTick < delayTicks); // Wait until desired time has passed
}

void Ultrasonic_Init(void) {
    // Enable DWT cycle counter for microsecond timing
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable trace
    DWT->CYCCNT = 0;                                // Reset cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             // Enable cycle counter

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = us_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // Configure as push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;              // No pull-up or pull-down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(us_gpio, &GPIO_InitStruct);

    HAL_GPIO_WritePin(us_gpio, us_pin, GPIO_PIN_RESET); // Set pin low
    HAL_Delay(50); // Wait for sensor stabilization
}

float Ultrasonic_Read(void) {
    uint32_t start = 0, stop = 0;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Send Trigger Pulse (10 µs)
    GPIO_InitStruct.Pin = us_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // Configure as output
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(us_gpio, &GPIO_InitStruct);

    HAL_GPIO_WritePin(us_gpio, us_pin, GPIO_PIN_SET); // Set pin high
    delay_us(10);                                     // Wait 10 µs
    HAL_GPIO_WritePin(us_gpio, us_pin, GPIO_PIN_RESET); // Set pin low

    // 2. Immediately switch pin to input mode to receive Echo
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(us_gpio, &GPIO_InitStruct);

    // 3. Wait for rising edge (start of echo) with timeout protection
    uint32_t timeout = 100000; // Safety timeout
    while(HAL_GPIO_ReadPin(us_gpio, us_pin) == GPIO_PIN_RESET && timeout--);
    start = DWT->CYCCNT; // Record start time

    // 4. Wait for falling edge (end of echo)
    timeout = 90000;
    while(HAL_GPIO_ReadPin(us_gpio, us_pin) == GPIO_PIN_SET && timeout--);
    stop = DWT->CYCCNT; // Record stop time

    // If timeout occurred or invalid timing, return error value
    if(timeout == 0 || stop <= start) return -1.0f;

    // 5. Distance calculation
    // Time (µs) = CPU cycles / (CPU frequency in MHz)
    float time_us = (float)(stop - start) / (HAL_RCC_GetHCLKFreq() / 1000000.0f);

    // Distance = (time × speed of sound 0.0343 cm/µs) / 2
    // Division by 2 because the sound travels to the object and back
    return (time_us * 0.0343f) / 2.0f;
}
