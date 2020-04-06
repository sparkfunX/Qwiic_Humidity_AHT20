#include <Wire.h>

#define ADDRESS 0x38
#define INITIALIZATION 0xBE
#define MEASUREMENT 0xAC

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic Humidity AHT20 test script");
  Wire.begin(); //Join I2C bus

  //START MEASUREMENT SEQUENCE!!

  //Wait 40 ms
  Serial.println("Wait 40 ms");
  delay(40);

  //Check calibration bit
  Wire.beginTransmission(ADDRESS);
  uint8_t stat = Wire.read();
  Wire.endTransmission();
  uint8_t temp = 1;
  temp = temp << 3;
  if (!(stat && temp))
  {
    Serial.println("Chip not callibrated! Freezing.");
    while (1);
  }
  Serial.println("AHT20 callibrated!");

  //Initialize
  Wire.beginTransmission(ADDRESS);
  Wire.write(INITIALIZATION);
  Wire.write((uint8_t *)0x0800, 2);
  Wire.endTransmission();
  Serial.println("AHT20 has been initialized.");

  //Trigger measurement
  Wire.beginTransmission(ADDRESS);
  Wire.write(MEASUREMENT);
  Wire.write((uint8_t *)0x3300, 2);
  Wire.endTransmission();
  Serial.println("AHT20 measurement has been triggered.");

  //Wait 75 ms
  Serial.println("Wait 75 ms");
  delay(75);

  //Check the busy bit
  Wire.begin(ADDRESS);
  stat = Wire.read();
  temp = 1;
  temp = temp << 7;
  if (!(stat && temp))
  {
    Serial.println("Can't continue. AHT20 indicating busy taking measurement. Freezing.");
    while (1);
  }
  Serial.println("AHT20 not busy. Continue.");

  //Read the next six bytes of data
  if (Wire.requestFrom(ADDRESS, 6) <= 0)
  {
    Serial.println("Error reading data from AHT20. Freezing.");
    while (1);
  }
  uint8_t state = Wire.read();

  uint32_t incoming = 0;
  incoming |= (uint32_t)Wire.read() << (8 * 2);
  incoming |= (uint32_t)Wire.read() << (8 * 1);
  uint8_t midByte = Wire.read();

  incoming |= midByte << (8 * 0);
  uint32_t humidity = incoming >> 4;

  uint32_t temperature = midByte << (8 * 2);
  temperature = (uint32_t)Wire.read() << (8 * 1);
  temperature = (uint32_t)Wire.read() << (8 * 0);

  //Need to get rid of data in bits > 20
  temperature = temperature & ~(0xFFF00000);
  Serial.println("Read-in AHT20 raw data");

  //Calculate degrees celcius and %RH
  temperature = (temperature / 2 ^ (20)) * 200 - 50;
  humidity = (humidity / 2 ^ (20)) * 100;

  //Print the result
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C \t Humidity: ");
  Serial.print(humidity);
  Serial.println("% RH");
}

void loop() {

}
