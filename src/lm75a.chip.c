// Wokwi Custom Chip - For information and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright (C) 2022 Uri Shaked / wokwi.com

#include <stdio.h>
#include <stdlib.h>
#include "wokwi-api.h"

const uint8_t LM75A_BASE_ADDRESS = 0x48;

typedef enum
{
  PIN_OS,
  PIN_A0,
  PIN_A1,
  PIN_A2,
  MAX_PINS
} Pins;

typedef enum
{
  REG_TEMPERATURE = 0,
  REG_CONFIGURATION = 1,
  REG_T_HYST = 2,
  REG_T_OS = 3,
  REG_PRODUCT_ID = 7
} Registers;

typedef enum
{
  ST_EXPECTING_CONNECT,
  ST_EXPECTING_READ_BYTE_1,
  ST_EXPECTING_READ_BYTE_2,
  ST_EXPECTING_WRITE_BYTE_1,
  ST_EXPECTING_WRITE_BYTE_2,
  ST_EXPECTING_WRITE_BYTE_3,
} States;

typedef struct
{
  pin_t pins[MAX_PINS];
  States state;
  timer_t conversion_timer_id;
  uint8_t internal_address;
  uint8_t temperature_byte_hi;
  uint8_t temperature_byte_lo;
  uint8_t config;
  uint8_t t_os_byte_hi;
  uint8_t t_os_byte_lo;
  uint8_t t_hyst_byte_hi;
  uint8_t t_hyst_byte_lo;
  uint8_t product_id;
  uint8_t fault_count;
  uint8_t fault_count_reload;
  bool os_interrupt_triggered_high;
  bool os_interrupt_triggered_low;
  uint32_t temp_attr;
} chip_state_t;

static bool on_i2c_connect(void *user_ctx, uint32_t address, bool connect);
static uint8_t on_i2c_read(void *user_ctx);
static bool on_i2c_write(void *user_ctx, uint8_t data);
static void chip_conversion_timer_event(void *user_data);

void chip_init()
{
  int16_t temperature;

  chip_state_t *chip = calloc(1, sizeof(chip_state_t));

  chip->pins[PIN_OS] = pin_init("OS", INPUT);  // Open drain floating.
  chip->pins[PIN_A0] = pin_init("A0", INPUT);
  chip->pins[PIN_A1] = pin_init("A1", INPUT);
  chip->pins[PIN_A2] = pin_init("A2", INPUT);

  const timer_config_t timer_config = {
    .callback = chip_conversion_timer_event,
    .user_data = chip,
  };
  chip->conversion_timer_id = timer_init(&timer_config);
  timer_start(chip->conversion_timer_id,  100000, true);

  chip->config = 0x00;

  temperature = 80 * 256;
  chip->t_os_byte_hi = (uint8_t)(temperature >> 8);
  chip->t_os_byte_lo = (uint8_t)temperature;
  
  temperature = 75 * 256;
  chip->t_hyst_byte_hi = (uint8_t)(temperature >> 8);
  chip->t_hyst_byte_lo = (uint8_t)temperature;

  chip->product_id = 0xA1;

  uint8_t reload = (chip->config >> 3 & 3) * 2;
  if (reload == 0) reload = 1;
  chip->fault_count_reload = reload;
  chip->fault_count = reload;

  chip->os_interrupt_triggered_high = false;
  chip->os_interrupt_triggered_low = false;

  //
  // Initialise I2C
  //
  
  const i2c_config_t i2c1 =
  {
    .address = LM75A_BASE_ADDRESS | (pin_read(chip->pins[PIN_A2]) << 2) | (pin_read(chip->pins[PIN_A1]) << 1) | pin_read(chip->pins[PIN_A0]),
    .scl = pin_init("SCL", INPUT), 
    .sda = pin_init("SDA", INPUT),
    .connect = on_i2c_connect,
    .read = on_i2c_read,
    .write = on_i2c_write,
    .user_data = chip,
  };
  i2c_init(&i2c1);

  // This attribute can be edited by the user. It's defined in lm75a.chip.json:
  chip->temp_attr = attr_init_float("temperature", 22);
}

static void ActivatePinOS(chip_state_t *chip)
{
  if (chip->config & 4)
  {
    // OS is active high, so set a high impedance input.
    pin_mode(chip->pins[PIN_OS], INPUT);
  }
  else
  {
    // OS is active low, so pull the output low.
    pin_mode(chip->pins[PIN_OS], OUTPUT_LOW);
  }
}

static void DeactivatePinOS(chip_state_t *chip)
{
  if (chip->config & 4)
  {
    // OS is active high, therefore the inactive state is low, so pull the output low.
    pin_mode(chip->pins[PIN_OS], OUTPUT_LOW);
  }
  else
  {
    // OS is active low, therefore the inactive state is high, so set a high impedance input.
    pin_mode(chip->pins[PIN_OS], INPUT);
  }
}

