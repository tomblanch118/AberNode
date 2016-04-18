#include <DHT.h>
#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include "Adafruit_TCS34725.h"
#include <SHT1x.h>
#include "LowPower.h"

#define DHT1PIN 8
#define DHT2PIN 10
#define DHT3PIN 14
#define SENSE_PWR_PIN 15
#define DHTTYPE     DHT22


#define NODEID      96
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

#define FLSH_NODE_ADDR 0xFFFFFF //flash address of node address
#define FLSH_NODE_ADDR_SET 0xFFFFFE //flash address of address set flag
#define FLSH_ADDR_SET 0xAA

#define dataPin  A0
#define clockPin A1

SHT1x sht1x(dataPin, clockPin);

DHT  dht1(DHT1PIN, DHTTYPE);
DHT  dht2(DHT2PIN, DHTTYPE);
DHT  dht3(DHT3PIN, DHTTYPE);

//uint8_t nodeID = 0xFF;


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)
byte sendSize = 0;
boolean requestACK = false;
SPIFlash flash(23, 0xEF30); //EF40 for 16mbit windbond chip
RFM69 radio;

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


  /* Setup usart using lazy arduino stuff
     Turn off usart RX otherwise it overrides the rx pins functionality,
     usart is for debug only so dont need to rx.
  */
  Serial.begin(115200);
  UCSR0B &= ~_BV(RXEN0);



  flash.initialize();

/*
  uint8_t addr_set = flash.readByte(FLSH_NODE_ADDR_SET);
  if(FLSH_ADDR_SET == addr_set)
  {
   nodeID = flash.readByte( 
  }
  else
  {
    Serial.println("No address found, error");
    while(1);  
  }*/
  
  
  flash.sleep();

   Serial.print("TEST2");
  Serial.flush();


  pinMode(SENSE_PWR_PIN, OUTPUT);
  //pinMode(12, OUTPUT);
  digitalWrite(SENSE_PWR_PIN, HIGH);
  pinMode(A4, INPUT);

  radio.initialize(FREQUENCY, nodeID, NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);




  if (tcs.begin())
  {
    Serial.println("Found sensor");
    tcs.setInterrupt(true); //Turn off led.
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections");
    //fix failure
  }

  Serial.flush();

  dht1.begin();
  dht2.begin();
  dht3.begin();
  delay(1000);




}

void sensors_on()
{
  tcs.enable();
  //  tcs.setInterrupt(true); //Turn off led.
  pinMode(DHT1PIN, INPUT_PULLUP);
  pinMode(DHT2PIN, INPUT_PULLUP);
  pinMode(DHT3PIN, INPUT_PULLUP);
  pinMode(SENSE_PWR_PIN, OUTPUT);
  digitalWrite(SENSE_PWR_PIN, HIGH);
  delay(500);
}

void sensors_off()
{
  tcs.disable();
  pinMode(SENSE_PWR_PIN, INPUT);
  pinMode(DHT1PIN, INPUT);
  pinMode(DHT2PIN, INPUT);
  pinMode(DHT3PIN, INPUT);
}

//int a = 0;
void loop() {


  sensors_on();

  sense_data.soil_t = sht1x.readTemperatureC();

  sense_data.soil_h = sht1x.readHumidity();

  sense_data.batt = analogRead(A4) * 2.0f * 0.0032f;

  tcs.getRawData(&sense_data.r, &sense_data.g, &sense_data.b, &sense_data.c);

  sense_data.h1 = dht1.readHumidity();
  sense_data.t1 = dht1.readTemperature();
  sense_data.h2 = dht2.readHumidity();
  sense_data.t2 = dht2.readTemperature();
  sense_data.h3 = dht3.readHumidity();
  sense_data.t3 = dht3.readTemperature();

  sensors_off();
  if (radio.sendWithRetry(GATEWAYID, (const void*)(&sense_data), sizeof(sense_data)))
  {
    //   Serial.print(" ok!");
  }
  else
  {
    //  Serial.print(" nothing...");
  }


  radio.sleep();
  mcu_sleep();


}

void mcu_sleep()
{

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //8
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//16
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//24
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//32
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//40
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//48
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);//56
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);//60
}

