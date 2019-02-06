#ifndef PAULSTOFFREGEN_EFFECT_COMPRESSOR_H
#define PAULSTOFFREGEN_EFFECT_COMPRESSOR_H

#include <Arduino.h>
#include "AudioStream.h"

const long NumlogTablePoints = 100;

class AudioEffectCompressor : public AudioStream {
public:
    AudioEffectCompressor() : AudioStream(1, inputQueueArray) { 
      buildLogTable();
    }
    virtual void update(void);

    uint16_t getThreshold();
    uint8_t getGainReduction();
    float getInputGain();
    bool getEnabled();

    void setThreshold(uint16_t threshold);
    void setGainReduction(uint8_t gainReduction);
    void setInputGain(float inputGain);
    void setEnabled(bool enabled);
    void toggleEnabled();
    int16_t interpolate(int16_t input);
    static bool _initialized;
    
private:
    
    bool _enabled = true;
    uint16_t _threshold = 0x5000;
    uint8_t _gainReduction = 3;
    float _inputGain = 1.0;
    void buildLogTable();
   
    audio_block_t *inputQueueArray[1];

    static double logtable[NumlogTablePoints];
};

#endif //PAULSTOFFREGEN_EFFECT_COMPRESSOR_H
