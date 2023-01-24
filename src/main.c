#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#if DEBUG
#define LOG_LEVEL LOG_LEVEL_DBG
#else
#define LOG_LEVEL LOG_LEVEL_WRN
#endif

#define ENABLE_HTS221 DT_NODE_HAS_STATUS(DT_NODELABEL(hts221), okay)

LOG_MODULE_REGISTER(pcs_weather, LOG_LEVEL);

#define BLINK_THREAD_STACKSIZE 1024
#define BLINK_THREAD_PRIORITY 4

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_cb_data;

#if ENABLE_HTS221
static const struct i2c_dt_spec hts221_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(hts221));
#endif

K_FIFO_DEFINE(work_fifo);

uint8_t read_buff[2] = {0};
const uint8_t hts221_whoAmI = 0x0f;

void blink_thread() {
    int irq_lock_key;

    while (1) {
        k_fifo_get(&work_fifo, K_FOREVER);

        irq_lock_key = irq_lock();

        LOG_DBG("Blink thread started");
        gpio_pin_toggle_dt(&led);
#if ENABLE_HTS221
        int err = i2c_write_read_dt(&hts221_i2c, &hts221_whoAmI, 1, read_buff, 1);
        if (err != 0)
            LOG_DBG("Error %d: failed to write/read I2C device address %x at reg. %x n", err, hts221_i2c.addr,
                    hts221_whoAmI);
        else
            LOG_INF("I2C %x device name: %x", hts221_i2c.addr, read_buff[0]);
#endif
        gpio_pin_toggle_dt(&led);
        LOG_DBG("Blink thread ended");

        irq_unlock(irq_lock_key);
    }
}

struct data_item_t {
    void *fifo_reserved; /* 1st word reserved for use by FIFO */
};

struct data_item_t work_data;

void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    k_fifo_put(&work_fifo, &work_data);
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
        LOG_DBG("Error during button ISR configuration: %u", err);
        return 1;
    }
    gpio_init_callback(&button_cb_data, button_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

#if ENABLE_HTS221
    err = i2c_write_read_dt(&hts221_i2c, &hts221_whoAmI, 1, read_buff, 1);
    if (err != 0)
        LOG_DBG("Failed to write/read I2C device address %x at Reg. %x n", hts221_i2c.addr, read_buff[0]);
    LOG_INF("I2C %x device name: %x", hts221_i2c.addr, read_buff[0]);
#endif

    return 0;
}

K_THREAD_DEFINE(work_thread, BLINK_THREAD_STACKSIZE, blink_thread, NULL, NULL, NULL, BLINK_THREAD_PRIORITY, 0, 0);
