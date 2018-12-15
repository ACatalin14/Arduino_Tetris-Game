#define NUMBER_OF_PIECES 8
#define STARTING_SPEED 500		// lower = faster

int score;		// current score
int level;		// current level
int descendingDelay;
unsigned long lastDescendingTime;	// last time we updated the matrix for descending a piece

struct Piece {
	int id;
	int shape[4][4];	// the current shape of the piece
	int boundingBoxSize;// length of the square's edge of the boundig box (a number between 1 and 4)
	int row, column;	// row/column between 0 and 7 of the boundigBox's top-left corner
	int hasDescended;	// true when current piece descended completely
	int rowsDescended;	// number of rows descended by the piece; when it is 0, it's game over
};

Piece currentPiece;

int patterns[8][4][4] = {
	// the dot piece:
	{ 
		{1, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},
	// the I-piece:
	{ 
		{0, 0, 0, 0},
		{1, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0} 
	},
	// the J-piece:
	{ 
		{1, 0, 0, 0},
	 	{1, 1, 1, 0},
	 	{0, 0, 0, 0},
	 	{0, 0, 0, 0}
	},
	// the L-piece
	{
		{0, 0, 1, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},
	// the O-piece:
	{
		{1, 1, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},
	// the S-piece:
	{
		{0, 1, 1, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},
	// the T-piece:
	{
		{0, 1, 0, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},
	// the Z-piece
	{
		{1, 1, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};

void gamePlaySetup() {
	// clear the gameMatrix
	for (int row = 0; row < FINISH_LINE; row++) {
		for (int col = START_COLUMN; col < FINISH_COLUMN; col++) {
			gameMatrix[row][col] = 0;
		}
	}
	
	lcd.clear();
	lcd.home();
	lcd.print("Level: 1");
	lcd.setCursor(0, 1);
	lcd.print("Score: 0");

	currentPiece.id = -1;
	score = 0;
	level = 1;
	descendingDelay = STARTING_SPEED;
	lastDescendingTime = millis();
}

void gamePlayLoop() {
	if (currentPiece.id == -1) {
		createPiece();
		lastDescendingTime = millis();
	}

	//checkHorizontalAxis(moveHorizontally);

	if (millis() - lastDescendingTime > descendingDelay) {
		if (checkToDescend()) {
			descendPiece();
		} else {
			currentPiece.hasDescended = true;
		}

		if (currentPiece.hasDescended) {	// getting ready to generate another piece
			currentPiece.hasDescended = false;
			currentPiece.id = -1;
			
			if (currentPiece.rowsDescended == 0) {	// check for game over
				gameState = 3;
			} else {
				currentPiece.rowsDescended = 0;
			}

			// check for score increasing
		}

		lastDescendingTime = millis();
	}

	drawMatrix();
}

void drawMatrix() {
//	for (int row = START_LINE; row < FINISH_LINE; row++) {
//		for (int col = START_COLUMN; col < FINISH_COLUMN; col++) {
//			ledMatrix.setLed(0, col - START_COLUMN, row - START_LINE, gameMatrix[row][col]);
//		}
//	}

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			ledMatrix.setLed(0, col, row, gameMatrix[START_LINE + row][START_COLUMN + col]);
		}
	}
}

void createPiece() {
	currentPiece.id = random(NUMBER_OF_PIECES);
	currentPiece.hasDescended = false;
	currentPiece.rowsDescended = 0;
	assign(currentPiece.shape, patterns[ currentPiece.id ]);
	
	// setting the current row and column of the piece's bounding box
	switch (currentPiece.id) {
		case 0:	// the dot piece
			currentPiece.boundingBoxSize = 1;
			currentPiece.row = START_LINE - 1;
			currentPiece.column = START_COLUMN + 3;
			break;
		case 1:	// the I-piece
			currentPiece.boundingBoxSize = 4;
			currentPiece.row = START_LINE - 2;
			currentPiece.column = START_COLUMN + 2;
			break;
		case 4:	// the O-piece
			currentPiece.boundingBoxSize = 2;
			currentPiece.row = START_LINE - 2;
			currentPiece.column = START_COLUMN + 3;
			break;
		case 2:	// the J-piece
		case 3:	// the L-piece
		case 5:	// the S-piece
		case 6:	// the T-piece
		case 7:	// the Z-piece
			currentPiece.boundingBoxSize = 3;
			currentPiece.row = START_LINE - 2;
			currentPiece.column = START_COLUMN + 2;
	}

	//setLedPattern(1);
	setGameMatrixPattern(1);
}

// returns true if we are able to descend the current piece, and false the other case
int checkToDescend() {
	for (int col = 0; col < currentPiece.boundingBoxSize; col++) {
		// try to find the lowest dot in the structure of our piece on the column col of the bounding box
		int lowestDotRow;
		for (lowestDotRow = currentPiece.boundingBoxSize - 1; lowestDotRow >= 0; lowestDotRow--) {
			if (currentPiece.shape[lowestDotRow][col] == 1) {
				break;
			}
		}

		// now check if there is an object right under our piece on the column col
		if (lowestDotRow != -1) {
			int interestRow = currentPiece.row + lowestDotRow + 1;
			int interestCol = currentPiece.column + col;
			if (interestRow <= FINISH_LINE && gameMatrix[interestRow][interestCol] == 1) {
				// we can't descend the piece anymore
				return false;
			}
		}
	}
	return true;
}

void descendPiece() {
	setGameMatrixPattern(0);
	currentPiece.row++;
	setGameMatrixPattern(1);
	currentPiece.rowsDescended++;
}

// draw or erase the shape of the current piece at its coords on the game matrix
void setGameMatrixPattern(int option) {
	for (int row = 0; row < currentPiece.boundingBoxSize; row++) {
		for (int col = 0; col < currentPiece.boundingBoxSize; col++) {
			if (currentPiece.shape[row][col] == 1) {
				gameMatrix[currentPiece.row + row][currentPiece.column + col] = option;
			}
		}
	}
}

// movement (-1 or 1) tells us where the player wants to move the piece, to the left (-1) or to the right (1)
void moveHorizontally(int movement) {
	int colExtremeDot;	// the column where lies the most extreme point (on the horizontal line to the left/right) in the structure of our piece
	int startingCol;
	switch (movement) {
		case -1:		// move towards LEFT
			startingCol = 0;
			break;
		case 1:			// move piece towards RIGHT
			startingCol = currentPiece.boundingBoxSize - 1;
			break;
	}

	// trying to treat both cases using just once the 2 nested for-loops
	for (int row = 0; row < currentPiece.boundingBoxSize; row++) {
		int mostExtremeDotColumn;
		for (int mostExtremeDotColumn = startingCol; abs(mostExtremeDotColumn - startingCol) < currentPiece.boundingBoxSize; mostExtremeDotColumn -= movement) {
			if (currentPiece.shape[row][mostExtremeDotColumn] == 1) {
				break;
			}
		}

		if (mostExtremeDotColumn >= 0 && mostExtremeDotColumn < currentPiece.boundingBoxSize) { // our piece has a dot on the current row
			// check if there is an object beside our piece at current row
			int interestColumn = currentPiece.column + mostExtremeDotColumn + movement;
			int interestRow = currentPiece.row + row;

			//if (interestRow 
			
		}
	}
}

void assign(int a[4][4], int b[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			a[i][j] = b[i][j];
		}
	}
}

// returns 0 is setting was succesful and -1 otherwise
//int setLedIfPossible(int col, int row, int setting) {
//	if (col >= 0 && col < 8 && 
//		row >= 0 && row < 8 && 
//		(setting == LOW || setting == HIGH)	) {
//		ledMatrix.setLed(0, col, row, setting);
//		return 0;
//	}
//	return -1;
//}
