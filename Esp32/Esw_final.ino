#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include "ArduinoJson.h"
int sensorValue;
int light_status;
int curr_light_status;
int change_detected;
const char* ssid = "*********"; // Enter wifi ssid here
const char* password = "*********"; // Enter corresponding wifi password here
void connect_to_wifi();
String cse_ip = "onem2m.iiit.ac.in";
String cse_port = "443";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String createCI(String server, String ae, String cnt, String val)
{
  HTTPClient http;
  Serial.println("Before begin");
  http.begin(server + ae + "/" + cnt + "/");
  Serial.println("After Begin");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "100");
  http.addHeader("Connection", "close");
  Serial.println("Before post");
  int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": " + String(val) + "}}");
  Serial.println("After post");
  http.end();
  Serial.print(code);
  delay(300);
}

void setup()
{
  pinMode(5, OUTPUT);
  curr_light_status = 1;
  Serial.begin(115200); // starts the serial port at 9600
  WiFi.begin(ssid, password);
}
int get_set_projector_status() {
  if (sensorValue > 30) {
    digitalWrite(5, LOW);
    light_status = 0;
  }
  else {
    digitalWrite(5, HIGH);
    light_status = 1;
  }
  return light_status;
}
void loop()
{
  sensorValue = analogRead(A0); // read analog input pin 0
  Serial.print(sensorValue, DEC); // prints the value read
  Serial.print(" \n"); // prints a space between the numbers
  int val = get_set_projector_status(); // turn light off
  //  Serial.println(val);
  if (curr_light_status != val) {
    String str = "Projector";
    if (val == 0) {
      str = str + " ON";
    }
    else {
      str = str + " OFF";
    }
    String sensor_string = str;
    sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE
    Serial.println(sensor_string);
    if (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, password);
      connect_to_wifi();
      //            delay(1000);
    }
    if (WiFi.status() == WL_CONNECTED) {
      createCI(server, "Team25_Lighting_dim_control_based_on_speaker_presentation", "node_1", sensor_string);
    }
    delay(500);
  }
  curr_light_status = val;
  delay(3000);
}

void connect_to_wifi() {//TIMER_1=0;
  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    if (millis() - startTime >= 5000) {
      break;
    }
 //   Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}
