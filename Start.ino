#define SCROLL_DELAY 1000
#define DEBOUNCE_BUTTON_DELAY 50
#define JOYSTICK_HORIZONTAL_MIN 1023	// the VRx read when joystick is inclined to left-most (my joystick is inverted, and also, the Y-axis is my horizontal line)
#define JOYSTICK_HORIZONTAL_MAX 0
#define JOYSTICK_HORIZONTAL_DIRECTION -1	// -1 or 1 whether joystick is inverted or not
#define JOYSTICK_CENTER 510					// center on any axis of joystick
#define JOYSTICK_THRESHOLD 200				// a value of 200 means an X value from 0 to 200 will determine the joystick to trigger an event for left movement  
#define DEBOUNCE_JOYSTICK_DELAY 50

char welcomeMessage[37];
char visibleWelcomeMessage[17];	// stores the 16 chars which are visible on the LCD
int indexWelcomeMessage;		// shows which char of welcomeMessage is at cell (15,0) of LCD
unsigned long lastScrollTime; 	// used for scrolling the welcome message
int buttonState;				// 1 = up, 0 = down button of joystick
int lastButtonState;
unsigned long lastDebounceButtonTime;	// used for the joystick's button
int selectedOption;				// current selected option from menu: 0 = Play, 1 = Exit
int changedOption;				// tells us if we changed or not the option in the current frame
int horizontalState;			// -1 = left inclined, 0 = centered, 1 = right inclined joystick
int lastHorizontalState;
unsigned long lastDebounceHorizontalTime;

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

	buttonState = 1;
	lastButtonState = 1;
	selectedOption = 0;
	changedOption = 0;
	horizontalState = 0;
	lastHorizontalState = 0;
}

void gameStartLoop() {
	//scrollWelcomeMessage();	// uncomment this for autoscrolling a title
	
	// last to execute:
	useJoystick();	
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

void useJoystick() {
	checkButton();
	checkMenu();
}

void checkButton() {
	int reading = digitalRead(JOYSTICK_BUTTON_PIN);

	if (reading != lastButtonState) {
		lastDebounceButtonTime = millis();
	}

	if (millis() - lastDebounceButtonTime > DEBOUNCE_BUTTON_DELAY) {
		if (reading != buttonState) {
			buttonState = reading;

			if (buttonState == 1) {	// button was pressed entirely (down+up)
				if (selectedOption == 0) {
					gameState = 2;
				} else {
					gameState = 0;
				}
			}
		}
	}

	lastButtonState = reading;
}

void checkMenu() {
	int horizontalValue = analogRead(JOYSTICK_Y_PIN);	// my joystick is inverted
	int horizontalRead = 0;
	
	if (abs(horizontalValue - JOYSTICK_HORIZONTAL_MIN) < JOYSTICK_THRESHOLD) {
		horizontalRead = -1;
	} else if (abs(horizontalValue - JOYSTICK_HORIZONTAL_MAX) < JOYSTICK_THRESHOLD) {
		horizontalRead = 1;
	}

	if (horizontalRead != lastHorizontalState) {
		lastDebounceHorizontalTime = millis();	// we detected an event for joystick (LEFT or RIGHT)
	}
	
	if (millis() - lastDebounceHorizontalTime > DEBOUNCE_JOYSTICK_DELAY) { // now we take care of that event
		if (horizontalRead != horizontalState) {
			horizontalState = horizontalRead;
			
			if (horizontalState == -1) {	// LEFT
				if (selectedOption > 0) {
					selectedOption--;
					changedOption = 1;
				}
			} else if (horizontalState == 1) {	// RIGHT
				if (selectedOption < 1) {
					selectedOption++;
					changedOption = 1;
				}
			}	
		}
	}
	
	lastHorizontalState = horizontalRead;
	
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
