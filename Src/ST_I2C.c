#include "ST_I2C.h"


#define TRUE 1
#define FALSE 0

static uint8_t ST_IIC_Start(void);				
static void ST_IIC_Stop(void);	  		
static void ST_IIC_Send_Byte(uint8_t txd);	
static uint8_t ST_IIC_Read_Byte(void);
static uint8_t ST_IIC_Wait_Ack(void); 	
static void ST_IIC_Ack(void);				
static void ST_IIC_NAck(void);		




static void IIC_delay(void)
{
   volatile uint8_t i=5;//第一次使用时需要增加到1000
   while(i)
   {
     i--;
   }
}


/**************************实现函数********************************************
*函数原型:		void ST_IIC_Start(void)
*功　　能:		产生IIC起始信号
*******************************************************************************/
static uint8_t ST_IIC_Start(void)
{
	SDA_H;
	SCL_H;
	IIC_delay();
	if(!SDA_read)
		return FALSE;	
	SDA_L;
	IIC_delay();
	if(SDA_read)
		return FALSE;	
	SDA_L;
	IIC_delay();
	return TRUE;
}

/**************************实现函数********************************************
*函数原型:		void ST_IIC_Stop(void)
*功　　能:	    //产生IIC停止信号
*******************************************************************************/
static void ST_IIC_Stop(void)
{
	SCL_L;
	IIC_delay();
	SDA_L;
	IIC_delay();
	SCL_H;
	IIC_delay();
	SDA_H;
	IIC_delay();
}

/**************************实现函数********************************************
*函数原型:		uint8_t ST_IIC_Wait_Ack(void)
*功　　能:	    等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
*******************************************************************************/
static uint8_t ST_IIC_Wait_Ack(void) 	
{
	SCL_L;
	IIC_delay();
	SDA_H;
	IIC_delay();
	SCL_H;
	IIC_delay();
	if(SDA_read)
	{
    SCL_L;
	  IIC_delay();
      return FALSE;
	}
	SCL_L;
	IIC_delay();
	return TRUE;
}

/**************************实现函数********************************************
*函数原型:		void ST_IIC_Ack(void)
*功　　能:	    产生ACK应答
*******************************************************************************/
static void ST_IIC_Ack(void)
{
	SCL_L;
	IIC_delay();
	SDA_L;
	IIC_delay();
	SCL_H;
	IIC_delay();
	SCL_L;
	IIC_delay();
}

/**************************实现函数********************************************
*函数原型:		void ST_IIC_NAck(void)
*功　　能:	    产生NACK应答
*******************************************************************************/
static void ST_IIC_NAck(void)
{
	SCL_L;
	IIC_delay();
	SDA_H;
	IIC_delay();
	SCL_H;
	IIC_delay();
	SCL_L;
	IIC_delay();
}

/**************************实现函数********************************************
*函数原型:		void ST_IIC_Send_Byte(uint8_t txd)
*功　　能:	    IIC发送一个字节
*******************************************************************************/
static void ST_IIC_Send_Byte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
			SCL_L;
			IIC_delay();
			if(SendByte&0x80)
				SDA_H;
			else
				SDA_L;
			SendByte<<=1;
			IIC_delay();
			SCL_H;
			IIC_delay();
    }
    SCL_L;
}

/**************************实现函数********************************************
*函数原型:		uint8_t ST_IIC_Read_Byte(unsigned char ack)
*功　　能:	    //读1串字节，ack=1时，发送ACK，ack=0，发送nACK
*******************************************************************************/
static unsigned char ST_IIC_Read_Byte(void)  
{
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;
    while(i--)
    {
			ReceiveByte<<=1;
			SCL_L;
			IIC_delay();
			SCL_H;
			IIC_delay();
			if(SDA_read)
			{
				ReceiveByte|=0x01;
			}
    }
    SCL_L;
    return ReceiveByte;
}

/**************************实现函数********************************************
*函数原型:		unsigned char IIC_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	I2C_Addr  目标设备地址
		addr	   寄存器地址
返回   读出来的值
*******************************************************************************/
uint8_t IIC_ReadOneByte(uint8_t SlaveAddress,uint8_t REG_Address)
{
		if(!ST_IIC_Start())
			return FALSE;
    ST_IIC_Send_Byte(SlaveAddress); 
    if(!ST_IIC_Wait_Ack())
		{
			ST_IIC_Stop();
			return FALSE;
		}
    ST_IIC_Send_Byte((uint8_t) REG_Address);   
    ST_IIC_Wait_Ack();
    ST_IIC_Start();
    ST_IIC_Send_Byte(SlaveAddress+1);
    ST_IIC_Wait_Ack();

		ST_IIC_Read_Byte();
    ST_IIC_NAck();
    ST_IIC_Stop();
    //return TRUE;
		return 0;

}


/**************************实现函数********************************************
*函数原型:		uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*功　　能:	    读取指定设备 指定寄存器的 length个值
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要读的字节数
		*data  读出的数据将要存放的指针
返回   读出来的字节数量
*******************************************************************************/
uint8_t ST_IICreadBytes(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t len,uint8_t *data)
{
		uint8_t i = 0;
		if(!ST_IIC_Start())
			return FALSE;
    ST_IIC_Send_Byte(SlaveAddress); 
    if(!ST_IIC_Wait_Ack())
		{
			ST_IIC_Stop();
			return FALSE;
		}
    ST_IIC_Send_Byte((uint8_t) REG_Address);   
    ST_IIC_Wait_Ack();
    ST_IIC_Start();
    ST_IIC_Send_Byte(SlaveAddress+1);
    ST_IIC_Wait_Ack();

		for(i = 0;i<len;i++)
		{
			if(i != (len -1))
			{
				data[i]= ST_IIC_Read_Byte();
				ST_IIC_Ack();
			}
			else
			{
				data[i]= ST_IIC_Read_Byte();
				ST_IIC_NAck();
			}
		}
		ST_IIC_Stop();
    //return TRUE;
		return 0;
}

/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*功　　能:	    将多个字节写入指定设备 指定寄存器
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要写的字节数
		*data  将要写的数据的首地址
返回   返回是否成功
*******************************************************************************/
uint8_t ST_IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{

 	uint8_t count = 0;
	ST_IIC_Start();
	ST_IIC_Send_Byte(dev);	   
	ST_IIC_Wait_Ack();
	ST_IIC_Send_Byte(reg);   
    ST_IIC_Wait_Ack();
	for(count=0;count<length;count++)
	{
		ST_IIC_Send_Byte(data[count]);
		ST_IIC_Wait_Ack();
	 }
	ST_IIC_Stop();

    return 0; //status == 0;
}

/**************************实现函数********************************************
*函数原型:		uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	dev  目标设备地址
		reg	   寄存器地址
		*data  读出的数据将要存放的地址
返回   1
*******************************************************************************/
uint8_t ST_IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data)
{
	*data=IIC_ReadOneByte(dev, reg);
    return 0;
}

/**************************实现函数********************************************
*函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
*功　　能:	    写入指定设备 指定寄存器一个字节
输入	dev  目标设备地址
		reg	   寄存器地址
		data  将要写入的字节
返回   1
*******************************************************************************/
uint8_t ST_IICwriteByte(uint8_t dev, uint8_t reg, uint8_t data)
{
    return ST_IICwriteBytes(dev, reg, 1, &data);
}


/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitNum  要修改目标字节的bitNum位
		data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1
 		失败为0
*******************************************************************************/
uint8_t ST_IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    ST_IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return ST_IICwriteByte(dev, reg, b);
}

//------------------End of File----------------------------
