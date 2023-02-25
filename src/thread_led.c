#include "thread_led.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "events.h"

extern struct k_event events;

int blink_thread() {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);

    const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
    int err;

    if (!device_is_ready(led.port)) {
        LOG_ERR("LED port %s not ready.", led.port->name);
        return 1;
    }

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (err < 0) {
        LOG_ERR("Error during LED 0 configuration.");
        return 1;
    }

    while (1) {  // ---------------------------------------------------------------------------------------------------
        k_event_set_masked(&events, 0, EVENT_LED_BLINK);  // Clear event before waiting
        k_event_wait(&events, EVENT_LED_BLINK, false, K_FOREVER);

        gpio_pin_toggle_dt(&led);
        k_msleep(100);
        gpio_pin_toggle_dt(&led);
    }

    return 0;
}
