// Include required libraries
// Wireless connection libraries
#include <WiFi.h>
#include <WiFiClient.h>
// Blynk app libraries
#include <BlynkSimpleEsp32.h>

// Setup buttons
#define BUTTON_OPEN 25
#define BUTTON_CLOSE 26

// Setup demo output
#define LED_OPEN 23
#define LED_CLOSE 22
#define LED_TIMER 4

// Setup open/close states
volatile unsigned char ready_to_open = 0;
volatile unsigned char ready_to_close = 1;

// Setup Wifi credentials & Blynk authorization token
char auth[] = "WvQCVHcLTHXi343qrq8WZ88mvGruAybt";
char ssid[] = "Duc Hoang";
char pass[] = "08997370";

void setup() {
  // put your setup code here, to run once:
  
  // Setup demo output to LED pins
  pinMode(LED_OPEN, OUTPUT);
  digitalWrite(LED_OPEN, LOW);
  pinMode(LED_CLOSE, OUTPUT);
  digitalWrite(LED_CLOSE, LOW);
  pinMode(LED_TIMER, OUTPUT);
  digitalWrite(LED_TIMER, LOW);

  // Configure control via Blynk app, connect via WiFi
  Serial.begin(115200); // Initiate Serial monitor
  delay(10);

  // Watch over the WiFi while establishing connection
  Serial.print("Connecting to : ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  int wifi_control = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.print("WiFi connected to ");
  Serial.println(ssid);

  // Connect to Blynk server
  Blynk.begin(auth, ssid, pass, "sv.bangthong.com", 8080);
  
  // Write debugging information to Serial/Blynk app
  Serial.println("Connected to Blynk server!");
  Blynk.virtualWrite(V5, "clr");
  Blynk.virtualWrite(V5, "Connected to WiFi:");
  Blynk.virtualWrite(V5, ssid);
  Blynk.virtualWrite(V5, "Connected to ESP32 - Curtain control"); 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  // Run Blynk app
  Blynk.run();

}

// Setup some Blynk virtual pin functions
// If the curtain is closed & need to open
BLYNK_WRITE(V0)
{
  unsigned int pinValue = param.asInt();
  
  if (pinValue == 1 && ready_to_open == 0)
  {
    Serial.println("Curtain is opened, please close before trying to open");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is opened, please close before trying to open!");
  }
  if (pinValue == 1 && ready_to_open == 1)
  {
    curtain_open();
  }
  else if (pinValue == 0 && ready_to_open == 1)
  {
    curtain_stop_open();
    Serial.println("Curtain is opened!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is opened!");
  }
}

// If the curtain is opened & need to close
BLYNK_WRITE(V1)
{
  unsigned int pinValue = param.asInt();
  if (pinValue == 1 && ready_to_close == 0)
  {
    Serial.println("Curtain is closed, please open before trying to close");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is closed, please open before trying to close!");
  }
  if (pinValue == 1 && ready_to_close == 1)
  {
    curtain_close();
  }
  else if (pinValue == 0 && ready_to_close == 1)
  {
    curtain_stop_close();
    Serial.println("Curtain is closed!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is closed!");
  }
}

// Working with Blynk timer
// Working with Timer, to open
BLYNK_WRITE(V2)
{
  unsigned int pinValue = param.asInt();
  
  if (pinValue == 1 && ready_to_open == 0)
  {
    Serial.println("Curtain is opened, please close before trying to open");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is opened, please close before trying to open!");
  }
  if (pinValue == 1 && ready_to_open == 1)
  {
    curtain_open();
  }
  else if (pinValue == 0 && ready_to_open == 1)
  {
    curtain_stop_open();
    Serial.println("Curtain is opened!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is opened!");
  }
}

// Working with Timer, to close
BLYNK_WRITE(V3)
{
  unsigned int pinValue = param.asInt();
  if (pinValue == 1 && ready_to_close == 0)
  {
    Serial.println("Curtain is closed, please open before trying to close");
    Blynk.virtualWrite(V5, "Curtain is closed, please open before trying to close");
  }
  if (pinValue == 1 && ready_to_close == 1)
  {
    curtain_close();
  }
  else if (pinValue == 0 && ready_to_close == 1)
  {
    curtain_stop_close();
    Serial.println("Curtain is closed!");
    
  }
}

// Setup curtain control functions
// Open the curtain
void curtain_open()
{
  digitalWrite(LED_OPEN, HIGH);
}

// Close the curtain
void curtain_close()
{
  digitalWrite(LED_CLOSE, HIGH);
}

// If the curtain is fully opened, then stop opening
void curtain_stop_open()
{
  digitalWrite(LED_OPEN, LOW);
  ready_to_open = 0;
  ready_to_close = 1;
}

// If the curtain is fully closed, then stop closing
void curtain_stop_close()
{
  digitalWrite(LED_CLOSE, LOW);
  ready_to_open = 1;
  ready_to_close = 0;
}
