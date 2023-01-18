#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define SLEEP_TIME_MS 1000

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main() {
    int ret;

    if (!device_is_ready(led.port))
        return 1;

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
        return 1;

    while (1) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0)
            return 1;

        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
