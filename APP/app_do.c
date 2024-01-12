#include "kalman_filter.h"
#include "mean_filter.h"
#include "bootloader.h"
#include "bsp_log.h"
#include "bsp_wdg.h"
#include "hardware_wrapper.h"
#include "app_do.h"
#include "app_config.h"
#include "app_adc.h"
#include "app_modbus.h"
#include "bsp_flash.h"
#include "math.h"

//PT1000阻值计算公式 Rt=1000*(1+0.0039083*t-0.0000005775*t*t)
const float RT_PT1000[510] =   // 表格中阻值以0.1℃为1步，从0℃到50.9℃，共计510个点
{
	1000.000, 1000.391, 1000.782, 1001.172, 1001.563, 1001.954, 1002.345, 1002.736, 1003.126, 1003.517, // 0℃ ~ 0.9℃
	1003.908, 1004.298, 1004.689, 1005.080, 1005.470, 1005.861, 1006.252, 1006.642, 1007.033, 1007.424, // 1℃ ~ 1.9℃
	1007.814, 1008.205, 1008.595, 1008.986, 1009.377, 1009.767, 1010.158, 1010.548, 1010.939, 1011.329, // 2℃ ~ 2.9℃
	1011.720, 1012.110, 1012.501, 1012.891, 1013.282, 1013.672, 1014.062, 1014.453, 1014.843, 1015.234, // 3℃ ~ 3.9℃
	1015.624, 1016.014, 1016.405, 1016.795, 1017.185, 1017.576, 1017.966, 1018.356, 1018.747, 1019.137, // 4℃ ~ 4.9℃
	1019.527, 1019.917, 1020.308, 1020.698, 1021.088, 1021.478, 1021.868, 1022.259, 1022.649, 1023.039, // 5℃ ~ 5.9℃
	1023.429, 1023.819, 1024.209, 1024.599, 1024.989, 1025.380, 1025.770, 1026.160, 1026.550, 1026.940, // 6℃ ~ 6.9℃
	1027.330, 1027.720, 1028.110, 1028.500, 1028.890, 1029.280, 1029.670, 1030.060, 1030.450, 1030.840, // 7℃ ~ 7.9℃
	1031.229, 1031.619, 1032.009, 1032.399, 1032.789, 1033.179, 1033.569, 1033.958, 1034.348, 1034.738, // 8℃~ 8.9℃
	1035.128, 1035.518, 1035.907, 1036.297, 1036.687, 1037.077, 1037.466, 1037.856, 1038.246, 1038.636, // 9℃ ~ 9.9℃
	1039.025, 1039.415, 1039.805, 1040.194, 1040.584, 1040.973, 1041.363, 1041.753, 1042.142, 1042.532, // 10℃ ~ 10.9℃
	1042.921, 1043.311, 1043.701, 1044.090, 1044.480, 1044.869, 1045.259, 1045.648, 1046.038, 1046.427, // 11℃ ~ 11.9℃
	1046.816, 1047.206, 1047.595, 1047.985, 1048.374, 1048.764, 1049.153, 1049.542, 1049.932, 1050.321, // 12℃ ~ 12.9℃
	1050.710, 1051.100, 1051.489, 1051.878, 1052.268, 1052.657, 1053.046, 1053.435, 1053.825, 1054.214, // 13℃ ~ 13.9℃
	1054.603, 1054.992, 1055.381, 1055.771, 1056.160, 1056.549, 1056.938, 1057.327, 1057.716, 1058.105, // 14℃ ~ 14.9℃
	1058.495, 1058.884, 1059.273, 1059.662, 1060.051, 1060.440, 1060.829, 1061.218, 1061.607, 1061.996, // 15℃ ~ 15.9℃
	1062.385, 1062.774, 1063.163, 1063.552, 1063.941, 1064.330, 1064.719, 1065.108, 1065.496, 1065.885, // 16℃ ~ 16.9℃
	1066.274, 1066.663, 1067.052, 1067.441, 1067.830, 1068.218, 1068.607, 1068.996, 1069.385, 1069.774, // 17℃ ~ 17.9℃
	1070.162, 1070.551, 1070.940, 1071.328, 1071.717, 1072.106, 1072.495, 1072.883, 1073.272, 1073.661, // 18℃ ~ 18.9℃
	1074.049, 1074.438, 1074.826, 1075.215, 1075.604, 1075.992, 1076.381, 1076.769, 1077.158, 1077.546, // 19℃ ~ 19.9℃
	1077.935, 1078.324, 1078.712, 1079.101, 1079.489, 1079.877, 1080.266, 1080.654, 1081.043, 1081.431, // 20℃ ~ 20.9℃
	1081.820, 1082.208, 1082.596, 1082.985, 1083.373, 1083.762, 1084.150, 1084.538, 1084.926, 1085.315, // 21℃ ~ 21.9℃
	1085.703, 1086.091, 1086.480, 1086.868, 1087.256, 1087.644, 1088.033, 1088.421, 1088.809, 1089.197, // 22℃ ~ 22.9℃
	1089.585, 1089.974, 1090.362, 1090.750, 1091.138, 1091.526, 1091.914, 1092.302, 1092.690, 1093.078, // 23℃ ~ 23.9℃
	1093.467, 1093.855, 1094.243, 1094.631, 1095.019, 1095.407, 1095.795, 1096.183, 1096.571, 1096.959, // 24℃ ~ 24.9℃
	1097.347, 1097.734, 1098.122, 1098.510, 1098.898, 1099.286, 1099.674, 1100.062, 1100.450, 1100.838, // 25℃ ~ 25.9℃
	1101.225, 1101.613, 1102.001, 1102.389, 1102.777, 1103.164, 1103.552, 1103.940, 1104.328, 1104.715, // 26℃ ~ 26.9℃
	1105.103, 1105.491, 1105.879, 1106.266, 1106.654, 1107.042, 1107.429, 1107.817, 1108.204, 1108.592, // 27℃ ~ 27.9℃
	1108.980, 1109.367, 1109.755, 1110.142, 1110.530, 1110.917, 1111.305, 1111.693, 1112.080, 1112.468, // 28℃ ~ 28.9℃
	1112.855, 1113.242, 1113.630, 1114.017, 1114.405, 1114.792, 1115.180, 1115.567, 1115.954, 1116.342, // 29℃ ~ 29.9℃
	1116.729, 1117.117, 1117.504, 1117.891, 1118.279, 1118.666, 1119.053, 1119.441, 1119.828, 1120.215, // 30℃ ~ 30.9℃
	1120.602, 1120.990, 1121.377, 1121.764, 1122.151, 1122.538, 1122.926, 1123.313, 1123.700, 1124.087, // 31℃ ~ 31.9℃
	1124.474, 1124.861, 1125.248, 1125.636, 1126.023, 1126.410, 1126.797, 1127.184, 1127.571, 1127.958, // 32℃ ~ 32.9℃
	1128.345, 1128.732, 1129.119, 1129.506, 1129.893, 1130.280, 1130.667, 1131.054, 1131.441, 1131.828, // 33℃ ~ 33.9℃
	1132.215, 1132.602, 1132.988, 1133.375, 1133.762, 1134.149, 1134.536, 1134.923, 1135.309, 1135.696, // 34℃ ~ 34.9℃
	1136.083, 1136.470, 1136.857, 1137.243, 1137.630, 1138.017, 1138.404, 1138.790, 1139.177, 1139.564, // 35℃ ~ 35.9℃
	1139.950, 1140.337, 1140.724, 1141.110, 1141.497, 1141.884, 1142.270, 1142.657, 1143.043, 1143.430, // 36℃ ~ 36.9℃
	1143.817, 1144.203, 1144.590, 1144.976, 1145.363, 1145.749, 1146.136, 1146.522, 1146.909, 1147.295, // 37℃ ~ 37.9℃
	1147.681, 1148.068, 1148.454, 1148.841, 1149.227, 1149.614, 1150.000, 1150.386, 1150.773, 1151.159, // 38℃ ~ 38.9℃
	1151.545, 1151.932, 1152.318, 1152.704, 1153.091, 1153.477, 1153.863, 1154.249, 1154.636, 1155.022, // 39℃ ~ 39.9℃
	1155.408, 1155.794, 1156.180, 1156.567, 1156.953, 1157.339, 1157.725, 1158.111, 1158.497, 1158.883,  // 40℃ ~ 40.9℃
	1159.270, 1159.656, 1160.042, 1160.428, 1160.814, 1161.200, 1161.586, 1161.972, 1162.358, 1162.744,  // 41℃ ~ 41.9℃
	1163.130, 1163.516, 1163.902, 1164.288, 1164.674, 1165.060, 1165.446, 1165.831, 1166.217, 1166.603,  // 42℃ ~ 42.9℃
	1166.989, 1167.375, 1167.761, 1168.147, 1168.532, 1168.918, 1169.304, 1169.690, 1170.076, 1170.461,  // 43℃ ~ 43.9℃
	1170.847, 1171.233, 1171.619, 1172.004, 1172.390, 1172.776, 1173.161, 1173.547, 1173.933, 1174.318,  // 44℃ ~ 44.9℃
	1174.704, 1175.090, 1175.475, 1175.861, 1176.247, 1176.632, 1177.018, 1177.403, 1177.789, 1178.174,  // 45℃ ~ 45.9℃
	1178.560, 1178.945, 1179.331, 1179.716, 1180.102, 1180.487, 1180.873, 1181.258, 1181.644, 1182.029,  // 46℃ ~ 46.9℃
	1182.414, 1182.800, 1183.185, 1183.571, 1183.956, 1184.341, 1184.727, 1185.112, 1185.497, 1185.883,  // 47℃ ~ 47.9℃
	1186.268, 1186.653, 1187.038, 1187.424, 1187.809, 1188.194, 1188.579, 1188.965, 1189.350, 1189.735,  // 48℃ ~ 48.9℃
	1190.120, 1190.505, 1190.890, 1191.276, 1191.661, 1192.046, 1192.431, 1192.816, 1193.201, 1193.586,  // 49℃ ~ 49.9℃
	1193.971, 1194.356, 1194.741, 1195.126, 1195.511, 1195.896, 1196.281, 1196.666, 1197.051, 1197.436,  // 50℃ ~ 50.9℃
};


