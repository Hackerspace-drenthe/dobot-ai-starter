#include "AIStarter.h";

const int SNELHEID = 50;
const float LIJN_CORRECTIE_FACTOR = 1;
const int SONAR_VOOR_AFSTAND = 20;
const int WACHT_TIJD = 5;

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
  if (aantal > 0) {
    waarde = (totaal / aantal);

    // filter ruis
    laatsteLocatie = 0.9 * laatsteLocatie + 0.1 * waarde;
  }
  return (laatsteLocatie);
}

// tel aantal IR leds dat iets detecteert
int aantalIRaan() {
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
  return (aantalAan >= 1 && aantalAan <= 2);
}

// staan we op de stoplijn?
bool isStopLijn() {
  int aantalAan = aantalIRaan();
  return (aantalAan >= 4 && aantalAan <= 6);
}

//geef waarde van dichtsbijzijnde robot
float sonarMinimum()
{
  float l = AIStarter_SmartBotGetSonar(SONAR1);
  float r = AIStarter_SmartBotGetSonar(SONAR2);
  float m = AIStarter_SmartBotGetSonar(SONAR3);

  float ret = min(l, r);
  ret = min(ret, m);
  return (ret);
}

long int laatstGezien = 0;

void loop() {
  int snelheid = SNELHEID;

  /////////////////////////// SONAR
  // is er wat voor ons?
  float sonarVoor = sonarMinimum();
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
        if (sonarMinimum() > 10)
          vrijTeller++;
        else
          vrijTeller = 0;
      }
    }
  }

  /////////////////////// VOLG LIJN
  if (isVolgLijn()) {
    laatstGezien = millis();

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
    // lijn tijdje kwijt, probeer te zoeken
    if (millis() - laatstGezien > 500) {
      // const int draaisnelheid = 100;
      const int draaisnelheid = 15;

      // 1 sec links
      // AIStarter_SmartBotSetMovment(LEFT, draaisnelheid);
      AIStarter_SmartBotSetMotor(MOTORL, -draaisnelheid, true);
      AIStarter_SmartBotSetMotor(MOTORR, 0, true);
      while ((millis() - laatstGezien < 2000)) {
        if (isVolgLijn()) {
          AIStarter_SmartBotSetMovment(FRONT, 0);
          return;
        }
      }

      // 2 sec rechts
      // AIStarter_SmartBotSetMovment(RIGHT, draaisnelheid);
      AIStarter_SmartBotSetMotor(MOTORL, 0, true);
      AIStarter_SmartBotSetMotor(MOTORR, -draaisnelheid, true);
      while ((millis() - laatstGezien < 4000)) {
        if (isVolgLijn()) {
          AIStarter_SmartBotSetMovment(FRONT, 0);
          return;
        }
      };

      // 1 sec links (weer in t midden)
      // AIStarter_SmartBotSetMovment(LEFT, draaisnelheid);
      AIStarter_SmartBotSetMotor(MOTORL, -draaisnelheid, true);
      AIStarter_SmartBotSetMotor(MOTORR, 0, true);
      while ((millis() - laatstGezien < 5000)) {
        if (isVolgLijn()) {
          AIStarter_SmartBotSetMovment(FRONT, 0);
          return;
        }
      };

      // wacht
      AIStarter_SmartBotSetMotor(MOTORL, 0);
      AIStarter_SmartBotSetMotor(MOTORR, 0);
      while (!isVolgLijn())
        ;
    }

    // stop pas als hij de lijn een tijdje niet ziet
    //  if (millis()-laatstGezien>1000)
    //  {
    //    // stop
    //    AIStarter_SmartBotSetLED(LED1, ON);
    //    AIStarter_SmartBotSetLED(LED2, ON);
    //  }
  }

  /////////////////////// STOP LIJN
  if (isStopLijn())
  // if (false)
  {
    AIStarter_SmartBotSetMotor(MOTORL, 0);
    AIStarter_SmartBotSetMotor(MOTORR, 0);
    AIStarter_SmartBotSetLED(LED1, BLINK);
    AIStarter_SmartBotSetLED(LED2, BLINK);

    // piep
    digitalWrite(BEEP, HIGH);
    delay(100);
    digitalWrite(BEEP, LOW);

    // blinken lights
    //  for (int t = 0; t < WACHT_TIJD;t++)
    AIStarter_ColorFlash();
    while (isStopLijn()) {
      // delay(1000);
      if (millis() - laatstGezien > 30000) {
        // trap een stukje naar voren
        AIStarter_SmartBotSetMotor(MOTORL, SNELHEID);
        AIStarter_SmartBotSetMotor(MOTORR, SNELHEID);
        delay(250);
        AIStarter_SmartBotSetMotor(MOTORL, 0);
        AIStarter_SmartBotSetMotor(MOTORR, 0);
        break;
      }
    }

    // while (isStopLijn())
    //   delay(1);
  }
}
