#include "w25q64.h"
#include "spi.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
//#include "esp.h"
//#include "mqtt.h"
void W25Q64_ReadID_0x90()
{
	//1.片选拉低 
//	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET); 
  //2.发送数据0x90
  SPI2_SendRecByte(0x90);
  //3.连续发送三个字节的地址
  SPI2_SendRecByte(0x00);	
	SPI2_SendRecByte(0x00);
	SPI2_SendRecByte(0x00);
	//4.接受第一个字节
	uint8_t Buff[2]={0};
	Buff[0]=SPI2_SendRecByte(0x5F); //假数据，和命令不冲突；
	//5.再接收一个字节
	Buff[1]=SPI2_SendRecByte(0x5F);
	//6.把片选拉高
//	GPIO_SetBits(GPIOB,GPIO_Pin_12);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);  
	printf("收到0x90返回的数据 %x %x\r\n",Buff[0],Buff[1]);
}

void W25Q64_ReadID_0x9F()
{
	//1.片选拉低 
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
  //2.发送数据0x9F
  SPI2_SendRecByte(0x9F);
  
	//4.接受第一个字节
	uint8_t Buff[3]={0};
	Buff[0]=SPI2_SendRecByte(0x5F); //假数据，和命令不冲突；
	Buff[1]=SPI2_SendRecByte(0x5F);
	Buff[2]=SPI2_SendRecByte(0x5F);
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);
	printf("收到0x9F返回的数据 %x %x %x\r\n",Buff[0],Buff[1],Buff[2]);
}
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  W25Q64_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  SPI2_SendRecByte(sFLASH_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  SPI2_SendRecByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  SPI2_SendRecByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  SPI2_SendRecByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    SPI2_SendRecByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  W25Q64_WaitBusy();
}
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  /* 计算地址在页内的偏移和第一页剩余空间 */
  Addr = WriteAddr % sFLASH_SPI_PAGESIZE;           /* 计算页内偏移地址 */
  count = sFLASH_SPI_PAGESIZE - Addr;               /* 计算当前页剩余空间 */
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE; /* 计算完整页数 */
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE; /* 计算剩余字节数 */

  /* 情况1：起始地址页对齐 */
  if (Addr == 0)
  {
    /* 子情况1.1：写入数据不超过一页 */
    if (NumOfPage == 0)
    {
      sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    /* 子情况1.2：写入数据超过一页 */
    else
    {
      /* 先写入所有完整页 */
      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr += sFLASH_SPI_PAGESIZE;  //偏移到W25Q64的下一页
        pBuffer += sFLASH_SPI_PAGESIZE;    //偏移到待写入数组的未写入部分
      }
      
      /* 写入剩余不足一页的数据 */
      sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  /* 情况2：起始地址不对齐 */
  else
  {
    /* 子情况2.1：写入数据不超过一页 */
    if (NumOfPage == 0)
    {
      /* 子情况2.1.1：写入数据会跨越页边界 */
      if (NumOfSingle > count)
      {
        temp = NumOfSingle - count;
        
        /* 先写入第一页剩余空间 */
        sFLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;
        
        /* 再写入第二页的数据 */
        sFLASH_WritePage(pBuffer, WriteAddr, temp);
      }
      /* 子情况2.1.2：写入数据不跨越页边界 */
      else
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    /* 子情况2.2：写入数据超过一页 */
    else
    {
      /* 调整剩余要写入的字节数 */
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / sFLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;
      
      /* 先写入第一页的剩余空间 */
      sFLASH_WritePage(pBuffer, WriteAddr, count);
      WriteAddr += count;
      pBuffer += count;
      
      /* 写入所有完整页 */
      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr += sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }
      
      /* 如果还有剩余数据，写入最后一页 */
      if (NumOfSingle != 0)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
   /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  SPI2_SendRecByte(sFLASH_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  SPI2_SendRecByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  SPI2_SendRecByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  SPI2_SendRecByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = SPI2_SendRecByte(0xA5);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  sFLASH_CS_HIGH();
}


/* —— 写结构体 —— */
void W25Q64_WriteStruct(uint32_t addr, const Cfg *stu)
{
    const uint8_t *p = (const uint8_t*)stu;
    uint16_t n = sizeof(*stu);

    W25Q64_WriteEnable();
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
    SPI2_SendRecByte(0x02);              // Page Program
    SPI2_SendRecByte(addr >> 16);
    SPI2_SendRecByte(addr >> 8);
    SPI2_SendRecByte(addr);

    for (uint16_t i = 0; i < n; i++) {
        SPI2_SendRecByte(p[i]);
    }
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);

    W25Q64_WaitBusy();                    // 等待编程完成
}

/* —— 读结构体 —— */
void W25Q64_ReadStruct(uint32_t addr, Cfg *stu)
{
    uint8_t *p = (uint8_t*)stu;
    uint16_t n = sizeof(*stu);

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
    SPI2_SendRecByte(0x03);              // Read Data
    SPI2_SendRecByte(addr >> 16);
    SPI2_SendRecByte(addr >> 8);
    SPI2_SendRecByte(addr);
    for (uint16_t i = 0; i < n; i++) {
        p[i] = SPI2_SendRecByte(0xFF);
    }
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);
}

static void W25Q64_WriteEnable(void)
{
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
    SPI2_SendRecByte(0x06);              // WREN
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);
}

static uint8_t W25Q64_ReadSR1(void)
{
    uint8_t sr;
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
    SPI2_SendRecByte(0x05);              // RDSR1
    sr = SPI2_SendRecByte(0xFF);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);
    return sr;
}

static void W25Q64_WaitBusy(void)
{
    while (W25Q64_ReadSR1() & 0x01) { }   // WIP=1 忙
}

void W25Q64_SectorErase(uint32_t addr)
{
    W25Q64_WriteEnable();
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_RESET);
    SPI2_SendRecByte(0x20);              // Sector Erase 4KB
    SPI2_SendRecByte(addr >> 16);
    SPI2_SendRecByte(addr >> 8);
    SPI2_SendRecByte(addr);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,GPIO_PIN_SET);
    W25Q64_WaitBusy();
}
void sFLASH_EraseSector_NUM(uint32_t SectorAddr,uint16_t Num) 
{ 
 for(uint16_t i=0;i<Num;i++) 
 { 
  W25Q64_SectorErase(SectorAddr);  //调用扇区擦除函数 
  SectorAddr+=sFLASH_SPI_SECTOR;   //偏移到下一个待擦除的扇区 
 } 
} 
//重写单字节发送和接收函数                                          
uint8_t SPI2_SendRecByte(uint8_t Byte) 
{ 
//HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData, 
//                                          
/* 
参数1    指定的SPI，也就是SPI几 
参数2    待发送的数据地址 
参数3    待接收的数据存放的地址 
参数4    发送和接收长度 
参数5    超时时间 
*/ 
uint8_t Rec_Data; 
HAL_SPI_TransmitReceive(&hspi2,&Byte,&Rec_Data,1,0x100); 
return Rec_Data; 
} 





