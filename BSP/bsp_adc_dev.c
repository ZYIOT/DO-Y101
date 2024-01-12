#include "bsp_adc_dev.h"
#include "ADC/ltc2450.h"
#include "hardware_wrapper.h"
#include "bsp_log.h"
#include "utils.h"
// 实际使用的芯片是 MS1100 用来替代 LTC2451 
#define BSP_ADC_DEVICE_ID   (LTC2450_A0_B0_C0)
#define BSP_ADC_TRY_TIMES   (3)

typedef LTC2450_FUN_TypeDef BSP_ADC_FUN_TypeDef;

static GPIO_IIC_TypeDef gpio_iic = {
    .sda_gpio = IIC_SDA_GPIO_Port,
    .sda_pin = IIC_SDA_Pin,
    .scl_gpio = IIC_SCL_GPIO_Port,
    .scl_pin = IIC_SCL_Pin,
};

GPIO_LTC2450_TypeDef bsp_ltc2450 = {
    .handle = &gpio_iic,
    .fun = &LTC2450_Fun,
    .address_type = ADDRESS_TYPE_8_BIT,
};

static uint8_t _adc_dev_WriteBytes(uint8_t *data, uint8_t WriteAddr, uint16_t len)
{
    int rc = 0;
    BSP_ADC_FUN_TypeDef *opt = bsp_ltc2450.fun;
#ifdef SHOW_IIC_DATA_DEBUG
    BSP_LOG_trace("write adc <%d> data :[\r\n", len);
#endif
    for (int i = 0; i < BSP_ADC_TRY_TIMES; i++)
    {
        rc = opt->writes(&bsp_ltc2450, BSP_ADC_DEVICE_ID, data, len, 3000);
        if (rc == HAL_OK)
        {
#ifdef SHOW_IIC_DATA_DEBUG
            BSP_LOG_trace_hex(data, len);
            BSP_LOG_trace("\r\n]\r\n");
#endif
            return rc;
        }        
        osDelay(5);        
    }
    return rc;
}

static uint8_t _adc_dev_ReadBytes(uint8_t* data, uint8_t ReadAddr, uint16_t len)
{
    int rc = 0;
    BSP_ADC_FUN_TypeDef *opt = bsp_ltc2450.fun;
#ifdef SHOW_IIC_DATA_DEBUG
    BSP_LOG_trace("read adc <%d> data:[\r\n", len);
#endif    
    for (int i = 0; i < BSP_ADC_TRY_TIMES; i++)
    {
        rc = opt->reads(&bsp_ltc2450, BSP_ADC_DEVICE_ID, data, len, 3000);
        if (rc == HAL_OK)
        {
#ifdef SHOW_IIC_DATA_DEBUG
            BSP_LOG_trace_hex(data, len);
            BSP_LOG_trace("\r\n]\r\n");
#endif            
            return rc;
        }
        osDelay(5);
    }    
    return rc;
}

uint8_t BSP_ADC_WriteByte(uint8_t WriteAddr, uint8_t data)
{
    _adc_dev_WriteBytes(&data, WriteAddr, 1);
    return 0;
}

uint8_t BSP_ADC_ReadByte(uint8_t* data, uint8_t ReadAddr)
{
    _adc_dev_ReadBytes(data, ReadAddr, 1);
    return 0;
}

static uint16_t ADC_Get_Value(void)
{
    uint16_t ADC_Value;
    uint8_t Adc_Temp[2] = {0};
    _adc_dev_ReadBytes(Adc_Temp, ADC_DEVICE_ADDRESS, 2);
    ADC_Value = Adc_Temp[0];
    ADC_Value <<= 8;
    ADC_Value += Adc_Temp[1];

    return  ADC_Value;
}



/*
    16bit分辨率，最高位表示电压的+-属性，因此电压的实际分辨率为15bit 
    Vref = 2.048V   Vdd = 3.3V   R1 = 20KΩ  PGA = 4
    Vadc = (Vref*ADC)/(PGA*2^15)
    I = (Vdd-Vadc)/R1
    R = Vadc/I = (Vadc*R1)/(Vdd-Vadc) = R1/((Vdd/Vadc)-1) 
      = 20000/((3.3/Vadc)-1) 
      = 20000/(((3.3*2^17)/(2.048*ADC))-1) 
      = (20000*ADC)/(((3.3*2^17)/2.048)-ADC) 
      = (20000*ADC)/(((3300*2^17)/2048)-ADC) 
      = (20000*ADC)/((3300*2^6)-ADC) 
*/
static float _adc_dev_Voltage_Convert(void)
{
    uint16_t Voltage;
    float Measure;
    Voltage = ADC_Get_Value();
//#ifdef SHOW_ADC_TEMPERATURE    
 //   BSP_LOG_trace("Voltage Val=%04X\r\n", Voltage);
//#endif    
    if((Voltage&0x8000)==0x8000)
    {
        Voltage&=0x7fff;
        Voltage=(~Voltage)+1;
        Voltage &=0x7fff;
        Measure = (float)(Voltage)*20000/(131072*3.3/2.048-(float)(Voltage));
    }
    else
    {
        Voltage&=0x7fff;
        // Measure = (float)(Voltage)*20000/(131072*3.3/2.048-(float)(Voltage));
        Measure = (float)(Voltage)*10000/(131072*2.505/2.048-(float)(Voltage));
        // Measure = (float)(Voltage)*20000/(262144*3.3/2.048-(float)(Voltage));
    }
    return  Measure;
}

// 单周期转换、16bit、PGA=8 
void BSP_ADC_DEV_config_mode(void)
{
    BSP_ADC_WriteByte(ADC_DEVICE_ADDRESS, 0x9E);
}

// 计算出 PT1000 的阻抗 
float BSP_ADC_DEV_read_resistance(void)
{
	float RES1;
	BSP_ADC_DEV_config_mode();
	RES1 = _adc_dev_Voltage_Convert();
//#ifdef SHOW_ADC_TEMPERATURE    
//    BSP_LOG_trace("RES1 Val=%.3f\r\n", RES1);
//#endif    
	return (RES1);
}
