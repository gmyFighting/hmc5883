/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2020-02-28     MyGuo       the first version
 */
 
#include "hmc5883.h"
#include <rtdbg.h> // LOG_E
#include <rtdevice.h> // spi iic
#include <board.h> // GET_PIN

/**
 * This function writes the value of the register for hmc5883
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883
 * @param data value to write
 *
 * @return the writing status, RT_EOK represents writing the value of the register successfully.
 */
static rt_err_t _hmc5883_write_reg(struct hmc5883_device *dev, rt_uint8_t reg, rt_uint8_t data)
{
    rt_int8_t res = 0;
    rt_uint8_t tmp[2];
    struct rt_i2c_msg msg;
    tmp[0] = reg;
    tmp[1] = data;
    
    if (dev->bus->type == RT_Device_Class_I2CBUS)
    {
        msg.addr  = dev->i2c_addr;    /* slave address */
        msg.flags = RT_I2C_WR;        /* write flag */
        msg.buf   = tmp;              /* Send data pointer */
        msg.len   = 2;
        
        if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, &msg, 1) == 1)
        {
            res = RT_EOK;
        }
        else
        {
            res = -RT_ERROR;
        }
    }
    return res;
}

/**
 * This function reads the value of registers for hmc5883
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883
 * @param len number of register
 * @param buf read data pointer
 *
 * @return the reading status, RT_EOK represents reading the value of registers successfully.
 */
static rt_err_t _hmc5883_read_regs(struct hmc5883_device *dev, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)
{
    rt_int8_t res = 0;
    struct rt_i2c_msg msgs[2];
    if (dev->bus->type == RT_Device_Class_I2CBUS)
    {
        msgs[0].addr  = dev->i2c_addr;    /* Slave address */
        msgs[0].flags = RT_I2C_WR;        /* Write flag */
        msgs[0].buf   = &reg;             /* Slave register address */
        msgs[0].len   = 1;                /* Number of bytes sent */

        msgs[1].addr  = dev->i2c_addr;    /* Slave address */
        msgs[1].flags = RT_I2C_RD;        /* Read flag */
        msgs[1].buf   = buf;              /* Read data pointer */
        msgs[1].len   = len;              /* Number of bytes read */

        if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, msgs, 2) == 2)
        {
            res = RT_EOK;
        }
        else
        {
            res = -RT_ERROR;
        }
    }
    return res;
}

/**
 * This function writes multi-bit value of registers for bmi088
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for bmi088
 * @param start_bit the start position of the register
 * @param len number of bits to write
 * @param data value to write
 *
 * @return the writing status, RT_EOK represents writing multi-bit value of registers successfully.
 */
