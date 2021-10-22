#include <heltec.h>
#include "WiFi.h"
#include "esp_sleep.h"
#include <HTTPClient.h>
#include <DHT.h>
#include "lib/esp32-oled/main.c"
#include "lib/esp32-time/main.c"

#define MYLED 25
#define DHTPIN 21
#define DHTTYPE DHT22
#define CYCLETIME 30
DHT dht(DHTPIN, DHTTYPE);

// Define classes
OLED oled;
Time t;

String serverPath = "https://api.ipify.org/?format=string";
String version = "Firmware v0.2.36";

// Define global variables
float temperature = 0.00;
float humidity = 0.00;
int secondCounter = 30;
String loadingBar = "";

void setup()
{
    enableLogging();
    showInitialLogs();
    // connectToWiFi();
    initializeLED();
    dht.begin();
}

void loop()
{
    // Clear screen
    oled.clear();

    // Start blink LED
    digitalWrite(MYLED, !digitalRead(MYLED));

    // Read hygrometer and update global variable
    readUpdateHygrometer();
    // End blink LED
    digitalWrite(MYLED, !digitalRead(MYLED));
    displayReadings();
    showLoadingBar();
    oled.echo("Sending in " + String(secondCounter) + " seconds.");
    // fireTheHttpRequest();
    delay(1000);

    // Cycle the secondCounter
    secondCounter--;

    if (secondCounter <= 0)
    {
        secondCounter = 30;
    }
}

void showLoadingBar()
{
    loadingBar = "";
    int i = 0;
    while (i <= secondCounter)
    {
        loadingBar = loadingBar + "|";
        i++;
    }
    oled.echo(loadingBar);
}
void readUpdateHygrometer()
{
    float t = dht.readTemperature();
    if (isnan(t))
    {
        t = -1;
    }
    float f = (t * 9 / 5) + 32;

    temperature = f;

    float h = dht.readHumidity();
    if (isnan(h))
    {
        h = -1;
    }
    humidity = h;
}
void displayReadings()
{
    oled.echo("Temperature: " + String(temperature));
    oled.echo("Humidity: " + String(humidity));
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
