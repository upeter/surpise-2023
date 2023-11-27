#include <Arduino.h>
#include <WS2812B.h>
#include <ledUtils.h>
#include "buttonUtils.cpp"
#include <vector>

#define NUM_LEDS 16

#define LED_BUILTIN PC13

#define 	IR_SENSOR PA9
#define 	LED_1 PA0
#define 	LED_2 PA1
#define 	BUTTON_BOARD PA4

/*
 * Note. Library uses SPI1
 * Connect the WS2812B data input to MOSI on your board.
 */
WS2812B strip = WS2812B(NUM_LEDS);

uint8_t LEDGamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
uint32_t redColor = strip.Color(255, 0, 0); // Red
  uint32_t greenColor = strip.Color(0, 255, 0); // Green
  uint32_t blueColor = strip.Color(0, 0, 255); // Blue
  uint32_t yellowcolor = strip.Color(255, 255, 0); // Yellow
  uint32_t whitecolor = strip.Color(255, 255, 255); // White

struct ColorCode {
    uint32_t color;
    int count;
};



class NonBlockingDelay {
public:
  NonBlockingDelay(unsigned long delayMillis) {
    delayMillis_ = delayMillis;
    startMillis_ = millis();
    triggered_ = false;
  }

  bool update() {
    unsigned long currentMillis = millis();
    if (currentMillis - startMillis_ >= delayMillis_ && !triggered_) {
      triggered_ = true;
      return true;  // Delay is complete
    }
    return false;  // Delay still in progress
  }

  void restart() {
    startMillis_ = millis();
    triggered_ = false;
  }

	unsigned long getMillisStarted() {
		return startMillis_;
	}

private:
  unsigned long startMillis_;
  unsigned long delayMillis_;
  bool triggered_;
};


class CodeReveal {
public:
  CodeReveal(WS2812B & strip,std::vector<ColorCode> & colorCodes, long colorPeriodMillis, uint8_t wait) : strip_(strip), colorCodes_(colorCodes), nbDelay_(wait), colorPeriodMillis_(colorPeriodMillis), colorStartedMillis_(0), fadeMax_(100.0), fadeVal_(0), loops_(256), loopCount_(0), loopCountUp_(true) {
    // Additional setup for your function
  }
  ColorCode getNextCode() {
	if (currentIndex_ == colorCodes_.size()) {
		currentIndex_ = 0;
	}
   	return colorCodes_[currentIndex_++];
	};

  void next() {
    // Additional logic before starting the non-blocking delay
    nbDelay_.restart();
	currentCode_ = getNextCode();
	colorStartedMillis_ = millis();
	strip_.clear();
	loopCount_ = 0;
  }

  void update() {
	
    if (nbDelay_.update()) {
		//white different
		if(currentCode_.color == whitecolor) {
			for (int i = 0; i < strip_.numPixels(); i++) {
				strip_.setPixelColor(i, strip.Color(LEDGamma[loopCount_],LEDGamma[loopCount_],LEDGamma[loopCount_]));
			
			}
		} else {
		
		if(millis() - colorStartedMillis_ <= colorPeriodMillis_) {
			//all
			for (int i = 0; i < strip_.numPixels(); i++) {
				strip_.setPixelColor(i, currentCode_.color);
			}
		} else {
			//count
			for (int i = 0; i < strip_.numPixels(); i++) {
				uint32_t color = i < currentCode_.count ? currentCode_.color :	0;
				strip_.setPixelColor(i,color);
			}
        }
		}

		strip_.show();
		if (loopCount_ >= 255)
		{
			loopCountUp_ = false;
		}
		else if (loopCount_ <= 5)
		{
			loopCountUp_ = true;
		}
		if(loopCountUp_) {
			loopCount_++;
		} else {
			loopCount_--;
		}
       
		nbDelay_.restart();
    }
  }

  void stop() {
	 strip_.clear();
	 strip_.show();
  }

private:
  WS2812B strip_;
  std::vector<ColorCode> & colorCodes_;
  NonBlockingDelay nbDelay_;
  size_t currentIndex_ = 0;
  ColorCode currentCode_;
  long colorPeriodMillis_;
  long colorStartedMillis_;
  float fadeMax_;
  int fadeVal_;
  int loops_;
  int loopCount_;
  boolean loopCountUp_;
  
};




