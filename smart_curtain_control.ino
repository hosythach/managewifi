// Smart curtain project - Control source code

// ---------------------------------------------------------------------------------------------------------------------
// Include required libraries
// WiFi functions library
#include <WiFi.h>
#include <WiFiManager.h>                // https://github.com/tzapu/WiFiManager
// Web application library
#include <WebServer.h>
#include <DNSServer.h>
// Blynk application library
#include <BlynkSimpleEsp32.h>
// Liquid crystal display (LCD) library
// #include <LiquidCrystal_I2C.h>       // https://github.com/johnrickman/LiquidCrystal_I2C
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Set up used pins on Microprocessor - ESP32
// Physical control buttons
const char BUTTON_OPEN = 27;
const char BUTTON_CLOSE = 26;
// L298N module's control pins
const char L298_EN_A = 23;
const char L298_EN_B = 22;
const char L298_IN_1 = 19;
const char L298_IN_2 = 18;
const char L298_IN_3 = 17;
const char L298_IN_4 = 16;
// Physical indicator LEDs
const char LED_OPEN = 14;
const char LED_CLOSE = 12;
// Pulse Width Modulator (PWM) channels, use to lower motor speed
const int frequency = 10000;
const char PWM_CHANNEL_A = 0;
const char PWM_CHANNEL_B = 1;
const char resolution = 8;
// Blynk application authorization token
char auth[] = "WvQCVHcLTHXi343qrq8WZ88mvGruAybt";
char server_address[] = "sv.bangthong.com";
// Liquid crystal display
// LiquidCrystal_I2C lcd(0x27, 20, 2);
// Set up WiFi server
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxillary variables to store the current output state
String OUTPUT_CLOSE_STATE = "off";
String OUTPUT_OPEN_STATE = "off";
// Select control mode
char mode_select = 0;
// Set up opened/closed states
// By default, the curtain is closed
unsigned int ready_to_open = 1;
unsigned int ready_to_close = 0;
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Additional functions to control motors
void control_open()                     // Turn on motor, to open the curtain
{
  digitalWrite(LED_OPEN, HIGH);         // Indicate the curtain is being open, physically on board (green)
  digitalWrite(L298_IN_1, LOW);         // Control the left motor, clockwise
  digitalWrite(L298_IN_2, HIGH);
  digitalWrite(L298_IN_3, LOW);         // Control the right motor, clockwise
  digitalWrite(L298_IN_4, HIGH);
}

void control_close()                     // Turn on motor, to close the curtain
{
  digitalWrite(LED_CLOSE, HIGH);         // Indicate the curtain is being closed, physically on board (red)
  digitalWrite(L298_IN_1, HIGH);         // Control the left motor, counter-clockwise
  digitalWrite(L298_IN_2, LOW);
  digitalWrite(L298_IN_3, HIGH);         // Control the right motor, counter-clockwise
  digitalWrite(L298_IN_4, LOW);
}

void control_release_open()              // Turn off motor, work with Blynk application
{ 
  // Turn off indicator LEDs
  digitalWrite(LED_OPEN, LOW);          
  // Turn off all motors
  digitalWrite(L298_IN_2, LOW);
  digitalWrite(L298_IN_4, LOW);
  // Toggle status value
  ready_to_open = 0;
  ready_to_close = 1;
}

void control_release_close()              // Turn off motor, work with Blynk application
{
  // Turn off indicator LEDs
  digitalWrite(LED_CLOSE, LOW);          
  // Turn off all motors
  digitalWrite(L298_IN_1, LOW);
  digitalWrite(L298_IN_3, LOW);
  // Toggle status value
  ready_to_open = 1;
  ready_to_close = 0;
}

