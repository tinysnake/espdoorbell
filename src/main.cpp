#include "Arduino.h"
#include "consts.h"
#include "ESPHelper.h"
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>

#define NAME "doorbell_plus"
#define WILL_INIT "online"
#define WILL_MSG "offline"

int signalState = HIGH;
unsigned long signalTime;
char ringTopic[32];
char willTopic[32];

ESPHelper esp;
SoftwareSerial softSerial(SOFT_SERIAL_RX_PIN, SOFT_SERIAL_TX_PIN); // RX, TX
DFRobotDFPlayerMini dfPlayer;

void mqttCallback(char *topic, uint8_t *payload, unsigned int length);
void printDFDetail(uint8_t type, int value);
void trigger();
void ring();

void setup()
{
  Serial.begin(115200);
  softSerial.begin(9600);

  pinMode(TRIGGER_PIN, INPUT);

  Serial.println("Initializing DFPlayer ...");

  if (!dfPlayer.begin(softSerial))
  {
    Serial.println("Unable to init DFPlayer:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while (true)
      ;
  }
  Serial.println("DFPlayer Mini online.");

  dfPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  dfPlayer.volume(30);      //Set volume value (0~30).
  dfPlayer.play(1);

  Serial.println("Setting Up ESP network.");

  strcpy(willTopic, NAME);
  strcat(willTopic, "/lwt");

  if (const_mqtt_broker[0] != '\0')
  {
    strcpy(ringTopic, NAME);
    strcat(ringTopic, "/ringOk");
    Serial.println("setting up mqtt");
    Serial.println(ringTopic);
    esp.addSubscription(ringTopic);
    esp.setMQTTCallback(mqttCallback);
  }

  esp.OTA_enable();
  esp.OTA_setPassword(const_ota_pass);
  esp.OTA_setHostnameWithVersion(NAME);

  esp.begin(const_ssid, const_ssid_pass, const_mqtt_broker, const_mqtt_user, const_mqtt_user_pass, 1883, willTopic, WILL_MSG, 0, 1);

  esp.publish(willTopic, WILL_INIT);

  esp.listSubscriptions();
}

void loop()
{
  esp.loop();

  if (dfPlayer.available())
  {
    printDFDetail(dfPlayer.readType(), dfPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }

  int pinVal = digitalRead(TRIGGER_PIN);
  if (pinVal == HIGH && signalState == LOW)
  {
    signalState = pinVal;
    signalTime = millis();
    trigger();
  }
  else if (signalState == HIGH && millis() - signalTime > 500)
  {
    signalState = LOW;
    Serial.println("next ring available");
  }

  yield();
}

void trigger()
{
  Serial.println("trigger!");
  if (const_mqtt_broker[0] == '\0')
  {
    ring();
  }
  else
  {
    String topic = String(NAME);
    topic += "/ring";
    Serial.print("sending mqtt:");
    Serial.println(topic);
    esp.publish(topic.c_str(), "");
  }
  //ring();
}

void ring()
{
  dfPlayer.play(2);
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  Serial.print("mqtt callback: ");
  Serial.println(topic);
  String tStr = String(topic);
  if (tStr.endsWith("ringOk"))
  {
    ring();
  }
}

void printDFDetail(uint8_t type, int value)
{

  String msg;
  if (type == DFPlayerError)
    msg = "DFPlayerError: ";
  switch (type)
  {
  case TimeOut:
    Serial.println("Time Out!");
    break;
  case WrongStack:
    Serial.println("Stack Wrong!");
    break;
  case DFPlayerCardInserted:
    Serial.println("Card Inserted!");
    break;
  case DFPlayerCardRemoved:
    Serial.println("Card Removed!");
    break;
  case DFPlayerCardOnline:
    Serial.println("Card Online!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(String("Number: ") + value + String(" Play Finished!"));
    break;
  case DFPlayerError:
    switch (value)
    {
    case Busy:
      msg += "Card not found";
      break;
    case Sleeping:
      msg += "Sleeping";
      break;
    case SerialWrongStack:
      msg += "Get Wrong Stack";
      break;
    case CheckSumNotMatch:
      msg += "Check Sum Not Match";
      break;
    case FileIndexOut:
      msg += "File Index Out of Bound";
      break;
    case FileMismatch:
      msg += "Cannot Find File";
      break;
    case Advertise:
      msg += "In Advertise";
      break;
    default:
      break;
    }
    Serial.println(msg);
    break;
  default:
    break;
  }
}