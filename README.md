# hmc5883

## 简介

本软件包是为 Honeywell 公司的三轴系列传感器提供的通用传感器驱动包,本软件包新的版本已经对接到了 Sensor 框架。

## 支持情况

| 包含设备         |  磁力计  | 
| ---------------- | -------- |
| **通讯接口**     |          |
| IIC              | √        |
| SPI              |          |
| **工作模式**     |          |
| 轮询             | √        |
| 中断             |          |
| FIFO             |          |
| **电源模式**     |          |
| 掉电             |          |
| 低功耗           |          |
| 普通             | √        |
| 高功耗           |          |
| **数据输出速率** | √        |
| **测量范围**     | √        |
| **自检**         | √        |
| **多实例**       | √        |

## 使用说明

### 依赖

- RT-Thread 4.0.0+
- Sensor 组件
- IIC/SPI 驱动：hmc5883 设备使用 IIC 进行数据通讯，需要系统 IIC驱动支持。

### 获取软件包

使用 hmc5883 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
hmc5883: hmc5883 Digital 3-axis sensor
        Version (latest)  --->
```

**Version**：软件包版本选择

### 使用软件包

hmc5883 软件包初始化函数如下所示：

```
rt_err_t rt_hw_hmc5883_init(const char *name, struct rt_sensor_config *cfg);
```

`rt_hw_hmc5883_init`函数需要由用户调用，函数主要完成的功能有，

- 对传感器相应IIC设备进行配置；
- 注册相应的传感器设备；
- 硬件初始化。


## 注意事项

暂无

## 联系人信息

维护人:

- [MyGuo](https://github.com/gmyFighting)
