
/**
 *	Write CSV fields feature example.
 */

#include <CSVFile.h>
#include <SdFat.h>
#include "can.h"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t Nact_filtered;
extern CAN_message_t Vout_msg;
extern CAN_message_t Iq_cmd_msg;
extern CAN_message_t Iq_actual_msg;
extern CAN_message_t Mout_msg;
extern CAN_message_t I_lim_inuse_msg;
extern CAN_message_t I_actual_filtered_msg;
extern CAN_message_t Tpeak_msg;
extern CAN_message_t Imax_peak_msg;
extern CAN_message_t I_con_eff_msg;

extern int soc;
extern int current;
extern int packVoltage;
extern int Nact;
extern int Vout;
extern int Iq_cmd;
extern int Iq_actual;
extern int Mout;
extern int I_lim_inuse;
extern int I_actual_filtered;
extern int Tpeak;
extern int Imax_peak;
extern int I_con_eff;
extern int motorTemp;
extern int powerStageTemp;

SdFat sd;
CSVFile csv;

int t = 0;

elapsedMillis writeTIMER;

#define PIN_SD_CS 44                  //! Não sei se está certo, coloquei para remover os erros de compilação
#define SD_CARD_SPEED SD_SCK_MHZ(50)  //! Same aqui, foi o copilot que escreveu

void setup_csv() {
    // Setup pinout
    pinMode(PIN_SPI_MOSI, OUTPUT);
    pinMode(PIN_SPI_MISO, INPUT);
    pinMode(PIN_SPI_SCK, OUTPUT);
    // Disable SPI devices
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);

#if PIN_OTHER_DEVICE_CS > 0
    pinMode(PIN_OTHER_DEVICE_CS, OUTPUT);
    digitalWrite(PIN_OTHER_DEVICE_CS, HIGH);
#endif  // PIN_OTHER_DEVICE_CS > 0

    // Setup serial
    Serial.begin(9600);
    while (!Serial) { /* wait for Leonardo */
    }
    // Setup SD card
    if (!sd.begin(PIN_SD_CS, SD_CARD_SPEED)) {
        Serial.println("SD card begin error");
        return;
    }
    can1.write(Nact_filtered); 
    can1.write(Vout_msg);
    can1.write(Iq_cmd_msg);
    can1.write(Iq_actual_msg);
    can1.write(Mout_msg);
    can1.write(I_lim_inuse_msg);
    can1.write(I_actual_filtered_msg);
    can1.write(Tpeak_msg);
    can1.write(Imax_peak_msg);
    can1.write(I_con_eff_msg);
}


void write() {

    if(writeTIMER > 100){
        
        if (sd.exists("data.csv") && !sd.remove("data.csv")){
            Serial.println("Failed init remove file");
            return;
        }
        if (!csv.open("data.csv", O_RDWR | O_CREAT)) {
            Serial.println("Failed open file");
        }

        csv.addField(t);

        // N act (filt) - 0xA8
        csv.addField(Nact);
        Serial.printf("N act filtered - %d \n",Nact);

        // Vout - 0x8A
        csv.addField(Vout);
        Serial.printf("Vout - %d \n",Vout);

        // Iq cmd - 0x26
        csv.addField(Iq_cmd);
        Serial.printf("Iq command - %d \n",Iq_cmd);

        // Iq actual - 0x27
        csv.addField(Iq_actual);
        Serial.printf("Iq actual - %d \n",Iq_actual);

        // M out - 0xA0
        csv.addField(Mout);
        Serial.printf("M out - %d \n",Mout);

        // I lim inuse - 0x48
        csv.addField(I_lim_inuse);
        Serial.printf("I lim inuse - %d \n",I_lim_inuse);

        // I act (filt) - 0x5F
        csv.addField(I_actual_filtered);
        Serial.printf("I actual filtered - %d \n",I_actual_filtered);

        // T-peak - 0xF0
        csv.addField(Tpeak);
        Serial.printf("T-peak - %d \n",Tpeak);

        // Imax pk - 0xC4
        csv.addField(Imax_peak);
        Serial.printf("I max peak - %d \n",Imax_peak);

        // I con eff - 0xC5
        csv.addField(I_con_eff);
        Serial.printf("I con eff - %d \n",I_con_eff);

        // T-motor - 0x49
        csv.addField(motorTemp);
        Serial.printf("T-motor - %d \n",motorTemp);

        // T-igbt - 0x4A
        csv.addField(powerStageTemp);
        Serial.printf("T-igbt - %d \n",powerStageTemp);

        // SoC
        csv.addField(soc);
        Serial.printf("SoC - %d \n",soc);

        // V bat
        csv.addField(packVoltage);
        Serial.printf("V bat - %d \n",packVoltage);

        // I bat
        csv.addField(current);
        Serial.printf("I bat - %d \n",current);

        csv.addLine();

        // We don't add empty line at the end of file.
        // CSV file shouldn't end by '\n' char.

        // Don't forget close the file.
        csv.close();

        t++;

    }
}
