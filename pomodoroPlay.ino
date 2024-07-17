#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSD1306_I2C_ADDRESS 0x3C  // I2C address for SSD1306 display
#define BUZZPIN 2
#define BUZZDELAY 200
#define PLAYPIN 8
#define RESTARTPIN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long endTime = 0;  // End time for the countdown (in milliseconds)
String input;

void setup() {
  pinMode(BUZZPIN, OUTPUT);
  pinMode(PLAYPIN, INPUT_PULLUP);
  pinMode(RESTARTPIN, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(2000);  // Pause for 2 seconds

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);

  // Set the initial endTime to 30 minutes from the current time
  unsigned long currentTime = millis();
  // endTime = currentTime + 1800000;  // 30 minutes = 30 * 60 * 1000 milliseconds
  endTime =  currentTime + 15000;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  unsigned long elapsedTime = (endTime > currentTime) ? (endTime - currentTime) : 0;

  int minutes = (elapsedTime / 60000) % 60;
  int seconds = (elapsedTime / 1000) % 60;

  display.clearDisplay();

  // Calculate the position to center the text
  int16_t x1, y1;
  uint16_t w, h;

  // Display the countdown timer
  String timerText = String(minutes) + "m " + String(seconds) + "s";
  display.getTextBounds(timerText, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  display.print(timerText);

  display.display();

  if (elapsedTime == 0) {
    playBuzz();
    // Wait for the button to be pressed
    while (digitalRead(PLAYPIN) == HIGH) {
      delay(50); // Small delay to avoid flooding the CPU
    }
    // sending over 1 to python
    Serial.println(true);

    // Loop until a serial message/object is received from Python
    while (Serial.available() == 0) {
      delay(50); // Small delay to avoid flooding the CPU
    }
    input = Serial.readString();
    // recive one back from python indicating that video has been played
    if (input == "1"){
      // endTime = currentTime + 1800000;  // Set the end time to 30 minutes from now
      while (digitalRead(RESTARTPIN) == HIGH){
        delay(50);
      }
      currentTime = millis();
      endTime = currentTime + 15000;  // Set the end time to 30 minutes from now
    }
  }

  delay(1000);
}

void playBuzz() {
  for (int i = 0; i < 2; i++){
    tone(BUZZPIN, 494);
    delay(BUZZDELAY);  
    noTone(BUZZPIN);   
    delay(BUZZDELAY);
  }
}