class RainbowCycle {
public:
  RainbowCycle(WS2812B & strip, uint8_t wait) : strip_(strip), nbDelay_(wait), fadeMax_(100.0), fadeVal_(0), wheelVal_(0),
                                          redVal_(0), greenVal_(0), blueVal_(0), rainbowLoops_(10), k_(0), j_(0) {
    // Additional setup for your function
  }

  void start() {
    // Additional logic before starting the non-blocking delay
    nbDelay_.restart();
  }

  void update() {
    // Your common method's logic here
    if (nbDelay_.update()) {
      // Code to execute after the delay has passed
      // This will only execute once after the delay
       nbDelay_.restart();  // Restart the delay for the next iteration

        for (int i = 0; i < strip_.numPixels(); i++) {

          wheelVal_ = Wheel(strip_, ((i * 256 / strip_.numPixels()) + j_) & 255);

          redVal_ = red(wheelVal_) * (float)(fadeVal_ / fadeMax_);
          greenVal_ = green(wheelVal_) * (float)(fadeVal_ / fadeMax_);
          blueVal_ = blue(wheelVal_) * (float)(fadeVal_ / fadeMax_);

          strip_.setPixelColor(i, strip_.Color(redVal_, greenVal_, blueVal_));
        }

        // First loop, fade in!
        if (k_ == 0 && fadeVal_ < fadeMax_ - 1) {
          fadeVal_++;
        }
        // Last loop, fade out!
        else if (k_ == rainbowLoops_ - 1 && j_ > 255 - fadeMax_) {
          fadeVal_--;
        }

        strip_.show();
        //delay(1);

        // Update loop variables for the next iteration
		if (j_ >= 256) {
			j_= 0;
			if(k_ >= rainbowLoops_) {
				kUp_ = false;
			} else {
            	kUp_ = true;
			}
			// if(kUp_) {
			// 	k_++;
			// } else {
			// 	k_--;
			// }
		}
		j_++;
    }
  }

  void stop() {
	 strip_.clear();
	 strip_.show();
  }

private:
  WS2812B strip_;
  NonBlockingDelay nbDelay_;
  float fadeMax_;
  int fadeVal_;
  uint32_t wheelVal_;
  int redVal_;
  int greenVal_;
  int blueVal_;
  int rainbowLoops_;
  int k_;
  boolean kUp_;
  int j_;
  boolean jUp_;
  
};


class Eyes {
	public:
	void start() {
		digitalWrite(LED_1, HIGH);   
		digitalWrite(LED_2, HIGH);   
	}

	void stop() {
		digitalWrite(LED_1, LOW);   
		digitalWrite(LED_2, LOW);   
	}
};

class CoinEffects {
	public:
  		CoinEffects(long durationMillis, RainbowCycle & ring, Eyes & eyes, CodeReveal & code) : durationMillis_(durationMillis), ring_(ring), eyes_(eyes), code_(code), startMillis_(0), triggered_(false) {

		}

		void start() {
			if(!triggered_) {
				startMillis_ = millis();
				triggered_ = true;
				eyes_.start();
				//ring_.start();
				code_.next();
			}

		};

		void update() {
		unsigned long currentMillis = millis();
			if (currentMillis - startMillis_ <= durationMillis_ && triggered_) {
				//ring_.update();
				code_.update();
			} else if(triggered_) {
				triggered_ = false;
				//ring_.stop();
				eyes_.stop();
				code_.stop();
			}
 };


private:
	unsigned long durationMillis_;
	RainbowCycle ring_;
	Eyes eyes_;
	CodeReveal code_;
	unsigned long startMillis_;
	boolean triggered_;


};

class Unlocker{

	public:
  		Unlocker(std::vector<ColorCode> & colorCodes, WS2812B & strip):colorCodes_(colorCodes), strip_(strip), unlocked_(false) {
			initRequirdColorCode();
			resetCounts();
		}

		void initRequirdColorCode() {
			for (int i = 0; i < colorCodes_.size(); i++) {
				ColorCode code =  colorCodes_[i];
				if(code.color != whitecolor) {
					for (int j = 0; j < code.count; j++) {
						requiredColorCodes_.push_back(code.color);
					}
				}
			}
		}

