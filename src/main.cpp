#include <Arduino.h>
#include <Bounce2.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#include "apps.h"
#include "can.h"
#include "debug.h"
#include "display.h"

#define buzzerPin 4  //! trocar para pino 2 no shield novo

#define R2D_PIN 32
#define R2D_TIMEOUT 500

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define STARTUP_DELAY_MS 10000

#define APPS_READ_PERIOD_MS 20
#define BAMOCAR_ATTENUATION_FACTOR 1

int current_BMS = 0;

volatile bool disabled = false;
volatile bool BTBReady = false;
volatile bool transmissionEnabled = false;

volatile bool TSOn = false;
volatile bool R2DOverride = false;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t statusRequest;

extern CAN_message_t disable;

extern CAN_message_t DCVoltageRequest;
extern CAN_message_t actualSpeedRequest;

int8_t current_byte1; // MSB
int8_t current_byte2;        // LSB
CAN_message_t current_msg_lemos;

enum status {
    IDLE,    // waiting for r2d && ATS off
    DRIVING  // r2d pressed && ATS on
};

status R2DStatus;
Bounce r2dButton = Bounce();

elapsedMillis R2DTimer;
elapsedMillis APPSTimer;
elapsedMillis CURRENTtimer;

elapsedMicros mainLoopPeriod;

void playR2DSound() {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
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
    R2DTimer = 0;

    delay(STARTUP_DELAY_MS);

    can1.write(disable);
    can1.write(statusRequest);
    can1.write(DCVoltageRequest);

#if DATA_DISPLAY > 0
    // can1.write(actualSpeedRequest);
    displaySetup();
#endif
#ifdef MAIN_DEBUG
    LOG("Setup complete, Waiting for R2D\n");
#endif
}

void loop() {
    if (mainLoopPeriod < 10)
        return;

#ifdef DATA_LOGGING
    write()
#endif

#if DATA_DISPLAY > 0
        displayUpdate();
#endif

    switch (R2DStatus) {
        case IDLE:
            r2dButton.update();

#ifdef R2D_DEBUG
            LOG("R2D Button: %d\tR2D: %s", r2dButton.read(), TSOn ? "MAINS OK" : "MAINS OFF");
            Serial.print("\tR2D Timer: ");
            Serial.println(R2DTimer);
#endif

            if ((r2dButton.fell() and TSOn and R2DTimer < R2D_TIMEOUT) or R2DOverride) {
#ifdef R2D_DEBUG
                LOG("R2D OK, Switching to drive mode\n");
#endif
                playR2DSound();
                initBamocarD3();
                request_dataLOG_messages();
                R2DStatus = DRIVING;
                break;
            }
            break;

        case DRIVING:
            if (not TSOn and not R2DOverride) {
                R2DStatus = IDLE;
                can1.write(disable);
                break;
            }

            if (APPSTimer > APPS_READ_PERIOD_MS) {
                APPSTimer = 0;
                int apps_value = readApps();

                if (apps_value >= 0)
                    sendTorqueVal(apps_value);
                else
                    sendTorqueVal(0);
                break;
            }

            if(CURRENTtimer > 8) {
                CURRENTtimer = 0;
                
                current_byte1 = (current_BMS >> 8) & 0xFF;  // MSB
                current_byte2 = current_BMS & 0xFF;         // LSB

                current_msg_lemos.id = 0x201;
                current_msg_lemos.len = 5;
                current_msg_lemos.buf[0] = 0xfb;
                current_msg_lemos.buf[1] = current_byte2;
                current_msg_lemos.buf[2] = current_byte1;
                current_msg_lemos.buf[3] = 0x00;
                current_msg_lemos.buf[4] = 0x00;               
                can1.write(current_msg_lemos);
            }

            //Serial.printf("Message ID: %x\n",current_msg.id);
            //Serial.printf("Message length: %d\n", current_msg.len);
            //Serial.printf("Message data: ");
            //for (int i = 0; i < current_msg.len; i++)
                //Serial.printf("%x ", current_msg.buf[i]);
            //Serial.println();

            break;
        default:
            ERROR("Invalid r2d_status");
            break;
    }
}
