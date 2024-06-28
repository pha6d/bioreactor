#include "esp_camera.h"
#include <WiFi.h>
#include "config.h"
#include <WebServer.h>

// Configurations de la caméra
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

WebServer server(80);

const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP32-CAM Stream</title>
  </head>
  <body>
    <h1>ESP32-CAM Stream</h1>
    <img src="/stream" id="stream" />
  </body>
</html>
)rawliteral";

void startCameraServer();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Initialisation de la caméra
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialisation de la caméra
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Démarrer le serveur
  startCameraServer();
}

void loop() {
  server.handleClient();
}

void startCameraServer() {
  server.on("/", HTTP_GET, [](){
    server.send_P(200, "text/html", index_html);
  });

  server.on("/stream", HTTP_GET, [](){
    server.send_P(200, "text/html", R"rawliteral(
      <html>
        <body>
          <img src="" id="stream" />
          <script>
            document.getElementById('stream').src = `http://${window.location.hostname}:81/stream`;
          </script>
        </body>
      </html>
    )rawliteral");
  });

  server.begin();
}
