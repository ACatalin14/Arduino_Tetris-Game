#define LED_CHANGING_DELAY 120
#define BLINK_OPTION_DELAY 1200
#define DEBOUNCE_BUTTON_DELAY_TWO 50

unsigned long lastChangedLedTime;
int turnOnLeds;				// a boolean used for alternating the effect of turning on and off the leds
int gameMatrixLive[8][8];	// the live matrix shown on the leds
int currentLedNumber;		// the current led to check if we want to turn on or off; it takes values between 0-63
unsigned long lastBlinkTime;	// last time when the cursor option blinked
int arrowOnScreen;				// 1 = the arrow is showing for the option, 0 = it is not showing
int buttonState2;				// 1 = up, 0 = down button of joystick
int lastButtonState2;
unsigned long lastDebounceButtonTime2;	// used for the joystick's button

void gameEndSetup() {
	lcd.clear();
	//lcd.home();
	lcd.setCursor(3, 0);
	lcd.print("GAME OVER!");
	lcd.setCursor(0, 1);
	lcd.print("Try again? >YES");

	arrowOnScreen = 1;
	lastChangedLedTime = millis();
	lastBlinkTime = millis();
	
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			gameMatrixLive[i][j] = gameMatrix[START_LINE + i][j];
		}
	}

	currentLedNumber = 0;
	turnOnLeds = true;

	buttonState2 = 1;
	lastButtonState2 = 1;
}

void gameEndLoop() {
	int currentLedRow = currentLedNumber / 8;
	int currentLedCol = currentLedNumber % 8;

	if (currentLedNumber == 64) {	// we finished a stage of the animation
		currentLedNumber = 0;
		turnOnLeds = !turnOnLeds;
	} else {
		if (turnOnLeds) {
			if (millis() - lastChangedLedTime > LED_CHANGING_DELAY) {
				while (currentLedNumber < 64 && gameMatrixLive[currentLedRow][currentLedCol] == 1) {
					currentLedNumber++;
					currentLedRow = currentLedNumber / 8;
					currentLedCol = currentLedNumber % 8;
				}
				if (currentLedNumber < 64) {
					gameMatrixLive[currentLedRow][currentLedCol] = 1;
					ledMatrix.setLed(0, currentLedCol, currentLedRow, HIGH);
					currentLedNumber++;
					lastChangedLedTime = millis();	
				}
			}
		} else {
			if (millis() - lastChangedLedTime > LED_CHANGING_DELAY) {
				while (currentLedNumber < 64 && gameMatrix[START_LINE + currentLedRow][currentLedCol] == 1) {
					currentLedNumber++;
					currentLedRow = currentLedNumber / 8;
					currentLedCol = currentLedNumber % 8;
				}
				if (currentLedNumber < 64) {
					gameMatrixLive[currentLedRow][currentLedCol] = 0;
					ledMatrix.setLed(0, currentLedCol, currentLedRow, LOW);
					currentLedNumber++;
					lastChangedLedTime = millis();	
				}
			}
		}
	}

	blinkOption();
	checkButton2();	
}

void blinkOption() {
	if (millis() - lastBlinkTime > BLINK_OPTION_DELAY) {
		if (arrowOnScreen) {
			lcd.setCursor(11, 1);
			lcd.print(" ");
			arrowOnScreen = !arrowOnScreen;
		} else {
			lcd.setCursor(11, 1);
			lcd.print(">");
			arrowOnScreen = !arrowOnScreen;
		}
		lastBlinkTime = millis();
	}
}

void checkButton2() {
	int reading = digitalRead(JOYSTICK_BUTTON_PIN);

	if (reading != lastButtonState2) {
		lastDebounceButtonTime2 = millis();
	}

	if (millis() - lastDebounceButtonTime2 > DEBOUNCE_BUTTON_DELAY_TWO) {
		if (reading != buttonState2) {
			buttonState2 = reading;

			if (buttonState2 == 1) {	// button was pressed entirely (down+up)
				gameState = 1;
			}
		}
	}

	lastButtonState2 = reading;
}
