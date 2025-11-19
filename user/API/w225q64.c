#include "w25q64.h"
#include "spi.h"
#include "stdio.h"

//读ID  参考W25Q64手册中文  10.2.15编程
void W25Q64_ReadID_0x90(void)
{
	uint8_t Buff[2]={0};
	//1.片选拉低
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	
	//2.发送命令0x90
	SPI_Send_Rec_Byte(0x90);
	
	//3.连续发送3个字节的地址
	SPI_Send_Rec_Byte(0x00);
	SPI_Send_Rec_Byte(0x00);
	SPI_Send_Rec_Byte(0x00);	
	
	//4.接收第一个字节
	Buff[0]=SPI_Send_Rec_Byte(0x5F);  //0x5F就是个假数据,和命令不冲突就行,无意义
	
	//5.接收第二个字节	
	Buff[1]=SPI_Send_Rec_Byte(0x5F);  //0x5F就是个假数据,和命令不冲突就行,无意义

	//6.把片选拉高
	GPIO_SetBits(GPIOB,GPIO_Pin_12);	
  printf("收到0x90返回的数据 %x %x\r\n",Buff[0],Buff[1]);	
}



//读ID  参考W25Q64手册英文  11.2.28编程
void W25Q64_ReadID_0x9F(void)
{
	uint8_t Buff[3]={0};
	//1.片选拉低
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	
	//2.发送命令0x9F
	SPI_Send_Rec_Byte(0x9F);
	
	//3.连续接收3个字节
	Buff[0]=SPI_Send_Rec_Byte(0x5F);  //0x5F就是个假数据,和命令不冲突就行,无意义
  Buff[1]=SPI_Send_Rec_Byte(0x5F); 
  Buff[2]=SPI_Send_Rec_Byte(0x5F);	
	
	//4.把片选拉高
	GPIO_SetBits(GPIOB,GPIO_Pin_12);	
  printf("收到0x9F返回的数据 %x %x %x\r\n",Buff[0],Buff[1],Buff[2]);
  printf("收到0x9F返回的数据%x\r\n",(Buff[0]<<16)+(Buff[1]<<8)+Buff[2]);	
}

/**读状态寄存器(0x05)   W25Q64手册 中文 10.2.6
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void sFLASH_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sFLASH_SendByte(sFLASH_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  }
  while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}


/**写使能(0x06)   W25Q64手册 英文 11.2.4
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Write Enable" instruction */
  sFLASH_SendByte(sFLASH_CMD_WREN);

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}


/**扇区擦除(0x20)   W25Q64手册 中文 10.2.16
参数  扇区首地址
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void sFLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
  sFLASH_SendByte(sFLASH_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);  //发送24位的高8位
  /*!< Send SectorAddr medium nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);   //发送24位的中间8位
  /*!< Send SectorAddr low nibble address byte */
  sFLASH_SendByte(SectorAddr & 0xFF);       //发送24位的低8位
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**页编程(0x02)   W25Q64手册 中文 10.2.14 -- 不支持跨页
参数1  待写入的数据的地址
参数2  待写入W25Q64中的地址
参数3  待写入的长度
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  sFLASH_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sFLASH_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sFLASH_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**任意写  -- 二次封装得到的，解决页编程不支持跨页的问题
参数1  待写入的数据的地址
参数2  待写入W25Q64中的地址
参数3  待写入的长度
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
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

/**(0x03)   W25Q64手册 中文 10.2.8
参数1  从W25Q64读取数据存放的首地址
参数2  读取W25Q64的地址
参数3  待读取的长度
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  sFLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}