		void resetCounts() {
			chosenColorCodes_.clear();
		}

		boolean checkUnlock() {
			Serial.println("Chosen Size: " + (String)chosenColorCodes_.size() + " Required Size: " + (String)requiredColorCodes_.size());

			if(chosenColorCodes_.size() != requiredColorCodes_.size()) {
				return false;
			} else {
				for (int i = 0; i < chosenColorCodes_.size(); i++) {
					Serial.println("Chosen: " + (String)chosenColorCodes_[i] + " Required: " + (String)requiredColorCodes_[i]);
				}

				for (int i = 0; i < chosenColorCodes_.size(); i++) {
					if(chosenColorCodes_[i] != requiredColorCodes_[i]) {
						return  false;
					}
				}
			}
			return true;

			// int chosenColor, requiredColor; 
			// for (const auto &colorCode : colorCodes_) {
			// 	uint32_t color = colorCode.color;
			// 	if (color == redColor) {
			// 		redCount = colorCode.count;
			// 	} else if (color == greenColor) {
			// 		greenCount = colorCode.count;
			// 	} else if (color == blueColor) {
			// 		blueCount = colorCode.count;
			// 	} else if (color == yellowcolor) {
			// 		yellowCount = colorCode.count;
			// 	}
			// }
			// Serial.println("Code Red:    " + (String)redCount_ + " Entry: " + (String)redCount);
			// Serial.println("Code Blue:   " + (String)blueCount_ + " Entry: " + (String)blueCount);
			// Serial.println("Code Green:  " + (String)greenCount_ + " Entry: " + (String)greenCount);
			// Serial.println("Code Yellow: " + (String)yellowCount_ + " Entry: " + (String)yellowCount);
			// return redCount_ == redCount && blueCount_ == blueCount && greenCount_ == greenCount && yellowCount_ && yellowCount;
		}

		void updateCount(Key k) {
			strip_.clear();
			int color = 0;
			if(k == up) {
				color = redColor;
			} else if(k == down) {
				color = greenColor;
			} else if(k == left) {
				color = blueColor;
			} else if(k == right) {
				color = yellowcolor;
			} 
			if(color != 0 && chosenColorCodes_.size() <= strip_.numPixels()) {
				chosenColorCodes_.push_back(color);
				showChosenColorCode();
			}

			if(k == enter) {
 				//clear all colors
				clearColors();

				//if it was unlocked, lock it
				if(unlocked_) {
					unlocked_ = false;
					clearColors();
				} else {
					unlocked_ = checkUnlock();
					Serial.println("Unlock is: " + (String)unlocked_);
					if(!unlocked_) {
						showWrongCode();
					} 
					resetCounts();
				}

			}
		} 

		void showChosenColorCode() {
			for (int i = 0; i < chosenColorCodes_.size(); i++) {
					strip_.setPixelColor(i+1, chosenColorCodes_[i]);
				}				
				strip_.show();
		}

		void clearColors() {
				for(uint16_t i=0; i<strip_.numPixels(); i++) {
        			strip_.setPixelColor(i, 0);
				}
				strip_.show();
		}

		void showWrongCode() {
				for(uint16_t j=0; j<5; j++) {
					for(uint16_t i=0; i<strip_.numPixels(); i++) {
						strip_.setPixelColor(i, redColor);
					}
				strip_.show();
				delay(400);
				clearColors();
				delay(400);
			}
		}

		// void showSuccessCode() {
		// 		uint16_t i, j;

		// 		while(unlocked_){ 
		// 			for(i=0; i< strip_.numPixels(); i++) {
		// 				strip_.setPixelColor(i, Wheel(strip_, ((i * 256 / strip.numPixels()) + j) & 255));
		// 			}
		// 			strip_.show();
		// 			delay(20);
		// 		}
		// 	}

		

		void showRainbow() {

		}


		boolean unlocked() {
			return unlocked_;
		};
			private:	 
				std::vector<ColorCode> colorCodes_;
				WS2812B strip_;
				std::vector<int> requiredColorCodes_ = {};
				std::vector<int> chosenColorCodes_ = {};
				int redCount_;
				int greenCount_;
				int blueCount_;
				int yellowCount_;
				boolean unlocked_;


};

