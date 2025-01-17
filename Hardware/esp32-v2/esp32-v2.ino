#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
HardwareSerial MySerial1(1);

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 250;

// DATA Vars
String TData = "[";
String EData = "[";
String BData = "[";
String SData = "[";
String PData = "[";
String P_data = "["; //to store ADC (P data) not a buffer
long PAT = 0;        //for PAT signal

String payload;
char rx = 'X'; //to read first byte from serial buffer

const char *ssid = "TP-LINK_85D38C";
const char *password = "passcode1";

const char *nodejs_ip = "192.168.1.100"; //"192.168.1.101";
const int nodejs_port = 90;

WiFiServer server(80);

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{

    WiFi.begin("Username", "password"); //to prevent restarts
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
    }
    //Display
    display.display();
    display.clearDisplay();
    Welcome();
    Index();
    //Serial pins
    //  pinMode(23, INPUT);
    //  pinMode(22, INPUT);
    //Serial com
    Serial.begin(115200);
    MySerial1.begin(2000000, SERIAL_8N1, 2, 4); //https://quadmeup.com/arduino-esp32-and-3-hardware-serial-ports/
    Serial2.begin(2000000);
    //Setting Serial Buffer size
    MySerial1.setRxBufferSize(1024);
    Serial2.setRxBufferSize(1024);

    //Display
    ConnectingWIFI();
    //Connecting to WiFi
    Serial.println();
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    //Starting server
    server.begin();
    Serial.println("Server started at...");
    Serial.println(WiFi.localIP());
}

void loop()
{
    ConnectingNode();
    WiFiClient client = server.available();
    Send_data(client);
    delay(1);
}

void Send_data(WiFiClient cl)
{
    if (cl.connect(nodejs_ip, nodejs_port))
    {
        Sending();
        Serial.println("connected to Nodejs");
        HTTPClient http;
        http.begin("http://" + (String)nodejs_ip + ":90/" + "saveData/");
        http.addHeader("Content-Type", "application/json");
        Serial.println("Sending data");

        while (1)
        {
            currentMillis = millis();
            while (currentMillis - previousMillis < interval)
            {
                currentMillis = millis();
                delayMicroseconds(5);
//                readPressure();
                readBuffer(MySerial1);
//                readPressure(); // include in readBuffer to increase Sampling
                readBuffer(Serial2);
            }
            previousMillis = currentMillis;
//            removeComma(P_data);
            removeComma(TData);
            removeComma(EData);
            removeComma(BData);
            removeComma(SData);
            removeComma(PData);

            http.addHeader("Content-Type", "application/json");
            payload = "{\"PData\":" + PData + "], \"SData\":" + SData + "] , \"BData\" : " + BData + "], \"TData\" : " + TData + "], \"EData\" : " + EData + "] }";
            Serial.println(payload);
            int httpCode = http.POST(payload);
            resetData();
        }
        http.end();
    }
    else
    {

        Serial.println("connection failed - Nodejs");
    }
}

void readPressure()
{
    P_data.concat(String(analogRead(34)) + ",");
    delayMicroseconds(10);
}
// buffer reading function
void readBuffer(HardwareSerial buffReader)
{ //R
    boolean active = false;
    String *combineTo;
    while (buffReader.available())
    { //Reading buffer
        rx = buffReader.read();
        //    Serial.print(rx);
        switch (rx)
        {
        case 'X':
            if (active)
            {
                active = false;
                *combineTo = *combineTo + ",";
            }
            break;
        case 'T':
            active = true;
            combineTo = &TData;
            break;
        case 'E':
            active = true;
            combineTo = &EData;
            break;
        case 'I':
            active = true;
            combineTo = &SData;
            break;
        case 'R':
            active = true;
            combineTo = &BData;
            break;
         case 'P':
            active = true;
            combineTo = &PData;
            break;
        default:
            if (active)
            {
                *combineTo = *combineTo + rx;
            }
        }
    }
    //print state
    //  Serial.println();
    //  Serial.print("BData");
    //  Serial.println(BData);
    //  Serial.print("SData");
    //  Serial.println(SData);
    //  Serial.print("TData");
    //  Serial.println(TData);
    //  Serial.print("EData");
    //  Serial.println(EData);
}
void removeComma(String &memRef)
{
    if (memRef[memRef.length() - 1] == ',')
    {
        memRef.remove(memRef.length() - 1); //removing ending coma
    }
}
void resetData()
{
    Serial.println("Resetting");
    P_data = "[";
    TData = "[";
    EData = "[";
    SData = "[";
    BData = "[";
    PData = "[";
}
//Functions for display
void configureDisplay()
{
    display.clearDisplay();
    display.setCursor(0, 0); // Start at to  p-left corner
    display.setTextSize(2);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
}
void Index(void)
{
    configureDisplay();
    display.println("160648A");
    display.println("160145E");
    display.println("160173L");
    display.println("160199V");
    display.display();
    delay(2000);
}

void Welcome(void)
{
    configureDisplay();
    display.println(" WELCOME!");
    display.println("  Final");
    display.println("   Year");
    display.println(" Project");
    display.display();
    delay(2000);
}
void ConnectingWIFI(void)
{
    configureDisplay();
    display.println("Connecting to WiFi:");
    display.println(String(ssid));
    display.display();
    //  delay(1000);
}
void ConnectingNode(void)
{
    configureDisplay();
    display.println("Connected to WiFi!");
    display.println("Connecting to NodeJS");
    display.println("Target IP:");
    display.println(String(nodejs_ip));
    display.println("Target port:");
    display.println(String(nodejs_port));
    display.display();
    //  delay(1000);
}

void Sending(void)
{
    configureDisplay();
    display.println("Connected!");
    display.println("Sending");
    display.println("data");
    display.display();
    //  delay(2000);
}
