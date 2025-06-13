#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/ledc.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Motor control pins
#define AIN1 14
#define AIN2 27
#define BIN1 33
#define BIN2 25
#define STBY 26

// PWM configuration
#define PWM_FREQ     1000
#define PWM_RES      LEDC_TIMER_8_BIT
#define DUTY_FORWARD 200

void setup() {
  Serial.begin(115200);

  // Motor standby (enable)
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // Configure LEDC timer
  ledc_timer_config_t pwm_timer = {
    .speed_mode       = LEDC_HIGH_SPEED_MODE,
    .duty_resolution  = PWM_RES,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = PWM_FREQ,
    .clk_cfg          = LEDC_AUTO_CLK
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

  // OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("ESP32-WROOM Ready");
  display.display();

  delay(1000);
}

void loop() {
  // Forward
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Motors Forward");
  display.display();

  setMotor(true, true);
  delay(2000);

  // Backward
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Motors Reverse");
  display.display();

  setMotor(false, false);
  delay(2000);

  // Stop
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Motors Stopped");
  display.display();

  stopMotor();
  delay(2000);
}

void setMotor(bool aForward, bool bForward) {
  // Motor A
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, aForward ? DUTY_FORWARD : 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, aForward ? 0 : DUTY_FORWARD);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

  // Motor B
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, bForward ? DUTY_FORWARD : 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, bForward ? 0 : DUTY_FORWARD);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);
}

void stopMotor() {
  for (int ch = 0; ch < 4; ch++) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ch, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ch);
  }
}
