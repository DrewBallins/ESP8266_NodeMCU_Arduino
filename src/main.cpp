/******************************
 * Includes
 *****************************/
#include <Arduino.h>
#include "DHT.h"

/******************************
 * Defines
 *****************************/
#define TWO_SEC_DELAY      2000
#define DHT_PIN            D1
#define DHT_TYPE           DHT11
//#define DHT_TYPE         DHT22
//#define DHT_TYPE         DHT21

/*******************************
 * Local Function Declarations
 ******************************/
static void calculate_dht_data(void);
static void display_dht_data(void);

/*******************************
 * Initialize DHT Sensor
 ******************************/
DHT dht(DHT_PIN, DHT_TYPE);
float dht_humidity;
float dht_temperature;

void setup() {
   Serial.begin(115200);
   dht.begin();
}

void loop() {
   delay(TWO_SEC_DELAY); // wait for two seconds between DHT measurements   
   
   calculate_dht_data();
   display_dht_data();
}

/*******************************
 * Local Function Definitions
 ******************************/
static void calculate_dht_data(void)
{
   dht_humidity = dht.readHumidity();
   dht_temperature = dht.readTemperature();
}

static void display_dht_data(void)
{
   Serial.print("DHT Humidity = ");
   Serial.print(dht_humidity);
   Serial.println("%");
   Serial.print("DHT Temperature = ");
   Serial.print(dht_temperature);
   Serial.println("C");   
}
