/**
 * Written by Liang He
 */

#include <ArduinoJson.h>
//#include <Arduino_JSON.h>

#include <WiFi.h>
#include <HTTPClient.h>


// WiFi related
const char* ssid = "University of Washington";
const char* password =  "";

/*** LED related ***/
const int ledPin = 15;  // 15 corresponds to GPIO15
// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8; // 8 bits used for brightness: 0 ~ 255

/*** City selection related ***/
String cities [7] = {"Beijing,cn","Tokyo,jp","Seattle,us","Pittsburgh,us","Honolulu,us","atlanta,us","alaska,us"};
int cityIdx = 0;
const int buttonPin = 32;
int buttonState = 0; 
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/***  OpenWeather access related ***/
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=";
const String key = "&APPID=36a58953580b02d7fb70e04e39d8930c";

void setup() {
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // the network is successfully connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    /*** LED setup ***/
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(ledPin, ledChannel);

    /*** city selection ***/
    pinMode(buttonPin, INPUT);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    /*** check if the city has been changed ***/
    int reading = digitalRead(buttonPin);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }
  
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
  
      // if the button state has changed:
      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH) {
           // change a new city
          cityIdx = (cityIdx + 1)%7;
          Serial.println();
          Serial.print("City: ");
          Serial.println(cities[cityIdx]);

          /*** Obtain the temperature of the new city ***/
          HTTPClient http;
          http.begin(endpoint + cities[cityIdx] + key);
        
          int httpCode = http.GET();
          //Check for the returning code
          if (httpCode > 0) { 
            String payload = http.getString();
            // Serial.println(httpCode);
            // Serial.println(payload);
      
             /*** Change the LED brightness based on the temperature ***/
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload);
            if (error)
              return;
        
            int temp = doc["main"]["temp"];
            temp = temp - 273.16; // convert Kelvin value to Celsius
            Serial.print("temp: ");
            Serial.println(temp);
      
            int brightness = map(temp, 280, 310, 0, 255);
            Serial.print("LED brightness: ");
            Serial.println(brightness);
            Serial.println();
            
            ledcWrite(ledChannel, brightness);
          }
          else {
            Serial.println("Error on HTTP request");
          }
        
          http.end();
        }
      }
    }

    lastButtonState = reading;
   
  }

  // Only one OpenWeather query is allowed every 30 seconds
  // delay(30000);
}
