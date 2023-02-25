#ifndef HTS221_H
#define HTS221_H

#include <stdint.h>
#include <zephyr/drivers/i2c.h>

#define HTS221_MULTIPLE_BYTES_READ 0b10000000

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
} hts221_av_conf_t;

typedef enum {
    HTS221_ODR_ONE_SHOT = 0x00,  // one shot mode
    HTS221_ODR_1_HZ = 0x01,      // 1 Hz
    HTS221_ODR_7_HZ = 0x02,      // 7 Hz
    HTS221_ODR_12_5_HZ = 0x03,   // 12.5 Hz
} hts221_odr_config_t;

/**
 * @brief Reads the content of the WHO_AM_I register.
 *
 * @param spec I2C specification from devicetree.
 * @param read_buf Pointer to the variable that stores the read data.
 * @return a value from i2c_write_read_dt().
 */
int hts221_read_whoami(const struct i2c_dt_spec *spec, uint8_t *read_buf);

/************************
 * Sensor Configuration *
 ************************/

/**
 * @brief Reads the AV_CONF register and return both temperature and humidity averaged samples configurations.
 *
 * @param spec I2C specification from devicetree.
 * @param temp_conf Pointer to the variable that stores the read temperature configuration.
 * @param humidity_conf Pointer to the variable that stores the read humidity configuration.
 * @return a value from i2c_reg_read_byte_dt().
 */
int hts221_read_av_conf(const struct i2c_dt_spec *spec, hts221_av_conf_t *temp_conf, hts221_av_conf_t *humidity_conf);

/**
 * @brief Sets the number of average sample for both temperature and humidify readings.
 *
 * @param spec I2C specification from devicetree.
 * @param temp_conf Average samples for the temperature readings.
 * @param humidity_conf Average samples for the humidity readings.
 * @return a value from i2c_reg_write_byte_dt().
 */
int hts221_set_av_conf(const struct i2c_dt_spec *spec, const hts221_av_conf_t temp_conf,
                       const hts221_av_conf_t humidity_conf);

/**
 * @brief Enables the sensor.
 *
 * @param spec I2C specification from devicetree.
 * @return a value from either i2c_reg_write_byte_dt() or i2c_reg_read_byte_dt().
 */
int hts221_enable(const struct i2c_dt_spec *spec);

/**
 * @brief Powers off the sensor.
 *
 * @param spec I2C specification from devicetree.
 * @return a value from either i2c_reg_write_byte_dt() or i2c_reg_read_byte_dt().
 */
int hts221_disable(const struct i2c_dt_spec *spec);

/**
 * @brief Sets the sensor output data rate (ODR).
 *
 * @param spec I2C specification from devicetree.
 * @param odr_conf ODR configuration code.
 * @return a value from i2c_write_dt().
 */
int hts221_set_odr(const struct i2c_dt_spec *spec, const hts221_odr_config_t odr_conf);

/**
 * @brief Reads the sensor output data rate.
 *
 * @param spec I2C specification from devicetree.
 * @param odr_conf ODR configuration code.
 * @return a value from i2c_reg_read_byte_dt().
 */
int hts221_read_odr(const struct i2c_dt_spec *spec, hts221_odr_config_t *odr_conf);

/**
 * @brief Sets the sensor
 *
 * @param spec I2C specification from devicetree.
 * @param continuous_update
 * @return int
 */
int hts221_set_bdu(const struct i2c_dt_spec *spec, const bool continuous_update);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param is_continuous_update
 * @return int
 */
int hts221_read_bdu(const struct i2c_dt_spec *spec, bool *is_continuous_update);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param enable
 * @return int
 */
int hts221_set_heater_status(const struct i2c_dt_spec *spec, const bool enable);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param is_enabled
 * @return int
 */
int hts221_read_heater_status(const struct i2c_dt_spec *spec, bool *is_enabled);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @return int
 */
int hts221_trigger_one_shot(const struct i2c_dt_spec *spec);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param active_low
 * @return int
 */
int hts221_config_data_ready(const struct i2c_dt_spec *spec, const bool active_low);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param enable
 * @return int
 */
int hts221_enable_data_ready(const struct i2c_dt_spec *spec, const bool enable);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param new_humidity_available
 * @param new_temp_available
 * @return int
 */
int hts221_read_status(const struct i2c_dt_spec *spec, bool *new_humidity_available, bool *new_temp_available);

/**
 * @brief
 *
 * @param spec I2C specification from devicetree.
 * @param av_conf
 * @param ctrl_reg1
 * @param ctrl_reg2
 * @param ctrl_reg3
 * @param status_reg
 * @return int
 */
int hts221_read_all_conf_reg(const struct i2c_dt_spec *spec, uint8_t *av_conf, uint8_t *ctrl_reg1, uint8_t *ctrl_reg2,
                             uint8_t *ctrl_reg3, uint8_t *status_reg);

/*******************
 * Data Conversion *
 *******************/

/**
 * @brief Reads the temperature registers from the sensors and return a temperature value.
 *
 * @param spec I2C specification from devicetree.
 * @param temperature Temperature value converted from the 2 temperature registers (in degree Celsius).
 * @return a value from i2c_write_read_dt().
 */
int hts221_read_temperature(const struct i2c_dt_spec *spec, float *temperature);

/**
 * @brief Reads the humidity registers from the sensors and return a humidity value.
 *
 * @param spec I2C specification from devicetree.
 * @param humidity Humidity value converted from the 2 humidity registers.
 * @return a value from i2c_write_read_dt().
 */
int hts221_read_humidity(const struct i2c_dt_spec *spec, float *humidity);

/**
 * @brief Reads both the temperature and humidity registes and return respective values.
 *
 * @param spec I2C specification from devicetree.
 * @param humidity Humidity value converted from the 2 humidity registers.
 * @param temperature Temperature value converted from the 2 temperature registers (in degree Celsius).
 * @return a value from i2c_write_read_dt().
 */
int hts221_read_all(const struct i2c_dt_spec *spec, float *humidity, float *temperature);

/**
 * @brief Read all the calibration coefficients.
 *
 * @details The user should call this function when initializing the sensor at startup. Read data is store privately and
 * not exposed to the user. The calibration coefficients are required to convert both temperature and humidity registers
 * value into floats.
 *
 * @param spec I2C specification from devicetree.
 * @return a value from i2c_write_read_dt().
 */
int hts221_read_calibration(const struct i2c_dt_spec *spec);

#endif
