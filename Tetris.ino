#include <LiquidCrystal.h>
#include "LedControl.h"

#define LCD_CONTRAST_PIN 9 		// V0 pin of LCD
#define JOYSTICK_X_PIN A1 		// VRx pin of the joystick
#define JOYSTICK_Y_PIN A0 		// VRy pin of the joystick
#define JOYSTICK_BUTTON_PIN 8	// SW pin of the joystick
#define START_LINE 2			// the line #0 of our displayed matrix (it is inside our matrix)
#define FINISH_LINE 10			// the "floor" line of our matrix (it is outside of the matrix)
#define START_COLUMN 1			// the column #0 of our displayed matrix (it is inside)
#define FINISH_COLUMN 9			// the column #8 of our displayed matrix (it is outside)

#define DEBOUNCE_BUTTON_DELAY 50
#define DEBOUNCE_JOYSTICK_DELAY 25
#define JOYSTICK_HORIZONTAL_LEFT 1023	// the VRx value when joystick is inclined to left-most
#define JOYSTICK_HORIZONTAL_RIGHT 0
#define JOYSTICK_VERTICAL_UP 0			// the VRy value when joystick is inclined to up-most
#define JOYSTICK_VERTICAL_DOWN 1023
#define JOYSTICK_THRESHOLD 100			// a value of 100 means an X value from 0 to 100 will determine the joystick to trigger an event for left movement  

int gameState;		// current game state: 1 = waiting to begin the gameplay; 2 = gameplay; 3 = waiting to start again the game
int lastGameState;	// it can help saying if the gameState changed
int score;			// current score

int buttonState;						// 1 = up, 0 = down button of joystick
int lastButtonState;
unsigned long lastDebounceButtonTime;	// used for the joystick's button
int horizontalState;					// -1 = left inclined, 0 = centered, 1 = right inclined joystick
int lastHorizontalState;
unsigned long lastDebounceHorizontalTime;
int verticalState;						// -1 = down inclined, 0 = centered, 1 = up inclined joystick
int lastVerticalState;
unsigned long lastDebounceVerticalTime;


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
LedControl ledMatrix = LedControl(12, 11, 10, 1);

int gameMatrix[11][10] = {
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	// imaginary line #-2, above our original matrix, made for spawning pieces
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	// imaginary line #-1
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	// line #0 of the printed matrix for the leds
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	// line #7 of the printed matrix
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}	// imaginary line #8, the "floor" of the matrix
};									// also, I've added imaginary "walls" on the left and on the right side

void setup() {
	gameState = 1;
	lastGameState = -1;
	buttonState = 1;
	lastButtonState = 1;
	horizontalState = 0;
	lastHorizontalState = 0;
	verticalState = 0;
	lastVerticalState = 0;
	randomSeed(analogRead(0));
	
	lcdSetup();
	joystickSetup();
	ledMatrixSetup();
}

void loop() {
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

// this function will call onClickFunc whenever the joystick's button is pressed
void checkButton( void (*onClickFunc)() ) {
	int reading = digitalRead(JOYSTICK_BUTTON_PIN);

	if (reading != lastButtonState) {
		lastDebounceButtonTime = millis();
	}

	if (millis() - lastDebounceButtonTime > DEBOUNCE_BUTTON_DELAY) {
		if (reading != buttonState) {
			buttonState = reading;
			if (buttonState == 1) {	
				// button was pressed entirely (down+up)
				onClickFunc();
			}
		}
	}

	lastButtonState = reading;
}

// this function will call onHorizontalTriggerFunc whenever the joystick is inclined towards LEFT (parameter == -1) or RIGHT (param == 1)
void checkHorizontalAxis( void (*onHorizontalTriggerFunc)(int) ) {
	int horizontalValue = analogRead(JOYSTICK_X_PIN);
	int horizontalRead = 0;
	
	if (abs(horizontalValue - JOYSTICK_HORIZONTAL_LEFT) < JOYSTICK_THRESHOLD) {
		horizontalRead = -1;
	} else if (abs(horizontalValue - JOYSTICK_HORIZONTAL_RIGHT) < JOYSTICK_THRESHOLD) {
		horizontalRead = 1;
	}

	if (horizontalRead != lastHorizontalState) {
		// we detected an event for joystick
		lastDebounceHorizontalTime = millis();
	}
	
	if (millis() - lastDebounceHorizontalTime > DEBOUNCE_JOYSTICK_DELAY) { 
		// now we take care of that event
		if (horizontalRead != horizontalState) {
			horizontalState = horizontalRead;
			if (horizontalState == -1) {	
				// LEFT
				onHorizontalTriggerFunc(-1);
			} else if (horizontalState == 1) {	
				// RIGHT
				onHorizontalTriggerFunc(1);
			}	
		}
	}
	
	lastHorizontalState = horizontalRead;
}

// this function will call onVerticalTriggerFunc whenever the joystick is inclined towards DOWN (parameter == -1) or UP (param == 1)
void checkVerticalAxis( void (*onVerticalTriggerFunc)(int) ) {
	int verticalValue = analogRead(JOYSTICK_Y_PIN);
	int verticalRead = 0;
	
	if (abs(verticalValue - JOYSTICK_VERTICAL_DOWN) < JOYSTICK_THRESHOLD) {
		verticalRead = -1;
	} else if (abs(verticalValue - JOYSTICK_VERTICAL_UP) < JOYSTICK_THRESHOLD) {
		verticalRead = 1;
	}

	if (verticalRead != lastVerticalState) {
		// we detected an event for joystick
		lastDebounceVerticalTime = millis();
	}
	
	if (millis() - lastDebounceVerticalTime > DEBOUNCE_JOYSTICK_DELAY) { 
		// now we take care of that event
		if (verticalRead != verticalState) {
			verticalState = verticalRead;
			if (verticalState == -1) {
				// DOWN
				onVerticalTriggerFunc(-1);
			} else if (verticalState == 1) {
				// UP
				onVerticalTriggerFunc(1);
			}	
		}
	}
	
	lastVerticalState = verticalRead;
}