std::vector<ColorCode> colorCodes = {
    {whitecolor, 16},
	{redColor, 1},
    {greenColor, 2},
    {blueColor, 2},
	{yellowcolor, 3},
	{whitecolor, 16}
};
CodeReveal codeReveal(strip, colorCodes, 3000, 10);
RainbowCycle rainbowCycle(strip, 5);
Eyes eyes;
CoinEffects effects(5000, rainbowCycle, eyes, codeReveal);
Unlocker unlocker(colorCodes, strip);
ButtonBoard board = ButtonBoard(BUTTON_BOARD, [](Key k) -> void {  unlocker.updateCount(k);});



const unsigned long debounceDelay = 500;  // Adjust the debounce delay as needed
volatile unsigned long lastDebounceTime = 0;
volatile bool motionDetected = false;  // Flag to indicate motion detection


void setup() 
{
      Serial.begin(9600);
	  delay(500);
	
	pinMode(LED_BUILTIN, OUTPUT);
	 pinMode(IR_SENSOR, INPUT); // IR 
	 pinMode(LED_1, OUTPUT);
	 pinMode(LED_2, OUTPUT);
  	Serial.println("strip begin");
 
  strip.begin();// Sets up the SPI
  strip.show();// Clears the strip, as by default the strip data is set to all LED's off.
  Serial.println("setup complete");
 // strip.setBrightness(8);
 
     //digitalWrite(LED_1, HIGH);   // turn the LED on (HIGH is the voltage level)
    
}



void loop()
{ 
//	Serial.println("loop");
//     digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//   delay(1000);                       // wait for a second
//    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//   delay(100);   
  //rainbowCycle(200);
  
  	//rainbowCycle.update();
	// if(millis() - lastDebounceTime > debounceDelay) {
	// 	if (motionDetected) {
	// 		// Perform action when motion is detected
	// 		digitalWrite(LED_BUILTIN, HIGH);  // Turn off the onboard LED
	// 		Serial.println("Motion Detected!");  // Print "Motion Detected!" on the serial monitor window

	// 		// Reset the flag and update the last debounce time
	// 		motionDetected = false;
	// 	} else {
	// 		digitalWrite(LED_BUILTIN, LOW);  // Turn off the onboard LED

	// 	}
	// 		lastDebounceTime = millis();
	// }
	
	board.update();
	if(unlocker.unlocked()) {
		rainbowCycle.update();
	} else {
	effects.update();

  int sensorStatus = digitalRead(IR_SENSOR); // Set the GPIO as Input
  if (!motionDetected && sensorStatus == 0) // Check if the pin high or not
  {
    // if the pin is high turn off the onboard Led
    digitalWrite(LED_BUILTIN, LOW); // LED LOW
    //Serial.print((String)millis()); 
	//Serial.println("Motion Detected!"); // print Motion Detected! on the serial monitor window
	effects.start();
	motionDetected = true;
  }
  else  if(motionDetected && millis() - lastDebounceTime > debounceDelay) { 
    //else turn on the onboard LED
    digitalWrite(LED_BUILTIN, HIGH); // LED High
    //Serial.print((String)millis()); 
	//Serial.println(" Motion Ended!"); 
	lastDebounceTime = millis();
	motionDetected = false;
  }
	}


  // colorWipe(strip.Color(0, 255, 0), 20); // Green
  // colorWipe(strip.Color(255, 0, 0), 20); // Red
  // colorWipe(strip.Color(0, 0, 255), 20); // Blue
  //rainbow(10);
   //rainbowCycle();
  //theaterChase(strip.Color(255, 0, 0), 20); // Red
  // theaterChase(strip.Color(0, 255, 0), 20); // Green
  // theaterChase(strip.Color(0, 0, 255), 20); // Blue
  // theaterChaseRainbow(10);
  // whiteOverRainbow(20,75,5);  
  // pulseWhite(5); 
  // delay(250);
  // fullWhite();
  // delay(250);
	//rainbowFade2White(3,3,1);
}

