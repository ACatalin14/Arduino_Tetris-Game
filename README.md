# Arduino_Tetris-Game
This is a small arduino course project which uses an Arduino UNO Board, an 8x8 matrix, a thumb joystick and a 2x16 LCD. In order to create my own **Tetris**, as a reference, I used [this site][Tetris Wiki] to get some guidelines.

The project is divided in 4 _.ino_ files:
* **Tetris.ino**: the main file which makes the game run as a whole;
* **Start.ino**: it takes the first part of the game, where the player must choose and option from the main menu printed on the LCD (_Play_ or _Exit_);
* **Play.ino**: it contains the game itself, all the logic needed to run the gameplay;
* **End.ino**: it takes the ending part of a game where the player is asked to give an input in order to return to the main menu, while a nice animation is played on the matrix.

## The main file _Tetris.ino_

### Used Libraries
* [**LiquidCrystal.h:**][LCD Library] used for working with the LCD;
* [**LedControl.h:**][LedControl Library] used for working with an 8x8 matrix;

### Constants
Arduino Pins:
* _LCD_CONTRAST_PIN_: the pin which controls the LCD's contrast;
* _JOYSTICK_X_PIN_: the pin which controls the joystick's VRx;
* _JOYSTICK_Y_PIN_: the pin which controls the joystick's VRy;
* _JOYSTICK_BUTTON_PIN_: the pin which controls the joystick's SW;  

Matrix related:
* _START_LINE (2)_: the corresponding line number for line #0 from our physical matrix to our _gameMatrix_ global variable;
* _FINISH_LINE (10)_: the corresponding line number for the inexistent line #8 from our physical matrix to our _gameMatrix_ global variable;
* _START_COLUMN (1)_: the corresponding column number for column #0 from our physical matrix to our _gameMatrix_ global variable;
* _FINISH_COLUMN (9)_: the corresponding column number for the inexistent column #8 from our physical matrix to our _gameMatrix_ global variable;

Input Settings:
* _DEBOUNCE_BUTTON_DELAY_: time interval in ms which tells us how rarely to check for input from joystick's button;
* _DEBOUNCE_JOYSTICK_DELAY_: time interval in ms which tells us how rarely to check for input from joystick's VRx or VRy;
* _JOYSTICK_HORIZONTAL_LEFT (1023)_: the VRx value when joystick is inclined to left-most (in my case, my joystick is reverted);
* _JOYSTICK_HORIZONTAL_RIGHT (0)_: the VRx value when joystick is inclined to right-most (again, my joystick is reverted);
* _JOYSTICK_VERTICAL_UP (0)_: the VRy value when joystick is inclined to up-most (my joystick is not like _the others_);
* _JOYSTICK_VERTICAL_DOWN (1023)_: the VRy value when joystick is inclined to down-most (why did I have to revert my joystick, again? Maybe nobody reads this);
* _JOYSTICK_THRESHOLD_: the sensitiveness of the joystick on the horizontal and vertical axis (needs a value between 0 and 1023).

### Global variables
* _gameState_: 
  * tells us the current state of the game, taking one of the values below:
    * **1**, the Start state, showing the _Main Menu_;
    * **2**, the Play state, where the _gameplay_ is actually happening;
    * **3**, the End state, where the _game is over_ and we ask for an input so that we can return to the _Main Menu_;
  * it dictates which loop functions to execute in the main loop of the program using a _switch_ block
* _lastGameState_:
  * it helps us at finding out if _gameState_ changed at any moment;
  * it will always keep the game state that was 1ms before;
* _score_:
  * the player's score at gameplay and printed at the end stage;
  * it is declared globally in this file so that it can be seen for sure by **Play.ino** and **End.ino**;
* _buttonState_:
  * tells us the current state of the joystick's button;
  * can take one of the values below:
    * **1**, the button is _UP_;
    * **0**, the button is pressed _DOWN_;