extern uint16_t MODBUS_DATA_FRAME[32] ;			  

//配置数据
uint8_t  gs_uint8_t_Addr485 = 1;				//传感器地址，[1-127], 默认:1
uint8_t  gs_uint8_t_Salinity = 0;               //盐度值，单位:1/1000, 范围[0-40], 默认:0
uint16_t gs_uint16_t_Pressure = 1013;           //大气压力值，单位:0.1kPa, 默认:1013, 高字节在前

uint16_t gs_uint16_t_CaliPhase = 13000;         //零氧校准时的相位差值，单位:0.0001, 范围[0-20000], 默认:13000
uint16_t gs_uint16_t_CaliT0 = 250;              //零氧校准时的温度值，单位:1/10℃, 范围[0-510], 默认:250

uint16_t gs_uint16_t_CaliPhaseB = 3800;         //标准时的相位值，单位:0.0001, 范围[0-20000], 默认:3800
uint16_t gs_uint16_t_CaliTemp = 250;            //标准时的温度值，单位:1/10℃, 范围[0-510], 默认:250

uint16_t gs_uint16_t_CaliT_BC = 0;              //温度补偿值，单位:0.0001℃，默认:0
uint16_t gs_uint16_t_CaliT_BCFLAG = 0;			//温度补偿趋势标志
uint16_t gs_uint16_t_CaliT_BCFactor = 10000;	//温度补偿系数（未使用）

