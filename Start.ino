int selectedOption;		// current selected option from menu: 0 = Play, 1 = Exit
int changedOption;		// tells us if we changed or not the option in the current frame

void gameStartSetup() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			ledMatrix.setLed(0, col, row, LOW);
		}
	}
	
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Tetris Main Menu");
	lcd.setCursor(2, 1);
	lcd.print(">Play");
	lcd.setCursor(10, 1);
	lcd.print("Exit");

	selectedOption = 0;
	changedOption = 0;
	horizontalState = 0;
	lastHorizontalState = 0;
}

void gameStartLoop() {
	checkButton(changeStartState);
	checkHorizontalAxis(changeMenuOption);
	checkMenu();	
}

void changeStartState() {
	if (selectedOption == 0) {
		gameState = 2;
	} else {
		gameState = 0;
	}
}

void changeMenuOption(int trigger) {
	switch(trigger) {
		case -1:	// joystick was inclined towards LEFT
			if (selectedOption > 0) {
				selectedOption--;
				changedOption = 1;
			}
			break;
		case 1:		// joystick was inclined towards RIGHT
			if (selectedOption < 1) {
				selectedOption++;
				changedOption = 1;
			}
			break;
		default:
			Serial.println("changeMenuOption(int): Why am I here?");
			delay(60000);
	}
}

void checkMenu() {
	if (changedOption) {
		changedOption = 0;
		switch(selectedOption) {
			case 0:
				lcd.setCursor(2, 1);
				lcd.print(">");
				lcd.setCursor(9, 1);
				lcd.print(" ");
				break;
			case 1:
				lcd.setCursor(2, 1);
				lcd.print(" ");
				lcd.setCursor(9, 1);
				lcd.print(">");
				break;
		}
	}
}
