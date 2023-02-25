#ifndef THREAD_HTS221_H
#define THREAD_HTS221_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

extern struct k_event events;

/**
 * @brief Button ISR callback function.
 */
void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/**
 * @brief HTS221 data ready pin ISR callback function.
 */
void hts221_drdy_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

/**
 * @brief Main entry point for the thread managing readings from the HTS221 sensor.
 */
int hts221_thread();

/**
 * @brief Configures the button pin and ISR callback.
 */
int config_button(const struct gpio_dt_spec *button, struct gpio_callback *button_cb_data);

/**
 * @brief Configures HTS221 data ready pin and ISR callback.
 */
int config_hts221_int_pin(const struct gpio_dt_spec *hts221_drdy, struct gpio_callback *hts221_drdy_cb_data);

/**
 * @brief Configures the HTS221 sensor.
 */
int config_hts221(const struct i2c_dt_spec *hts221_i2c);

#endif
