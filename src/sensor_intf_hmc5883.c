/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-06     MYGuo        the first version
 */

#include "sensor_intf_hmc5883.h"
#include "hmc5883.h"
#include <rtdbg.h>

static struct hmc5883_device *mag_dev;

static rt_err_t _hmc5883_init(struct rt_sensor_intf *intf)
{
    rt_uint8_t i2c_addr = (rt_uint32_t)(intf->user_data) & 0xff;

    mag_dev = hmc5883_init(intf->dev_name, i2c_addr);

    if (mag_dev == RT_NULL)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t _hmc5883_set_range(rt_sensor_t sensor, rt_uint8_t range)
{
    rt_uint8_t result;
    result = hmc5883_set_param(mag_dev, HMC5883_MAG_RANGE, range);
    
    if (result == RT_EOK) 
    {
        switch (range) 
        {
        case HMC5883_RANGE_0_88Ga:
            mag_dev->range_scale = 0.88f/2047.0f;
            break;
        case HMC5883_RANGE_1_3Ga:
            mag_dev->range_scale = 1.3f/2047.0f;
            break;
        case HMC5883_RANGE_1_9Ga:
            mag_dev->range_scale = 1.9f/2047.0f;
            break;
        case HMC5883_RANGE_2_5Ga:
            mag_dev->range_scale = 2.5f/2047.0f;
            break;
        case HMC5883_RANGE_4_0Ga:
            mag_dev->range_scale = 4.0f/2047.0f;
            break;
        case HMC5883_RANGE_4_7Ga:
            mag_dev->range_scale = 4.7f/2047.0f;
            break;
        case HMC5883_RANGE_5_6Ga:
            mag_dev->range_scale = 5.6f/2047.0f;
            break;
        case HMC5883_RANGE_8_1Ga:
            mag_dev->range_scale = 8.1f/2047.0f;
            break;
        }            
    }

    return result;
}

static rt_err_t _hmc5883_set_mode(rt_sensor_t sensor, rt_uint8_t mode)
{
    if (mode == RT_SENSOR_MODE_POLLING)
    {
        hmc5883_set_param(mag_dev, HMC5883_MAG_OPER_MODE, HMC5883_SINGLE_MEAS);
    }
    else
    {
        LOG_D("Unsupported mode, code is %d", mode);
        return -RT_ERROR;
    }
    return RT_EOK;
}

static rt_err_t _hmc5883_set_odr(rt_sensor_t sensor, rt_uint8_t odr)
{
    hmc5883_set_param(mag_dev, HMC5883_MAG_ODR, odr);

    return RT_EOK;
}

static rt_size_t _hmc5883_polling_get_data(rt_sensor_t sensor, struct rt_sensor_data *data)
{
    struct hmc5883_3axes mag_data;
    hmc5883_set_param(mag_dev, HMC5883_MAG_OPER_MODE, HMC5883_SINGLE_MEAS);
    if (hmc5883_get_mag_raw(mag_dev, &mag_data) != RT_EOK)
    {
        return 0;
    }

    data->type = RT_SENSOR_CLASS_MAG;
    data->data.mag.x = mag_data.x * mag_dev->range_scale * 1000;
    data->data.mag.y = mag_data.y * mag_dev->range_scale * 1000;
    data->data.mag.z = mag_data.z * mag_dev->range_scale * 1000;
    data->timestamp = rt_sensor_get_ts();

    return 1;
}

static rt_size_t _hmc5883_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    RT_ASSERT(buf);

    if (sensor->config.mode == RT_SENSOR_MODE_POLLING)
    {
        return _hmc5883_polling_get_data(sensor, buf);
    }
    else
        return 0;
}

static rt_err_t _hmc5883_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    rt_err_t result = RT_EOK;

    switch (cmd)
    {
    case RT_SENSOR_CTRL_GET_ID:
        *(uint8_t *)args = mag_dev->id;
        break;
    case RT_SENSOR_CTRL_SET_RANGE:
        result = _hmc5883_set_range(sensor, (rt_uint32_t)args & 0xff);
        break;
    case RT_SENSOR_CTRL_SET_ODR:
        result = _hmc5883_set_odr(sensor, (rt_uint32_t)args & 0xff);
        break;
    case RT_SENSOR_CTRL_SET_MODE:
        result = _hmc5883_set_mode(sensor, (rt_uint32_t)args & 0xff);
        break;
    case RT_SENSOR_CTRL_SELF_TEST:
        break;
    default:
        return -RT_ERROR;
    }
    return result;
}

static struct rt_sensor_ops sensor_ops =
{
    _hmc5883_fetch_data,
    _hmc5883_control
};

int rt_hw_hmc5883_init(const char *name, struct rt_sensor_config *cfg)
{
    rt_int8_t result;
    rt_sensor_t sensor_mag = RT_NULL;

    /* mag sensor register */
    {
        sensor_mag = rt_calloc(1, sizeof(struct rt_sensor_device));
        if (sensor_mag == RT_NULL)
            goto __exit;

        sensor_mag->info.type       = RT_SENSOR_CLASS_MAG;
        sensor_mag->info.vendor     = RT_SENSOR_VENDOR_UNKNOWN;
        sensor_mag->info.model      = "hmc5883";
        sensor_mag->info.unit       = RT_SENSOR_UNIT_MGAUSS;
        sensor_mag->info.intf_type  = RT_SENSOR_INTF_I2C;
        sensor_mag->info.range_max  = 8100;
        sensor_mag->info.range_min  = 880;
        sensor_mag->info.period_min = 6;

        rt_memcpy(&sensor_mag->config, cfg, sizeof(struct rt_sensor_config));
        sensor_mag->ops = &sensor_ops;

        result = rt_hw_sensor_register(sensor_mag, name, RT_DEVICE_FLAG_RDWR, RT_NULL);
        if (result != RT_EOK)
        {
            LOG_E("device register err code: %d", result);
            goto __exit;
        }
    }

    result = _hmc5883_init(&cfg->intf);
    if (result != RT_EOK)
    {
        LOG_E("hmc5883 init err code: %d", result);
        goto __exit;
    }

    LOG_I("sensor init success");
    return RT_EOK;

__exit:
    if (sensor_mag)
        rt_free(sensor_mag);
    if (mag_dev)
        hmc5883_deinit(mag_dev);
    return -RT_ERROR;
}