* _lastButtonState_: keeps the button state that was 1ms before;
* _lastDebouneButtonTime_: last time when the user changed _buttonState_;
* _horizontalState_:
  * tells us the current state of the joystick's VRx;
  * can take one of the values below:
    * **-1**, the joystick is inclined towards _LEFT_;
    * **0**, the joystick is not inclined on the horizontal axis;
    * **1**, the joystick is inclined towards _RIGHT_;
* _lastHorizontalState_: keeps the horizontal state that was 1ms before;
* _lastDebounceHorizontalTime_: last time when the user changed _horizontalState_;
* _verticalState_:
  * tells us the current state of the joystick's VRy;
  * can take one of the values below:
    * **-1**, the joystick is inclined towards _DOWN_;
    * **0**, the joystick is not inclined on the vertical axis;
    * **1**, the joystick is inclined towards _UP_;
* _lastVerticalState_: keeps the vertical state that was 1ms before;
* _lastDebounceVerticalTime_: last time when the user changed _verticalState_;
* _lcd_: this is our liquid crystal display;
* _ledMatrix_: this is our 8x8 physical matrix;
* _gameMatrix_: 
  * this is the matrix on which we are going to make all the changes needed so that the game can work properly
  * it is defined as follows:

        int gameMatrix[11][10] = {
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // START_LINE
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},	
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1} // FINISH_LINE
                ^                       ^
                START_COLUMN            FINISH_COLUMN
        };
  * you can see I have added **2 extra lines** above the start line (that being the place where new pieces of Tetris can spawn) and **one extra line** at the bottom representing the _"floor"_ of the matrix where all pieces descend; also there are **two walls** so that pieces can't exit the matrix by moving them to the left/right.

### Functions

* _setup()_: initializes the global variables with the right values and sets the seed for the game's randomness;
* _loop()_:
  * the main loop of the project;
  * based on the _gameState_ value, it chooses which stage loop to execute (_gameStartLoop()_, _gamePlayLoop()_, _gameEndLoop()_ );
  * if _gameState_ changed at any moment, then a _setup function_ will be executed first (_gameStartSetup()_, _gamePlaySetup()_, _gameEndSetup()_ ) and then the corresponding _loop function_;
* _lcdSetup()_: initializes _lcd_;
* _joystickSetup()_: initializes the joystick;
* _ledMatrixSetup()_: initializes _ledMatrix_;
* _gameClose()_: it is executed when the player selected _Exit_ from the _Main Menu_, and he/she "closed" the game;
* _checkButton( void (*onClickFunc)() )_:
  * each time we enter this function, we take a _reading_ from the joystick's button, and we compare it to the last state we read before
  * in case we found a different reading from the last time, **and** at least _DEBOUNCE_BUTTON_DELAY_ ms passed since the last click, we can check if the button was pressed entirely;
  * if yes, we will execute the function given as a parameter;
* _checkHorizontalAxis( void (*onHorizontalTriggerFunc)(int) )_:
  * here we apply the same principle as above, but this time we will keep track of two states, instead of one, where we will give _onHorizontalTriggerFunc(int)_ a parameter depending on the state we are at a given moment;
  * if we detected a _LEFT_ event (the joystick was inclined towards _LEFT_), we will call _onHorizontalTriggerFunc(-1)_;
  * if we detected a _RIGHT_ event (the joystick was inclined towards _RIGHT_), we will call _onHorizontalTriggerFunc(1)_;
* _checkVerticalAxis( void (*onVerticalTriggerFunc)(int) )_:
  * this function is the same as the one above but applied for the **vertical** axis, where:
    * **-1** = DOWN;
    * **1** = UP.
  
        
[Tetris Wiki]: <http://tetris.wikia.com/wiki/SRS>
[LedControl Library]: https://github.com/wayoda/LedControl
[LCD Library]: https://www.arduinolibraries.info/libraries/liquid-crystal