#include "hts221.h"

struct Hts221_Conv_Coeff {
    uint16_t t0_degC_x8;
    uint16_t t1_degC_x8;

    int16_t t0_out;
    int16_t t1_out;

    float t_m;
    float t_q;
};

static struct Hts221_Conv_Coeff conv_coeff;

int hts221_read_whoami(const struct i2c_dt_spec *spec, uint8_t *read_buf) {
    const hts221_reg_t reg = HTS221_WHO_AM_I;
    return i2c_write_read_dt(spec, &reg, 1, read_buf, 1);
}

/************************
 * Sensor Configuration *
 ************************/

int hts221_read_avg_config(const struct i2c_dt_spec *spec, uint8_t *temp_conf, uint8_t *humidity_conf) {
    const hts221_reg_t reg = HTS221_AV_CONF;
    uint8_t answer;
    int err = i2c_write_read_dt(spec, &reg, 1, &answer, 1);
    if (err != 0)
        return err;

    *temp_conf = (answer & 0b00111000) >> 3;
    *humidity_conf = answer & 0b00000111;

    return 0;
}

int hts221_set_avg_config(const struct i2c_dt_spec *spec, const hts221_avg_config_t temp_conf,
                          const hts221_avg_config_t humidity_conf) {
    const uint8_t regs[2] = {HTS221_AV_CONF, (temp_conf << 3) | humidity_conf};
    return i2c_write_dt(spec, regs, 2);
}

int hts221_set_active_mode(const struct i2c_dt_spec *spec) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    return i2c_reg_write_byte_dt(spec, HTS221_CTRL_REG1, (ctrl_reg1_value & 0b01111111) | 0b10000000);
}

int hts221_set_power_down_mode(const struct i2c_dt_spec *spec) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    return i2c_reg_write_byte_dt(spec, HTS221_CTRL_REG1, ctrl_reg1_value & 0b01111111);
}

int hts221_set_odr(const struct i2c_dt_spec *spec, const hts221_odr_config_t odr_conf) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    const uint8_t regs[2] = {HTS221_CTRL_REG1, (ctrl_reg1_value & 0b11111100) | odr_conf};
    return i2c_write_dt(spec, regs, 2);
}

int hts221_read_odr(const struct i2c_dt_spec *spec, hts221_odr_config_t *odr_conf) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    *odr_conf = ctrl_reg1_value & 0b00000011;
    return 0;
}

int hts221_set_bdu(const struct i2c_dt_spec *spec, const bool continuous_update) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    const uint8_t bdu_mask = continuous_update ? 0b00000000 : 0b00000100;

    const uint8_t regs[2] = {HTS221_CTRL_REG1, (ctrl_reg1_value & 0b11111011) | bdu_mask};
    return i2c_write_dt(spec, regs, 2);
}

int hts221_read_bdu(const struct i2c_dt_spec *spec, bool *is_continuous_update) {
    uint8_t ctrl_reg1_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, &ctrl_reg1_value);
    if (err != 0)
        return err;

    *is_continuous_update = (ctrl_reg1_value & 0b00000100) ? false : true;
    return 0;
}

int hts221_set_heater_status(const struct i2c_dt_spec *spec, const bool enable) {
    uint8_t ctrl_reg2_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG2, &ctrl_reg2_value);
    if (err != 0)
        return err;

    const uint8_t heater_mask = enable ? 0b00000010 : 0b00000000;

    const uint8_t regs[2] = {HTS221_CTRL_REG2, (ctrl_reg2_value & 0b11111101) | heater_mask};
    return i2c_write_dt(spec, regs, 2);
}

int hts221_read_heater_status(const struct i2c_dt_spec *spec, bool *is_enabled) {
    uint8_t ctrl_reg2_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG2, &ctrl_reg2_value);
    if (err != 0)
        return err;

    *is_enabled = (ctrl_reg2_value & 0b00000010) ? true : false;
    return 0;
}

int hts221_start_one_shot_conversion(const struct i2c_dt_spec *spec) {
    return i2c_reg_write_byte_dt(spec, HTS221_CTRL_REG2, 0b00000001);
}

int hts221_config_data_ready(const struct i2c_dt_spec *spec, const bool active_low) {
    uint8_t ctrl_reg3_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG3, &ctrl_reg3_value);
    if (err != 0)
        return err;

    const uint8_t drdy_mask = active_low ? 0b10000000 : 0b00000000;
    return i2c_reg_write_byte_dt(spec, HTS221_CTRL_REG3, (ctrl_reg3_value & 0b01111111) | drdy_mask);
}

