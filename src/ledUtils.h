#include <Arduino.h>
#include <WS2812B.h>

uint8_t red(uint32_t c);
uint8_t green(uint32_t c);
uint8_t blue(uint32_t c);




// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(WS2812B & strip, byte WheelPos) ;