static rt_err_t _hmc5883_write_bits(struct hmc5883_device *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len, rt_uint8_t data)
{
    rt_uint8_t byte, mask;
    rt_err_t res;

    res = _hmc5883_read_regs(dev, reg, 1, &byte);
    if (res != RT_EOK)
    {
        return res;
    }

    mask = ((1 << len) - 1) << (start_bit - len + 1);
    data <<= (start_bit - len + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    byte &= ~(mask); // zero all important bits in existing byte
    byte |= data; // combine data with existing byte

    return _hmc5883_write_reg(dev, reg, byte);
}

/**
 * This function reads multi-bit value of registers for hmc5883
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883
 * @param start_bit the start position of the register
 * @param len number of bits to write
 * @param data read data pointer
 *
 * @return the reading status, RT_EOK reprensents  reading multi-bit value of registers successfully.
 */
static rt_err_t _hmc5883_read_bits(struct hmc5883_device *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len, rt_uint8_t *data)
{
    rt_uint8_t byte, mask;
    rt_err_t res;

    res = _hmc5883_read_regs(dev, reg, 1, &byte);
    if (res != RT_EOK)
    {
        return res;
    }

    mask = ((1 << len) - 1) << (start_bit - len + 1);
    byte &= mask;
    byte >>= (start_bit - len + 1);
    *data = byte;

    return RT_EOK;
}

/**
 * This function gets the raw data of the mag
 *
 * @param dev the pointer of device driver structure
 * @param mag the pointer of 3axes structure for receive data
 *
 * @return the reading status, RT_EOK represents reading the data successfully.
 */
rt_err_t hmc5883_get_mag_raw(struct hmc5883_device *dev, struct hmc5883_3axes *mag)
{
    rt_uint8_t buffer[10];
    rt_err_t res;

    res = _hmc5883_read_regs(dev, 0x03, 6, buffer);
    if (res != RT_EOK)
    {
        return res;
    }

    mag->x = (rt_int16_t)(buffer[1] | ((rt_int16_t)buffer[0] << 8));
    mag->y = (rt_int16_t)(buffer[5] | ((rt_int16_t)buffer[4] << 8));
    mag->z = (rt_int16_t)(buffer[3] | ((rt_int16_t)buffer[2] << 8));
    return RT_EOK;
}

/**
 * This function gets hmc5883 parameters.
 *
 * @param dev the pointer of device driver structure
 * @param cmd Configuration item
 * @param param read data pointer
 *
 * @return the reading status, RT_EOK represents reading the data successfully.
 */
rt_err_t hmc5883_get_param(struct hmc5883_device *dev, enum hmc5883_cmd cmd, rt_uint8_t *param)
{
    rt_uint8_t data = 0;
    rt_err_t res = RT_EOK;

    RT_ASSERT(dev);

    switch (cmd)
    {
    case HMC5883_MAG_RANGE:  
        res = _hmc5883_read_bits(dev, 0x01, 5, 3, &data);
        *param = data;
        break;
    case HMC5883_MAG_ODR: 
        res = _hmc5883_read_bits(dev, 0x00, 2, 3, &data);
        *param = data;
        break;
    case HMC5883_MAG_OPER_MODE: 
        res = _hmc5883_read_bits(dev, 0x02, 0, 2, &data);
        *param = data;
        break;
    case HMC5883_MAG_MEAS_MODE: 
        res = _hmc5883_read_bits(dev, 0x00, 5, 2, &data);
        *param = data;       
        break;
    }

    return res;
}

/**
 * This function set hmc5883 parameters.
 *
 * @param dev the pointer of device driver structure
 * @param cmd Configuration item
 * @param param Configuration item parameter
 *
 * @return the setting status, RT_EOK reprensents  setting the parameter successfully.
 */
rt_err_t hmc5883_set_param(struct hmc5883_device *dev, enum hmc5883_cmd cmd, rt_uint8_t param)
{
    rt_err_t res = RT_EOK;

    RT_ASSERT(dev);

    switch (cmd)
    {
    case HMC5883_MAG_RANGE:  
        res = _hmc5883_write_bits(dev, 0x01, 5, 3, param);
        dev->config.range = param;
        break;
    case HMC5883_MAG_ODR: 
        res = _hmc5883_write_bits(dev, 0x00, 2, 3, param);
        dev->config.odr = param;
        break;
    case HMC5883_MAG_OPER_MODE: 
        res = _hmc5883_write_bits(dev, 0x02, 1, 2, param);
        dev->config.oprt_mode = param;
        break;
    case HMC5883_MAG_MEAS_MODE: 
        res = _hmc5883_write_bits(dev, 0x00, 5, 2, param);
        dev->config.meas_mode = param;
        break;
    }

    return res;
}

rt_err_t _hmc5883_test_self(struct hmc5883_device *dev)
{
    rt_uint8_t data[3] = {0};
    _hmc5883_read_regs(dev, 0x0A, 3, data);
    
	if (data[0] == 0x48 && data[1] == 0x34 && data[2] == 0x33) 
    {
		return RT_EOK;
	}
    else
    {
        return -RT_ERROR;
    }
}  
    
/**
 * This function initialize the hmc5883 device.
 *
 * @param dev_name the name of transfer device
 * @param param the i2c device address for i2c communication
 *
 * @return the pointer of device driver structure, RT_NULL reprensents  initialization failed.
 */
struct hmc5883_device *hmc5883_init(const char *dev_name, rt_uint8_t param)
{
    struct hmc5883_device *dev = RT_NULL;
    rt_uint8_t res = RT_EOK;

    RT_ASSERT(dev_name);

    dev = rt_calloc(1, sizeof(struct hmc5883_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for hmc5883 device on '%s' ", dev_name);
        goto __exit;
    }

    dev->bus = rt_device_find(dev_name);
    if (dev->bus == RT_NULL)
    {
        LOG_E("Can't find device:'%s'", dev_name);
        goto __exit;
    }

    if (dev->bus->type == RT_Device_Class_I2CBUS)
    {
        if (param != RT_NULL)
        {
            dev->i2c_addr = param;
        }
        else
        {
            dev->i2c_addr = HMC5883_ADDR;
            LOG_D("Device i2c address is:'0x%x'!", dev->i2c_addr);
        }
    }
    else
    {
        LOG_E("Unsupported device:'%s'!", dev_name);
        goto __exit;
    }
    
    if (_hmc5883_test_self(dev) != RT_EOK)
    {
        LOG_E("hmc5883 test self fail!");
        goto __exit;    
    }  
    
    hmc5883_set_param(dev, HMC5883_MAG_RANGE, HMC5883_RANGE_1_9Ga);

    if (res == RT_EOK)
    {
        LOG_I("Device init succeed!");
    }
    else
    {
        LOG_W("Error in device initialization!");
    }
    return dev;

__exit:
    if (dev != RT_NULL)
    {
        rt_free(dev);
    }
    return RT_NULL;
}

/**
 * This function releases memory
 *
 * @param dev the pointer of device driver structure
 */
void hmc5883_deinit(struct hmc5883_device *dev)
{
    RT_ASSERT(dev);

    rt_free(dev);
}

