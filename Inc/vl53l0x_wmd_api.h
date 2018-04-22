/** 
* @brief        激光传感器导出的API函数
* @details  		因为现在的需求和本身能力 追求简单化只输出了两个函数 一个是初始化 一个就是读取值 
* @author      	WMD
* @date     		2018年4月22日23:26:22 
* @version  		1.1
* @par Copyright (c):  
*       WMD 
* @par 日志
*/  
#ifndef _VL53L0X_WMD_API_H
#define _VL53L0X_WMD_API_H


#ifdef __cplusplus
extern "C"
{
#endif
	
//!传感器初始化
void VL53L0X_Init(void);

//!获得距离值
unsigned short int VL53L0X_GetValue(void);

#ifdef __cplusplus
}
#endif

#endif
