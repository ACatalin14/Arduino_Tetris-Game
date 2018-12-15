#define SCROLL_DELAY 1000

char welcomeMessage[37];
char visibleWelcomeMessage[17];	// stores the 16 chars which are visible on the LCD
unsigned long lastScrollTime; 	// used for scrolling the welcome message
int selectedOption;				// current selected option from menu: 0 = Play, 1 = Exit
int changedOption;				// tells us if we changed or not the option in the current frame

void gameStartSetup() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			ledMatrix.setLed(0, col, row, 0);
		}
	}
	
	lcd.clear();
	lastScrollTime = millis();
	strcpy(welcomeMessage, "  Welcome to Catalin's Tetris Game  ");

	lcd.home();
	//lcd.print("  Welcome to Cat");	// uncomment this for autoscrolling a title
	lcd.print("Tetris Main Menu");		// comment this for autoscrolling a title
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
	//scrollWelcomeMessage();	// uncomment this for autoscrolling a title
	
	// last to execute:
	checkJoystick();
	checkMenu();	
}

void scrollWelcomeMessage() {
	if (millis() - lastScrollTime >= SCROLL_DELAY) {
		char firstChar = welcomeMessage[0];
		
		for (int i = 0; i < strlen(welcomeMessage) - 1; i++) {
			welcomeMessage[i] = welcomeMessage[i + 1];
		}
		welcomeMessage[ strlen(welcomeMessage) - 1 ] = firstChar;
		
		for (int i = 0; i < 16; i++) {
			visibleWelcomeMessage[i] = welcomeMessage[i];
		}
		visibleWelcomeMessage[16] = 0;
		
		lcd.home();
		lcd.print(visibleWelcomeMessage);

		lastScrollTime = millis();
	}
}

void checkJoystick() {
	checkButton(changeStartState);
	checkHorizontalAxis(changeMenuOption);
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
		case -1:	// joystick was inclined at LEFT
			if (selectedOption > 0) {
				selectedOption--;
				changedOption = 1;
			}
			break;
		case 1:		// joystick was inclined at RIGHT
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
