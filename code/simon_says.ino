
/* "Simon Says!" custom PCB memory game

 Keenan Murnion, 07/01/2026

The game consists of 4 LED's (Red, blue, yellow, green) that light up in a pattern.
After each LED sequence, the user must select the corresponding buttons in which
the lights appeared.

When you get the sequence right a happy beep will play
When a wrong sequence is inserted a sad tone will play

The original code and idea is from Arduino_Scuola on the Arduino Project Hub
https://projecthub.arduino.cc/Arduino_Scuola/a-simple-simon-says-game-6f7fef

The original code and project has been modified significantly by I. 
The code now includes an OLED display that displays winning/losing messages, 
as well as keeps track of the all-time high score throughout uses.

Additionally, the entire game is now mounted onto a custom built PCB, with
its own power source, making it an entirely mobile game

*/

// Display Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <EEPROM.h>

// Display definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_C5 523
#define NOTE_C3 131

// Notes to play
int notes[] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4 };
//Pins used for buttons
const int buttonPins[] = {8, 9, 10, 11 };
const int buttonCount = 4;//You could add more buttons and tones
const int actualSpeakerPin = 3;//Track speaker pin, but be able to turn it off
int speakerPin = actualSpeakerPin;
const int unusedPin = 2;
//Pins used for LEDs
const int ledPins[] = {4, 5, 6, 7};


const int startSpeed = 300;//If you think its too slow, easily adjustable
const int minSpeed = 50;//If you don't stop it here, it is too fast

const int MAX_LEVEL = 20;
byte sequence[MAX_LEVEL];
byte your_sequence[MAX_LEVEL];
int level  = 1;
int highScore = 0;
int velocity = startSpeed;

const char* win_messages[] = {
  "Good job!",
  "Keep going!",
  "Can you beat the high score?",
  "Great work!",
  "Awesome!"
};

const char* lose_messages[] = {
  "Cooked",
  "McFraud!",
  "Idiot..",
  "LMAOOO",
  "L + ratio"
};

void showText(const char* line1, const char* line2 = "", const char* line3 = ""){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (strlen(line2) > 0) {
    display.setCursor(0, 20);
    display.println(line2);
  }
  if (strlen(line3) > 0) {
    display.setCursor(0, 40);
    display.println(line3);
  }
  display.display();
}

void showScore() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.println(level - 1);
  display.setCursor(0, 20);
  display.print("High Score: ");
  display.println(highScore);
  display.display();
}


