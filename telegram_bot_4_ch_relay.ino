/*******************************************************************
    Control an LED strip using Inline Keyboard on Telegram

   Code By Brian Lough
 **********************/
#include <ESP8266WiFi.h> // for ESP8266 Board
//#include <ESP8266WiFi.h>//Use for eso32 Board
#include <WiFiClientSecure.h>

#include <UniversalTelegramBot.h>
// Library for interacting with the Telegram API
// Search for "Telegegram" in the Library manager and install UniversalTelegramBot by Brian Lough

// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot


#include <ArduinoJson.h>
// Library used for parsing Json from the API responses
// NOTE: There is a breaking change in the 6.x.x version,
// install the 5.x.x version instead
// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

char ssid[] = " your network SSID (name)";         // your network SSID (name)
char password[] = "your network password"; // your network password

#define TELEGRAM_BOT_TOKEN "Your Telegram Bot Token "  // Your Telegram Bot Token 


//------- ---------------------- ------


// This is the Wifi client that supports HTTPS
WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);
// Use 12, 12, 14, 27 instead of D0, D1, D2, D3 resp. for esp32
#define LED_PIN1 D0 // Digital pins of Esp8266
#define LED_PIN2 D1
#define LED_PIN3 D2
#define LED_PIN4 D3

int delayBetweenChecks = 1000;
unsigned long lastTimeChecked;   //last time messages' scan has been done

unsigned long lightTimerExpires;
boolean lightTimerActive = false;

void setup() 
{
  Serial.begin(115200);
 // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);
  pinMode(D3, OUTPUT);
  digitalWrite(D3, HIGH);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Only required on 2.5 Beta
  client.setInsecure();

  
  // longPoll keeps the request to Telegram open for the given amount of seconds if there are no messages
  // This hugely improves response time of the bot, but is only really suitable for projects
  // where the the initial interaction comes from Telegram as the requests will block the loop for
  // the length of the long poll
  bot.longPoll = 60;
}

void handleNewMessages(int numNewMessages)
{for (int i = 0; i < numNewMessages; i++) 
  {// If the type is a "callback_query", a inline keyboard button was pressed
    if (bot.messages[i].type ==  F("callback_query")) 
   {String text = bot.messages[i].text;
    Serial.print("Call back button pressed with text: ");
    Serial.println(text);

      if (text == F("ON1")) {digitalWrite(LED_PIN1, LOW);}
      else if (text == F("OFF1")) { digitalWrite(LED_PIN1, HIGH);}
      if (text == F("ON2")) { digitalWrite(LED_PIN2, LOW);}
      else if (text == F("OFF2")) { digitalWrite(LED_PIN2, HIGH);} 
      if (text == F("ON3")) {digitalWrite(LED_PIN3, LOW);}
      else if (text == F("OFF3")) { digitalWrite(LED_PIN3, HIGH);} 
      if (text == F("ON4")) {digitalWrite(LED_PIN4, LOW);}
      else if (text == F("OFF4")) {digitalWrite(LED_PIN4, HIGH);} 
  
    } else {
      String chat_id = String(bot.messages[i].chat_id);
      String text = bot.messages[i].text;
  
      

      if (text == F("/options")) 

        // Keyboard Json is an array of arrays.
        // The size of the main array is how many row options the keyboard has
        // The size of the sub arrays is how many coloums that row has

        // "The Text" property is what shows up in the keyboard
        // The "callback_data" property is the text that gets sent when pressed  
        
       { String keyboardJson = F("[[{ \"text\" : \"L1-ON\", \"callback_data\" : \"ON1\" },{ \"text\" : \"L1-OFF\", \"callback_data\" : \"OFF1\" },{ \"text\" : \"L2-ON\", \"callback_data\" : \"ON2\" },{\"text\" : \"L2-OFF\", \"callback_data\" : \"OFF2\"}],");
       keyboardJson += F("[{ \"text\" : \"L3-ON\", \"callback_data\" : \"ON3\" }, { \"text\" : \"L3-OFF\", \"callback_data\" : \"OFF3\" },{ \"text\" : \"L4-ON\", \"callback_data\" : \"ON4\" },{ \"text\" : \"L4-OFF\", \"callback_data\" : \"OFF4\" }]]");
        bot.sendMessageWithInlineKeyboard(chat_id, "Press The Buttons", "", keyboardJson);
      }

      // When a user first uses a bot they will send a "/start" command
      // So this is a good place to let the users know what commands are available
      if (text == F("/start")) 
      

       { bot.sendMessage(chat_id, "/options : returns the inline keyboard\n", "Markdown");}
      
    }
   }
  }


void loop() 

{ if (millis() > lastTimeChecked + delayBetweenChecks)  
  { // getUpdates returns 1 if there is a new message from Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages) {
                         Serial.println("got response");
                         handleNewMessages(numNewMessages);
                         }

    lastTimeChecked = millis();

    if (lightTimerActive && millis() > lightTimerExpires) {
                                                           lightTimerActive = false;
                                                           digitalWrite(LED_PIN1, LOW);
                                                           digitalWrite(LED_PIN2, LOW);
                                                           digitalWrite(LED_PIN3, LOW);
                                                           digitalWrite(LED_PIN4, LOW);
      
                                                           }
  }
}