void chip_conversion_timer_event(void *user_data) 
{
  chip_state_t *chip = (chip_state_t *)user_data;
  float f_temperature;
  int16_t temperature, t_os, t_hyst;

  if (chip->state == ST_EXPECTING_CONNECT)
  {
    f_temperature = attr_read_float(chip->temp_attr);

    // Temperature dependent ADC conversion time between 100,000 us and 300,000 us.
    timer_start(chip->conversion_timer_id, 100000 + (f_temperature + 55) / 180 * 200 * 1000, true);

    temperature = f_temperature * 256;
    chip->temperature_byte_hi = (uint8_t)(temperature >> 8);
    chip->temperature_byte_lo = (uint8_t)(temperature);

    if (!(chip->config & 1))  // If not in Shutdown Mode.
    {
      t_os = (int16_t)(chip->t_os_byte_hi << 8 | chip->t_os_byte_lo);
      t_hyst = (int16_t)(chip->t_hyst_byte_hi << 8 | chip->t_hyst_byte_lo);
      if (chip->config & 2)
      {
        // Interrupt mode.
        if (temperature > t_os)
        {
          // Decrement fault count.
          chip->fault_count--;
          if (chip->fault_count == 0)
          {
            if (temperature > t_os && !chip->os_interrupt_triggered_high)
            {
              ActivatePinOS(chip);  // Will be deactivated in chip_i2c_read() or Shutdown Mode.
              chip->os_interrupt_triggered_high = true;
              chip->os_interrupt_triggered_low = false;
            }
            chip->fault_count = chip->fault_count_reload;
          }
        }
        else if (temperature < t_hyst && !chip->os_interrupt_triggered_low)
        {
          ActivatePinOS(chip);  // Will be deactivated in chip_i2c_read() or Shutdown Mode.
          chip->os_interrupt_triggered_low = true;
          chip->os_interrupt_triggered_high = false;
          chip->fault_count = chip->fault_count_reload;
        }
        else
        {
          chip->fault_count = chip->fault_count_reload;
        }
      }
      else
      {
        // Comparator mode.
        if (temperature > t_os)
        {
          // Decrement fault count.
          chip->fault_count--;
          if (chip->fault_count == 0)
          {
            // OS is active.
            ActivatePinOS(chip);
            chip->fault_count = chip->fault_count_reload;
          }
        }
        else if (temperature < t_hyst)
        {
          // OS is inactive.
          DeactivatePinOS(chip);
          chip->fault_count = chip->fault_count_reload;
        }
        else
        {
          chip->fault_count = chip->fault_count_reload;
        }
      }
    }
  }
}

bool on_i2c_connect(void *user_ctx, uint32_t address, bool read)
{
  chip_state_t *chip = user_ctx;

  if (read)
  {
    chip->state = ST_EXPECTING_READ_BYTE_1;
  }
  else
  {
    chip->state = ST_EXPECTING_WRITE_BYTE_1;
  }
  return true; /* Ack */
}

uint8_t on_i2c_read(void *user_ctx)
{
  chip_state_t *chip = user_ctx;

  if (chip->config & 2)
  {
    // Interrupt mode.
    DeactivatePinOS(chip);
  }

  uint8_t result;

  switch (chip->state)
  {
    case ST_EXPECTING_READ_BYTE_1:
      switch (chip->internal_address)
      {
        case REG_TEMPERATURE:
          result = chip->temperature_byte_hi;
          chip->state = ST_EXPECTING_READ_BYTE_2;
          break;
        case REG_CONFIGURATION:
          result = chip->config;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        case REG_T_OS:
          result = chip->t_os_byte_hi;
          chip->state = ST_EXPECTING_READ_BYTE_2;
          break;
        case REG_T_HYST:
          result = chip->t_hyst_byte_hi;
          chip->state = ST_EXPECTING_READ_BYTE_2;
          break;
        case REG_PRODUCT_ID:
          result = chip->product_id;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        default:
          chip->state = ST_EXPECTING_CONNECT;
          result = 0;
          break;
      }
      break;
    case ST_EXPECTING_READ_BYTE_2:
      switch (chip->internal_address)
      {
        case REG_TEMPERATURE:
          result = chip->temperature_byte_lo;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        case REG_T_OS:
          result = chip->t_os_byte_lo;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        case REG_T_HYST:
          result = chip->t_hyst_byte_lo;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        default:
          result = 0;
          chip->state = ST_EXPECTING_CONNECT;
          break;
      }
      break;
    default:
      result = 0;
      chip->state = ST_EXPECTING_CONNECT;
      break;
  }
  return result;
}

bool on_i2c_write(void *user_ctx, uint8_t data)
{
  chip_state_t *chip = user_ctx;

  uint8_t reload;

  switch (chip->state)
  {
    case ST_EXPECTING_WRITE_BYTE_1:
      chip->internal_address = data;
      chip->state = ST_EXPECTING_WRITE_BYTE_2;
      break;
    case ST_EXPECTING_WRITE_BYTE_2:
      switch (chip->internal_address)
      {
        case REG_CONFIGURATION:
          chip->config = data;
          if (chip->config & 3)
          {
            // Shutdown Mode while in Interrupt Mode, so deactivate OS.
            DeactivatePinOS(chip);
          }
          reload = (chip->config >> 3 & 3) * 2;
          if (reload == 0) reload = 1;
          chip->fault_count_reload = reload;
          chip->fault_count = reload;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        case REG_T_OS:
          chip->t_os_byte_hi = data;
          chip->state = ST_EXPECTING_WRITE_BYTE_3;
          break;
        case REG_T_HYST:
          chip->t_hyst_byte_hi = data;
          chip->state = ST_EXPECTING_WRITE_BYTE_3;
          break;
        default:
          chip->state = ST_EXPECTING_CONNECT;
          break;
      }
      break;
    case ST_EXPECTING_WRITE_BYTE_3:
      switch (chip->internal_address)
      {
        case REG_T_OS:
          chip->t_os_byte_lo = data;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        case REG_T_HYST:
          chip->t_hyst_byte_lo = data;
          chip->state = ST_EXPECTING_CONNECT;
          break;
        default:
          chip->state = ST_EXPECTING_CONNECT;
          break;
      }
      break;
    default:
      chip->state = ST_EXPECTING_CONNECT;
      break;
  }
  return true; // Ack
}
