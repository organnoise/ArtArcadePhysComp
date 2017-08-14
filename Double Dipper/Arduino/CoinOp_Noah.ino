#define MECH_PIN 6
#define TAIL_PIN 12


//Set dip time here!
//2000 = 2 seconds
int dipTime = 20000;

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


void setup()
{
  Serial.begin(9600);
  //Start Serial Communication
  //attachInterrupt(coinInt, coinInserted, RISING);
  //If coinInt goes HIGH (a Pulse), call the coinInserted function
  //An attachInterrupt will always trigger, even if your using delays

  pinMode(MECH_PIN, OUTPUT);
  pinMode(TAIL_PIN, OUTPUT);

  //Stops Noise!
  pinMode(2, INPUT_PULLUP);

  attachInterrupt(coinInt, coinInserted, RISING);

  digitalWrite(MECH_PIN, HIGH);
  digitalWrite(TAIL_PIN, LOW);
}



void coinInserted()
//The function that is called every time it recieves a pulse
{
  //As we set the Pulse to represent 5p or 5c we add this to the coinsValue
  if(coinsChange == 0) coinsChange = 1;
  //Flag that there has been a coin inserted
}

void loop()
{
  if (coinsChange == 1)
    //Check if a coin has been Inserted
  {
    coinsChange = 2;
    //Run dip
    if(coinsChange == 2) dip();
  }
}



void dip() {
  coinsChange = 3;
  Serial.println("Coin!...");

  //Turn off Coin Mech, Turn on Motor
  digitalWrite(MECH_PIN, LOW);
  digitalWrite(TAIL_PIN, HIGH);
  //Run for x seconds
  delay(dipTime);
  //Turn on Mech, Turn off motor
  digitalWrite(MECH_PIN, HIGH);
  digitalWrite(TAIL_PIN, LOW);
  
  delay(2000);
  coinsChange = 0;
  Serial.println("Done");
}



