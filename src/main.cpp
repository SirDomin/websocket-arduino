
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <L298N.h>

WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
 

// Pin definition
const unsigned int IN1 = 5;
const unsigned int IN2 = 4;
const unsigned int ENABLE1 = 2;

const unsigned int IN3 = 12;
const unsigned int IN4 = 13;
const unsigned int ENABLE2 = 15;

L298N move_motor(ENABLE1, IN1, IN2);
L298N steering_motor(ENABLE2, IN3, IN4);
 
char ssid[] = "konp";  // use your own network ssid and password
char pass[] = "kxd4xvkn";
char json[] = "";

uint8_t direction = 2;
uint8_t steering_direction = 2;
int speed = 0;

int motor = 0;

DynamicJsonBuffer jsonBuffer;

void setEnginePower(int power) {
  move_motor.setSpeed(power);
}

void setEngineDirection(int engineDirection) {
}

void setDirection(int direction)
{

}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  if(type == WStype_TEXT)
  {
    String json = String((char *) &payload[0]);

    JsonObject& data = jsonBuffer.parseObject(json);

  // Test if parsing succeeds.
    if (!data.success()) {
      Serial.println("failed");
      return;
    } else {
      speed = data["speed"];
      move_motor.setSpeed(speed);

      direction = data["direction"];
      move_motor.run(direction);
      Serial.println(steering_motor.getSpeed());
      steering_direction = data["steering_direction"];
      steering_motor.run(steering_direction);
    }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();
 
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));  Serial.println(ssid);
  WiFi.begin(ssid,pass);
 
  // connection with timeout
  int count = 0; 
  while ( (WiFi.status() != WL_CONNECTED) && count < 17) 
  {
      Serial.print(".");  delay(500);  count++;
  }
 
  if (WiFi.status() != WL_CONNECTED)
  { 
     Serial.println("");  Serial.print("Failed to connect to ");  Serial.println(ssid);
     while(1);
  }
 
  Serial.println("");
  Serial.println(F("[CONNECTED]"));   Serial.print("[IP ");  Serial.print(WiFi.localIP()); 
  Serial.println("]");
 
  // start a server
  server.begin();
  Serial.println("Server started");


  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  steering_motor.setSpeed(800);
}
 


void loopEngines() 
{

}

void loop()
{
  loopEngines();
  webSocket.loop();

  WiFiClient client = server.available();     // Check if a client has connected
  if (!client)  {  return;  }

  client.flush(); 
}
