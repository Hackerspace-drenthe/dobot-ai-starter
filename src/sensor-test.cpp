#include "AIStarter.h";

void
setup()
{
  // direct starten
  promgramRun = true;

  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(36, INPUT);
  AIStarter_SmartBotInit();
  AIStarter_SmartBotSetLED(LED1, BLINK);
}

void
loop()
{
  char buffer[100];

  while (!AIStarter_ColorIsReady())
    delay(250);

  Serial.println();

  sprintf(buffer,
          "Kleuren     : L=(%3d,%3d,%3d)  R=(%3d,%3d,%3d) ",
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, RCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, GCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, BCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR2, RCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR2, GCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR2, BCOLOR));

  Serial.println(buffer);

  sprintf(buffer,
          "Lijn volger : %d%d%d%d%d",
          AIStarter_SmartBotGetIRModuleValue(IR1),
          AIStarter_SmartBotGetIRModuleValue(IR2),
          AIStarter_SmartBotGetIRModuleValue(IR3),
          AIStarter_SmartBotGetIRModuleValue(IR4),
          AIStarter_SmartBotGetIRModuleValue(IR5),
          AIStarter_SmartBotGetIRModuleValue(IR6));
  Serial.println(buffer);

  sprintf(buffer, "Licht sensor: %d", AIStarter_SmartBotGetLightAnalog());
  Serial.println(buffer);

  sprintf(buffer,
          "Sonar       : L=%d M=%d R=%d",
          (int)AIStarter_SmartBotGetSonar(SONAR1),
          (int)AIStarter_SmartBotGetSonar(SONAR2),
          (int)AIStarter_SmartBotGetSonar(SONAR3));
  Serial.println(buffer);

  sprintf(buffer, "Motor pos   : L=%d R=%d",
          (int)AIStarter_SmartBotGetMotorPose(MOTORL),
          (int)AIStarter_SmartBotGetMotorPose(MOTORR));
  Serial.println(buffer);

  sprintf(buffer, "Compas      : %d", (int)AIStarter_SmartBotGetCompass());
  Serial.println(buffer);

  AIStarter_ColorFlash();

}

