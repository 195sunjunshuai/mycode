#ifndef __W25Q64_H_ 
#define __W25Q64_H_ 
 
#include "stm32f10x.h" 
#define sFLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */
#define sFLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_SE             0x20  /*!< Sector Erase instruction *///需要改
#define sFLASH_CMD_BE             0xC7  /*!< Bulk Erase instruction */
#define sFLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define sFLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define sFLASH_DUMMY_BYTE         0xA5
#define sFLASH_SPI_PAGESIZE       0x100
typedef struct {
    uint32_t id;        
    char     name[16];
    float    score;
} Cfg;
#define sFLASH_SPI_SECTOR         0x1000   //扇区大小 
void sFLASH_EraseSector_NUM(uint32_t SectorAddr,uint16_t Num); 
void W25Q64_ReadID_0x90();
void W25Q64_ReadID_0x9F();
static uint8_t W25Q64_ReadSR1(void);
static void W25Q64_WriteEnable(void);
static void W25Q64_WaitBusy(void);
void W25Q64_SectorErase(uint32_t addr);
void W25Q64_WriteStruct(uint32_t addr, const Cfg *stu);
void W25Q64_ReadStruct(uint32_t addr, Cfg *stu);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void PARA_Init (void);
#endif