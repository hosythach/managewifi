#define LED_OPEN 25
#define LED_CLOSE 26
#define button_input_open 34
#define button_input_close 35
#define MOTOR_A_PIN_1 19
#define MOTOR_A_PIN_2 18
#define MOTOR_B_PIN_1 17
#define MOTOR_B_PIN_2 16
#define enable_motor_A 23
#define enable_motor_B 22

const int freq = 10000;
const int PWM_channel_A = 0;
const int PWM_channel_B = 1;
const int resolution = 8;

void control_open()
{
  digitalWrite(LED_OPEN, HIGH);
  digitalWrite(MOTOR_A_PIN_1, HIGH);
  digitalWrite(MOTOR_A_PIN_2, LOW);
  digitalWrite(MOTOR_B_PIN_1, HIGH);
  digitalWrite(MOTOR_B_PIN_2, LOW);
}

void control_close()
{
  digitalWrite(LED_CLOSE, HIGH);
  digitalWrite(MOTOR_A_PIN_1, LOW);
  digitalWrite(MOTOR_A_PIN_2, HIGH);
  digitalWrite(MOTOR_B_PIN_1, LOW);
  digitalWrite(MOTOR_B_PIN_2, HIGH);
}

void control_release()
{
  digitalWrite(LED_OPEN, LOW);
  digitalWrite(LED_CLOSE, LOW);
  digitalWrite(MOTOR_A_PIN_1, LOW);
  digitalWrite(MOTOR_A_PIN_2, LOW);
  digitalWrite(MOTOR_B_PIN_1, LOW);
  digitalWrite(MOTOR_B_PIN_2, LOW);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(button_input_open, INPUT);
  pinMode(button_input_close, INPUT);

  pinMode(LED_OPEN, OUTPUT);

  pinMode(LED_CLOSE, OUTPUT);

  pinMode(MOTOR_A_PIN_1, OUTPUT);
  pinMode(MOTOR_A_PIN_2, OUTPUT);
  pinMode(MOTOR_B_PIN_1, OUTPUT);
  pinMode(MOTOR_B_PIN_2, OUTPUT);

  pinMode(PWM_channel_A, OUTPUT);
  pinMode(PWM_channel_B, OUTPUT);
  control_release();

  ledcSetup(PWM_channel_A, freq, resolution);
  ledcAttachPin(enable_motor_A, PWM_channel_A);

  ledcSetup(PWM_channel_B, freq, resolution);
  ledcAttachPin(enable_motor_B, PWM_channel_B);

  ledcWrite(PWM_channel_A, 215);
  ledcWrite(PWM_channel_B, 215);
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned char button_open_pressed = LOW,
                       button_close_pressed = LOW;
  if (button_open_pressed == HIGH && button_close_pressed == LOW)
  {
    control_open();
  }
  else if (button_open_pressed == LOW && button_close_pressed == HIGH)
  {
    control_close();
  }
  else if (button_open_pressed == LOW && button_close_pressed == LOW)
  {
    control_release();
  }
  button_open_pressed = digitalRead(button_input_open);
  button_close_pressed = digitalRead(button_input_close);
}