void control_release()                      // Turn off motor, work with WebServer & Physicall buttons
{
  // Turn off indicator LEDs
  digitalWrite(LED_OPEN, LOW);
  digitalWrite(LED_CLOSE, LOW);
  digitalWrite(L298_IN_1, LOW);
  digitalWrite(L298_IN_3, LOW);
  digitalWrite(L298_IN_2, LOW);
  digitalWrite(L298_IN_4, LOW);
}
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                 // Start serial monitor, for future data write

  // Work with LCD
  // lcd.init();
  // lcd.backlight();

  // Initialize used pins on Microprocessor - ESP32
  // Physical buttons
  pinMode(BUTTON_OPEN, INPUT);
  pinMode(BUTTON_CLOSE, INPUT);
  // Physical LEDs
  pinMode(LED_OPEN, OUTPUT);
  pinMode(LED_CLOSE, OUTPUT);
  // L298 control pins
  pinMode(L298_IN_1, OUTPUT);
  pinMode(L298_IN_2, OUTPUT);
  pinMode(L298_IN_3, OUTPUT);
  pinMode(L298_IN_4, OUTPUT);
  pinMode(L298_EN_A, OUTPUT);
  pinMode(L298_EN_B, OUTPUT);
  control_release();                    // Clear all undefined output from pins
  // Set up PWM for L298_EN_A and L298_EN_B
  ledcSetup(PWM_CHANNEL_A, frequency, resolution);      // Channel A
  ledcAttachPin(L298_EN_A, PWM_CHANNEL_A);
  ledcSetup(PWM_CHANNEL_B, frequency, resolution);      // Channel B
  ledcAttachPin(L298_EN_B, PWM_CHANNEL_B);
  // Set PWM value to 170
  ledcWrite(PWM_CHANNEL_A, 170);
  ledcWrite(PWM_CHANNEL_B, 170);
  
  // WiFi functions
  // Auto connect to recent WiFi AP or create new AP in IP Address 192.168.4.1
  WiFiManager wifi_manager;
  wifi_manager.autoConnect("Smart curtains");
  Serial.println("Connected...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connect failed!");
    // lcd.println("Connect failed!");
    // lcd.clear();
  }
  // Print local IP address & start web server
  // lcd.setCursor(0, 1);
  // lcd.print(WiFi.localIP);
  Serial.println(WiFi.localIP());
  // Serial.println(WiFi.SSID());
  // Serial.println(WiFi.psk());
  // Start the web server on WiFi.localIP() address
  server.begin();
  Serial.println("Server has started!");

  // Connect to Blynk app
  // Get WiFi credentials
  // Get WiFi SSID & Passkey as char array, since Blynk input does not permit String type
  String ssid_string = WiFi.SSID();
  String psk_string = WiFi.psk();
  int ssid_length = ssid_string.length() + 1;
  int psk_length = psk_string.length() + 1;
  char ssid[ssid_length];
  char password[psk_length];
  ssid_string.toCharArray(ssid, ssid_length);
  psk_string.toCharArray(password, psk_length);
  // Establish connection to Blynk server
  Blynk.begin(auth, ssid, password, server_address, 8080);
  // When connection established, write information to Serial/Blynk application
  Serial.println("Connected to Blynk server!");
  Blynk.virtualWrite(V5, "clr");              // Set up Blynk terminal input pin V5
  Blynk.virtualWrite(V5, "Connected to ESP32 - Smart curtain control");
}
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    mode_select = Serial.read();
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Work with Blynk application
  if (mode_select == '0')
  {
    Blynk.run();
  }  
  // -------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------
  // Work with Web server
  else if (mode_select == '1')
  {    
    WiFiClient client = server.available();                     // Listen for incoming clients
    
    if (client)                                                 // If a new client connects
    {                         
      String currentLine = "";                                  // Create string to hold data from client
      while (client.connected())                                // Loop while the client's connected
      { 
        if (client.available())                                 // If there're bytes to read from the client,
        {                         
          char c = client.read();                               // Read the byte
          header += c;
          if (c == '\n')
          {                                       // If the byte is a newline character
                                                  // If the current line is blank, you got two newline characters in a row.
                                                  // That's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
                                                  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                                  // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: Open");
              client.println();
    
              // turns the GPIOs on and off
              if (header.indexOf("GET /Close/on") >= 0)
              {
                OUTPUT_CLOSE_STATE = "on";
                OUTPUT_OPEN_STATE = "off";
                control_close();
              }
              else if (header.indexOf("GET /Close/off") >= 0)
              {
                OUTPUT_CLOSE_STATE = "off";
                control_release();
              }
              else if (header.indexOf("GET /Open/on") >= 0)
              {
                OUTPUT_OPEN_STATE = "on";
                OUTPUT_CLOSE_STATE = "off";
                control_open();
              }
              else if (header.indexOf("GET /Open/off") >= 0)
              {
                OUTPUT_OPEN_STATE = "off";
                control_release();
              }
    
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Alegraya; display: inline-block; margin: 0px auto; text-align: center;text-color : #9999ff;}");
              client.println(".button { background-color: #00ff00; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #555555;}</style></head>");
    
              // Web Page Heading
              client.println("<body><h1>Web Control </h1>");
    
              // Display current state, and ON/OFF buttons for GPIO 26
              client.println("<p><<== - Close " + OUTPUT_CLOSE_STATE + "</p>");
              // If the output26State is off, it displays the ON button
              if (OUTPUT_CLOSE_STATE == "off")
              {
                client.println("<p><a href=\"/Close/on\"><button class=\"button\">ON</button></a></p>");
              }
              else
              {
                client.println("<p><a href=\"/Close/off\"><button class=\"button button2\">OFF</button></a></p>");
              }
    
              // Display current state, and ON/OFF buttons for GPIO 27
              client.println("<p>==>> - Open " + OUTPUT_OPEN_STATE + "</p>");
              // If the output27State is off, it displays the ON button
              if (OUTPUT_OPEN_STATE == "off")
              {
                client.println("<p><a href=\"/Open/on\"><button class=\"button\">ON</button></a></p>");
              }
              else
              {
                client.println("<p><a href=\"/Close/off\"><button class=\"button button2\">OFF</button></a></p>");
              }
              client.println("</body></html>");
    
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            }
            else
            { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          }
          else if (c != '\r')
          {                   // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Open the connection
      client.stop();
    }
  }
  // -------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------
  // Work with physical buttons
  else if (mode_select == '2')
  {
      // Get digital input from buttons
      static unsigned char  button_open_pressed   = LOW,
                            button_close_pressed  = LOW;
      // Work with open button
      if (button_open_pressed == HIGH && button_close_pressed == LOW)
        {
          control_open();
        }
      
      // Work with close button
      if (button_open_pressed == LOW && button_close_pressed == HIGH)
      {
        control_close();
      }
      
      if (button_open_pressed == LOW && button_close_pressed == LOW)
      {
        control_release();
      }
      
      // Read input from buttons
      button_open_pressed = digitalRead(BUTTON_OPEN);
      button_close_pressed = digitalRead(BUTTON_CLOSE);
  }
  // -------------------------------------------------------------------------------------------------------------------
}
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Set up additional functions for control over Blynk application

// Open using button
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
    control_open();
  }
  else if (pinValue == 0 && ready_to_open == 1)
  {
    control_release_open();
    Serial.println("Curtain is opened!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is opened!");
  }
}

// Close using button 
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
    control_close();
  }
  else if (pinValue == 0 && ready_to_close == 1)
  {
    control_release_close();
    Serial.println("Curtain is closed!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is closed!");
  }
}

// Working with Blynk timer
// Work with Timer, to open 
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
    control_open();
  }
  else if (pinValue == 0 && ready_to_open == 1)
  {
    control_release_open();
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
    control_close();
  }
  else if (pinValue == 0 && ready_to_close == 1)
  {
    control_release_close();
    Serial.println("Curtain is closed!");
    Blynk.virtualWrite(V5, "clr");
    Blynk.virtualWrite(V5, "Curtain is closed!");
  }
}
// ---------------------------------------------------------------------------------------------------------------------
