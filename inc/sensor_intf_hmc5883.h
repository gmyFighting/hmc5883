/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-06     MYGuo        the first version
 */

#ifndef __SENSOR_INTF_HMC5883_H__
#define __SENSOR_INTF_HMC5883_H__

#include "sensor.h"
#include "hmc5883.h"

int rt_hw_hmc5883_init(const char *name, struct rt_sensor_config *cfg);

#endif
