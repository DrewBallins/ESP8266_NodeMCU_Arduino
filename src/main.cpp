/******************************
 * Includes
 *****************************/
#include <Arduino.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"

/******************************
 * Defines
 *****************************/
#define TWO_SEC_DELAY      2000
#define NUM_DATA           4
#define DHT_PIN            D1
#define DHT_TYPE           DHT11
//#define DHT_TYPE         DHT22
//#define DHT_TYPE         DHT21
//#define SERIAL_DEBUG     // uncomment this #define to enable debug prints over serial

/*******************************
 * Local Function Declarations
 ******************************/
static void calculate_dht_data(void);
static void display_dht_data_serial(void);
static void send_dht_data_udp(void);

/*******************************
 * Initialize DHT Sensor
 ******************************/
DHT dht(DHT_PIN, DHT_TYPE);
float dht_humidity;
float dht_temperature;

/*******************************
 * Initialize UDP
 ******************************/
const char * ssid = ***REDACTED***
const char * password = ***REDACTED***
AsyncUDP udp;
IPAddress nodemcuv2_IP_address = IPAddress(192,168,1,100);
IPAddress rpi_IP_address = IPAddress(192,168,1,14);
uint16_t udp_port = 42069;

void setup() {
   Serial.begin(115200);
   dht.begin();

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   if (WiFi.waitForConnectResult() != WL_CONNECTED)
   {
   #ifdef SERIAL_DEBUG      
      Serial.println("WiFi Failed");
   #endif
      while (true)
      {
         delay(1000);
      }
   }
   if (udp.connect(nodemcuv2_IP_address, udp_port))
   {
   #ifdef SERIAL_DEBUG
      Serial.println("UDP connected!");
   #endif
   }
}

void loop() {
   delay(TWO_SEC_DELAY); // wait for two seconds between DHT measurements   
   
   calculate_dht_data();
#ifdef SERIAL_DEBUG
   display_dht_data_serial();
#endif
   send_dht_data_udp();
}

/*******************************
 * Local Function Definitions
 ******************************/
static void calculate_dht_data(void)
{
   dht_humidity = dht.readHumidity();
   dht_temperature = dht.readTemperature();
}

static void display_dht_data_serial(void)
{
   Serial.print("DHT Humidity = ");
   Serial.print(dht_humidity);
   Serial.println("%");
   Serial.print("DHT Temperature = ");
   Serial.print(dht_temperature);
   Serial.println("C");   
}

static void send_dht_data_udp(void)
{
   // first we need to pack dht data into uint8_t buffer array
   uint8_t dht_data_packed[NUM_DATA] = 
   {
      (uint8_t)dht_humidity,  // integer value
      (uint8_t)((dht_humidity - (uint8_t)dht_humidity) * 100), // decimal value
      (uint8_t)dht_temperature,  // integer value
      (uint8_t)((dht_temperature - (uint8_t)dht_temperature) * 100)  // decimal value
   };

   // next we create a message object and write our data to it
   AsyncUDPMessage dht_data_message = AsyncUDPMessage(sizeof(dht_data_packed));
   dht_data_message.write(dht_data_packed, sizeof(dht_data_packed));

   // finally we send the udp packet with packed data to rpi for processing
   size_t send_result = udp.sendTo(dht_data_message, rpi_IP_address, udp_port);

#ifdef SERIAL_DEBUG
   // print out error message (if any) or length
   Serial.print("send result = ");
   Serial.println(send_result);
#endif
}
