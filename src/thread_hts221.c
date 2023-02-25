#include "thread_hts221.h"

#include "config_log.h"
#include "events.h"
#include "hts221/hts221.h"

extern struct k_event events;

void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    k_event_post(&events, EVENT_HTS221_READ_ALL);
}

void hts221_drdy_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    k_event_post(&events, EVENT_HTS221_DATA_READY);
}

int hts221_thread() {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);

    // Button
    const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
    struct gpio_callback button_cb_data;
    // HTS221
    const struct i2c_dt_spec hts221_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(hts221));
    const struct gpio_dt_spec hts221_drdy = GPIO_DT_SPEC_GET(DT_NODELABEL(hts221), drdy_gpios);
    struct gpio_callback hts221_drdy_cb_data;
    //
    int err;
    float humidity, temperature;
    uint32_t triggered_event;

    err = config_button(&button, &button_cb_data);
    if (err != 0)
        return err;

    /*
     * Without a temporary sleep, the I2C is not configured correctly and the sensor does not respond.
     * FIXME: investigate the reason for this beahaviour.
     */
    k_msleep(100);

    err = config_hts221(&hts221_i2c);
    if (err != 0)
        return err;

    err = config_hts221_int_pin(&hts221_drdy, &hts221_drdy_cb_data);
    if (err != 0)
        return err;

    /*
     * When the drdy_en pin is enable on the HTS221 sensor, it set active because some data is ready in the sensor's
     * registers. The pin is set to inactive from the sensor only when both humidity and temperature are read.
     *
     * If the pin is not set inactive before calling 'k_event_wait', the app will not work properly.
     */
    hts221_read_all(&hts221_i2c, &temperature, &humidity);

    while (1) {  // ---------------------------------------------------------------------------------------------------
        k_event_set_masked(&events, 0, EVENT_HTS221_READ_ALL | EVENT_HTS221_DATA_READY);  // Clear events before waiting
        k_event_wait(&events, EVENT_HTS221_READ_ALL, false, K_FOREVER);
        k_event_post(&events, EVENT_LED_BLINK);

        err = hts221_trigger_one_shot(&hts221_i2c);
        if (err != 0) {
            LOG_ERR("Error %d: failed to start HTS221 (I2C@%x) one-shot conversion.", err, hts221_i2c.addr);
            continue;
        }

        triggered_event = k_event_wait(&events, EVENT_HTS221_DATA_READY, false, K_MSEC(1000));
        if (triggered_event == 0) {
            LOG_ERR("Error %d: no HTS221 (I2C@%x) data before TIMEOUT.", triggered_event, hts221_i2c.addr);
            continue;
        }

        LOG_DBG("HTS221 (I2C@%x), read new data. Events = 0x%x", hts221_i2c.addr, events.events);
        err = hts221_read_all(&hts221_i2c, &temperature, &humidity);
        if (err != 0) {
            LOG_ERR("Error %d: failed to read HTS221 (I2C@%x) data.", err, hts221_i2c.addr);
            continue;
        }
        LOG_INF("HTS221 (I2C@%x), humidity = %f, temperature = %f", hts221_i2c.addr, humidity, temperature);
    }
}

int config_button(const struct gpio_dt_spec *button, struct gpio_callback *button_cb_data) {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);
    int err;

    if (!device_is_ready(button->port)) {
        LOG_ERR("Button port %s not ready.", button->port->name);
        return 1;
    }

    err = gpio_pin_configure_dt(button, GPIO_INPUT);
    if (err < 0) {
        LOG_ERR("Error during button configuration.");
        return 1;
    }

    err = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
    if (err < 0) {
        LOG_ERR("Error during button ISR configuration: %u.", err);
        return 1;
    }
    gpio_init_callback(button_cb_data, button_isr, BIT(button->pin));
    gpio_add_callback(button->port, button_cb_data);

    return 0;
}

int config_hts221_int_pin(const struct gpio_dt_spec *hts221_drdy, struct gpio_callback *hts221_drdy_cb_data) {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);
    int err;

    err = gpio_pin_configure_dt(hts221_drdy, GPIO_INPUT);
    if (err < 0) {
        LOG_ERR("Error during HTS221 DRDY pin configuration.");
        return 1;
    }

    err = gpio_pin_interrupt_configure_dt(hts221_drdy, GPIO_INT_EDGE_TO_ACTIVE);
    if (err < 0) {
        LOG_DBG("Error %u during HTS221 DRDY ISR configuration.", err);
        return 1;
    }
    gpio_init_callback(hts221_drdy_cb_data, hts221_drdy_isr, BIT(hts221_drdy->pin));
    gpio_add_callback(hts221_drdy->port, hts221_drdy_cb_data);

    return 0;
}

int config_hts221(const struct i2c_dt_spec *hts221_i2c) {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);

    if (!device_is_ready(hts221_i2c->bus)) {
        LOG_ERR("I2C bus %s is not ready!\n\r", hts221_i2c->bus->name);
        return 1;
    }

    int err = hts221_set_av_conf(hts221_i2c, HTS221_AVG_CONFIG_2, HTS221_AVG_CONFIG_2);
    if (err != 0) {
        LOG_DBG("Failed to write HTS221 (I2C@%x) avg configuration.", hts221_i2c->addr);
        return 1;
    }

    err = hts221_set_odr(hts221_i2c, HTS221_ODR_ONE_SHOT);
    if (err != 0) {
        LOG_DBG("Failed to set ODR config in HTS221 (I2C@%x).", hts221_i2c->addr);
        return 1;
    }

    err = hts221_set_bdu(hts221_i2c, false);
    if (err != 0) {
        LOG_DBG("Failed to set BDU config in HTS221 (I2C@%x).", hts221_i2c->addr);
        return 1;
    }

    err = hts221_enable_data_ready(hts221_i2c, true);
    if (err != 0) {
        LOG_DBG("Failed to enable DATA READY config in HTS221 (I2C@%x).", hts221_i2c->addr);
        return 1;
    }

    err = hts221_read_calibration(hts221_i2c);
    if (err != 0) {
        LOG_DBG("Failed to read HTS221 (I2C@%x) conversion coefficients.", hts221_i2c->addr);
        return 1;
    }
    LOG_INF("HTS221 (I2C@%x) conversion coefficients read correctly.", hts221_i2c->addr);

    err = hts221_enable(hts221_i2c);
    if (err != 0) {
        LOG_DBG("Failed to activate HTS221 (I2C@%x).", hts221_i2c->addr);
        return 1;
    }

#if DEBUG
    uint8_t av_conf_reg, ctrl_reg1, ctrl_reg2, ctrl_reg3, status_reg;
    err = hts221_read_all_conf_reg(hts221_i2c, &av_conf_reg, &ctrl_reg1, &ctrl_reg2, &ctrl_reg3, &status_reg);
    if (err != 0) {
        LOG_DBG("Failed to read HTS221 (I2C@%x) config registers.", hts221_i2c->addr);
        return 1;
    }
    LOG_INF("HTS221 (I2C@%x) configuration registers:", hts221_i2c->addr);
    LOG_INF("\tav_conf    = 0x%x", av_conf_reg);
    LOG_INF("\tctrl_reg1  = 0x%x", ctrl_reg1);
    LOG_INF("\tctrl_reg2  = 0x%x", ctrl_reg2);
    LOG_INF("\tctrl_reg3  = 0x%x", ctrl_reg3);
    LOG_INF("\tstatus_reg = 0x%x", status_reg);
#endif

    return 0;
}
