#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Wifi name
#define WLAN_SSID "ssid"
//Wifi password
#define WLAN_PASS "password"

//setup Adafruit
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
//fill your username
#define AIO_USERNAME "username"
//fill your key
#define AIO_KEY "your_key"

//setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//set publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led");
Adafruit_MQTT_Publish temp_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

//setup subcribe
Adafruit_MQTT_Subscribe light_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led", MQTT_QOS_1);
Adafruit_MQTT_Subscribe temp_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temp", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;

void lightcallback(char* value, uint16_t len){
  if(value[0] == '0') Serial.print('a');
  if(value[0] == '1') Serial.print('A');
}

void setup() {
  // put your setup code here, to run once:
  //set pin 2,5 as OUTPUT
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  //set busy pin HIGH
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  //connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //subscribe light feed
  light_sub.setCallback(lightcallback);
  mqtt.subscribe(&light_sub);

  //connect MQTT
  while (mqtt.connect() != 0) {
    mqtt.disconnect();
    delay(500);
  }

  //finish setup, set busy pin LOW
  digitalWrite(5, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  //receive packet
  mqtt.processPackets(1);

  if(Serial.available()){
    char msg = Serial.read();
    if(msg == 'o') Serial.print('O');
    else if(msg == 'a') light_pub.publish(0);
    else if(msg == 'A') light_pub.publish(1);
    else if (msg == '!') {
      String packet = Serial.readStringUntil('#');
      if (packet.startsWith("TEMP:")) {
        String str_val = packet.substring(5);
        float temp = str_val.toFloat();
        temp_pub.publish(temp);
      }
    }
  }
  led_counter++;
  if(led_counter == 100){
    // every 1s
    led_counter = 0;
    //toggle LED
    if(led_status == HIGH) led_status = LOW;
    else led_status = HIGH;

    digitalWrite(2, led_status);
  }
  delay(10);
}
