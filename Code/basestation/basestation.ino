#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

RFM69 radio;
SPIFlash flash(8, 0xEF30); //EF40 for 16mbit windbond chip
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

typedef struct {
  float h1;
  float t1;
  float h2;
  float t2;
  float h3;
  float t3;

  float soil_t;
  float soil_h;

  float batt;

  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
} SensorData;
SensorData sense_data;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  if (flash.initialize())
    Serial.println("SPI Flash Init OK!");
  else
    Serial.println("SPI Flash Init FAIL! (is chip present?)");
}

byte ackCount = 0;
void loop() {


  if (radio.receiveDone())
  {
    Serial.print(radio.SENDERID, DEC); Serial.print(",");
  Serial.print(radio.readRSSI()); Serial.print(",");
    if (promiscuousMode)
    {
      Serial.print("to ["); Serial.print(radio.TARGETID, DEC); Serial.print("] ");
    }

    if (radio.DATALEN != sizeof(SensorData))
      Serial.print("Invalid payload received, not matching Payload struct!");
    else
    {
      sense_data = *(SensorData*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      //      Serial.println(a++);
      //Serial.print("\ndht1:");
      Serial.print(sense_data.h1);
      Serial.print(",");
      Serial.print(sense_data.t1);
      Serial.print(",");
      Serial.print(sense_data.h2);
      Serial.print(",");
      Serial.print(sense_data.t2);
      Serial.print(",");
      Serial.print(sense_data.h3);
      Serial.print(",");
      Serial.print(sense_data.t3);
      Serial.print(",");
      Serial.print(sense_data.r);
      Serial.print(",");
      Serial.print(sense_data.g);
      Serial.print(",");
      Serial.print(sense_data.b);
      Serial.print(",");
      Serial.print(sense_data.c);
      Serial.print(",");
      Serial.print(sense_data.soil_t);
      Serial.print(",");
      Serial.print(sense_data.soil_h);
      Serial.print(",");
      Serial.println(sense_data.batt);
      Serial.flush();
    }

    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      //Serial.print(" - ACK sent.");


    }
   // Serial.println();
    //Blink(LED, 3);
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}
