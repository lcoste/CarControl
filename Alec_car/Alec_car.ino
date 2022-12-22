#include "SPI.h"
#include "SD.h"
#include <TMRpcm.h>

// Pin assignments
#define leftInputPin 5
#define leftOutputPin 2
#define rightInputPin 6
#define rightOutputPin 3
#define lightInputPin 7
#define lightOutputPin 4
#define holdInputPin 8
#define SDChipSelectPin 10 
#define speakerOutputPin 9
#define hatUpInputPin A0
#define hatRightInputPin A1
#define hatLeftInputPin A2
#define hatDownInputPin A3

// Turn signal variables
bool leftPressedDown = false;
bool leftPressed = false;
bool rightPressedDown = false;
bool rightPressed = false;
bool lightPressedDown = false;
bool lightPressed = false;
bool holdPressedDown = false;
bool oldHoldPressedDown = false;
bool holdSignal = false;
bool holdLight = false;

// Debouncing variables
unsigned long lastTurnDebounceTime = 0;
unsigned long lastHoldDebounceTime = 0;
unsigned long lastLightDebounceTime = 0;
unsigned long lastSoundDebounceTime = 0;
unsigned long debounceDelay = 50;

// Sound variables
TMRpcm tmrpcm;
bool hatUpPressedDown = false;
bool hatUpPressed = false;
bool hatRightPressedDown = false;
bool hatRightPressed = false;
bool hatLeftPressedDown = false;
bool hatLeftPressed = false;
bool hatDownPressedDown = false;
bool hatDownPressed = false;

void setup() {
  // Turn setup
  pinMode(leftOutputPin, OUTPUT);
  pinMode(rightOutputPin, OUTPUT);
  pinMode(lightOutputPin, OUTPUT);
  pinMode(leftInputPin, INPUT_PULLUP);
  pinMode(rightInputPin, INPUT_PULLUP);
  pinMode(lightInputPin, INPUT_PULLUP);
  pinMode(holdInputPin, INPUT_PULLUP);

  digitalWrite(leftOutputPin, LOW);
  digitalWrite(rightOutputPin, LOW);
  digitalWrite(lightOutputPin, LOW);

  // Sound Setup
  pinMode(SDChipSelectPin, OUTPUT);
  digitalWrite(SDChipSelectPin, HIGH);
  
  pinMode(hatUpInputPin, INPUT_PULLUP);
  pinMode(hatRightInputPin, INPUT_PULLUP);
  pinMode(hatLeftInputPin, INPUT_PULLUP);
  pinMode(hatDownInputPin, INPUT_PULLUP);

  tmrpcm.speakerPin = speakerOutputPin;

  SD.begin(SDChipSelectPin);

  tmrpcm.play("startup.wav");
}

void resetOtherSignal(int outputPin) {
  if (outputPin == leftOutputPin) {
    digitalWrite(rightOutputPin, LOW);
  } else if (outputPin == rightOutputPin) {
    digitalWrite(leftOutputPin, LOW);
  }
}

void resetAllSignals() {
  digitalWrite(rightOutputPin, LOW);
  digitalWrite(leftOutputPin, LOW);
  holdSignal = false;
}

void handleSignal(bool pressed, int outputPin, bool &pressedDown) {
  if (pressed && holdPressedDown) {
    holdSignal = true;
  }

  if (pressed != pressedDown) {

    if ((millis() - lastTurnDebounceTime) > debounceDelay) {
      lastTurnDebounceTime = millis();

      if (pressed || !holdSignal) {
        digitalWrite(outputPin, pressed);
        holdSignal = false;
      }

      if (pressed) {
        resetOtherSignal(outputPin);
      }
    }
  }

  pressedDown = pressed;
}

void loop() {
  // Turn signal loop
  holdPressedDown = !digitalRead(holdInputPin);
  leftPressed = !digitalRead(leftInputPin);
  rightPressed = !digitalRead(rightInputPin);

  handleSignal(leftPressed, leftOutputPin, leftPressedDown);
  handleSignal(rightPressed, rightOutputPin, rightPressedDown);

  if (holdPressedDown && !oldHoldPressedDown && !leftPressed && !rightPressed && holdSignal) {
    if ((millis() - lastHoldDebounceTime) > debounceDelay) {
      lastHoldDebounceTime = millis();

      resetAllSignals();
    }
  }

  // Light loop
  lightPressed = !digitalRead(lightInputPin);

  if (lightPressed && holdPressedDown) {
    holdLight = true;
  }

  if (lightPressed != lightPressedDown) {

    if ((millis() - lastLightDebounceTime) > debounceDelay) {
      lastLightDebounceTime = millis();

      if (lightPressed || !holdLight) {
        digitalWrite(lightOutputPin, lightPressed);
        holdLight = false;
      }
    }
  }

  lightPressedDown = lightPressed;

  oldHoldPressedDown = holdPressedDown;

  // Sound loop
  hatUpPressed = !digitalRead(hatUpInputPin);
  hatRightPressed = !digitalRead(hatRightInputPin);
  hatLeftPressed = !digitalRead(hatLeftInputPin);
  hatDownPressed = !digitalRead(hatDownInputPin);

  if (hatUpPressed != hatUpPressedDown && hatUpPressed) {
    if ((millis() - lastSoundDebounceTime) > debounceDelay) {
      lastSoundDebounceTime = millis();

      tmrpcm.play("up.wav");
    }
  }
  if (hatRightPressed != hatRightPressedDown && hatRightPressed) {
    if ((millis() - lastSoundDebounceTime) > debounceDelay) {
      lastSoundDebounceTime = millis();

      tmrpcm.play("right.wav");
    }
  }
  if (hatLeftPressed != hatLeftPressedDown && hatLeftPressed) {
    if ((millis() - lastSoundDebounceTime) > debounceDelay) {
      lastSoundDebounceTime = millis();

      tmrpcm.play("left.wav");
    }
  }
  if (hatDownPressed != hatDownPressedDown && hatDownPressed) {
    if ((millis() - lastSoundDebounceTime) > debounceDelay) {
      lastSoundDebounceTime = millis();

      tmrpcm.play("down.wav");
    }
  }

  hatUpPressedDown = hatUpPressed;
  hatRightPressedDown = hatRightPressed;
  hatLeftPressedDown = hatLeftPressed;
  hatDownPressedDown = hatDownPressed;
}
