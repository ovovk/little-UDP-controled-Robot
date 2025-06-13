// - Wi-Fi AP mode (hotspot)
// - UDP handling
// - OLED display
// - PWM motor control using ESP32 Arduino Core 3.x API
// - Multitasking with FreeRTOS

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "roboteyes.h"
#include "AsyncUDP.h"
#include "seperatestring.h"
#include "driver/ledc.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Motor pins
#define AIN1 14
#define AIN2 27
#define BIN1 33
#define BIN2 25
#define STBY 26
#define MOTOR_B_BOOST 1.1    // or use 1.2, depending on how much extra power you need

#define CommandArray_SIZE 20
char *CommandArray[CommandArray_SIZE];
char *strData = NULL;

const char *AP_SSID = "LittleRobot";
const char *AP_PASS = "12345678";  // Must be at least 8 characters
AsyncUDP udp;
String PhrasedData = "";
int Speed = 255;
int aninum = 0;

TaskHandle_t xHandle = NULL;

// FreeRTOS eye animation
void dispTask(void *pvParameters) {
  while (1) {
    if (aninum == 1) look_left(500);
    else if (aninum == 2) look_right(500);
    else sad_blink(400, 1);
    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // Setup PWM timer
  ledc_timer_config_t pwm_timer = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&pwm_timer);
// AIN1
ledc_channel_config_t pwm_AIN1 = {
  .gpio_num   = AIN1,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .channel    = LEDC_CHANNEL_0,
  .intr_type  = LEDC_INTR_DISABLE,
  .timer_sel  = LEDC_TIMER_0,
  .duty       = 0,
  .hpoint     = 0
};
ledc_channel_config(&pwm_AIN1);

// AIN2
ledc_channel_config_t pwm_AIN2 = {
  .gpio_num   = AIN2,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .channel    = LEDC_CHANNEL_1,
  .intr_type  = LEDC_INTR_DISABLE,
  .timer_sel  = LEDC_TIMER_0,
  .duty       = 0,
  .hpoint     = 0
};
ledc_channel_config(&pwm_AIN2);

// BIN1
ledc_channel_config_t pwm_BIN1 = {
  .gpio_num   = BIN1,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .channel    = LEDC_CHANNEL_2,
  .intr_type  = LEDC_INTR_DISABLE,
  .timer_sel  = LEDC_TIMER_0,
  .duty       = 0,
  .hpoint     = 0
};
ledc_channel_config(&pwm_BIN1);

// BIN2
ledc_channel_config_t pwm_BIN2 = {
  .gpio_num   = BIN2,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .channel    = LEDC_CHANNEL_3,
  .intr_type  = LEDC_INTR_DISABLE,
  .timer_sel  = LEDC_TIMER_0,
  .duty       = 0,
  .hpoint     = 0
};
ledc_channel_config(&pwm_BIN2);


  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ESP32-WROOM Ready");

  // Wi-Fi Access Point setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Hotspot created. IP address: ");
  Serial.println(IP);

  // Show hotspot info on OLED
  display.setCursor(0, 10);
  display.println("SSID: LittleRobot");
  display.print("IP: ");
  display.println(IP);
  display.display();

  if (udp.listen(1234)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(IP);
    udp.onPacket([](AsyncUDPPacket packet) {
      char* tmpStr = (char*) malloc(packet.length() + 1);
      memcpy(tmpStr, packet.data(), packet.length());
      tmpStr[packet.length()] = '\0';
      PhrasedData = String(tmpStr);
      free(tmpStr);
      Serial.println("Received: " + PhrasedData);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      //display.println("Got:");
      //display.println(PhrasedData);
      display.display();
    });
  }

  // Start display task
  xTaskCreatePinnedToCore(dispTask, "dispTask", 4096, NULL, 0, &xHandle, 0);

// TEMPORARY: motor test after boot

 Serial.println("Motor test...");
 forward(200);
 delay(1000);
 Stop();


}

void loop() {

  Plzgetstring(PhrasedData);
  String cmd = CommandArray[0];
  Serial.print("CMD: "); Serial.println(cmd);
  Serial.print("Speed: "); Serial.println(Speed);

  if (cmd == "f" || cmd == "F") forward(Speed);
  else if (cmd == "b" || cmd == "B") backward(Speed);
  else if (cmd == "l" || cmd == "L") { left(Speed); aninum = 1; }
  else if (cmd == "r" || cmd == "R") { right(Speed); aninum = 2; }
  else if (cmd == "s" || cmd == "S") { Stop(); aninum = 0; }
}

void Plzgetstring(String phraseData) {
  int N = separate(phraseData, CommandArray, CommandArray_SIZE, &strData);
  if (N >= 2) {
    Speed = String(CommandArray[1]).toInt();
    if (Speed < 10 || Speed > 255) Speed = 200;
  } else {
    Speed = 200;
  }
  freeData(&strData);
}



void forward(int speed) {
  
  digitalWrite(STBY, HIGH);

  int bSpeed = speed * MOTOR_B_BOOST;
  if (bSpeed > 255) bSpeed = 255;  // Clamp max

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void backward(int speed) {


  digitalWrite(STBY, HIGH);

    int bSpeed = speed * MOTOR_B_BOOST;
  if (bSpeed > 255) bSpeed = 255;  // Clamp max
  
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void left(int speed) {

  digitalWrite(STBY, HIGH);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void right(int speed) {
 
  digitalWrite(STBY, HIGH);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, speed);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void Stop() {
  for (int ch = 0; ch < 4; ch++) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ch, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ch);
  }
  digitalWrite(STBY, LOW); // optional to save power
}
