#include <Arduino.h>
enum Key
{
  up,
  down,
  left,
  right,
  enter,
  none
};

class ButtonBoard
{

  int keyboardPin;
  Key lastKey = none;
  int Key_read = 0;
  int Prev_Key_read = 1023;
  long durationMs;
  unsigned long previousMs; // will store last time Target was updated

public:
typedef void (*CallbackFunction)(Key);
  ButtonBoard(int keyboardPin_, CallbackFunction callback):callback_(callback)
  {
    keyboardPin = keyboardPin_;
    durationMs = 100;
    previousMs = 0;
  }

  void update()
  {
    // needed because too many reads causes wrong values
    unsigned long currentMs = millis();
    if ((currentMs - previousMs) >= durationMs)
    {
      buttonPressed();
      previousMs = currentMs;
    }
  }

  void buttonPressed()
  {
    Key_read = analogRead(keyboardPin);
    if (Prev_Key_read > 2100 and Prev_Key_read != Key_read and Key_read < 2300)
    {
      if (Key_read > 10 and Key_read < 190)
      {
        Serial.println("LEFT (" + (String)Key_read + ")");
        callback_(left);
      }
      else if (Key_read > 500 and Key_read < 700)
      {
        Serial.println("DOWN (" + (String)Key_read + ")");
        callback_(down);
      }
      else if (Key_read > 200 and Key_read < 400)
      {
        Serial.println("UP (" + (String)Key_read + ")");
        callback_(up);
      }
      else if (Key_read > 1900 and Key_read < 2300)
      {
        Serial.println("ENTER (" + (String)Key_read + ")");
        callback_(enter);
      }
      else if (Key_read > 900 and Key_read < 1100)
      {
        Serial.println("RIGHT (" + (String)Key_read + ")");
        callback_(right);
      }
      else
      {
        Serial.println("UNKNOWN Key " + (String)Key_read);
      }
    }
    Prev_Key_read = Key_read;
  }

  private:
  CallbackFunction callback_;
};
