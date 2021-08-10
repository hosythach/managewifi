#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>       // https://github.com/tzapu/WiFiManager
#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C

const int l298n_In1 = 27; // 4 in control module L298N
const int l298n_In2 = 26;
const int l298n_In3 = 25;
const int l298n_In4 = 33;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Assign output variables to GPIO pins button ??
//const int pin_Control_Left = 2;
//const int pin_Control_Right = 4;

// Set web server port number to 80 ( can choose 81, 82....)
WiFiServer server(80);
// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output_Left_State = "off";
String output_Right_State = "off";
//
void stop_Motor()
{
  digitalWrite(l298n_In1, LOW);
  digitalWrite(l298n_In2, LOW);
  digitalWrite(l298n_In3, LOW);
  digitalWrite(l298n_In4, LOW);
}
void left_On()
{
  digitalWrite(l298n_In1, HIGH);
  digitalWrite(l298n_In2, LOW);
  digitalWrite(l298n_In3, LOW);
  digitalWrite(l298n_In4, HIGH);
}
void right_On()
{
  digitalWrite(l298n_In1, LOW);
  digitalWrite(l298n_In2, HIGH);
  digitalWrite(l298n_In3, HIGH);
  digitalWrite(l298n_In4, LOW);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  lcd.init();      // initialize the lcd
  lcd.backlight(); // Print a message to the LCD.
  // Initialize the output variables as outputs
  pinMode(l298n_In1, OUTPUT);
  pinMode(l298n_In2, OUTPUT);
  pinMode(l298n_In3, OUTPUT);
  pinMode(l298n_In4, OUTPUT);

  // Set all to LOW to off motor
  stop_Motor();

  //auto connect old wifi or new wifi in 192.168.4.1
  WiFiManager wifiManager;
  wifiManager.autoConnect("Smart curtains");
  Serial.println("Connected.....");

  while (WiFi.status() != WL_CONNECTED)
  {
    lcd.println("Failed"); // fail connect wifi
    lcd.clear();
  }
  // Print local IP address and start web server
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.print(WiFi.localIP());

  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available(); // Listen for incoming clients

  if (client)
  { // If a new client connects

    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /left/on") >= 0)
            {
              output_Left_State = "on";
              output_Right_State = "off";
              left_On();
            }
            else if (header.indexOf("GET /left/off") >= 0)
            {
              output_Left_State = "off";
              stop_Motor();
            }
            else if (header.indexOf("GET /right/on") >= 0)
            {
              output_Right_State = "on";
              output_Left_State = "off";
              right_On();
            }
            else if (header.indexOf("GET /right/off") >= 0)
            {
              output_Right_State = "off";
              stop_Motor();
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
            client.println("<p><<== - Left " + output_Left_State + "</p>");
            // If the output26State is off, it displays the ON button
            if (output_Left_State == "off")
            {
              client.println("<p><a href=\"/left/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/left/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27
            client.println("<p>==>> - Right " + output_Right_State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output_Right_State == "off")
            {
              client.println("<p><a href=\"/right/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/right/off\"><button class=\"button button2\">OFF</button></a></p>");
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
    // Close the connection
    client.stop();
  }
}
