#ifndef __HMC5883_H__
#define __HMC5883_H__

#include <rtthread.h>
#include "sensor.h"

#define HMC5883_ADDR   0x1E  

/* Supported configuration items */
enum hmc5883_cmd
{
    HMC5883_MAG_RANGE = 0,      /* Mag full scale range */
    HMC5883_MAG_ODR = 1,        /* Output data rate(only in continuous mode) */
    HMC5883_MAG_OPER_MODE = 2,  /* Continuous or single measurement mode */
    HMC5883_MAG_MEAS_MODE = 3   /* number of samples averaged (1 to 8) per
                               measurement output */
};

enum hmc5883_range
{
    HMC5883_RANGE_0_88Ga  = 0, // ¡À0.88G Gain:1370
    HMC5883_RANGE_1_3Ga   = 1, // ¡À1.3G  Gain:1090
    HMC5883_RANGE_1_9Ga   = 2, // ¡À1.9G  Gain:820
    HMC5883_RANGE_2_5Ga   = 3, // ¡À2.5G  Gain:660
    HMC5883_RANGE_4_0Ga   = 4, // ¡À4.0G  Gain:440
    HMC5883_RANGE_4_7Ga   = 5, // ¡À4.7G  Gain:390
    HMC5883_RANGE_5_6Ga   = 6, // ¡À5.6G  Gain:330
    HMC5883_RANGE_8_1Ga   = 7  // ¡À8.1G  Gain:230
};

enum hmc5883_odr
{
    HMC5883_ODR_0_75Hz   = 0, 
    HMC5883_ODR_1_5Hz    = 1, 
    HMC5883_ODR_3_0Hz    = 2, 
    HMC5883_ODR_7_5Hz    = 3, 
    HMC5883_ODR_15Hz     = 4, 
    HMC5883_ODR_30Hz     = 5, 
    HMC5883_ODR_75Hz     = 6, 
    HMC5883_ODR_Not_Used = 7  
};

enum hmc5883_meas_mode
{
    HMC5883_SAMPLES_AVER_1 = 0, 
    HMC5883_SAMPLES_AVER_2 = 1, 
    HMC5883_SAMPLES_AVER_4 = 2, 
    HMC5883_SAMPLES_AVER_8 = 3
};

enum hmc5883_oper_mode
{
    HMC5883_CONTINUOUS_MEAS = 0, 
    HMC5883_SINGLE_MEAS     = 1
};

/* 3-axis data structure */
struct hmc5883_3axes
{
    rt_int16_t x;
    rt_int16_t y;
    rt_int16_t z;
};

struct hmc5883_config
{
    rt_uint16_t range;
    rt_uint16_t odr;
    rt_uint8_t meas_mode;
    rt_uint8_t oprt_mode;
    
};

/* hmc5883 device structure */
struct hmc5883_device
{
    rt_device_t bus;
    rt_uint8_t id;
    rt_uint8_t i2c_addr;
    struct hmc5883_config config;
    float range_scale;
};

struct hmc5883_device *hmc5883_init(const char *dev_name, rt_uint8_t param);
void hmc5883_deinit(struct hmc5883_device *dev);
rt_err_t hmc5883_get_param(struct hmc5883_device *dev, enum hmc5883_cmd cmd, rt_uint8_t *param);
rt_err_t hmc5883_set_param(struct hmc5883_device *dev, enum hmc5883_cmd cmd, rt_uint8_t param);
rt_err_t hmc5883_get_mag_raw(struct hmc5883_device *dev, struct hmc5883_3axes *mag);
    
#endif // __HMC5883_H__
