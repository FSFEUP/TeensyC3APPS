#include <Arduino.h>
#include <Bounce2.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "debug.h"
#include "display.h"
#include "write_data.h"

#define buzzerPin 4

#define R2D_PIN 32
#define R2D_TIMEOUT 500

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define STARTUP_DELAY_MS 10000

#define APPS_READ_PERIOD_MS 20
#define BAMOCAR_ATTENUATION_FACTOR 1

volatile bool disabled = false;
volatile bool BTBReady = false;
volatile bool transmissionEnabled = false;

volatile bool R2D = false;
volatile bool R2DOverride = false;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t statusRequest;

extern CAN_message_t disable;

extern CAN_message_t DCVoltageRequest;
extern CAN_message_t actualSpeedRequest;

enum status {
  IDLE,    // waiting for r2d && ATS off
  DRIVING  // r2d pressed && ATS on
};

status R2DStatus;
elapsedMillis R2DTimer;

elapsedMillis APPSTimer;
Bounce r2dButton = Bounce();

void playR2DSound() {
  digitalWrite(buzzerPin, HIGH);
  delay(2000);
  digitalWrite(buzzerPin, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(APPS_1_PIN, INPUT);
  pinMode(APPS_2_PIN, INPUT);

  pinMode(buzzerPin, OUTPUT);
  canSetup();

  r2dButton.attach(R2D_PIN, INPUT);
  r2dButton.interval(0.1);

  R2DStatus = IDLE;

  delay(STARTUP_DELAY_MS);

  can1.write(disable);
  can1.write(statusRequest);
  can1.write(actualSpeedRequest);
  can1.write(DCVoltageRequest);

  displaySetup();
  //setup_csv();
#ifdef MAIN_DEBUG
  LOG("Setup complete, Waiting for R2D\n");
#endif
}

void loop() {
  displayUpdate();
  //write();
  switch (R2DStatus) {
    case IDLE:
      r2dButton.update();
      if ((r2dButton.fell() and R2D) or R2DOverride) {
        playR2DSound();
        initBamocarD3();
        R2DStatus = DRIVING;
        break;
      }
      break;

    case DRIVING:
      if (not R2D and not R2DOverride) {
        R2DStatus = IDLE;
        can1.write(disable);
        break;
      }

      if (APPSTimer > APPS_READ_PERIOD_MS) {
        APPSTimer = 0;
        int apps_value = readApps();

        if (apps_value >= 0)
          sendMsg(apps_value);
        else
          sendMsg(0);
        break;
      }
      break;

    default:
      ERROR("Invalid r2d_status");
      break;
  }
}
