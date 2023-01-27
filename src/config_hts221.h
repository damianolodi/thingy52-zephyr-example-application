#ifndef CONFIG_HTS221_H
#define CONFIG_HTS221_H

#include <zephyr/logging/log.h>

#include "config_log.h"
#include "hts221/hts221.h"

void config_hts221(const struct i2c_dt_spec *hts221_i2c) {
    LOG_MODULE_DECLARE(pcs_weather, LOG_LEVEL);

    int err = hts221_set_avg_config(hts221_i2c, HTS221_AVG_CONFIG_2, HTS221_AVG_CONFIG_2);
    if (err != 0)
        LOG_DBG("Failed to write HTS221 (I2C@%x) avg configuration.", hts221_i2c->addr);

    uint8_t temp_conf, humidity_conf;
    err = hts221_read_avg_config(hts221_i2c, &temp_conf, &humidity_conf);
    if (err != 0)
        LOG_DBG("Failed to read HTS221 (I2C@%x) avg configuration.", hts221_i2c->addr);
    LOG_INF("HTS221 (I2C@%x) configuration: T avg_config =  %x, RH avg config = %x.", hts221_i2c->addr, temp_conf,
            humidity_conf);

    err = hts221_set_odr(hts221_i2c, HTS221_ODR_ONE_SHOT);
    if (err != 0)
        LOG_DBG("Failed to set ODR config in HTS221 (I2C@%x).", hts221_i2c->addr);

    err = hts221_set_bdu(hts221_i2c, false);
    if (err != 0)
        LOG_DBG("Failed to set BDU config in HTS221 (I2C@%x).", hts221_i2c->addr);

    err = hts221_enable_data_ready(hts221_i2c, true);
    if (err != 0)
        LOG_DBG("Failed to enable DATA READY config in HTS221 (I2C@%x).", hts221_i2c->addr);

    err = hts221_read_conversion_coeff(hts221_i2c);
    if (err != 0)
        LOG_DBG("Failed to read HTS221 (I2C@%x) conversion coefficients.", hts221_i2c->addr);
    LOG_INF("HTS221 (I2C@%x) conversion coefficients read correctly.", hts221_i2c->addr);

#if DEBUG
    uint8_t av_conf_reg, ctrl_reg1, ctrl_reg2, ctrl_reg3, status_reg;
    err = hts221_read_all_config_reg(hts221_i2c, &av_conf_reg, &ctrl_reg1, &ctrl_reg2, &ctrl_reg3, &status_reg);
    if (err != 0)
        LOG_DBG("Failed to read HTS221 (I2C@%x) config registers.", hts221_i2c->addr);
    LOG_INF("HTS221 (I2C@%x) configuration registers:", hts221_i2c->addr);
    LOG_INF("\tav_conf    = %x", av_conf_reg);
    LOG_INF("\tctrl_reg1  = %x", ctrl_reg1);
    LOG_INF("\tctrl_reg2  = %x", ctrl_reg2);
    LOG_INF("\tctrl_reg3  = %x", ctrl_reg3);
    LOG_INF("\tstatus_reg = %x", status_reg);
#endif
}

#endif