uint16_t gs_uint16_t_CaliDO = 825;				//校准温度对应的饱和氧，单位:0.01
uint16_t gs_uint16_t_CaliDO_BC = 0;				//溶氧计算模型常数项，单位:0.01，默认:0
uint16_t gs_uint16_t_TcFactor = 44;				//零氧相位温度系数，单位:0.0001, 默认:48

uint16_t gs_uint16_t_KsvFactor = 4000;			//Stern-Volmer系数, 单位:0.0001, 默认:4000
uint16_t gs_uint16_t_KsvFactor_T = 280;			//Stern-Volmer系数随温度变化的系数，单位:0.0001, 默认:280

uint16_t gs_uint16_t_DOFIXPHASE_Base = 1600;    //相位延时基准值
uint16_t gs_uint16_t_DOFIXPHASE_Factor_T = 0;   //相位延时温度系数
uint16_t gs_uint16_t_DOFIXPHASE_Flag = 0;		//相位延时随温度增加而增加，为1，否则为0；

//传感器原始数据采集
uint16_t gs_uint16_t_PhaseDiff = 0;				//荧光相位差值

//传感器计算
uint16_t gs_uint16_t_Percent = 1000;			//溶解氧饱和度,单位:0.1
uint16_t gs_uint16_t_TCDO = 0;					//溶解氧温度补偿后的值
uint16_t gs_uint16_t_SCDO = 0;					//溶解氧盐度补偿后的值
uint16_t gs_uint16_t_PCDO = 0;					//溶解氧大气压力补偿后的值


float  get_value=0;
uint16_t  out_value=0;
//温度采集
uint16_t gs_uint16_t_TempValue = 250;			//传感器温度值，单位:1/10℃, 范围[0-510], 默认:250

//传感器警报状态
uint8_t gs_b1_SnrErr = 0;						//传感器异常
uint8_t gs_b1_CalErr = 0;						//校准异常

/* Private function prototypes -----------------------------------------------*/

void FLASH_Load_Data(void);
void FDO_Phase_Acquisition(uint16_t phase);
uint8_t DO_Calibration(uint8_t DataRev_485);
void DO_Value_Calculation(float PhaseValue);
uint16_t Salinity_Compensation(void);
void Do_Value_Update(float Phase_Value);

/** 荧光相位差获取，内部函数 ****************************************************/
void FDO_Phase_Acquisition(uint16_t phase)
{
	//获取荧光相位差值Phase，带4位小数
	gs_uint16_t_PhaseDiff = phase;
	MODBUS_DATA_FRAME[ADDR_R3] = phase;
	holding_registers.phase = phase;
}

/** 温度更新，外部函数 **********************************************************/
void Temp_Value_Update(float Value)
{
	uint16_t TempValue = 0;
	float RseValue;
	int k=0;

	//获取PT1000的电阻值RseValue
	RseValue = Value;
	if (PT1000_MIN_VALUE > RseValue)
	{
		RseValue = PT1000_MIN_VALUE;    	//温度下限0℃对应的电阻值
	}
	else if (PT1000_MAX_VALUE < RseValue)
	{
		RseValue = PT1000_MAX_VALUE;    	//温度上限100℃对应的电阻值
	}
	else
	{
		;
	}	
	
	//温度值计算
	// TempValue = (uint16_t)((RseValue - 1000.0)/0.388);
	//float a1 = RseValue - 1000.0;
	//	TempValue = (a1 / 0.388);
	//查表法
	while(k<510)
	{
		if(RseValue<RT_PT1000[k])
		{
			TempValue=k ;
			break;
		}
		k++;
		if(k==510)
		{
			TempValue=510;
		}
	}

	RseValue = (float)(gs_uint16_t_CaliT_BCFactor)/10000;
	if(0 == holding_registers.reserved15)
	{
		TempValue = TempValue*RseValue + (holding_registers.temperature_compensation/1000);	  
	}
	else
	{
		if((TempValue*RseValue) > (holding_registers.temperature_compensation/1000))
		{
			TempValue = TempValue*RseValue - (holding_registers.temperature_compensation/1000);
		}
		else
		{
			TempValue = 0;
		}
	}
	
	if ((uint16_t)DO_MAX_TEMP < TempValue)    //温度范围限制[0-51℃]
	{
		TempValue = (uint16_t)DO_MAX_TEMP;				
	}
	else
	{
		;			
	}
	gs_uint16_t_TempValue         = TempValue;
	MODBUS_DATA_FRAME[ADDR_TEMP]  = TempValue;
	holding_registers.temperature = TempValue;
}

