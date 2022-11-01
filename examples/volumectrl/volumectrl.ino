#include <WiFi.h>
#include <ELi_McM_4_00.h>
#include <ELi_VmM_4_01.h>

VmM_4_10 a(22);
volatile int samp;
hw_timer_t * timer = NULL;
float v_val;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  samp = analogRead(McM_ESP32D_4_00_CH_ADC_DAC)-2048;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();
  float  b = a.setGain(30);
  Serial.begin(115200);
  Serial.print("setup() running on core ");
  McM_ESP32D_4_00_Setmux(I_A,1);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 62.5, true);
  timerAlarmEnable(timer);
}

void loop() {
  v_val = 10*(samp/4096.0)*5.1;
//  Serial.println(samp);
  Serial.println(v_val,2);
//  delay(500);
}

