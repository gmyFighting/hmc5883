# API 说明

在 RT-Thread 上编程，需要用到 hmc5883 3 轴磁力计传感器时，使用 hmc5883 软件包就可以轻松完成传感器的配置以及传感器数据的读取，本章介绍 hmc5883 软件包提供的常用 API。

### 初始化函数

```{.c}
struct hmc5883_device *hmc5883_init(const char *dev_name, rt_uint8_t param)
```

使用指定的iic通信设备名初始化hmc5883 ，并返回控制句柄。

| 参数              | 描述                                |
|:------------------|:------------------------------------|
|dev_name            | iic设备名 |
|param               | iic总线地址 |
| **返回**          | **描述**                                |
|struct hmc5883_device  *     | hmc5883_device 结构体的指针，它在调用 hmc5883_device 库的其他函数时使用 |
|RT_NULL                 | 失败                                |

### 反初始化函数

```{.c}
void hmc5883_deinit(struct hmc5883_device *dev);
```

释放 hmc5883 设备占据的内存空间

| 参数     | 描述                        |
| :------- | :-------------------------- |
| dev      | hmc5883_device 结构体的指针 |
| **返回** | **描述**                    |
| 无返回值 |                             |


### 读取磁力计原始数据

```{.c}
rt_err_t hmc5883_get_mag_raw(struct hmc5883_device *dev, struct hmc5883_3axes *mag);
```

数据量程转换请参考hmc5883数据手册Configuration Register B

| 参数     | 描述                                    |
| :------- | :-------------------------------------- |
| dev      | hmc5883_device 结构体的指针             |
| buf      | 存储 hmc5883 3轴数据结构体的指针        |
| **返回** | **描述**                                |
| rt_err_t | 读取数据结果                            |
|  0       | 读取成功                                |


