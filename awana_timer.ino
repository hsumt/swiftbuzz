// Install steps:
// 1. Install Arduino IDE from the official Arduino download site
// 2. Open Arduino IDE: Sketch > Include Library. Then, search for Adafruit SSD1306, and Adafruit GFX
// 3. Compile first. Resolve bugs
// 4. Upload to the Mega via USB-B cable.
// 5. Now you're ready to rumble!

// Created by @hsumt 6/4/2026

const int NUM_BUZZERS = 10;
const int MAX_RANKED = 5;
const int buzzerPins[NUM_BUZZERS] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
bool buzzed[NUM_BUZZERS];
unsigned long buzzTime[NUM_BUZZERS];
unsigned long startTime = 0;
unsigned long questionStart = 0;
bool roundActive = false;
bool roundLocked = false;
int buzzCount = 0;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <limits.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  delay(500);
  Serial.begin(115200);

  for (int i=0; i < NUM_BUZZERS; i++) {
    pinMode(buzzerPins[i], INPUT_PULLUP);
    buzzed[i] = false;
    buzzTime[i] = 0;
  }
  Serial.println("Ready to time!");


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 24);
  display.setTextSize(2);
  display.println(" READY!");
  display.display();

}
void resetRound() {
  for (int i=0; i < NUM_BUZZERS; i++) {
    buzzed[i] = false;
    buzzTime[i] = 0;
  }
  startTime = 0;
  roundActive = false;
  roundLocked = false;
  buzzCount = 0;
  Serial.println("RESET! Ready for next round!");
  updateOLED();
}

void handleSerial() {
  if (!Serial.available()) return;
  char cmd = Serial.read();
  if (cmd == 'R' || cmd =='r') {
    resetRound();
  }
  if (cmd == 'Q' || cmd == 'q') {
    questionStart = micros();
    Serial.println("QUESTION_STARTED");
  }
}
void updateOLED() {
  display.clearDisplay();

  //  Winner (big, top half) 
  int winner = -1;
  unsigned long fastest = ULONG_MAX;
  for (int i = 0; i < NUM_BUZZERS; i++) {
    if (buzzed[i] && buzzTime[i] < fastest) {
      fastest = buzzTime[i];
      winner = i;
    }
  }

  if (winner == -1) {
    display.setTextSize(2);
    display.setCursor(8, 24);
    display.println("READY!");
    display.display();
    return;
  }

  // Winner block
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("#1 BZR ");
  display.println(winner + 1);
  display.setTextSize(1);
  display.setCursor(0, 18);
  display.print((fastest / 1000000.0), 2);
  display.println(" s");

  // Divider line
  display.drawLine(0, 27, 127, 27, SSD1306_WHITE);

  // ── Ranked list (bottom half) ──
  // Build sorted order
  int order[NUM_BUZZERS];
  int count = 0;
  for (int i = 0; i < NUM_BUZZERS; i++) {
    if (buzzed[i]) order[count++] = i;
  }
  // Bubble sort by buzzTime
  for (int a = 0; a < count - 1; a++) {
    for (int b = 0; b < count - a - 1; b++) {
      if (buzzTime[order[b]] > buzzTime[order[b+1]]) {
        int tmp = order[b]; order[b] = order[b+1]; order[b+1] = tmp;
      }
    }
  }

  // Print up to 4 runners-up (rows of 8px, starting at y=30)
  int maxRows = 4;
  int shown = 0;
  display.setTextSize(1);
  for (int r = 1; r < count && shown < maxRows; r++) {
    int idx = order[r];
    int y = 30 + (shown * 9);
    display.setCursor(0, y);
    display.print(r + 1);
    display.print(". Bzr ");
    display.print(idx + 1);
    display.print("  ");
    display.print((buzzTime[idx] / 1000000.0), 2);
    display.println("s");
    shown++;
  }

  display.display();
}
void loop() {
  handleSerial();

  for (int i=0; i < NUM_BUZZERS; i++) {
    if (buzzed[i]) continue;
    if (digitalRead(buzzerPins[i]) != LOW) continue;
    if (roundLocked) return;

    unsigned long now = micros();

    if(!roundActive) {
      startTime = now;
      roundActive = true;
    }

    unsigned long elapsed = now - startTime;

    if (elapsed < 50000 && buzzCount == 0) {
      startTime = now;
    }
    if (buzzCount ==0) {
      Serial.print("QUESTION_TIME,");
      Serial.println((now - questionStart) / 1000);
    }

    buzzed[i] = true;
    buzzTime[i] = elapsed;
    buzzCount++;
    updateOLED();

    Serial.print("BUZZ,");
    Serial.print(i+1);
    Serial.print(",");
    Serial.println(elapsed);

    if (buzzCount >= MAX_RANKED) {
      Serial.println("LOCKED");
      roundActive = false;
      roundLocked = true;
    }
  }
}
