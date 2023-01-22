#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pcs_weather, LOG_LEVEL_DBG);

#define SLEEP_TIME_MS 1000

// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_cb_data;
static const struct i2c_dt_spec hts221_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(hts221));

uint8_t read_buff[2] = {0};
const uint8_t hts221_whoAmI = 0x0f;

void pin_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    // int err = gpio_pin_toggle_dt(&led1);
    // if (err != 0) {
    //     LOG_DBG("Error %d: cannot toggle LED inside ISR.", err);
    // }
    int err = i2c_write_read_dt(&hts221_i2c, &hts221_whoAmI, 1, read_buff, 1);
    if (err != 0)
        LOG_DBG("Error %d: failed to write/read I2C device address %x at reg. %x n", err, hts221_i2c.addr,
                hts221_whoAmI);
    else
        LOG_INF("I2C %x device name: %x", hts221_i2c.addr, read_buff[0]);
}

void main() {
    int err;

    // if (!device_is_ready(led.port)) {
    //     LOG_DBG("LED port %s not ready.", led.port->name);
    //     return;
    // }
    if (!device_is_ready(button.port)) {
        LOG_DBG("Button port %s not ready.", button.port->name);
        return;
    }
    if (!device_is_ready(hts221_i2c.bus)) {
        LOG_DBG("I2C bus %s is not ready!\n\r", hts221_i2c.bus->name);
        return;
    }

    // err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    // if (err < 0) {
    //     LOG_DBG("Error during LED 0 configuration.");
    //     return;
    // }
    err = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (err < 0) {
        LOG_DBG("Error during button configuration.");
        return;
    }

    err = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (err < 0) {
        LOG_DBG("Error during button ISR configuration: %u", err);
        return;
    }
    gpio_init_callback(&button_cb_data, pin_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    err = i2c_write_read_dt(&hts221_i2c, &hts221_whoAmI, 1, read_buff, 1);
    if (err != 0)
        LOG_DBG("Failed to write/read I2C device address %x at Reg. %x n", hts221_i2c.addr, read_buff[0]);
    LOG_INF("I2C %x device name: %x", hts221_i2c.addr, read_buff[0]);

    // while (1) {
    //     // err = gpio_pin_toggle_dt(&led);
    //     // if (err < 0)
    //     //     return 1;

    //     k_msleep(SLEEP_TIME_MS * 100);
    // }

    // return;
}
