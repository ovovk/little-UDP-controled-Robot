#include "driver/ledc.h"

#define BIN1 33
#define BIN2 25
#define STBY 26

void setup() {
  Serial.begin(115200);
  Serial.println("Testing motor B driver output...");

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // Enable motor driver

  // Setup PWM timer
  ledc_timer_config_t pwm_timer = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&pwm_timer);

  // Configure PWM for BIN1
  ledc_channel_config_t pwm_BIN1 = {
    .gpio_num   = BIN1,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel    = LEDC_CHANNEL_2,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 255,  // full speed
    .hpoint     = 0
  };
  ledc_channel_config(&pwm_BIN1);

  // Configure PWM for BIN2 (LOW)
  ledc_channel_config_t pwm_BIN2 = {
    .gpio_num   = BIN2,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel    = LEDC_CHANNEL_3,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 0,  // 0 = LOW
    .hpoint     = 0
  };
  ledc_channel_config(&pwm_BIN2);

  // Done – motor should now be getting full voltage across its terminals
}

void loop() {
  // Nothing needed in loop – motor stays powered
}
