/*
  Board	          int.0	  int.1	  int.2	  int.3	  int.4	  int.5
 Uno, Ethernet	  2	  3
 Mega2560	  2	  3	  21	  20	  19	  18
 Leonardo	  3	  2	  0	  1
 Due	          (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)

 This example use 2 sensors at the same time
 */

#include <idDHT11.h>

int idDHT11pinA = 2; //Digital pin for comunications
int idDHT11intNumberA = 0; //interrupt number (must be the one that use the previus defined pin (see table above)
int idDHT11pinB = 3; //Digital pin for comunications
int idDHT11intNumberB = 1; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dht11_wrapperA(); // must be declared before the lib initialization
//declaration
void dht11_wrapperB(); // must be declared before the lib initialization

// Lib instantiate
idDHT11 DHT11A(idDHT11pinA, idDHT11intNumberA, dht11_wrapperA);
idDHT11 DHT11B(idDHT11pinB, idDHT11intNumberB, dht11_wrapperB);




void setup()
{
  Serial.begin(115200);
  Serial.println("idDHT11 Example program for two sensors");
  Serial.print("LIB version: ");
  Serial.println(IDDHT11LIB_VERSION);
  Serial.println("---------------");
}
// This wrapper is in charge of calling
// mus be defined like this for the lib work
void dht11_wrapperA() {
  DHT11A.isrCallback();
}
// This wrapper is in charge of calling
// mus be defined like this for the lib work
void dht11_wrapperB() {
  DHT11B.isrCallback();
}

void loop()
{
  Serial.print("\nRetrieving information from sensor A: ");
  Serial.print("Read sensor: ");
  
  DHT11A.acquire();
  // instruct the second sensor to acquire, it takes some time, and as the lib
  // is interrrupt driven, we gain some time
  DHT11B.acquire(); 
  while (DHT11A.acquiring()) // check if first sensor has ended acquiring
    ;
  int result = DHT11A.getStatus();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case IDDHTLIB_ERROR_CHECKSUM: 
    Serial.println("Error\n\r\tChecksum error"); 
    break;
  case IDDHTLIB_ERROR_TIMEOUT: 
    Serial.println("Error\n\r\tTime out error"); 
    break;
  case IDDHTLIB_ERROR_ACQUIRING: 
    Serial.println("Error\n\r\tAcquiring"); 
    break;
  case IDDHTLIB_ERROR_DELTA: 
    Serial.println("Error\n\r\tDelta time to small"); 
    break;
  case IDDHTLIB_ERROR_NOTSTARTED: 
    Serial.println("Error\n\r\tNot started"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  Serial.print("Humidity (%): ");
  Serial.println(DHT11A.getHumidity(), 2);

  Serial.print("Temperature (oC): ");
  Serial.println(DHT11A.getCelsius(), 2);

  Serial.print("Temperature (oF): ");
  Serial.println(DHT11A.getFahrenheit(), 2);

  Serial.print("Temperature (K): ");
  Serial.println(DHT11A.getKelvin(), 2);

  Serial.print("Dew Point (oC): ");
  Serial.println(DHT11A.getDewPoint());

  Serial.print("Dew Point Slow (oC): ");
  Serial.println(DHT11A.getDewPointSlow());

  Serial.print("\nRetrieving information from sensor B: ");
  Serial.print("Read sensor: ");
  
  while (DHT11B.acquiring()) // check if second sensor has ended acquiring
    ;
  result = DHT11B.getStatus();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case IDDHTLIB_ERROR_CHECKSUM: 
    Serial.println("Error\n\r\tChecksum error"); 
    break;
  case IDDHTLIB_ERROR_TIMEOUT: 
    Serial.println("Error\n\r\tTime out error"); 
    break;
  case IDDHTLIB_ERROR_ACQUIRING: 
    Serial.println("Error\n\r\tAcquiring"); 
    break;
  case IDDHTLIB_ERROR_DELTA: 
    Serial.println("Error\n\r\tDelta time to small"); 
    break;
  case IDDHTLIB_ERROR_NOTSTARTED: 
    Serial.println("Error\n\r\tNot started"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  Serial.print("Humidity (%): ");
  Serial.println(DHT11B.getHumidity(), 2);

  Serial.print("Temperature (oC): ");
  Serial.println(DHT11B.getCelsius(), 2);

  Serial.print("Temperature (oF): ");
  Serial.println(DHT11B.getFahrenheit(), 2);

  Serial.print("Temperature (K): ");
  Serial.println(DHT11B.getKelvin(), 2);

  Serial.print("Dew Point (oC): ");
  Serial.println(DHT11B.getDewPoint());

  Serial.print("Dew Point Slow (oC): ");
  Serial.println(DHT11B.getDewPointSlow());

  delay(2000);
}

