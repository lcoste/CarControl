#include "SPI.h"
#include "SD.h"
#include <TMRpcm.h>

// Pin assignments
int leftInputPin = 2;
int leftOutputPin = 4;
int rightInputPin = 3;
int rightOutputPin = 5;
int triggerInputPin = 6;
int pewInputPin = 7;
int buzzerPin = 8;
int pewOutputPin = 9;
int SDChipSelectPin = 10;

// Turn signal variables
bool leftPressedDown = false;
bool leftPressed = false;
bool rightPressedDown = false;
bool rightPressed = false;
bool triggerPressedDown = false;
bool oldTriggerPressedDown = false;
bool holdSignal = false;

// Debouncing variables
unsigned long lastTurnDebounceTime = 0;
unsigned long lastHoldDebounceTime = 0;
unsigned long debounceDelay = 50;

// Sound variables
TMRpcm tmrpcm;
int meowProbability = 50;
bool pewPressedDown = false;
bool pewPressed = false;

void setup() {
  // Turn setup
  pinMode(leftOutputPin, OUTPUT);
  pinMode(rightOutputPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(leftInputPin, INPUT_PULLUP);
  pinMode(rightInputPin, INPUT_PULLUP);
  pinMode(triggerInputPin, INPUT_PULLUP);

  digitalWrite(leftOutputPin, LOW);
  digitalWrite(rightOutputPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Sound Setup
  pinMode(SDChipSelectPin, OUTPUT);
  digitalWrite(SDChipSelectPin, HIGH);
  tmrpcm.speakerPin = pewOutputPin;

  SD.begin(SDChipSelectPin);
  
  pinMode(pewInputPin, INPUT_PULLUP);


  delay(5000);
  tmrpcm.play("startup.wav");
}

void resetOtherSignal(int outputPin) {
  if (outputPin == leftOutputPin) {
    digitalWrite(rightOutputPin, LOW);
  } else if (outputPin == rightOutputPin) {
    digitalWrite(leftOutputPin, LOW);
  }
}

void beep() {
  digitalWrite(buzzerPin, HIGH);
  delay(10);
  digitalWrite(buzzerPin, LOW);
}

void resetAllSignals() {
  digitalWrite(rightOutputPin, LOW);
  digitalWrite(leftOutputPin, LOW);
  holdSignal = false;
}

void handleSignal(bool pressed, int outputPin, bool &pressedDown) {
  if (pressed && triggerPressedDown) {
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
  triggerPressedDown = !digitalRead(triggerInputPin);
  leftPressed = !digitalRead(leftInputPin);
  rightPressed = !digitalRead(rightInputPin);

  handleSignal(leftPressed, leftOutputPin, leftPressedDown);
  handleSignal(rightPressed, rightOutputPin, rightPressedDown);

  if (triggerPressedDown && !oldTriggerPressedDown && !leftPressed && !rightPressed && holdSignal) {
    if ((millis() - lastHoldDebounceTime) > debounceDelay) {
      lastHoldDebounceTime = millis();

      resetAllSignals();
    }
  }

  oldTriggerPressedDown = triggerPressedDown;

  // Sound loop
  pewPressed = !digitalRead(pewInputPin);

  if (pewPressed != pewPressedDown && pewPressed) {
    if (random(meowProbability)) {
      tmrpcm.play("pew.wav");
    } else {
      tmrpcm.play("meow.wav");
    }
  }

  pewPressedDown = pewPressed;
}
