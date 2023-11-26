#include <Arduino.h>
#include <WS2812B.h>
#include <ledUtils.h>

#define NUM_LEDS 16

#define LED_BUILTIN PC13

#define 	IR_SENSOR PA9
#define 	LED_1 PA0
#define 	LED_2 PA1

/*
 * Note. Library uses SPI1
 * Connect the WS2812B data input to MOSI on your board.
 */
WS2812B strip = WS2812B(NUM_LEDS);



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

private:
  unsigned long delayMillis_;
  unsigned long startMillis_;
  bool triggered_;
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
  		CoinEffects(long durationMillis, RainbowCycle & ring, Eyes eyes) : durationMillis_(durationMillis), ring_(ring), eyes_(eyes), startMillis_(0), triggered_(false) {

		}

		void start() {
			startMillis_ = millis();
			triggered_ = true;
			eyes_.start();
			ring_.start();

		};

		void update() {
		unsigned long currentMillis = millis();
			if (currentMillis - startMillis_ <= durationMillis_ && triggered_) {
				ring_.update();
			} else if(triggered_) {
				triggered_ = false;
				ring_.stop();
				eyes_.stop();
			}
 };


private:
	unsigned long durationMillis_;
	RainbowCycle ring_;
	Eyes eyes_;
	unsigned long startMillis_;
	boolean triggered_;


};



RainbowCycle rainbowCycle(strip, 5);
Eyes eyes;
CoinEffects effects(5000, rainbowCycle, eyes);

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
void rainbowCycle_(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256*5; j++) 
  { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, Wheel(strip,((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
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
	
	
	effects.update();

  int sensorStatus = digitalRead(IR_SENSOR); // Set the GPIO as Input
  if (!motionDetected && sensorStatus == 0) // Check if the pin high or not
  {
    // if the pin is high turn off the onboard Led
    digitalWrite(LED_BUILTIN, LOW); // LED LOW
    Serial.print((String)millis()); 
	Serial.println("Motion Detected!"); // print Motion Detected! on the serial monitor window
	effects.start();
	motionDetected = true;
  }
  else  if(motionDetected && millis() - lastDebounceTime > debounceDelay) { 
    //else turn on the onboard LED
    digitalWrite(LED_BUILTIN, HIGH); // LED High
    Serial.print((String)millis()); 
	Serial.println(" Motion Ended!"); 
	lastDebounceTime = millis();
	motionDetected = false;
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

