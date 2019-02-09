/*************************************************************************
 * This demo is adapted from BALibrary for use with the TGA Pro audio board.
 * change USE_BA_LIBRARY=1 to compile for TGAPro audio board, 
 * or keep USE_BA_LIBRARY=0 for standard teensy audio board configuration
 *
 * The latest copy of the BA Guitar library can be obtained from
 * https://github.com/Blackaddr/BALibrary
 * 
 * Teensy audio library gui: https://www.pjrc.com/teensy/gui/index.html
 */

#define USE_BA_LIBRARY 0

#include <Wire.h>
#include <Audio.h>
#if USE_BA_LIBRARY
#include "BALibrary.h"
using namespace BALibrary;
#endif
#include "effect_compressor.h"

#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <ST7735_t3.h>

#define sclk 14 //27  // SCLK can also use pin 13,14,
#define mosi 7  // MOSI can also use pin 7,11,28
#define cs   6  // CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23
#define dc   12   //  but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22
#define rst  17   // RST can use any pin

//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);
ST7735_t3 tft = ST7735_t3(cs, dc, mosi, sclk, rst);

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=87,102
AudioFilterStateVariable filter1;        //xy=254,119
AudioFilterStateVariable filter2;        //xy=272,254
AudioEffectCompressor    compressor2;    //xy=438,168
AudioEffectCompressor    compressor1;    //xy=440,106
AudioEffectCompressor    compressor5;    //xy=440,382
AudioEffectCompressor    compressor6;    //xy=443,437
AudioEffectCompressor    compressor3;    //xy=444,225
AudioEffectCompressor    compressor4;    //xy=447,336
AudioMixer4              mixer2;         //xy=608,359
AudioMixer4              mixer1;         //xy=610,149
AudioOutputI2S           i2s2;           //xy=740,266
AudioRecordQueue         queue1; 
AudioRecordQueue         queue2; 
AudioConnection          patchCord1(i2s1, 0, filter1, 0);
AudioConnection          patchCord2(i2s1, 1, filter2, 0);
AudioConnection          patchCord18(i2s1, 0, queue2, 0);
AudioConnection          patchCord3(filter1, 0, compressor1, 0);
AudioConnection          patchCord4(filter1, 1, compressor2, 0);
AudioConnection          patchCord5(filter1, 2, compressor3, 0);
AudioConnection          patchCord6(filter2, 0, compressor4, 0);
AudioConnection          patchCord7(filter2, 1, compressor5, 0);
AudioConnection          patchCord8(filter2, 2, compressor6, 0);
AudioConnection          patchCord9(compressor2, 0, mixer1, 1);
AudioConnection          patchCord10(compressor1, 0, mixer1, 0);
AudioConnection          patchCord11(compressor5, 0, mixer2, 1);
AudioConnection          patchCord12(compressor6, 0, mixer2, 2);
AudioConnection          patchCord13(compressor3, 0, mixer1, 2);
AudioConnection          patchCord14(compressor4, 0, mixer2, 0);
AudioConnection          patchCord15(mixer2, 0, i2s2, 1);
AudioConnection          patchCord16(mixer1, 0, i2s2, 0);
AudioConnection          patchCord17(mixer2, 0, queue1, 0);
// GUItool: end automatically generated code

#if USE_BA_LIBRARY
BAAudioControlWM8731      codecControl;
#else
AudioControlSGTL5000 audioShield;
#endif

unsigned long t=0;

int16_t buffer[128];
int16_t lastbuffer[128];
uint32_t oscilliscope_x = 0;

int16_t buffer2[128];
int16_t lastbuffer2[128];
uint32_t oscilliscope_x2 = 0;

void setup() {

  Serial.begin(57600);
  //while (!Serial) delay (5);

  AudioMemory(24);
  
#if USE_BA_LIBRARY
  codecControl.disable();
  delay(100);
   
  codecControl.enable();
  delay(100);
#else
  // turn on the output
  audioShield.enable();
  audioShield.volume(0.5);
#endif
  
  tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab
  tft.setRotation(3);
  tft.setTextWrap(true);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_BLUE);   
  tft.println("Teensy");
  tft.println("multichannel");
  tft.println("audio");
  tft.println("compressor...");
 
/*
  long m = micros();
  for (int16_t i=-0x7FFF; i<0x7FFF; i++) {
    if (i % 0x4F == 0) {
      //Serial.print(i);
      //Serial.print(":");
      //;
      Serial.print(compressor1.interpolate(i));
      Serial.println();
    }
  } 
  Serial.println("Done.2..\n");

  long d = micros() - m;
  Serial.println(d/1000); */

  filter1.frequency(200);
  filter2.frequency(200);

  mixer1.gain(0, 1);
  mixer1.gain(1, 1);
  mixer1.gain(2, 1.1);

  mixer2.gain(0, 1);
  mixer2.gain(1, 1);
  mixer2.gain(2, 1.1);
  
  queue1.begin();
  queue2.begin();
}

void updateScope1() {
  oscilliscope_x = oscilliscope_x + 1;
  if (oscilliscope_x > 127) {
    return;
  }

  tft.drawLine(oscilliscope_x, 64 + (lastbuffer[oscilliscope_x-1] >> 9), oscilliscope_x + 1,  64 + (lastbuffer[oscilliscope_x] >> 9), ST7735_BLACK);
  tft.drawLine(oscilliscope_x, 64 + (buffer[oscilliscope_x-1] >> 9),     oscilliscope_x + 1,      64 + (buffer[oscilliscope_x] >> 9), ST7735_GREEN);
}


void updateScope2() {
  oscilliscope_x2 = oscilliscope_x2 + 1;
  if (oscilliscope_x2 > 127) {
    return;
  }

  tft.drawLine(oscilliscope_x2, 64 + (lastbuffer2[oscilliscope_x2-1] >> 9), oscilliscope_x2 + 1,  64 + (lastbuffer2[oscilliscope_x2] >> 9), ST7735_BLACK);
  tft.drawLine(oscilliscope_x2, 64 + (buffer2[oscilliscope_x2-1] >> 9),     oscilliscope_x2 + 1,    64 + (buffer2[oscilliscope_x2] >> 9), ST7735_BLUE);
}


void loop() {
  if ( queue1.available() ) {
    if (oscilliscope_x < 128) {
      // completely discard record buffers until x location >= 128
      while (queue1.available()) {
        queue1.readBuffer();     
        queue1.freeBuffer();   
      }
    } else  {
      // oscilliscope_x >= 128
      while (queue1.available() > 1) {
        queue1.readBuffer();     
        queue1.freeBuffer();   
      }
      memcpy(lastbuffer, buffer, 256);
      memcpy(buffer, queue1.readBuffer(), 256);
      oscilliscope_x = 0;
      queue1.freeBuffer();
    }
  }

  if ( queue2.available() ) {
    if (oscilliscope_x2 < 128) {
      // completely discard record buffers until x location >= 128
      while (queue2.available()) {
        queue2.readBuffer();     
        queue2.freeBuffer();   
      }
    } else  {
      // oscilliscope_x >= 128
      while (queue2.available() > 1) {
        queue2.readBuffer();     
        queue2.freeBuffer();   
      }
      memcpy(lastbuffer2, buffer2, 256);
      memcpy(buffer2, queue2.readBuffer(), 256);
      oscilliscope_x2= 0;
      queue2.freeBuffer();
    }
  }
  
  updateScope1();      
  updateScope2();        
}
