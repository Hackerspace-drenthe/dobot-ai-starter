#include "AIStarter.h";

void setup() {
  // direct starten
  promgramRun = true;

  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(36, INPUT);
  AIStarter_SmartBotInit();
  // AIStarter_SmartBotSetMotorPI(3.4, 0.2);

  //   AIStarter_SmartBotSetLED(LED1, BLINK);

  // AIStarter_SmartBotSetMotor(MOTORL, 50);
  // AIStarter_SmartBotSetMotor(MOTORR, 50);
  // AIStarter_SmartBotSetMovment(FRONT, 50);
}

float sensorLocaties[6] = {-30, -15, -5, 5, 15, 30};

// bereken positie van lijn tov het midden, in millimeters
float laatsteLocatie = 0;

float LijnLocatie() {
  int aantal = 0;
  float totaal = 0;

  for (int sensorNr = 0; sensorNr < 6; sensorNr++) {
    if (AIStarter_SmartBotGetIRModuleValue(sensorNr)) {
      aantal++;
      totaal = totaal + sensorLocaties[sensorNr];
    }
  }

  float waarde = 0;
  if (aantal > 0)
    waarde = (totaal / aantal);

  //filter ruis
  laatsteLocatie = 0.9 * laatsteLocatie + 0.1 * waarde;
  return (laatsteLocatie);
}


//tel aantal IR leds dat iets detecteert
int aantalIRaan()
{
  int aantalAan = 0;
  for (int sensorNr = 0; sensorNr < 6; sensorNr++) {
    if (AIStarter_SmartBotGetIRModuleValue(sensorNr)) {
      aantalAan++;
    }
  }
  return (aantalAan);
}


// is de volg lijn nog zichtbaar en geldig?
bool isVolgLijn() {
  int aantalAan = aantalIRaan();
  // lijn dikte
  return (aantalAan >=1 && aantalAan <=2);
}


// staan we op de stoplijn?
bool isStopLijn() {
  int aantalAan = aantalIRaan();
  return (aantalAan >= 4 && aantalAan <= 6);
}


const int SNELHEID = 50;
const float LIJN_CORRECTIE_FACTOR = 1;
const int SONAR_VOOR_AFSTAND = 20;
const int WACHT_TIJD = 5;

void loop() {
  int snelheid = SNELHEID;


  /////////////////////////// SONAR
  // is er wat voor ons?
  float sonarVoor = AIStarter_SmartBotGetSonar(SONAR2);
  float sonarRemAfstand = 20;
  if (sonarVoor < SONAR_VOOR_AFSTAND) {
    // rem af
    snelheid = snelheid * (sonarVoor / SONAR_VOOR_AFSTAND);

    // 5cm, stop!
    if (sonarVoor < 5) {
      AIStarter_SmartBotSetLED(LED1, BLINK);
      AIStarter_SmartBotSetLED(LED2, BLINK);
      AIStarter_SmartBotSetMotor(MOTORL, 0);
      AIStarter_SmartBotSetMotor(MOTORR, 0);
      // wacht tot hij zeker weten weer vrij is
      int vrijTeller = 0;
      while (vrijTeller < 50) {
        if (AIStarter_SmartBotGetSonar(SONAR2) > 10)
          vrijTeller++;
        else
          vrijTeller = 0;
      }
    }
  }


  /////////////////////// VOLG LIJN
  if (isVolgLijn()) {
    float locatie = LijnLocatie();
    // Serial.println(locatie);

    // ledjes zijn richting aanwijzer
    AIStarter_SmartBotSetLED(LED1, OFF);
    AIStarter_SmartBotSetLED(LED2, OFF);
    if (locatie < -5)
      AIStarter_SmartBotSetLED(LED1, BLINK);
    if (locatie > 5)
      AIStarter_SmartBotSetLED(LED2, BLINK);

    // breng error terug naar -1 tm 1
    float error = 0 - (locatie / 30);

    if (error > 0) {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid * (1 - error));
      AIStarter_SmartBotSetMotor(MOTORR, snelheid);
    } else {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid);
      AIStarter_SmartBotSetMotor(MOTORR, snelheid * (1 + error));
    }

    //  AIStarter_SmartBotSetMotor(MOTORL, snelheid - snelheidsVerschil);
    //  AIStarter_SmartBotSetMotor(MOTORR, snelheid + snelheidsVerschil);

  } else {
    // stop
    AIStarter_SmartBotSetMotor(MOTORL, 0);
    AIStarter_SmartBotSetMotor(MOTORR, 0);
    AIStarter_SmartBotSetLED(LED1, ON);
    AIStarter_SmartBotSetLED(LED2, ON);
  }

  /////////////////////// STOP LIJN
  if (isStopLijn())
  {
    //als het goed is zijn we hierboven al gestopt

    //knipperlichten aan
    AIStarter_SmartBotSetLED(LED1, BLINK);
    AIStarter_SmartBotSetLED(LED2, BLINK);

    //piep
    digitalWrite(BEEP, HIGH);
    delay(100);
    digitalWrite(BEEP, LOW);

    //blinken lights
    for (int t = 0; t < WACHT_TIJD;t++)
    {
      AIStarter_ColorFlash();
      delay(1000);
    }

    //trap een stukje naar voren
    AIStarter_SmartBotSetMotor(MOTORL, SNELHEID);
    AIStarter_SmartBotSetMotor(MOTORR, SNELHEID);
    delay(250);
    AIStarter_SmartBotSetMotor(MOTORL, 0);
    AIStarter_SmartBotSetMotor(MOTORR, 0);
  }
}
