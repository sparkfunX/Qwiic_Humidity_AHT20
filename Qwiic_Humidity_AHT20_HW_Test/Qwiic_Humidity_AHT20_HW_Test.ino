#include <Wire.h>
#include <math.h>

#define ADDRESS 0x38
#define INITIALIZATION 0xBE
#define MEASUREMENT 0xAC

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic Humidity AHT20 test script");
  Wire.begin(); //Join I2C bus
}

void loop() {
    //START MEASUREMENT SEQUENCE!!

  //Wait 40 ms
  Serial.println("Wait 40 ms");
  delay(40);

  //Check calibration bit
  uint8_t stat;
  Wire.requestFrom(ADDRESS, 1);
  while (Wire.available())
    stat = Wire.read();
  //DEBUGGING
  Serial.print("State: 0x");
  Serial.println(stat, HEX);
  uint8_t temp = 1;
  temp = temp << 3;
  //Make sure calibration bit IS set
  if (!(stat & temp))
  {
    Serial.println("Chip not callibrated! Freezing.");
    while (1);
  }
  //Continue
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
  Serial.println("Wait 100 ms");
  delay(100);

  //Check the busy bit
  Wire.requestFrom(ADDRESS, 1);
  while (Wire.available())
    stat = Wire.read();
  //DEBUGGING
  Serial.print("State: 0x");
  Serial.println(stat, HEX);
  temp = 1;
  temp = temp << 7;
  //Make sure busy bit IS NOT set
  if (stat & temp)
  {
    Serial.println("Can't continue. AHT20 indicating busy taking measurement. Freezing.");
    while (1);
  }
  //Continue
  Serial.println("AHT20 not busy. Continue.");

  //Read the next six bytes of data
  if (Wire.requestFrom(ADDRESS, 6) <= 0)
  {
    Serial.println("Error reading data from AHT20. Freezing.");
    while (1);
  }
  uint8_t state = Wire.read();
  Serial.print("State: 0x");
  Serial.println(state, HEX);

  uint32_t incoming = 0;
  incoming |= (uint32_t)Wire.read() << (8 * 2);
  Serial.println(incoming, HEX);
  incoming |= (uint32_t)Wire.read() << (8 * 1);
  Serial.println(incoming, HEX);
  uint8_t midByte = Wire.read();

  incoming |= midByte << (8 * 0);
  Serial.println(incoming, HEX);
  uint32_t humidity = incoming >> 4;
  Serial.println(humidity, HEX);  
  Serial.println("Read-in humidity correct, I think?");

  uint32_t temperature = (uint32_t)midByte << (8 * 2);
  Serial.println(temperature, HEX);
  temperature |= (uint32_t)Wire.read() << (8 * 1);
  Serial.println(temperature, HEX);
  temperature |= (uint32_t)Wire.read() << (8 * 0);
  Serial.println(temperature, HEX);

  //Need to get rid of data in bits > 20
  temperature = temperature & ~(0xFFF00000);
  Serial.println(temperature, HEX);
  Serial.println("Read-in temperature correct, too.");
  
  Serial.println("Read-in AHT20 raw data");
  Serial.print("Raw temp: 0x");
  Serial.println(temperature, HEX);
  Serial.print("Raw humidity: 0x");
  Serial.println(humidity, HEX);

  //Calculate degrees celcius and %RH
  float Temp;
  Temp = ((float)temperature / pow(2.0, 20.0)) * 200 - 50;
  float Humid;
  Humid = ((float)humidity / pow(2.0, 20.0)) * 100;

  //Print the result
  Serial.print("Temperature: ");
  Serial.print(Temp);
  Serial.print(" C \t Humidity: ");
  Serial.print(Humid);
  Serial.println("% RH");
  Serial.println();

  delay(3000);
}
