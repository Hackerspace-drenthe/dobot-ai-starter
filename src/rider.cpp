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

// const float P = 1;

void loop() {

  int snelheid = 25;

  if (isLijnOk()) {
    float locatie = LijnLocatie();
//    Serial.println(locatie);

    //maximum error is -30 en +30
    float error = 0 - locatie;


    // corrigeer door een motor af te remmen
    float snelheidsVerschil = error * 1;

    if (snelheidsVerschil>=0)
    {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid-snelheidsVerschil);
      AIStarter_SmartBotSetMotor(MOTORR, snelheid);
    } else {
      AIStarter_SmartBotSetMotor(MOTORL, snelheid);
      AIStarter_SmartBotSetMotor(MOTORR, snelheid + snelheidsVerschil);
    }


    //  AIStarter_SmartBotSetMotor(MOTORL, snelheid - snelheidsVerschil);
    //  AIStarter_SmartBotSetMotor(MOTORR, snelheid + snelheidsVerschil);

  }
  else
  {
    AIStarter_SmartBotSetMotor(MOTORL, 0);
    AIStarter_SmartBotSetMotor(MOTORR, 0);
  }


  // delay(100);
}
