#include "r2d.h"
#include <Arduino.h>
#include <FlexCAN_T4.h>


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

int pin_brake_sensor = 20;
int pin_current_sensor = 21;
int pin_selector_1 = 22;
int pin_selector_2 = 23;
int pin_shutdown_circuit = 9;
int pin_precharge = 10;
int pin_R2Dbutton = 11;
int buzzerPin = 15;



int check_bamocar() {
    CAN_message_t request;
    request.id = 0x181;
    request.len = 3;
    request.buf[0] = 0x30;
    request.buf[1] = 0x8F;
    request.buf[2] = 0x00;
    can1.write(request);

    CAN_message_t msg;
    if(can1.read(msg)) {
        if(msg.buf[0] == 0x8F) {
            uint32_t data = msg.buf[1] + msg.buf[2] + msg.buf[3] + msg.buf[4];
            switch (data) { //all possible errors. O que fazer quando são detetados??
                case 1:
                    //Parameter damaged 
                    Serial.print("Parameter damged");
                    return 1;
                    break;
                case 2:
                    //Hardware error
                    Serial.print("Hardware error");
                    return 1;
                    break;
                case 4:
                    //Safety circuit faulty (only active with RUN)
                    Serial.print("Safety circuit faulty (only active with RUN)");
                    return 1;
                    break;
                case 8:
                    //CAN TimeOut Time exceeded
                    Serial.print("CAN Timeout time exceeded");
                    return 1; 
                    break;
                case 16:
                    //Bad or wrong encoder signal
                    Serial.print("Bad or wrong encoder signal");
                    return 1;
                    break;
                case 32:
                    //Power voltage missing
                    Serial.print("Power voltage missing");
                    return 1;
                    break;
                case 64:
                    //Engine temperature too high 
                    Serial.print("Engine temperature too high");
                    return 1;
                    break;
                case 128:
                    //Unit temperature too high
                    Serial.print("Unit temperature too high");
                    return 1;
                    break;
                case 256:
                    //Overvoltage > 1.8 x UN reached
                    Serial.print("Overvoltage > 1.8 x UN reached");
                    return 1;
                    break;
                case 512:
                    //Overcurrent or strong oscillating current detected 
                    Serial.print("Overcurrent or strong oscillating current detected");
                    return 1;
                    break;
                case 1024:
                    //Spinning (without setpoint, wrong direction)
                    Serial.print("Spinning (without setpoint, wrong direction)");
                    return 1;
                    break;
                case 2048:
                    //User - Error selection
                    Serial.print("User - Error selection");
                    return 1;
                    break;
                case 16384:
                    //Current - Measurement error
                    Serial.print("Current - Measurement error");
                    return 1;
                    break;
                case 32768:
                    //Ballast circuit overloaded
                    Serial.print("Ballast circuit overloaded");
                    return 1; 
                    break;
            }
        }
    }
    return 0;
}

void can_setup() {
    can1.begin();
    can1.setBaudRate(500000);

    pinMode(buzzerPin, OUTPUT);

    pinMode(pin_brake_sensor, INPUT);
    pinMode(pin_current_sensor, INPUT);
    pinMode(pin_precharge, INPUT);
    pinMode(pin_selector_1, INPUT);
    pinMode(pin_selector_2, INPUT);
    pinMode(pin_shutdown_circuit, INPUT);
    pinMode(pin_R2Dbutton, INPUT);

    analogWrite(buzzerPin, 0); 
}

void send_to_bamocar(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    CAN_message_t msg;
    msg.id = 0x201;
    msg.len = 3;
    msg.buf[0] = 0x90;
    msg.buf[1] = byte2;
    msg.buf[2] = byte1;

    can1.write(msg);
}

int check_BMS() {
    return 0;
}

void play_r2d_sound() {
    analogWrite(buzzerPin, 189); // Turn off the buzzer for the other half of the period
    delay(4000);
    analogWrite(buzzerPin,0);
    delay(4000);
}

void r2d_state_update(r2d_mode* state) {
    return ;
}

r2d_mode r2d_state_machine(r2d_mode cur_state, int apps_value) {
    r2d_mode next_state = cur_state;
    switch (cur_state) {
        case R2D_MODE_STARTUP:
            delay(POWER_ON_DELAY_MS);
            next_state = R2D_MODE_IDLE;
            break;
        case R2D_MODE_IDLE:
            if(check_bamocar() != 0) next_state = R2D_MODE_ERROR;
            
            if(check_BMS()) {}
            // check apps
            // check modo dash
            // check modo volante
            
            
            // check fim precharge
            // check r2d button
            // check brake
            if(digitalRead(pin_precharge) == HIGH && digitalRead(pin_shutdown_circuit) == HIGH) {
                if(digitalRead(pin_R2Dbutton) == HIGH && analogRead(pin_brake_sensor) > 100) {
                    next_state = R2D_MODE_DRIVE;
                }
            }

            // update display
            break;
        case R2D_MODE_DRIVE:
            play_r2d_sound();

            if(check_bamocar() != 0) next_state = R2D_MODE_ERROR;

            //if(check_BMS()) {}

            // check apps
            // check modo dash
            // check modo volante

            //ler seletor do volante referente aos modos (para limitar o bamocar) -> usar código do Bernardo mas enviamos como parâmetros os valores lidos da BMS

            send_to_bamocar(apps_value);

            //if(o carro desligar temos de atualizar o estado para o básico) next_state = R2D_MODE_STARTUP;

            break;
        case R2D_MODE_ERROR:
            next_state = R2D_MODE_IDLE;
            break;

        default:
            next_state = R2D_MODE_ERROR;
            break;
    }
    return next_state;
}
