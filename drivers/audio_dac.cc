#include "audio_dac.h"

namespace ledGame {

/* static */
AudioDac* AudioDac::instance_;

void AudioDac::Init(int sample_rate, size_t block_size) {
  instance_ = this;
  block_size_ = block_size;
  callback_ = NULL;
  Codec_GPIO_Init();
  Codec_Reset();
  Codec_CtrlInterface_Init();
  Codec_Configure();
  Codec_AudioInterface_Init(sample_rate);
  DMA1_Stream7_IRQ_Init();
  DirectMemoryAccess_Init(block_size);
}

void AudioDac::Start(FillBufferCallback callback) {
  callback_ = callback;
  DMA_Cmd(DMA1_Stream7, ENABLE); /* Enable the I2S DMA Stream*/
}

void AudioDac::Fill(size_t offset) {
  (*callback_)(&tx_dma_buffer_[offset * block_size_], block_size_);
}

void AudioDac::Codec_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable Reset GPIO Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

  // Audio reset pin configuration
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // Enable I2S and I2C GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);

  // CODEC_I2C SCL anblock_size_A pins configuration
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Connect pins to I2C peripheral */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

  /* CODEC_I2S pins configuration: WS, SCK and SD pins -----------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // Connect pins to I2S peripheral
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  // CODEC_I2S pins configuration: MCK pin
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  // Connect pins to I2S peripheral
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
}

void AudioDac::Codec_Reset(void)
{
  /* Power Down the codec */
  GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_RESET);

  /* wait for a delay to insure registers erasing */
  __IO uint32_t nCount = 0x4FFF; // AKA CODEC_RESET_DELAY
  for (; nCount != 0; nCount--); // Inserts a delay time (not accurate timing).

  /* Power on the codec */
  GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET);
}

void AudioDac::Codec_CtrlInterface_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;

  /* Enable the CODEC_I2C peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  /* CODEC_I2C peripheral configuration */
  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x33;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;
  /* Enable the I2C peripheral */
  I2C_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &I2C_InitStructure);
}

void AudioDac::Codec_Configure(void)
{
  /* Keep Codec powered OFF */
  Codec_WriteRegister(0x02, 0x01);

  Codec_WriteRegister(0x04, 0xAF); /* SPK always OFF & HP always ON */

  /* Clock configuration: Auto detection */
  Codec_WriteRegister(0x05, 0x81);

  /* Set the Slave Mode and the audio Standard */
  Codec_WriteRegister(0x06, 0x04); // I2S_Standard_Phillips

  // /* Set the Master volume */
  // Codec_WriteRegister(0x20, 0x00); // 0dB change
  // Codec_WriteRegister(0x21, 0x00); // 0db change

  #define VOLUME_CONVERT(x)       ((x > 100)? 100:((uint8_t)((x * 255) / 100)))
  uint8_t Volume = VOLUME_CONVERT(70);

  /* Set the Master volume */
  if (Volume > 0xE6)
  {
    /* Set the Master volume */
    Codec_WriteRegister(0x20, Volume - 0xE7);
    Codec_WriteRegister(0x21, Volume - 0xE7);
  }
  else
  {
    /* Set the Master volume */
    Codec_WriteRegister(0x20, Volume + 0x19);
    Codec_WriteRegister(0x21, Volume + 0x19);
  }

  /* Power on the Codec */
  Codec_WriteRegister(0x02, 0x9E);

  /* Additional configuration for the CODEC. These configurations are done to reduce
      the time needed for the Codec to power off. If these configurations are removed,
      then a long delay should be added between powering off the Codec and switching
      off the I2S peripheral MCLK clock (which is the operating clock for Codec).
      If this delay is not inserted, then the codec will not shut down properly and
      it results in high noise after shut down. */

  /* Disable the analog soft ramp */
  Codec_WriteRegister(0x0A, 0x00);
  /* Disable the digital soft ramp */
  Codec_WriteRegister(0x0E, 0x04);
  /* Disable the limiter attack level */
  Codec_WriteRegister(0x27, 0x00);
  /* Adjust Bass and Treble levels */
  Codec_WriteRegister(0x1F, 0x0F);
  /* Adjust PCM volume level */
  Codec_WriteRegister(0x1A, 0x0A); // 0x34
  Codec_WriteRegister(0x1B, 0x0A); // 0x34
}

void AudioDac::Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue)
{
  #define CODEC_FLAG_TIMEOUT             ((uint32_t)0x1000)
  #define CODEC_LONG_TIMEOUT             ((uint32_t)(300 * CODEC_FLAG_TIMEOUT))
  __IO uint32_t  CODECTimeout = CODEC_LONG_TIMEOUT;

  /*!< While the bus is busy */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* Start the config sequence */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, 0x94, I2C_Direction_Transmitter); // 0x94  /* b00100111 */ /* The 7 bits Codec address (sent through I2C interface) */

  /* Test on EV6 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* Transmit the first address for write operation */
  I2C_SendData(I2C1, RegisterAddr);

  /* Test on EV8 and clear it */
  CODECTimeout = CODEC_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* Prepare the register value to be sent */
  I2C_SendData(I2C1, RegisterValue);

  /*!< Wait till all data have been physically transferred on the bus */
  CODECTimeout = CODEC_LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF))
  {
    if((CODECTimeout--) == 0) Codec_TIMEOUT_UserCallback();
  }

  /* End the configuration sequence */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void AudioDac::Codec_AudioInterface_Init(int sample_rate)
{
  I2S_InitTypeDef I2S_InitStructure;
  /* Enable the CODEC_I2S peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  /* CODEC_I2S peripheral configuration */
  SPI_I2S_DeInit(SPI3);
  I2S_InitStructure.I2S_AudioFreq = sample_rate;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  /* Initialize the I2S peripheral with the structure above */
  I2S_Init(SPI3, &I2S_InitStructure);
  // Note: I2S is NOT enabled here, this is done after DMA init with command: I2S_Cmd(SPI3, ENABLE);
}

void AudioDac::DMA1_Stream7_IRQ_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* I2S DMA IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void AudioDac::DirectMemoryAccess_Init(size_t block_size)
{
  // mutable elements codec.cc/.h are good reference points for this code:
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  DMA_Cmd(DMA1_Stream7, DISABLE);
  DMA_DeInit(DMA1_Stream7);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40003C0C;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(&tx_dma_buffer_[0]);
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 2 * block_size * 2; // 2 channels, 2 half blocks
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream7, &DMA_InitStructure);

  /* Enable the selected DMA interrupts (selected in "stm32f4_discovery_eval_audio_codec.h" defines) */
  DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Stream7, DMA_IT_HT, ENABLE);

  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE); /* Enable the I2S DMA request */
  I2S_Cmd(SPI3, ENABLE); // enable I2S over SPI3
}

void AudioDac::Codec_TIMEOUT_UserCallback(void)// Basic management of the timeout situation.
{
  while (1) { }
}

}  // namespace plaits

extern "C" {

void DMA1_Stream7_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_Stream7, DMA_FLAG_TCIF7) != RESET)
  {
    //fill second half of buffer whilst DMA transfers data from the first half:
    ledGame::AudioDac::GetInstance()->Fill(1);
    /* Clear the Interrupt flag */
    DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
  }

  /* Half Transfer complete interrupt */
  if (DMA_GetFlagStatus(DMA1_Stream7, DMA_FLAG_HTIF7) != RESET)
  {
    //fill first half of buffer whilst DMA transfers data from the second half:
    ledGame::AudioDac::GetInstance()->Fill(0);
    /* Clear the Interrupt flag */
    DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_HTIF7);
  }
}

}  // namespace ledGame
