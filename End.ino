#define LED_CHANGING_DELAY 120
#define BLINK_OPTION_DELAY 1200

unsigned long lastChangedLedTime;
int turnOnLeds;					// a boolean used for alternating the effect of turning on and off the leds
int gameMatrixLive[8][8];		// the live matrix shown on the leds
int currentLedNumber;			// the current led to check if we want to turn on or off; it takes values between 0-63
unsigned long lastBlinkTime;	// last time when the cursor option blinked
int arrowOnScreen;				// 1 = the arrow is showing for the option, 0 = it is not showing

void gameEndSetup() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("   Score: ");
	lcd.print(String(score));
	lcd.setCursor(0, 1);
	lcd.print("Try again? >YES");

	currentLedNumber = 0;
	turnOnLeds = true;
	arrowOnScreen = 1;
	lastChangedLedTime = millis();
	lastBlinkTime = millis();
	
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			gameMatrixLive[i][j] = gameMatrix[START_LINE + i][START_COLUMN + j];
		}
	}
}

void gameEndLoop() {
	int currentLedRow = currentLedNumber / 8;
	int currentLedCol = currentLedNumber % 8;

	if (currentLedNumber == 64) {
		// we finished a stage of the animation
		currentLedNumber = 0;
		turnOnLeds = !turnOnLeds;
	} else {
		if (turnOnLeds) {
			// the stage where we turn on the leds
			if (millis() - lastChangedLedTime > LED_CHANGING_DELAY) {
				// while currentLed is a led already turned on, skip to the next led to check it
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
			// the stage where we turn off the wanted leds
			if (millis() - lastChangedLedTime > LED_CHANGING_DELAY) {
				// while our currentLed is an original turned on led, skip it and check the next led
				while (currentLedNumber < 64 && gameMatrix[START_LINE + currentLedRow][START_COLUMN + currentLedCol] == 1) {
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
	checkButton(changeEndState);	
}

void changeEndState() {
	gameState = 1;
}

void blinkOption() {
	if (millis() - lastBlinkTime > BLINK_OPTION_DELAY) {
		if (arrowOnScreen) {
			lcd.setCursor(11, 1);
			lcd.print(" ");
		} else {
			lcd.setCursor(11, 1);
			lcd.print(">");
		}
		
		arrowOnScreen = !arrowOnScreen;
		lastBlinkTime = millis();
	}
}
