#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "amigo_config.h"

char ssid[] = "alix.cool";
char pass[] = "nr8wjs01";
const char mqtt_server[] = "broker.hivemq.com"; 
char publishUrlChar[1000];

const int buttonPin = 2;
const int vibratorPin = 3;
int buttonState = 0;  

int status = WL_IDLE_STATUS;

String postData;
String postVariable = "temp=";
boolean highSent=false;
boolean lowSent=false;


WiFiClient wifiClient;
PubSubClient client(wifiClient);

int otherAmigoId() {
  if(amigoId == 1) {
    return 2;
  }
  else {
    return 1;
  }
  
}

void reconnect(){

  while(!client.connected()){
    
    Serial.print("Attempting MQTT connection ....");
    String amigoClientId = "amigo" + String(amigoId);
    char amigoClientIdChar[1000];
    amigoClientId.toCharArray(amigoClientIdChar, 1000);
    String subscribeUrl = "bracelet_amigo/" + String(amigoId);
    char subscribeUrlChar[1000];
    subscribeUrl.toCharArray(subscribeUrlChar, 1000);
    if (client.connect(amigoClientIdChar, "", NULL)) { 
      client.subscribe(subscribeUrlChar);
      client.setCallback(subscribeReceive);
      Serial.println("Connected to MQTT Broker");
    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }
    
    
  }
  
}

void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
 
  // Print the message
  Serial.print("received: ");
  for(int i = 0; i < length; i ++)
  {
    Serial.print(char(payload[i]));
  }
  // Print a newline
  Serial.println("");
  if(char(payload[0]) == 'h') {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(vibratorPin, HIGH);
  }
  else if(char(payload[0]) == 'l') {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(vibratorPin, LOW);
  }
  
}



void setup() {

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(vibratorPin, OUTPUT);

  String publishUrl = "bracelet_amigo/" + String(otherAmigoId());
  publishUrl.toCharArray(publishUrlChar, 1000);
  
  delay(3000);
  Serial.print("Amigo: ");
  Serial.println(amigoId);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  client.setServer(mqtt_server, 1883);
  reconnect();

 
}

void loop() {
  client.loop();
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    if(highSent == false) {
      postData = "h";
      if (!client.connected()) {
        reconnect();
      }
      char attributes[1000];
      postData.toCharArray(attributes, 1000);
      client.publish(publishUrlChar, attributes);
      Serial.print("Sent:");
      Serial.println(attributes);
      highSent = true;
      lowSent = false;
    }
  }
  else {
    if(lowSent == false) {
      postData = "l";
      if (!client.connected()) {
        reconnect();
      }
      char attributes[1000];
      postData.toCharArray(attributes, 1000);
      client.publish(publishUrlChar, attributes);
      Serial.print("Sent:");
      Serial.println(attributes);
      lowSent = true;
      highSent = false;
    }
  }
}
