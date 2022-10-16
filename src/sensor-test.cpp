
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
    delay(0);

  sprintf(buffer,
          "%3d, %3d, %3d",
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, RCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, GCOLOR),
          AIStarter_SmartBotGetColorSenor(COLORSENOR1, BCOLOR));

  Serial.println(buffer);
  AIStarter_ColorFlash();
}