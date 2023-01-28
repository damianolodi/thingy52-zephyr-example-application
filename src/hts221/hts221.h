#ifndef HTS221_H
#define HTS221_H

#include <stdint.h>
#include <zephyr/drivers/i2c.h>

typedef enum {
    HTS221_WHO_AM_I = 0x0f,        // r
    HTS221_AV_CONF = 0x10,         // r/w
    HTS221_CTRL_REG1 = 0x20,       // r/w
    HTS221_CTRL_REG2 = 0x21,       // r/w
    HTS221_CTRL_REG3 = 0x22,       // r/w
    HTS221_STATUS_REG = 0x27,      // r
    HTS221_HUMIDITY_OUT_L = 0x28,  // r
    HTS221_HUMIDITY_OUT_H = 0x29,  // r
    HTS221_TEMP_OUT_L = 0x2a,      // r
    HTS221_TEMP_OUT_H = 0x2b,      // r
    HTS221_CALIB_0 = 0x30,         // r/w
} hts221_reg_t;

typedef enum {                   // T[#]  RH[#] [nr. of internal average]
    HTS221_AVG_CONFIG_0 = 0x00,  //   2      4
    HTS221_AVG_CONFIG_1 = 0x01,  //   4      8
    HTS221_AVG_CONFIG_2 = 0x02,  //   8     16
    HTS221_AVG_CONFIG_3 = 0x03,  //  16     32
    HTS221_AVG_CONFIG_4 = 0x04,  //  32     64
    HTS221_AVG_CONFIG_5 = 0x05,  //  64    128
    HTS221_AVG_CONFIG_6 = 0x06,  // 128    256
    HTS221_AVG_CONFIG_7 = 0x07,  // 256    512
} hts221_avg_config_t;

typedef enum {
    HTS221_ODR_ONE_SHOT = 0x00,  // one shot mode
    HTS221_ODR_1_HZ = 0x01,      // 1 Hz
    HTS221_ODR_7_HZ = 0x02,      // 7 Hz
    HTS221_ODR_12_5_HZ = 0x03,   // 12.5 Hz
} hts221_odr_config_t;

int hts221_read_whoami(const struct i2c_dt_spec *spec, uint8_t *read_buf);

/************************
 * Sensor Configuration *
 ************************/

int hts221_read_avg_config(const struct i2c_dt_spec *spec, uint8_t *temp_conf, uint8_t *humidity_conf);

int hts221_set_avg_config(const struct i2c_dt_spec *spec, const hts221_avg_config_t temp_conf,
                          const hts221_avg_config_t humidity_conf);

int hts221_set_active_mode(const struct i2c_dt_spec *spec);

int hts221_set_power_down_mode(const struct i2c_dt_spec *spec);

int hts221_set_odr(const struct i2c_dt_spec *spec, const hts221_odr_config_t odr_conf);

int hts221_read_odr(const struct i2c_dt_spec *spec, hts221_odr_config_t *odr_conf);

int hts221_set_bdu(const struct i2c_dt_spec *spec, const bool continuous_update);

int hts221_read_bdu(const struct i2c_dt_spec *spec, bool *is_continuous_update);

int hts221_set_heater_status(const struct i2c_dt_spec *spec, const bool enable);

int hts221_read_heater_status(const struct i2c_dt_spec *spec, bool *is_enabled);

int hts221_start_one_shot_conversion(const struct i2c_dt_spec *spec);

int hts221_config_data_ready(const struct i2c_dt_spec *spec, const bool active_low);

int hts221_enable_data_ready(const struct i2c_dt_spec *spec, const bool enable);

int hts221_read_status_reg(const struct i2c_dt_spec *spec, bool *new_humidity_available, bool *new_temp_available);

int hts221_read_all_config_reg(const struct i2c_dt_spec *spec, uint8_t *av_conf, uint8_t *ctrl_reg1, uint8_t *ctrl_reg2,
                               uint8_t *ctrl_reg3, uint8_t *status_reg);

/*******************
 * Data Conversion *
 *******************/

int hts221_read_humidity(const struct i2c_dt_spec *spec, uint16_t *humidity);

int hts221_read_temperature(const struct i2c_dt_spec *spec, uint16_t *temperature);

int hts221_read_all(const struct i2c_dt_spec *spec, float *humidity, float *temperature);

int hts221_read_conversion_coeff(const struct i2c_dt_spec *spec);

#endif
