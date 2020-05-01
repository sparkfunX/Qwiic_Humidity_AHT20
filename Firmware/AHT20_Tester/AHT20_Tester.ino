/*

*/
#include <Wire.h>

#include <SparkFun_Qwiic_Humidity_AHT20.h> //Click here to get the library: http://librarymanager/All#Qwiic_Humidity_AHT20 by SparkFun
AHT20 humiditySensor;

#define STAT_LED 13

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic Humidity AHT20 examples");

  pinMode(STAT_LED, OUTPUT);
  digitalWrite(STAT_LED, LOW);

  Wire.begin();
}

void loop()
{
  //Check if the AHT20 will acknowledge
  if (humiditySensor.begin() == true)
  {
    while (1)
    {
      //If a new measurement is available
      if (humiditySensor.available() == true)
      {
        //Get the new temperature and humidity value
        float temperature = humiditySensor.getTemperature();
        float humidity = humiditySensor.getHumidity();

        Serial.print(humidity, 2);
        Serial.print("% ");
        Serial.print(temperature, 2);
        Serial.print("C");

        if (temperature > 22 && temperature < 30 && humidity > 5 && humidity < 80)
        {
          digitalWrite(STAT_LED, HIGH);
          Serial.print(" Good!");
        }

        Serial.println();
      }

      delay(50);

      if (humiditySensor.begin() == false) break;
    }
  }
  else
  {
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
    digitalWrite(STAT_LED, LOW);
  }
  delay(50);

}
