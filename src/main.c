#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "config_log.h"
#include "events.h"
#include "thread_hts221.h"
#include "thread_led.h"

LOG_MODULE_REGISTER(pcs_weather, LOG_LEVEL);

#define BLINK_THREAD_STACKSIZE 1024
#define HTS221_THREAD_STACKSIZE 1024
#define BLINK_THREAD_PRIORITY 4
#define HTS221_THREAD_PRIORITY 4

K_EVENT_DEFINE(events);

K_THREAD_DEFINE(blink_thread_id, BLINK_THREAD_STACKSIZE, blink_thread, NULL, NULL, NULL, BLINK_THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(hts221_thread_id, HTS221_THREAD_STACKSIZE, hts221_thread, NULL, NULL, NULL, HTS221_THREAD_PRIORITY, 0,
                0);
