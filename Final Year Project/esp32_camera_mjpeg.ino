/*

  This is a simple MJPEG streaming webserver implemented for AI-Thinker ESP32-CAM and
  ESP32-EYE modules.
  This is tested to work with VLC and Blynk video widget.

  Inspired by and based on this Instructable: $9 RTSP Video Streamer Using the ESP32-CAM Board
  (https://www.instructables.com/id/9-RTSP-Video-Streamer-Using-the-ESP32-CAM-Board/)

  Board: AI-Thinker ESP32-CAM

*/

#include "src/OV2640.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

/*
Next one is an include with wifi credentials.
This is what you need to do:

1. Create a file called "home_wifi_multi.h" in the same folder   OR   under a separate subfolder of the "libraries" folder of Arduino IDE. (You are creating a "fake" library really - I called it "MySettings"). 
2. Place the following text in the file:*/
#define SSID1 "Himanshu Singh"
#define PWD1 "himanshu"

//#include "home_wifi_multi.h"

OV2640 cam;

WebServer server(80);
//WiFiServer server(80);
const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

void handle_jpg_stream(void)
{
  char buf[32];
  int s;

  WiFiClient client = server.client();

  client.write(HEADER, hdrLen);
  client.write(BOUNDARY, bdrLen);

  while (true)
  {
    if (!client.connected()) break;
    cam.run();
    s = cam.getSize();
    client.write(CTNTTYPE, cntLen);
    sprintf( buf, "%d\r\n\r\n", s );
    client.write(buf, strlen(buf));
    client.write((char *)cam.getfb(), s);
    client.write(BOUNDARY, bdrLen);
  }
}

const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n";
const int jhdLen = strlen(JHEADER);

void handle_jpg(void)
{   
  WiFiClient client = server.client();

  cam.run();
  if (!client.connected()) return;

  client.write(JHEADER, jhdLen);
  client.write((char *)cam.getfb(), cam.getSize());

}

void led_on(void)
{
  digitalWrite(13,HIGH);
  server.send(200,"text/html","LED is turned on");
}

void led_off(void)
{
  digitalWrite(13,LOW);
  server.send(200,"text/html","LED is turned off");
}

void fan_on(void)
{
  digitalWrite(2,HIGH);
  server.send(200,"text/html","Fan is turned on");
}

void fan_off(void)
{
  digitalWrite(2,LOW);
  server.send(200,"text/html","Fan is turned off");
}
void bulb_on(void)
{
  digitalWrite(14,HIGH);
  server.send(200,"text/html","Bulb is turned on");
}

void bulb_off(void)
{
  digitalWrite(14,LOW);
  server.send(200,"text/html","Bulb is turned off");
}

void gate_open(void)
{
  digitalWrite(15,HIGH);
  server.send(200,"text/html","<div style=\"background-color:red;color:white;padding:2%;text-align:center;\"><h1>!! GATE OPENED !!</h1></div>");
}
void gate_close(void)
{
  digitalWrite(15,LOW);
  server.send(200,"text/html","<div style=\"background-color:green;color:white;padding:2%;text-align:center;\"><h1>!! GATE CLOSED !!</h1></div>");
}

void temp_status(void)
{
  int temp = digitalRead(12);
  if(temp == HIGH)
  {
    server.send(200,"text/html","<div style=\"background-color:red;color:white;padding:2%;text-align:center;\"><h1>!! Attention !!<br>Temprature is High. !</h1></div>"); 
  }
  else
  {
    server.send(200,"text/html","<div style=\"background-color:green;color:white;padding:2%;text-align:center;\"><h1>Relax !!<br>Temprature is Normal.</h1></div>");
  }
}

void gas_status(void)
{
  int gas = digitalRead(4);
  if(gas == HIGH)
  {
    server.send(200,"text/html","<div style=\"background-color:red;color:white;padding:2%;text-align:center;\"><h1>!! Attention !!<br>Gas Leakage is Detected !</h1></div>"); 
  }
  else
  {
    server.send(200,"text/html","<div style=\"background-color:green;color:white;padding:2%;text-align:center;\"><h1>Relax !!<br>Gas Status is Normal.</h1></div>");
  }
}

void ultrasonic(void)
{
  int ultra = digitalRead(1);
  if(ultra == HIGH)
  {
    server.send(200,"text/html","<div style=\"background-color:red;color:white;padding:2%;text-align:center;\"><h1>!! Attention !!<br>Someone is on the Door !</h1></div>"); 
  }
  else
  {
    server.send(200,"text/html","<div style=\"background-color:green;color:white;padding:2%;text-align:center;\"><h1>Relax !!<br>No one is on the Door.</h1></div>");
  }
}

void handleNotFound()
{
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text / plain", message);
}

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);

  pinMode(12, INPUT);
  pinMode(4, INPUT);
  pinMode(1, INPUT);
  
  Serial.begin(115200);
  //while (!Serial);            //wait for serial connection.

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

  // Frame parameters
  //  config.frame_size = FRAMESIZE_UXGA;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  cam.init(config);

  IPAddress ip;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID1, PWD1);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  ip = WiFi.localIP();
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(ip);
  Serial.print("Stream Link: http://");
  Serial.print(ip);
  Serial.println("/mjpeg/1");
  server.on("/mjpeg/1", HTTP_GET, handle_jpg_stream);
  server.on("/jpg", HTTP_GET, handle_jpg);
  server.on("/led_on", HTTP_GET, led_on);
  server.on("/led_off", HTTP_GET, led_off);
  server.on("/fan_on", HTTP_GET, fan_on);
  server.on("/fan_off", HTTP_GET, fan_off);
  server.on("/bulb_on", HTTP_GET, bulb_on);
  server.on("/bulb_off", HTTP_GET, bulb_off);
  server.on("/gate_open", HTTP_GET, gate_open);
  server.on("/gate_close", HTTP_GET, gate_close);
  
  server.on("/temp_status", HTTP_GET, temp_status);
  server.on("/gas_status", HTTP_GET, gas_status);
  server.on("/ultrasonic", HTTP_GET, ultrasonic);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop()
{
  server.handleClient();
  //delay(2);
}
