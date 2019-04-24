#include "config.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// pin connected to DH22 data line
#define DATA_PIN 12

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     13 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");

void setup() {

  // start the serial connection
  Serial.begin(115200);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // set up the OLED
  delay(2000);
  display.clearDisplay();      // clear the OLED
  display.setTextSize(1);      // set text size
  display.setTextColor(WHITE); // set text color to white
  display.setCursor(0, 0);     // set cursor to start at top-left corner
  display.display();           // displaying the OLED
}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event; // creates sensors_event_t object named event
  dht.temperature().getEvent(&event); // reads sensor data for temperature

  float celsius = event.temperature; // getting temp from sensor reading
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print("celsius: ");
  Serial.print(celsius); //printing the celsius to serial
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit); //printing the fahrenheit to serial
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(fahrenheit); //updating adafruit io

  dht.humidity().getEvent(&event); //reads sensor data for humidity

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity); // printing humidity to serial
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  // print to the OLED
  display.clearDisplay();  // clear the display
  display.setCursor(0, 0); // reset cursor to top left
  
  display.println("Temperature(C): " + (String)celsius);
  display.println("Temperature(F): " + (String)fahrenheit);
  display.println("Humidity: " + (String)event.relative_humidity);
  display.display();
  

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(5000);
}