#define WRITE_CONFIG_AND_RATIO_INIT() \
    rc = APP_CONFIG_write_settings(); \
    APP_CHECK_RC;                     \
    return APP_OK;

uint8_t DO_Write_Salinity(uint16_t DO_Salinity)
{
	int rc = 0;
	if (gs_uint8_t_Salinity != (uint8_t)(DO_Salinity/10))
	{ 
		if (400 < DO_Salinity)
		{
			return APP_ERROR;
			//gs_uint8_t_Salinity = 40;
			//MODBUS_DATA_FRAME[ADDR_SAL] = (uint16_t)400;
			//holding_registers.salinity = (uint16_t)400;
		}
		else
		{
			gs_uint8_t_Salinity = (uint8_t)(DO_Salinity/10);
			MODBUS_DATA_FRAME[ADDR_SAL] = DO_Salinity;
			holding_registers.salinity = DO_Salinity;
		}
	}
	WRITE_CONFIG_AND_RATIO_INIT()	
}

uint8_t DO_Write_0_temperature_coefficient(uint16_t value)
{
	int rc = 0;
	if (MAX_TcFactor < value)
	{ 
		return APP_ERROR;
		//gs_uint16_t_TcFactor = MAX_TcFactor;
		//MODBUS_DATA_FRAME[ADDR_R5] = MAX_TcFactor;
		//holding_registers.calibrate_0_temperature_coefficient = MAX_TcFactor;
	}	
	else
	{	
		gs_uint16_t_TcFactor = value;
		MODBUS_DATA_FRAME[ADDR_R5] = value;
		holding_registers.calibrate_0_temperature_coefficient = value;
	}
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_100_temperature_coefficient(uint16_t value)
{
	int rc = 0;
	if (MAX_TcFactor < value)
	{ 
		return APP_ERROR;
		//gs_uint16_t_TcFactor = MAX_TcFactor;
		//MODBUS_DATA_FRAME[ADDR_R5] = MAX_TcFactor;
		//holding_registers.calibrate_0_temperature_coefficient = MAX_TcFactor;
	}	
	else
	{	
		holding_registers.reserved8 = value;
	}
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_temperature_flag(uint16_t value)
{
	int rc = 0;
	if (value>1) 
	{
		return APP_ERROR;
	}
	if (0 == value)		
	{
		gs_uint16_t_CaliT_BCFLAG = value;
		MODBUS_DATA_FRAME[ADDR_R10] = value;
		holding_registers.reserved15 = value;
	}
	else
	{
		gs_uint16_t_CaliT_BCFLAG = 1;
		MODBUS_DATA_FRAME[ADDR_R10] = 1;
		holding_registers.reserved15 = 1;
	}	
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_temperature_compensation(uint16_t value)
{
	int rc = 0;
	gs_uint16_t_CaliT_BC = value;
	MODBUS_DATA_FRAME[ADDR_R11] = value;
	holding_registers.temperature_compensation= value;
	WRITE_CONFIG_AND_RATIO_INIT()  
}

uint8_t DO_Write_phase_delay_base(uint16_t value)
{
	int rc = 0;
	if (MAX_DOFIXPHASE_Base < value)	
	{
		return APP_ERROR;
		//gs_uint16_t_DOFIXPHASE_Base = MAX_DOFIXPHASE_Base;
		//MODBUS_DATA_FRAME[ADDR_R6] = MAX_DOFIXPHASE_Base;
		//holding_registers.phase_delay_base= MAX_DOFIXPHASE_Base;
	}
	else
	{
		gs_uint16_t_DOFIXPHASE_Base = value;
		MODBUS_DATA_FRAME[ADDR_R6] = value;
		holding_registers.phase_delay_base= value;
	}	
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_phase_delay_compensation(uint16_t value)
{
	int rc = 0;
	if (MAX_DOFIXPHASE_Factor_T < value)	
	{
		return APP_ERROR;
		//gs_uint16_t_DOFIXPHASE_Factor_T = MAX_DOFIXPHASE_Factor_T;
		//MODBUS_DATA_FRAME[ADDR_R7] = MAX_DOFIXPHASE_Factor_T;
		//holding_registers.phase_delay_compensation= MAX_DOFIXPHASE_Factor_T;
	}	
	else
	{
		gs_uint16_t_DOFIXPHASE_Factor_T = value;
		MODBUS_DATA_FRAME[ADDR_R7] = value;
		holding_registers.phase_delay_compensation= value;
	}		
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_phase_flag(uint16_t value)
{
	int rc = 0;
	if (value>1) 
	{
		return APP_ERROR;
	}
	if(0 == value)					
	{
		gs_uint16_t_DOFIXPHASE_Flag = value;
		MODBUS_DATA_FRAME[ADDR_R8] = value;
		holding_registers.reserved30= value;
	}
	else
	{
		gs_uint16_t_DOFIXPHASE_Flag = 1;
		MODBUS_DATA_FRAME[ADDR_R8] = 1;
		holding_registers.reserved30= 1;
	}	
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_ksv_temperature_coefficient(uint16_t value)
{
	int rc = 0;
	if(MAX_KsvFactor_T < value)								
	{
		return APP_ERROR;
		//gs_uint16_t_KsvFactor_T = MAX_KsvFactor_T;
		//MODBUS_DATA_FRAME[ADDR_R9] = MAX_KsvFactor_T;
		//holding_registers.ksv_temperature_coefficient= MAX_KsvFactor_T;
	}	
	else
	{	
		gs_uint16_t_KsvFactor_T = value;
		MODBUS_DATA_FRAME[ADDR_R9] = value;
		holding_registers.ksv_temperature_coefficient= value;
	}	
	WRITE_CONFIG_AND_RATIO_INIT()
}

uint8_t DO_Write_atmosphere(uint16_t value)
{
	int rc = 0;
	if (1075 < value)
	{
		return APP_ERROR;
		//value = 1075;
	}
	else if (540 > value)
	{
		return APP_ERROR;
		//value = 540;
	}
	else
	{
		;
	}
	gs_uint16_t_Pressure = value;	//更新全局变量
	MODBUS_DATA_FRAME[ADDR_PRE] = value;
	holding_registers.atmosphere= value;
	WRITE_CONFIG_AND_RATIO_INIT()
}

/** DO校准函数，内部函数 ********************************************************/
uint8_t DO_Calibration(uint8_t DataRev_485)
{
	int rc = 0;
	uint8_t  Ret = 0;
	uint16_t DO_FIX_PHASE;
	float Cal_Value;
	
	if (gs_uint16_t_PhaseDiff > MAX_PhaseDiff)
	{
		//相位差超上限，校准异常
		gs_b1_CalErr = 1;
		return (Ret);
	}	
	else
	{
		;
	}
	
	switch(DataRev_485)
	{
	  case 0x01:          												//零氧校准
			Ret = 1;
			MODBUS_DATA_FRAME[ADDR_R1] = gs_uint16_t_PhaseDiff;   		//TEST
			holding_registers.calibrate_0_phase = gs_uint16_t_PhaseDiff;
			gs_uint16_t_CaliPhase = gs_uint16_t_PhaseDiff;   			//更新零氧校准时的相位差值
			MODBUS_DATA_FRAME[ADDR_R2] = gs_uint16_t_TempValue;   		//TEST
			holding_registers.calibrate_0_temperature = gs_uint16_t_TempValue;
			gs_uint16_t_CaliT0 = gs_uint16_t_TempValue;   				//更新零氧校准时的温度值
			WRITE_CONFIG_AND_RATIO_INIT()	
		break;
			
		case 0x02:          											//饱和氧校准
			MODBUS_DATA_FRAME[ADDR_NCAIL] = gs_uint16_t_PhaseDiff;   	//TEST
			holding_registers.calibrate_100_phase = gs_uint16_t_PhaseDiff;
			gs_uint16_t_CaliPhaseB = gs_uint16_t_PhaseDiff;   			//更新零氧校准时的相位差值
			gs_uint16_t_CaliTemp = gs_uint16_t_TempValue;
			MODBUS_DATA_FRAME[ADDR_R4] = gs_uint16_t_TempValue;		  	//TEST
			holding_registers.calibrate_100_temperature = gs_uint16_t_TempValue;
			//不同温度下对应的饱和氧:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
			//得到测量温度下的饱和氧，保留2位小数			
			Cal_Value = (-0.0075*(gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp))/1000 + (0.7945*(gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp))/100 - (40.506*gs_uint16_t_CaliTemp)/10 + 1460.1 + 0.5; 				
			gs_uint16_t_CaliDO = (uint16_t)Cal_Value;
						
			//溶解氧计算模型
			//根据Stern-Volmer方程,计算Stern-Volmer常数Ksv
			//F0/F = 1+Ksv*[Q]
			//Ksv = (F0/F-1)/[Q]
			//温度补偿，计算饱和氧校准温度下的tao_0值：gs_uint16_t_CaliPhase
			if (holding_registers.calibrate_100_temperature > holding_registers.calibrate_0_temperature)
			{
				Cal_Value = holding_registers.calibrate_0_phase - ((holding_registers.calibrate_100_temperature - holding_registers.calibrate_0_temperature)*holding_registers.calibrate_0_temperature_coefficient)/10;	
			}
			else
			{
				Cal_Value = holding_registers.calibrate_0_phase + ((holding_registers.calibrate_0_temperature - holding_registers.calibrate_100_temperature)*holding_registers.calibrate_0_temperature_coefficient)/10;		
			}	

            if(holding_registers.reserved30 == 0)
			{
				DO_FIX_PHASE = holding_registers.phase_delay_base - (gs_uint16_t_TempValue*holding_registers.phase_delay_compensation)/100;  //相位延时温度系数
			}		
			else
			{
				DO_FIX_PHASE = holding_registers.phase_delay_base + (gs_uint16_t_TempValue*holding_registers.phase_delay_compensation)/100;			
			}
			
			if (gs_uint16_t_PhaseDiff < (uint16_t)Cal_Value)
			{
				Cal_Value = (Cal_Value - gs_uint16_t_CaliPhaseB )/(gs_uint16_t_CaliPhaseB - DO_FIX_PHASE);
				Cal_Value = ((Cal_Value*10000)/(gs_uint16_t_CaliDO - gs_uint16_t_CaliDO_BC))*100;          //计算斜率，保留4位小数		
			}
			else
			{
				return (Ret);	
			}
					
			//FLASH存储			
			gs_uint16_t_KsvFactor = (uint16_t)Cal_Value;
			MODBUS_DATA_FRAME[ADDR_FACT] = (uint16_t)Cal_Value;
			holding_registers.stern_volmer_coefficient = (uint16_t)Cal_Value;
			WRITE_CONFIG_AND_RATIO_INIT()
			Ret = 1;
		break;

		case 0x0A:		//恢复出厂默认设置
			gs_uint8_t_Salinity = 0;
			MODBUS_DATA_FRAME[ADDR_SAL] = 0; 
			holding_registers.salinity= 0;											
			gs_uint16_t_Pressure = 1013;
			MODBUS_DATA_FRAME[ADDR_PRE] = 1013;	
			holding_registers.atmosphere= 1013;		
			gs_uint16_t_TcFactor = 44;
		  holding_registers.reserved8=32;
			MODBUS_DATA_FRAME[ADDR_R5] = 44;
			holding_registers.calibrate_0_temperature_coefficient= 44;
			////系统固定参数 start
			//延时相位基准值,2个字节
			gs_uint16_t_DOFIXPHASE_Base = 1600;
			MODBUS_DATA_FRAME[ADDR_R6] = 1600;
			holding_registers.phase_delay_base = 1600;
			//延时相位温度系数,2个字节
			gs_uint16_t_DOFIXPHASE_Factor_T = 0;
			MODBUS_DATA_FRAME[ADDR_R7] = 0;
			holding_registers.phase_delay_compensation = 0;
			//延时相位趋势标志,2个字节
			gs_uint16_t_DOFIXPHASE_Flag = 0;
			MODBUS_DATA_FRAME[ADDR_R8] = 0;
			holding_registers.reserved30 = 0;
			//Stern-Volmer系数随温度变化的系数,2个字节
			gs_uint16_t_KsvFactor_T = 280;
			MODBUS_DATA_FRAME[ADDR_R9] = 280;
			holding_registers.ksv_temperature_coefficient = 280;
			//温度补偿趋势标志
			gs_uint16_t_CaliT_BCFLAG = 0;
			MODBUS_DATA_FRAME[ADDR_R10] = 0;
			holding_registers.reserved15 = 0;
			//温度补偿值，单位:0.0001℃，默认:0
			gs_uint16_t_CaliT_BC = 0; 
			MODBUS_DATA_FRAME[ADDR_R11] = 0;
			holding_registers.temperature_compensation = 0;
			////系统固定参数 end

			//不同温度下对应的饱和氧:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
			//得到测量温度下的饱和氧，保留2位小数			
			Cal_Value = (-0.0075*(gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp))/1000 + (0.7945*(gs_uint16_t_CaliTemp*gs_uint16_t_CaliTemp))/100 - (40.506*gs_uint16_t_CaliTemp)/10 + 1460.1 + 0.5; 				
			gs_uint16_t_CaliDO = (uint16_t)Cal_Value;

			//溶解氧计算模型
			//根据Stern-Volmer方程,计算Stern-Volmer常数Ksv
			//F0/F = 1+Ksv*[Q]
			//Ksv = (F0/F-1)/[Q]
			//温度补偿，计算饱和氧校准温度下的tao_0值：gs_u16_CaliPhase
			if (holding_registers.calibrate_100_temperature > holding_registers.calibrate_0_temperature)
			{
				Cal_Value = holding_registers.calibrate_0_phase - ((holding_registers.calibrate_100_temperature- holding_registers.calibrate_0_temperature)*holding_registers.calibrate_0_temperature_coefficient)/10;	
			}
			else
			{
				Cal_Value = holding_registers.calibrate_0_phase + ((holding_registers.calibrate_0_temperature - holding_registers.calibrate_100_temperature)*holding_registers.calibrate_0_temperature_coefficient)/10;		
			}	

			if(holding_registers.reserved30 == 0)
			{
				DO_FIX_PHASE = holding_registers.phase_delay_base - (gs_uint16_t_TempValue*holding_registers.phase_delay_compensation)/100;  //相位延时温度系数
			}		
			else
			{
				DO_FIX_PHASE = holding_registers.phase_delay_base + (gs_uint16_t_TempValue*holding_registers.phase_delay_compensation)/100;			
			}
				
			if ((gs_uint16_t_PhaseDiff < (uint16_t)Cal_Value)&&(gs_uint16_t_PhaseDiff > DO_FIX_PHASE))
			{
				Cal_Value = (Cal_Value - gs_uint16_t_PhaseDiff )/(gs_uint16_t_PhaseDiff - DO_FIX_PHASE);
				Cal_Value = ((Cal_Value*10000)/(gs_uint16_t_CaliDO - gs_uint16_t_CaliDO_BC))*100;          //计算斜率，保留4位小数		
			}
			else
			{
			}
			gs_uint16_t_KsvFactor = (uint16_t)Cal_Value;
			MODBUS_DATA_FRAME[ADDR_FACT] = (uint16_t)Cal_Value;
			holding_registers.stern_volmer_coefficient= (uint16_t)Cal_Value;
			WRITE_CONFIG_AND_RATIO_INIT()				
			break;

		default:		
			break;
	}
	
	return (Ret);
}

/** 溶氧值计算，内部函数 ********************************************************/
void DO_Value_Calculation(float PhaseValue)
{
	float Cal_Value, Ratio, Sat_DO;
	float CaliT0_b, CaliTemp_b;
	float CaliT0_PHASE, CaliTemp_PHASE;
	uint16_t KsvFactor_T, DO_FIX_PHASE;
	uint32_t tmp;
	DO_FIX_PHASE=holding_registers.phase_delay_base;
	
	FDO_Phase_Acquisition((uint16_t)PhaseValue);    	//获取原始相位差值，带2位小数
	
	//不同温度下对应的饱和溶氧值:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
	//得到测量温度下的饱和氧，保留2位小数	
	Sat_DO = ((gs_uint16_t_TempValue*gs_uint16_t_TempValue*gs_uint16_t_TempValue)*(-0.0075))/1000+((gs_uint16_t_TempValue*gs_uint16_t_TempValue)*0.7945)/100-(40.506*gs_uint16_t_TempValue)/10+1460.1+0.5; 
	//零氧下相位与温度关系
//	CaliT0_b=holding_registers.calibrate_0_temperature/10*holding_registers.calibrate_0_temperature_coefficient+holding_registers.calibrate_0_phase;
//	CaliT0_PHASE=CaliT0_b-holding_registers.calibrate_0_temperature_coefficient*gs_uint16_t_TempValue/10;

	//饱和氧下相位与温度关系
//	CaliTemp_b=holding_registers.calibrate_100_temperature/10*holding_registers.reserved8+holding_registers.calibrate_100_phase;
//	CaliTemp_PHASE=CaliTemp_b-holding_registers.reserved8*gs_uint16_t_TempValue/10;

	//CaliT0_b=holding_registers.calibrate_0_temperature/10*44+holding_registers.calibrate_0_phase;
	//CaliT0_PHASE=CaliT0_b-44*gs_uint16_t_TempValue/10;
	CaliT0_b=holding_registers.calibrate_0_temperature/10*17+holding_registers.calibrate_0_phase+holding_registers.calibrate_0_temperature*holding_registers.calibrate_0_temperature/300;
	CaliT0_PHASE=CaliT0_b-17*gs_uint16_t_TempValue/10-gs_uint16_t_TempValue*gs_uint16_t_TempValue/300;

	//饱和氧下相位与温度关系
	//CaliTemp_b=holding_registers.calibrate_100_temperature/10*25+holding_registers.calibrate_100_phase;
	//CaliTemp_PHASE=CaliTemp_b-25*gs_uint16_t_TempValue/10;
	CaliTemp_b=holding_registers.calibrate_100_temperature/10*34+holding_registers.calibrate_100_phase-holding_registers.calibrate_100_temperature*holding_registers.calibrate_100_temperature/500;
	CaliTemp_PHASE=CaliTemp_b-34*gs_uint16_t_TempValue/10+gs_uint16_t_TempValue*gs_uint16_t_TempValue/500;
	
	Ratio=((CaliT0_PHASE-DO_FIX_PHASE)/(CaliTemp_PHASE-DO_FIX_PHASE)-1)*100/Sat_DO;
	
	MODBUS_DATA_FRAME[ADDR_FACT]=	(uint16_t)(Ratio*10000);
	holding_registers.stern_volmer_coefficient = (uint16_t)(Ratio*10000);
	
	Cal_Value=((CaliT0_PHASE-DO_FIX_PHASE)/(PhaseValue-DO_FIX_PHASE)-1)/Ratio;
	Cal_Value  = Cal_Value* 100;
	//get_value = Cal_Value;
	//out_value = (uint16_t)get_value;
	/*	
	//温度补偿，计算当前温度下的tao_0,延时相位值：gs_uint16_t_CaliPhase
	if (gs_uint16_t_TempValue > gs_uint16_t_CaliT0)
	{	
		Cal_Value = gs_uint16_t_CaliPhase - ((gs_uint16_t_TempValue - gs_uint16_t_CaliT0)*gs_uint16_t_TcFactor)/10;		   
	}
	else
	{	
		Cal_Value = gs_uint16_t_CaliPhase + ((gs_uint16_t_CaliT0 - gs_uint16_t_TempValue)*gs_uint16_t_TcFactor)/10;		
	}	
	//温度补偿，计算当前温度下的延时相位值：DO_FIX_PHASE
	if(gs_uint16_t_DOFIXPHASE_Flag == 0)
	{
			DO_FIX_PHASE = gs_uint16_t_DOFIXPHASE_Base - (gs_uint16_t_TempValue*gs_uint16_t_DOFIXPHASE_Factor_T)/100;  //相位延时温度系数
	}		
	else
	{
			DO_FIX_PHASE = gs_uint16_t_DOFIXPHASE_Base + (gs_uint16_t_TempValue*gs_uint16_t_DOFIXPHASE_Factor_T)/100;			
	}
	*/
	if (gs_uint16_t_PhaseDiff > (uint16_t)CaliT0_PHASE)
	{
		//相位差大于零氧校准时的相位差
		gs_uint16_t_TCDO = 0;
		gs_uint16_t_Percent	= 0; 	
		MODBUS_DATA_FRAME[ADDR_PERC] = 0;
		holding_registers.saturation = 0;
		return;		
	}
	else
	{
		;
	}
	/*
	//根据Stern-Volmer方程,计算溶解氧浓度
	//F0/F = 1+Ksv*[Q]
	//[Q] = (F0/F-1)/Ksv	
	Cal_Value = (Cal_Value - gs_uint16_t_PhaseDiff)/(gs_uint16_t_PhaseDiff - DO_FIX_PHASE);
		
	if (gs_uint16_t_TempValue > gs_uint16_t_CaliTemp)
	{
		//tmp   = gs_uint16_t_KsvFactor_T * (gs_uint16_t_TempValue - gs_uint16_t_CaliTemp);
		KsvFactor_T = gs_uint16_t_KsvFactor_T - (uint16_t)((gs_uint16_t_TempValue - gs_uint16_t_CaliTemp)*0.15);
		tmp   = KsvFactor_T * (gs_uint16_t_TempValue - gs_uint16_t_CaliTemp);
		
		Ratio = gs_uint16_t_KsvFactor * exp(((float)tmp)/100000);
	}
	else
	{
		//tmp   = gs_uint16_t_KsvFactor_T * (gs_uint16_t_CaliTemp - gs_uint16_t_TempValue);
		KsvFactor_T  = gs_uint16_t_KsvFactor_T + (uint16_t)((gs_uint16_t_CaliTemp - gs_uint16_t_TempValue)*0.25);
		tmp   = KsvFactor_T * (gs_uint16_t_CaliTemp - gs_uint16_t_TempValue);
		
		Ratio = gs_uint16_t_KsvFactor*exp(-((float)tmp)/100000);
	}
	
	MODBUS_DATA_FRAME[ADDR_FACT]=	Ratio;
	holding_registers.stern_volmer_coefficient = Ratio;
	Cal_Value = ((Cal_Value*10000)/Ratio)*100;   		//带2位小数
	Cal_Value += gs_uint16_t_CaliDO_BC;      						//带2位小数	
	*/
	//计算当前经温度补偿后的溶解氧值
	if ((uint16_t)DO_MAX_VALUE < gs_uint16_t_TCDO)	//溶氧上限设定21mg/L
	{
		gs_uint16_t_TCDO = (uint16_t)DO_MAX_VALUE;
	}
	else
	{
		gs_uint16_t_TCDO = (uint16_t)Cal_Value;
	}
	
	gs_uint16_t_Percent = (uint16_t)((gs_uint16_t_TCDO / Sat_DO)*1000); 	
	MODBUS_DATA_FRAME[ADDR_PERC] = (uint16_t)((gs_uint16_t_TCDO / Sat_DO)*1000); 
	holding_registers.saturation = (uint16_t)((gs_uint16_t_TCDO / Sat_DO)*1000);
}

/** 盐度补偿，内部函数 **********************************************************/
uint16_t Salinity_Compensation(void)
{
	uint16_t Do_Value = 0;              //盐度补偿后的DO值
	float TempValue = 0;
	
	//DO_salt(T) = DO(theory) - Solid(T)*n                 (公式1)
	//Solid(T) = 10-3(0.037T2 - 2.912T +91.231), T[0, 40]  (公式2)
	//计算盐度因子 Solid_T :公式2 * 10000后, 数据范围:[340-925]
	//TempValue = (Solid_T * Salinity * 100) / 10000 + 0.5;  //乘以100保留2位小数
	
	TempValue = (((0.37*(gs_uint16_t_TempValue*gs_uint16_t_TempValue))/100 - (29.12*gs_uint16_t_TempValue)/10 + 912.31 + 0.5)*(uint8_t)(holding_registers.salinity/10))/100;
	TempValue *= gs_uint16_t_Percent;
	TempValue /= 1000;
	
	if (gs_uint16_t_TCDO > (uint16_t)TempValue)
	{
		Do_Value = gs_uint16_t_TCDO - (uint16_t)(TempValue);
	}
	else
	{
		Do_Value = 0;
	}	
	
	//大气压力补偿
	//lnPw = 12.062 - 4039.558/(T+235.379)                 (公式3) 计算饱和水蒸汽压力，T单位℃，Pw单位bar = 100kPa
	//Cs' = Cs*(P-Pw)/(101.3-Pw)                           (公式4) 大气压力补偿后的溶氧值，大气压力单位:kPa

	TempValue  = 1000/exp(4039.558/(0.1*gs_uint16_t_TempValue+235.379) - 12.062);
	TempValue  = ((holding_registers.atmosphere-TempValue)/(1013-TempValue))*Do_Value;
	Do_Value   = (uint16_t)TempValue;
	
	return (Do_Value);
}

/** 溶氧更新，外部函数 **********************************************************/
void Do_Value_Update(float Phase_Value)
{
	uint16_t Do_Value = 0;
	DO_Value_Calculation(10000*Phase_Value);
	Do_Value = Salinity_Compensation();
	
	if ((uint16_t)DO_MAX_VALUE < Do_Value)	//溶氧上限设定21mg/L
	{
		gs_uint16_t_SCDO = (uint16_t)DO_MAX_VALUE;
		MODBUS_DATA_FRAME[ADDR_DO] = (uint16_t)DO_MAX_VALUE;
		holding_registers.value = (uint16_t)DO_MAX_VALUE;
	}
	else
	{
		gs_uint16_t_SCDO = (uint16_t)Do_Value;
		MODBUS_DATA_FRAME[ADDR_DO] = (uint16_t)Do_Value;
		holding_registers.value = (uint16_t)Do_Value;
	}
}
