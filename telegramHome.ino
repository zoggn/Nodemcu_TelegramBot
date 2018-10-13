#include "ESP8266WiFi.h"
#include  <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

WiFiClientSecure client; //For ESP8266 boards

/* Create bot object */ 
UniversalTelegramBot bot(BOTtoken, client);

/* Misc settings */
long timing = 0;
#define led_r  14
#define led_g  12
#define led_b  13
int ledVal[3];


unsigned long M = 1000000;
unsigned int k = 1000;

/* Temp sensor */
OneWire ds(2);
DallasTemperature sensors(&ds);
/* End */

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting..");

  }

  Serial.println("Connected to WiFi Network");
  sensors.begin();

  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
}

void loop() {  
  //Telegram logic
  String text = "";
  String id = "";


  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
    
  }
}


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text.equals("/help")) {
      String helpPg = "/temp -> Show temperature at zoggn's house";
      bot.sendMessage(chat_id, helpPg);
    }

    if (text.equals("/temp")) {
      sensors.requestTemperatures();
      float temp = sensors.getTempCByIndex(0);
      String tempStr = String(temp);
      bot.sendMessage(chat_id, "Temperature at zoggn's home is near = " + tempStr);
    }

    if (text.compareTo("/led_on")) {
      //text.replace("/led_on"," ");
      char tmp[20];
      text.toCharArray(tmp, 20);
      sscanf(tmp,"/led_on %d %d %d",&ledVal[0], &ledVal[1], &ledVal[2]);
      setLed(ledVal[0],ledVal[1],ledVal[2]);
      bot.sendMessage(chat_id, "Enabled");
    }

    if (text.equals("/start")) {
      String startPg = "Hello, I'm bot which can show some information about home. Now i only can show temperature in home, just type /temp. Or use /help if you need more information(haha, i have only one command(for now)).";
      bot.sendMessage(chat_id, startPg);
    }
  }
}

void setLed(int valR, int valG, int valB){
  analogWrite(led_r, valR);
  analogWrite(led_g, valG);
  analogWrite(led_b, valB);
}
