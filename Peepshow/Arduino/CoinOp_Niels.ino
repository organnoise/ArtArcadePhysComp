#define DIR_PIN 15
#define STEP_PIN 14
#define MECH_PIN 6
#define BUTTON 7

static const long STEP_DELAY_SHOW = 250;
unsigned long t = 0;

const int coinInt = 0;
//Attach coinInt to Interrupt Pin 0 (Digital Pin 2). Pin 3 = Interrpt Pin 1.

volatile float coinsValue = 0.00;
//Set the coinsValue to a Volatile float
//Volatile as this variable changes any time the Interrupt is triggered
int coinsChange = 0;
int coinsChangeP = 1;
//A Coin has been inserted flag
int numSteps = 0;

boolean startShow = false;

int buttonState;
int lastButtonState;
int reading;
unsigned long lastDebounceTime;
unsigned long debounceDelay = 50;

void setup()
{
  Serial.begin(9600);
  //Start Serial Communication
  attachInterrupt(coinInt, coinInserted, RISING);
  //If coinInt goes HIGH (a Pulse), call the coinInserted function
  //An attachInterrupt will always trigger, even if your using delays
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(MECH_PIN, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  //Stops Noise!
  pinMode(2, INPUT_PULLUP);

  digitalWrite(MECH_PIN, HIGH);
}

void coinInserted()
//The function that is called every time it recieves a pulse
{
  //As we set the Pulse to represent 5p or 5c we add this to the coinsValue
  coinsChange = 1;
}

void loop()
{

  //digitalWrite(MECH_PIN, HIGH);
  //if (coinsChange == 1 ) Serial.println("Int");
  if (coinsChange == 1 ) wait();
  updateButton();
}


void step(long stepDelay) {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(STEP_PIN, LOW);
}

void runShow() {

  goUp(10000, 350);
  //WAIT
  delay(2000);
  goDown(10000, 350);
  //WAIT
  delay(2000);
  startShow == false;
  coinsChange = 0;
  attachInterrupt(coinInt, coinInserted, RISING);
  digitalWrite(MECH_PIN, HIGH);
  //Serial.println("end");
}

void wait() {
  Serial.println("Coin! now waiting...");
  digitalWrite(MECH_PIN, LOW);
  detachInterrupt(0);
  coinsChange = 2;
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
        if (coinsChange != 2) Serial.println("Button hit, but no coin");
        if (coinsChange == 2) runShow();
        //runShow();
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

}

void goUp(int steps, int _speed) {
  go(HIGH, steps, _speed);
}

void goDown(int steps, int _speed) {
  go(LOW, steps, _speed);
}

void go(int dir, int steps, int _speed) {
  digitalWrite(DIR_PIN, dir);
  //Serial.println("start");

  for (int i = 0; i < steps; i++) {
    step(_speed);
  }
}
