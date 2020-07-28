#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include "esp.h"

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

String server = "onem2m.iiit.ac.in:443";
const char* ssid = "esw-m19@iiith";
const char* password = "e5W-eMai@3!20hOct";
int is_change_detected = 0;
void startCameraServer();
String get_data();
String curr_projector_status = "Projector OFF";
void connect_to_wifi();
String projector_status = "Projector OFF";
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  //  WiFi.begin(ssid, password);
  //
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //
  //  startCameraServer();
  //
  //  Serial.print("Camera Ready! Use 'http://");
  //  Serial.print(WiFi.localIP());
  //  Serial.println("' to connect");
}
void loop() {
  // put your main code here, to run repeatedly:
  //  if(projector_status == "Projector OFF"){
  //    projector_status = "Projector ON";
  //    }
  //  else{
  //    projector_status = "Projector OFF";
  //    }
  if (WiFi.status() !=  WL_CONNECTED) {
    WiFi.begin(ssid, password);
    connect_to_wifi();
  }
  String projector_status = get_data();
  Serial.println(projector_status);
  if (curr_projector_status == projector_status) {
    is_change_detected = 0;
  }
  else {
    is_change_detected = 1;
  }
  if (is_change_detected == 1) {
    if (projector_status == "Projector ON") {
      if (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        connect_to_wifi();
      }
      Serial.println("");
      Serial.println("WiFi connected");
      start_server();
    }
    else if (projector_status == "Projector OFF") {
      //        if (WiFi.status() == WL_CONNECTED) {
      //          WiFi.disconnect();
      //        }
      ESP.restart();
    }
  }
  else {
    curr_projector_status = projector_status;
  }
  delay(5000);
}
void start_server() {
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}
String get_data() {

  String get_string = "http://" + server + "/~/in-cse/in-name/" + "Team25_Lighting_dim_control_based_on_speaker_presentation" + "/" + "node_1" + "/" + "la";
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;
    Serial.println(get_string);

    http.begin(get_string); //Specify the URL
    http.addHeader("X-M2M-Origin", "admin:admin");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "close");
    int httpCode = http.GET();                                        //Make the request
    if (httpCode > 0) { //Check for the returning code

      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      Serial.println(payload);
      String b = doc["m2m:cin"]["con"];
      http.end(); //Free the resources
      return b;
    }
  }
  return "Not connected to Wifi";
}
void connect_to_wifi() {
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}
