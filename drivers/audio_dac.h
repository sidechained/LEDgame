#ifndef LEDGAME_DAC_H_
#define LEDGAME_DAC_H_

#include "stm32f4xx.h" // CMSIS Cortex-M4 Device Peripheral Access Layer Header File.
#include <cstddef>

static const float kSampleRate = 48000.0f;
static const float kCorrectedSampleRate = 47872.34f;
const float a0 = (440.0f / 8.0f) / kCorrectedSampleRate;

const size_t kMaxBlockSize = 24;
const size_t kBlockSize = 12;

class AudioDac {
 public:
  AudioDac() { }
  ~AudioDac() { }

  typedef struct { uint16_t l; uint16_t r; } Frame; // 16 bit audio (16-24 bit possible)

  typedef void (*FillBufferCallback)(Frame* tx, size_t size);

  void Init(int sample_rate, size_t block_size);
  void Start(FillBufferCallback callback);
  void Fill(size_t offset);

  static AudioDac* GetInstance() { return instance_; }

 private:
  // statics removed below:
  // (you don't need static unless the variable is defined with the same name in some other file)
  void Codec_GPIO_Init(void);
  void Codec_Reset(void);
  void Codec_CtrlInterface_Init(void);
  void Codec_Configure(void);
  void Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
  void Codec_AudioInterface_Init(int sample_rate);
  void DMA1_Stream7_IRQ_Init(void);
  void DirectMemoryAccess_Init(size_t block_size);
  // these are external c in .cc file, what to do?
  // void EVAL_AUDIO_TransferComplete_CallBack();
  // void EVAL_AUDIO_HalfTransfer_CallBack();
  // void EVAL_AUDIO_Error_CallBack(void* pData);
  void Codec_TIMEOUT_UserCallback(void);

  static AudioDac* instance_;

  size_t block_size_;
  FillBufferCallback callback_;
  Frame tx_dma_buffer_[kMaxBlockSize * 2]; // * 2 for l, r channels

};

#endif  // LEDGAME_DAC_H_
