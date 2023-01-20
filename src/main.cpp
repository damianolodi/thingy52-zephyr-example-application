#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pcs_weather, LOG_LEVEL_INF);

#define SLEEP_TIME_MS 1000

#define LED0_NODE DT_ALIAS(led0)
#define HTS221_NODE DT_NODELABEL(hts221)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct i2c_dt_spec hts221_i2c = I2C_DT_SPEC_GET(HTS221_NODE);

uint8_t read_buff[2] = {0};
const uint8_t hts221_whoAmI = 0x0f;

int main() {
    int err;

    if (!device_is_ready(led.port)) {
        LOG_DBG("LED port not ready.");
        return 1;
    }
    if (!device_is_ready(hts221_i2c.bus)) {
        LOG_DBG("I2C bus %s is not ready!\n\r", hts221_i2c.bus->name);
        return 1;
    }

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (err < 0) {
        LOG_DBG("Error during LED configuration.");
        return 1;
    }

    while (1) {
        err = gpio_pin_toggle_dt(&led);
        if (err < 0)
            return 1;

        err = i2c_write_read_dt(&hts221_i2c, &hts221_whoAmI, 1, read_buff, 1);
        if (err != 0)
            LOG_DBG("Failed to write/read I2C device address %x at Reg. %x n", hts221_i2c.addr, read_buff[0]);
        LOG_INF("I2C %x device name: %x", hts221_i2c.addr, read_buff[0]);

        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
