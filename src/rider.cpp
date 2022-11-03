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
float LijnLocatie() {
  int aantal = 0;
  float totaal = 0;

  for (int sensorNr = 0; sensorNr < 6; sensorNr++) {
    if (AIStarter_SmartBotGetIRModuleValue(sensorNr)) {
      aantal++;
      totaal = totaal + sensorLocaties[sensorNr];
    }
  }

  if (aantal == 0)
    return (0);

  // gemiddelde waarde
  return (totaal / aantal);
}

// is de lijn nog zichtbaar en geldig?
bool isLijnOk() {
  int aantalAan = 0;
  for (int sensorNr = 0; sensorNr < 6; sensorNr++) {
    if (AIStarter_SmartBotGetIRModuleValue(sensorNr)) {
      aantalAan++;
    }
  }

  // als alle sensors aan of uit zijn dan is er wat mis
  return (aantalAan != 0 && aantalAan != 6);
}

const int SNELHEID = 50;
const float LIJN_CORRECTIE_FACTOR = 1;
const int SONAR_VOOR_AFSTAND = 20;

void loop() {

  int snelheid = SNELHEID;

  // is er wat voor ons?
  // float sonarVoor = AIStarter_SmartBotGetSonar(SONAR2);
  // float sonarRemAfstand = 20;
  // if (sonarVoor < SONAR_VOOR_AFSTAND) {
  //   //rem af
  //   snelheid = snelheid * (sonarVoor / SONAR_VOOR_AFSTAND);
  // }

  if (isLijnOk()) {
    float locatie = LijnLocatie();
//    Serial.println(locatie);

    //breng error terug naar -1 tm 1
    float error = 0 - (locatie/30);


    if (error>0)
    {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid*(1-error));
      AIStarter_SmartBotSetMotor(MOTORR, snelheid);
    } else {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid);
      AIStarter_SmartBotSetMotor(MOTORR, snelheid*(1+error));
    }


    //  AIStarter_SmartBotSetMotor(MOTORL, snelheid - snelheidsVerschil);
    //  AIStarter_SmartBotSetMotor(MOTORR, snelheid + snelheidsVerschil);

  }
  else
  {
    //stop
    AIStarter_SmartBotSetMotor(MOTORL, 0);
    AIStarter_SmartBotSetMotor(MOTORR, 0);
  }



  // delay(100);
}
