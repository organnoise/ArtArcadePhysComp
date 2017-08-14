#define MECH_PIN 6
#define SHOW_PIN 12
#define BUTTON 7


const int coinInt = 0;
//Attach coinInt to Interrupt Pin 0 (Digital Pin 2). Pin 3 = Interrpt Pin 1.

//Set the coinsValue to a Volatile float
//Volatile as this variable changes any time the Interrupt is triggered
int coinsChange = 0;
int coinsChangeP = 1;
//A Coin has been inserted flag
int numSteps = 0;

boolean mechState;

int buttonState;
int lastButtonState;
int reading;

unsigned long lastDebounceTime;
unsigned long debounceDelay = 50;

char bytes[2];

void setup()
{
  Serial.begin(9600);
  //Start Serial Communication
  //attachInterrupt(coinInt, coinInserted, RISING);
  //If coinInt goes HIGH (a Pulse), call the coinInserted function
  //An attachInterrupt will always trigger, even if your using delays

  pinMode(MECH_PIN, OUTPUT);
  pinMode(SHOW_PIN, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  //Stops Noise!
  //pinMode(2, INPUT_PULLUP);

  digitalWrite(MECH_PIN, HIGH);
}


void loop()
{
  serialRead();
  digitalWrite(MECH_PIN, mechState);
  digitalWrite(SHOW_PIN, mechState);
  updateButton();
}



void coinEvent() {
  //Serial.println("Coin!...");
  //coinsChange = 0;
  //digitalWrite(MECH_PIN, LOW);
  mechState = LOW;
  serialPrint(0, 1);
}

void updateButton() {

  // read the state of the switch into a local variable:

  reading = digitalRead(BUTTON);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        //DO SOMETHING HERE
        //if (coinsChange != 2) Serial.println("Button hit, but no coin");
        //if (coinsChange == 2) runShow();

        //LISTEN FOR COIN!!
        coinEvent();
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

}

void serialPrint(int section, int data) {
  Serial.print("[");
  Serial.print(section);
  Serial.print(",");
  Serial.print(data);
  Serial.println("]");
}


void serialRead() {
  if (Serial.available() ) {
    if (Serial.read() == 0xff) {
      Serial.readBytes(bytes, 2);
      if (bytes[0] < 2) {
        if (mechState != int(bytes[1])) {
          mechState = int(bytes[1]);
          //serialPrint(0, 2);
          delay(1000);
        }
      }
    }
  }
}


