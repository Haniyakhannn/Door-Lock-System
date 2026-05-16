#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>
#include <Servo.h>   

#define buzzer 11

LiquidCrystal_I2C lcd(0x27, 16, 2);

// SERVO
Servo servo;
int servoPin = 10;

// Password
Password password = Password("0123");

byte maxPasswordLength = 4;
byte currentPasswordLength = 0;
byte cursorPos = 5;

int wrongAttempts = 0;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(buzzer, OUTPUT);

  // SERVO SETUP
  servo.attach(servoPin);
  servo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(3, 0);
  lcd.print("WELCOME");
  lcd.setCursor(0, 1);
  lcd.print("LOCK SYSTEM");

  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("ENTER PASSWORD");

  char key = keypad.getKey();

  if (key != NO_KEY) {

    delay(50);

    if (key == 'C') {
      resetPassword();
    }
    else if (key == 'D') {
      checkPassword();
    }
    else {
      enterPassword(key);
    }
  }
}


void enterPassword(char key) {
  if (currentPasswordLength < maxPasswordLength) {

    lcd.setCursor(cursorPos, 1);
    lcd.print("*");
    cursorPos++;

    password.append(key);
    currentPasswordLength++;
  }
}

void checkPassword() {
  lcd.clear();

  if (password.evaluate()) {

    wrongAttempts = 0;  // reset attempts on success

    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);

    // SERVO OPEN
    servo.write(90);

    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(0, 1);
    lcd.print("WELCOME");

    delay(5000);

    // SERVO CLOSE
    servo.write(0);

  } else {

    wrongAttempts++;

    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzer, HIGH);
      delay(150);
      digitalWrite(buzzer, LOW);
      delay(150);
    }

    lcd.setCursor(0, 0);
    lcd.print("WRONG PASSWORD");

    if (wrongAttempts >= 3) {
      activateAlarm();
    } else {
      lcd.setCursor(0, 1);
      lcd.print("TRY AGAIN");
      delay(1500);
    }
  }

  delay(2000);
  resetPassword();
}

void activateAlarm() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARM ACTIVE!");
  lcd.setCursor(0, 1);
  lcd.print("SECURITY ALERT");

  unsigned long startTime = millis();

  while (millis() - startTime < 60000) {  

    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(300);
  }

  wrongAttempts = 0; // reset after alarm
  resetPassword();
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  cursorPos = 5;
  lcd.clear();
}