int hts221_enable_data_ready(const struct i2c_dt_spec *spec, const bool enable) {
    uint8_t ctrl_reg3_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG3, &ctrl_reg3_value);
    if (err != 0)
        return err;

    const uint8_t drdy_enable_mask = enable ? 0b00000100 : 0b00000000;
    return i2c_reg_write_byte_dt(spec, HTS221_CTRL_REG3, (ctrl_reg3_value & 0b11111011) | drdy_enable_mask);
}

int hts221_read_status_reg(const struct i2c_dt_spec *spec, bool *new_humidity_available, bool *new_temp_available) {
    uint8_t status_reg_value;
    int err = i2c_reg_read_byte_dt(spec, HTS221_STATUS_REG, &status_reg_value);
    if (err != 0)
        return err;

    *new_humidity_available = (status_reg_value & 0b00000010) >> 1;
    *new_temp_available = status_reg_value & 0b00000001;
    return 0;
}

int hts221_read_all_config_reg(const struct i2c_dt_spec *spec, uint8_t *av_conf, uint8_t *ctrl_reg1, uint8_t *ctrl_reg2,
                               uint8_t *ctrl_reg3, uint8_t *status_reg) {
    int err = i2c_reg_read_byte_dt(spec, HTS221_AV_CONF, av_conf);
    if (err != 0)
        return err;

    err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, ctrl_reg1);
    if (err != 0)
        return err;

    err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, ctrl_reg2);
    if (err != 0)
        return err;

    err = i2c_reg_read_byte_dt(spec, HTS221_CTRL_REG1, ctrl_reg3);
    if (err != 0)
        return err;

    err = i2c_reg_read_byte_dt(spec, HTS221_STATUS_REG, status_reg);
    if (err != 0)
        return err;

    return 0;
}

/*******************
 * Data Conversion *
 *******************/

int hts221_read_humidity(const struct i2c_dt_spec *spec, uint16_t *humidity) {
    const hts221_reg_t reg = HTS221_HUMIDITY_OUT_L | 0b10000000;
    uint8_t buffer[2];
    const int err = i2c_burst_read_dt(spec, reg, buffer, 2);
    if (err != 0)
        return err;
    *humidity = buffer[1] << 8 | buffer[0];
    return 0;
}

int hts221_read_temperature(const struct i2c_dt_spec *spec, uint16_t *temperature) {
    const hts221_reg_t reg = HTS221_TEMP_OUT_L | 0b10000000;
    uint8_t buffer[2];
    const int err = i2c_burst_read_dt(spec, reg, buffer, 2);
    if (err != 0)
        return err;
    *temperature = buffer[1] << 8 | buffer[0];
    return 0;
}

int hts221_read_all(const struct i2c_dt_spec *spec, float *humidity, float *temperature) {
    const hts221_reg_t reg = HTS221_HUMIDITY_OUT_L | 0b10000000;
    uint8_t buffer[4];
    const int err = i2c_write_read_dt(spec, &reg, 1, buffer, 4);
    if (err != 0)
        return err;
    *humidity = buffer[1] << 8 | buffer[0];

    const int16_t temp_x8 = (buffer[3] << 8) | buffer[2];

    *temperature = (float)temp_x8 * conv_coeff.t_m + conv_coeff.t_q;
    *temperature /= 8.f;
    return 0;
}

int hts221_read_conversion_coeff(const struct i2c_dt_spec *spec) {
    const hts221_reg_t reg = HTS221_CALIB_0 | 0b10000000;
    uint8_t buffer[16];
    const int err = i2c_write_read_dt(spec, &reg, 1, buffer, 16);
    if (err != 0)
        return err;

    conv_coeff.t0_degC_x8 = ((buffer[5] & 0b00000011) << 8) | buffer[2];
    conv_coeff.t1_degC_x8 = ((buffer[5] & 0b00001100) << 6) | buffer[3];
    conv_coeff.t0_out = (buffer[13] << 8) | buffer[12];
    conv_coeff.t1_out = (buffer[15] << 8) | buffer[14];

    conv_coeff.t_m = (float)(conv_coeff.t1_degC_x8 - conv_coeff.t0_degC_x8) /  //
                     (float)(conv_coeff.t1_out - conv_coeff.t0_out);
    conv_coeff.t_q = (float)(conv_coeff.t1_degC_x8) - (float)conv_coeff.t1_out * conv_coeff.t_m;

    return 0;
}
