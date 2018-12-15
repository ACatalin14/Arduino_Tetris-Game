#include <LiquidCrystal.h>
#include "LedControl.h"
#include <string.h>
#include <stdlib.h>

#define LCD_CONTRAST_PIN 9 	// V0 pin of LCD
#define JOYSTICK_X_PIN A0 	// VRx pin of the joystick
#define JOYSTICK_Y_PIN A1 	// VRy pin of the joystick
#define JOYSTICK_BUTTON_PIN 8		// SW pin of the joystick
#define START_LINE 2		// the line #0 of our matrix (contained in our matrix)
#define FINISH_LINE 10		// the "floor" line of our matrix (it is outside of the matrix)

int gameState;	// current game state: 1 = waiting to begin the gameplay; 2 = gameplay; 3 = waiting to start again the game
int lastGameState;	// it can help saying if the gameState changed
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
LedControl ledMatrix = LedControl(12, 11, 10, 1);

int gameMatrix[11][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},	// imaginary line #-2, above our original matrix, made for spawning pieces
	{0, 0, 0, 0, 0, 0, 0, 0},	// imaginary line #-1, above our original matrix, made for spawning pieces
	{0, 0, 0, 0, 0, 0, 0, 0},	// line #0 of the printed matrix for the leds
	{0, 0, 0, 0, 0, 0, 0, 0},	
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},	// line #7 of the printed matrix
	{1, 1, 1, 1, 1, 1, 1, 1}	// imaginary line #8, the "floor" of the matrix
};

void setup() {
	Serial.begin(9600);
	gameState = 1;
	lastGameState = -1;
	randomSeed(analogRead(0));
	lcdSetup();
	joystickSetup();
	ledMatrixSetup();
}

void loop() {
	//Serial.println(gameState);
	switch(gameState) {
		case 1:
			if (lastGameState != gameState) {
				lastGameState = gameState;
				gameStartSetup();
			}
			gameStartLoop();
			break;
		case 2:
			if (lastGameState != gameState) {
				lastGameState = gameState;
				gamePlaySetup();
			}
			gamePlayLoop();
			break;
		case 3:
			if (lastGameState != gameState) {
				lastGameState = gameState;
				gameEndSetup();
			}
			gameEndLoop();
			break;
		default:
			if (lastGameState != gameState) {
				lastGameState = gameState;
				gameClose();
			}
	}
}

void lcdSetup() {
	lcd.begin(16, 2);
	lcd.home();
	pinMode(LCD_CONTRAST_PIN, OUTPUT);
	analogWrite(LCD_CONTRAST_PIN, 100);
}

void joystickSetup() {
	pinMode(JOYSTICK_X_PIN, INPUT);
	pinMode(JOYSTICK_Y_PIN, INPUT);
	pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
}

void ledMatrixSetup() {
	ledMatrix.shutdown(0, false);
	ledMatrix.setIntensity(0, 7);
	ledMatrix.clearDisplay(0);
}

void gameClose() {
	lcd.clear();
	lcd.home();
	lcd.print("Thx for playing!");
	lcd.setCursor(0, 1);
	lcd.print("See you soon! ;)");
}