void setup() {
  //Using a for loop to set all the pins as input
  for (int i = 0; i < buttonCount; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  //LED output pins
  for (int i = 0; i < buttonCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    //Start them off
    digitalWrite(ledPins[i], LOW);
  }


display.begin(0x3C, true);

// Loading high score from EEPROM
highScore = EEPROM.read(0);
if (highScore == 255) highScore = 0;

showText("Keenan Says!", "Button 1: Sound", "Button 2: Silent");

}

void loop()
{
// Secret reset: hold buttons 3 and 4 together
  if (level == 1 &&
      digitalRead(buttonPins[0]) == LOW &&
      digitalRead(buttonPins[1]) == LOW) {
    delay(500); // hold for half a second to confirm
    if (digitalRead(buttonPins[0]) == LOW &&
        digitalRead(buttonPins[1]) == LOW) {
      EEPROM.write(0, 0);
      highScore = 0;
      showText("High score", "RESET!");
      delay(2000);
      showText("Keenan Says!", "Button 1: Sound", "Button 2: Silent");
      while (digitalRead(buttonPins[0]) == LOW ||
             digitalRead(buttonPins[1]) == LOW) { delay(10); }
    }
  }

  
     
  
  if  (level == 1)
  generate_sequence();//generate a sequence;

  if (digitalRead(buttonPins[3])  == LOW) //If first button is pressed or if you're winning
  {
    //Start game normally with buzzer
    speakerPin = 3;
  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence
  }else if (digitalRead(buttonPins[2])  == LOW) //If 2nd button pressed
  {
    //Start game silently
    speakerPin = 2; //Disable speaker
  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence
  }else if (level != 1) //Or if winning
  {
  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence
  }
}

void show_sequence(){
  char buf[20];
  sprintf(buf, "Round %d", level);
  showText(buf, "Watch closely...");
  delay(800);

  //Start off
  allLow();
  delay(100);
  //Show the sequence
  for (int i = 0; i < level; i++){
    digitalWrite(ledPins[sequence[i]], HIGH); // Use index, not pin number
    tone(speakerPin, notes[sequence[i]], 160);
    delay(velocity);
    digitalWrite(ledPins[sequence[i]], LOW);
    noTone(speakerPin);
    delay(200);
  }

  showText("Your turn!");
  delay(100);
}

void get_sequence() {
  for (int i = 0; i < level; i++) {
    bool pressed = false;
    while (!pressed) {
      for (int j = 0; j < buttonCount; j++) {
        if (digitalRead(buttonPins[j]) == LOW) {
          delay(20); // debounce: wait for signal to settle
          if (digitalRead(buttonPins[j]) == LOW) { // confirm still pressed
            digitalWrite(ledPins[j], HIGH);
            tone(speakerPin, notes[j]);
            your_sequence[i] = j;
            delay(200); // LED/sound feedback
            digitalWrite(ledPins[j], LOW);
            noTone(speakerPin);
            // Wait for button release to avoid double press
            while (digitalRead(buttonPins[j]) == LOW) { delay(10); }
            delay(50); // debounce: wait after release
            pressed = true;
            if (your_sequence[i] != sequence[i]) {
              wrong_sequence();
              return;
            }
            break;
          }
        }
      }
    }
  }
  right_sequence();
}

void generate_sequence(){
  //Generate a random sequence
  randomSeed(millis());
  for (int i = 0; i < MAX_LEVEL; i++)
  {
    sequence[i]  = random(0, buttonCount); // Store index (0-3)
  }
}

void wrong_sequence(){
  int score = level - 1;

  delay(300);
  tone(speakerPin, NOTE_G4, 250);
  delay(400);
  delay(200);
  tone(speakerPin, NOTE_G4, 250);
  delay(400);
  delay(300);
  tone(speakerPin, NOTE_C3, 1200);
  delay(1400);
  noTone(speakerPin);

  allHigh();
  delay(250);
  allLow();
  delay(250);

  const char* msg = lose_messages[random(0, 5)];
  char scoreBuf[20];
  sprintf(scoreBuf, "Score: %d", score);

  if (score > highScore) {
    highScore = score;
    EEPROM.write(0, highScore);

    showText(msg, scoreBuf, "NEW HIGH SCORE!");
  } else {
    char hsBuf[20];
    sprintf(hsBuf, "High: %d", highScore);
    showText(msg, scoreBuf, hsBuf);
  }

  delay(3000);
  showText("Keenan Says!", "Button 1: Sound", "Button 2: Silent");

  level = 1;
  velocity = startSpeed;
}

void right_sequence(){
  int score = level;

  if (score > highScore) {
    highScore = score;
    EEPROM.write(0, highScore);
    
    // High score fanfare - different from normal win sound
    tone(speakerPin, NOTE_C4, 100);
    delay(120);
    tone(speakerPin, NOTE_E4, 100);
    delay(120);
    tone(speakerPin, NOTE_G4, 100);
    delay(120);
    tone(speakerPin, NOTE_C5, 100);
    delay(120);
    tone(speakerPin, NOTE_G4, 100);
    delay(120);
    tone(speakerPin, NOTE_C5, 400);
    delay(450);
    noTone(speakerPin);

    char scoreBuf[20];
    sprintf(scoreBuf, "Score: %d", score);
    showText("NEW HIGH SCORE!", scoreBuf);
    delay(2000);
  } else {
    // Normal win
    tone(speakerPin, NOTE_C4, 100);
    delay(120);
    tone(speakerPin, NOTE_E4, 100);
    delay(120);
    tone(speakerPin, NOTE_C5, 250);
    delay(300);
    noTone(speakerPin);

    const char* msg = win_messages[random(0, 5)];
    char scoreBuf[20];
    sprintf(scoreBuf, "Score: %d", score);
    showText(msg, scoreBuf);
    delay(1500);
  }

  if (level < MAX_LEVEL)
    level++;

  velocity -= 50;
  if (velocity < minSpeed) {
    velocity = minSpeed;
  }
}
void allHigh(){
  for (int i = 0; i < buttonCount; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}
void allLow(){
  for (int i = 0; i < buttonCount; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}
