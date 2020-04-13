/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2020-03-10     MyGuo       the first version
 */
 
#include <rtthread.h>
#include <rtdbg.h> // LOG_E
#include "hmc5883.h"
#include "sensor_intf_hmc5883.h"
#include "hmc5883_sample.h"

struct hmc5883_device *mag;

void mag_thread_entry(void* parameter)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.dev_name = "i2c1";
    cfg.intf.user_data = (void *)HMC5883_ADDR;
    
    rt_hw_hmc5883_init("hmc", &cfg);
    
    while (1)
    {
        rt_thread_delay(1000);
    
    
    }
    
}
