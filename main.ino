#include <heltec.h>
#include "WiFi.h"
#include "esp_sleep.h"
#include <HTTPClient.h>
#include <DHT.h>
#include "esp32-oled/main.c"
#include "esp32-time/main.c"

#define MYLED 25
#define DHTPIN 21
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
// Define classes
OLED oled;
Time t;

String serverPath = "https://api.ipify.org/?format=string";
String version = "Firmware v0.2.29";

void setup()
{
    enableLogging();
    showInitialLogs();
    // connectToWiFi();
    initializeLED();
    // Start Dev
    dht.begin();
    // End Dev
}

void loop()
{
    digitalWrite(MYLED, !digitalRead(MYLED));
    delay(5000);
    Serial.println(version);
    // Start Dev
    oled.clear();
    // delay(10000);
    // Let the library get the Temperature that DHT11 probe is measuring.
    float t = dht.readTemperature();
    // if (isnan(t))
    // {
    //     t = -1; // if so, check the connection of your DHT11 sensor... something is disconnected ;-)
    // }
    float h = dht.readHumidity();
    // if (isnan(h))
    // {
    //     h = -1; // if so, check the connection of your DHT11 sensor... something is disconnected ;-)
    // }
    digitalWrite(MYLED, !digitalRead(MYLED));
    delay(1000);
    float f = (t * 9/5) + 32;
    oled.echo("Temperature: " + String(f));
    oled.echo("Humidity: " + String(h));
    // End dev

    // fireTheHttpRequest();
    // lightupLedThenSleep();
}

void enableLogging()
{
    // Enable serial
    Serial.begin(9600);

    // Enable OLED
    oled.init();
}
void showInitialLogs()
{
    // Print to serial
    Serial.println("Booting up.");
    Serial.println(version);

    // Initialize and test oled
    oled.echo(version);
}
void connectToWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        oled.echo("Connecting to WiFi..");
    }

    oled.echo("Connected to the WiFi network");
    Serial.println(WiFi.localIP());
}
void initializeLED()
{
    // Initialize for white LED
    pinMode(MYLED, OUTPUT);
    digitalWrite(MYLED, LOW);
}
void fireTheHttpRequest()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());

        // Send HTTP GET request
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
            oled.echo(payload);
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}
void lightupLedThenSleep()
{
    // Toggle LED every cycle
    digitalWrite(MYLED, !digitalRead(MYLED));
    delay(t.mSeconds(3)); // Let the LED light up for 3 seconds
    esp_deep_sleep(t.uSeconds(30));
}