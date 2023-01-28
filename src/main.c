#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "config_hts221.h"
#include "config_log.h"
#include "events.h"
#include "hts221/hts221.h"

#define ENABLE_HTS221 DT_NODE_HAS_STATUS(DT_NODELABEL(hts221), okay)

LOG_MODULE_REGISTER(pcs_weather, LOG_LEVEL);

#define BLINK_THREAD_STACKSIZE 1024
#define HTS221_THREAD_STACKSIZE 1024
#define BLINK_THREAD_PRIORITY 4
#define HTS221_THREAD_PRIORITY 4

K_EVENT_DEFINE(events);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_cb_data;

#if ENABLE_HTS221
static const struct i2c_dt_spec hts221_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(hts221));
static const struct gpio_dt_spec hts221_drdy = GPIO_DT_SPEC_GET(DT_NODELABEL(hts221_drdy0), gpios);
static struct gpio_callback hts221_drdy_cb_data;
#endif

void blink_thread() {
    while (1) {
        k_event_wait(&events, EVENT_LED_BLINK, true, K_FOREVER);
        gpio_pin_toggle_dt(&led);
        k_msleep(100);
        gpio_pin_toggle_dt(&led);
    }
}

void hts221_thread() {
    int err;
    float humidity, temperature;
    // bool humidity_available, temp_available;
    uint32_t triggered_event;

    while (1) {
        k_event_wait(&events, EVENT_HTS221_READ_ALL, true, K_FOREVER);
        k_event_post(&events, EVENT_LED_BLINK);

#if ENABLE_HTS221
        LOG_ERR("DRDY_PIN value: %d", gpio_pin_get_dt(&hts221_drdy));
        err = hts221_start_one_shot_conversion(&hts221_i2c);
        if (err != 0) {
            LOG_DBG("Error %d: failed to start HTS221 (I2C@%x) one-shot conversion.", err, hts221_i2c.addr);
            continue;
        }
        LOG_DBG("Trigger HTS221 (I2C@%x) one shot conversion.", hts221_i2c.addr);

        triggered_event = k_event_wait(&events, EVENT_HTS221_DATA_READY, true, K_MSEC(5000));
        if (triggered_event == 0) {
            LOG_DBG("Error %d: no HTS221 (I2C@%x) data before TIMEOUT.", triggered_event, hts221_i2c.addr);
        }

        // hts221_read_status_reg(&hts221_i2c, &humidity_available, &temp_available);
        // LOG_INF("HTS221 (I2C@%x), humidity ready = %d, temp ready = %d", hts221_i2c.addr, humidity_available,
        //         temp_available);

        LOG_ERR("DRDY_PIN value: %d", gpio_pin_get_dt(&hts221_drdy));
        LOG_INF("HTS221 (I2C@%x), read new data. Events = 0x%x", hts221_i2c.addr, events.events);
        err = hts221_read_all(&hts221_i2c, &humidity, &temperature);
        if (err != 0) {
            LOG_DBG("Error %d: failed to read HTS221 (I2C@%x) data.", err, hts221_i2c.addr);
            continue;
        }
        LOG_INF("HTS221 (I2C@%x), humidity = %f, temperature = %f", hts221_i2c.addr, humidity, temperature);

        // hts221_read_status_reg(&hts221_i2c, &humidity_available, &temp_available);
        // LOG_INF("HTS221 (I2C@%x), humidity ready = %d, temp ready = %d", hts221_i2c.addr, humidity_available,
        //         temp_available);
#endif
    }
}

void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    k_event_post(&events, EVENT_HTS221_READ_ALL);
}

void hts221_drdy_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    LOG_WRN("HTS221_DRDY toggled. Status = %d", gpio_pin_get_dt(&hts221_drdy));
    k_event_post(&events, EVENT_HTS221_DATA_READY);
}

// ----------------------------------------------------------------------------
// MAIN -----------------------------------------------------------------------
// ----------------------------------------------------------------------------

int main() {
    int err;

    if (!device_is_ready(led.port)) {
        LOG_DBG("LED port %s not ready.", led.port->name);
        return 1;
    }

    if (!device_is_ready(button.port)) {
        LOG_DBG("Button port %s not ready.", button.port->name);
        return 1;
    }

#if ENABLE_HTS221
    if (!device_is_ready(hts221_i2c.bus)) {
        LOG_DBG("I2C bus %s is not ready!\n\r", hts221_i2c.bus->name);
        return 1;
    }
#endif

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (err < 0) {
        LOG_DBG("Error during LED 0 configuration.");
        return 1;
    }
    err = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (err < 0) {
        LOG_DBG("Error during button configuration.");
        return 1;
    }

    err = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (err < 0) {
        LOG_DBG("Error during button ISR configuration: %u.", err);
        return 1;
    }
    gpio_init_callback(&button_cb_data, button_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

#if ENABLE_HTS221
    LOG_ERR("DRDY_PIN value: %d", gpio_pin_get_dt(&hts221_drdy));

    err = gpio_pin_configure_dt(&hts221_drdy, GPIO_INPUT);
    if (err < 0) {
        LOG_DBG("Error during HTS221 DRDY pin configuration.");
        return 1;
    }

    err = gpio_pin_interrupt_configure_dt(&hts221_drdy, GPIO_INT_EDGE_BOTH);
    if (err < 0) {
        LOG_DBG("Error %u during HTS221 DRDY ISR configuration.", err);
        return 1;
    }
    gpio_init_callback(&hts221_drdy_cb_data, hts221_drdy_isr, BIT(hts221_drdy.pin));
    gpio_add_callback(hts221_drdy.port, &hts221_drdy_cb_data);
#endif

#if ENABLE_HTS221
    LOG_ERR("DRDY_PIN value: %d", gpio_pin_get_dt(&hts221_drdy));
    config_hts221(&hts221_i2c);
    LOG_ERR("DRDY_PIN value: %d", gpio_pin_get_dt(&hts221_drdy));
#endif

    return 0;
}

K_THREAD_DEFINE(blink_thread_id, BLINK_THREAD_STACKSIZE, blink_thread, NULL, NULL, NULL, BLINK_THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(hts221_thread_id, HTS221_THREAD_STACKSIZE, hts221_thread, NULL, NULL, NULL, HTS221_THREAD_PRIORITY, 0,
                0);
