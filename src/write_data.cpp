
/**
 *	Write CSV fields feature example.
 */

#include <SdFat.h>
#include <CSVFile.h>

SdFat sd;
CSVFile csv;

int t = 0;

void setup_csv() {
  // Setup pinout
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_CLK, OUTPUT);
  //Disable SPI devices
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);

#if PIN_OTHER_DEVICE_CS > 0
  pinMode(PIN_OTHER_DEVICE_CS, OUTPUT);
  digitalWrite(PIN_OTHER_DEVICE_CS, HIGH);
#endif  //PIN_OTHER_DEVICE_CS > 0

  // Setup serial
  Serial.begin(9600);
  while (!Serial) { /* wait for Leonardo */
  }
  // Setup SD card
  if (!sd.begin(PIN_SD_CS, SD_CARD_SPEED)) {
    Serial.println("SD card begin error");
    return;
  }
}


void initSdFile() {
  if (sd.exists(FILENAME) && !sd.remove(FILENAME)) {
    Serial.println("Failed init remove file");
    return;
  }
  // Important note!
  // You should use flag O_RDWR even if you use CSV File
  // only for writting.
  if (!csv.open(FILENAME, O_RDWR | O_CREAT)) {
    Serial.println("Failed open file");
  }
}


void write() {
  // Data in CSV file is stored in lines.
  // Each line have some (or zero) fields.
  // First you should add line and next
  // add fields. After you can add next line.

  // Each line is ended by end line character '\n',
  // (UNIX style - without '\r').
  // You shouldn't use "println" method (and similars).
  // The fields are separated by delimiter ';'.
  // You can change this character in source file.
  // Your CSV file shouldn't contain this characters.

  // Important note!
  // You should use flag O_RDWR for initialize CSV File even if you use CSV File
  // only for writting.

  initSdFile();

  // At the begin of file we don't need
  // add new line.

  // 2. Number field with non-fixed size.
  //    Use this field if you don't need
  //    edit field's value later.
  //    Support only positive integers.
  //    It is function designed for write
  //    line numbers.

  csv.addField(t)


  //N act (filt) - 0xA8
  csv.addField();

  //Vout - 0x8AL
  csv.addField();

  //Iq cmd - 0x26
  csv.addField();

  //Iq actual - 0x27
  csv.addField();

  //M out - 0xA0
  csv.addField();

  //I lim inuse - 0x48
  csv.addField();

  //I act (filt) - 0x5F
  csv.addField();

  //T-peak - 0xF0
  csv.addField();

  //Imax pk - 0xC4
  csv.addField();

  //I con eff - 0xC5
  csv.addField();

  //T-motor - 0x49
  csv.addField();

  //T-igbt - 0x4A
  csv.addField();

  //SoC
  csv.addField();

  //V bat
  csv.addField();

  //I bat
  csv.addField();

  csv.addLine();


  // We don't add empty line at the end of file.
  // CSV file shouldn't end by '\n' char.

  // Don't forget close the file.
  csv.close();

  t++;

  // After this operations your CSV file should look like this
  // ('\0' is null character):

  /*0;65535;3444\n
   */

  delay(100)
}