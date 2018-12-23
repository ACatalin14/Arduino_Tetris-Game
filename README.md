# Arduino_Tetris-Game
This is a small arduino course project made for an 8x8 led matrix using [the Arduino IDE][Arduino Software]. In order to create my own **Tetris**, as a reference, I used [this site][Tetris Wiki] to get some guidelines.

If you want to see a more in-depth analysis of the code, you can visit the [Documentation Wiki page][Documentation].  
Also, to get a feel for this project, you can check [this video][Gameplay Video] I made myself.

## Required physical components

* Arduino UNO Board;
* 16x2 LCD;
* 220 _Ω_ and 100k _Ω_ resistors;
* a thumb joystick;
* MAX7219 Driver;
* SH1388ASR B LED Matrix (this is the one I used);
* 104 _pF_ ceramic capacitor;
* 10 _µF_ 50 _V_ capacitor;
* USB cable;
* many _many_ **many** wires;

## Game Rules

You are given an 8x8 matrix where tetris pieces are falling from above. There are 7 types of pieces in the original game whose shapes and names you can see [here][Tetris Shapes], but in my version I have added one more shape - the _dot_ shape (a 1x1 block);  
A piece is spawned in the upper bound of the matrix, and each iteration, it's falling down until it reaches the floor or another piece underneath. If the descended piece creates a full row on the matrix, then the row is burned, and all the rows above it will descend one unit;  
You can perform the following operations on the descending piece:
* moving horizontally, by using the joystick's horizontal axis;
* clockwise rotation, by presing the joystick's button;
* soft drop (increase its descending speed for bonus score points), by holding the joystick in the _DOWN_ position on the vertical axis;
* hard drop (force the piece to fall directly where it's supposed to go), by moving the joystick once in the _UP_ position on the vertical axis;

If the piece can't descend anymore from the moment it spawned, then it's **Game Over**.  

As any arcade game, this one has **level** and **score** functionalities. The game starts at **Level 1**. Every time you burn 5 rows, the game takes you to the next level. Each level has its own difficulty (the piece's descending speed) and after **Level 5**, each one has the same descending speed. The score starts from **0** and by gaining score points you can see at the end of the game how much progress you have made. Here is a list with all possible ways to gain score points (_**SP**_):
* _40 * currentLevel **SP**_, by burning **1 row** using one piece;
* _100 * currentLevel **SP**_, by burning **2 rows** using one piece;
* _300 * currentLevel **SP**_, by burning **3 rows** using one piece;
* _1200 * currentLevel **SP**_, by burning **4 rows** using one piece;
* _2 * rows **SP**_, by making a _hard drop_, where _rows_ = number of descended rows from where _the hard drop_ was made;
* _1 * rows **SP**_, by making a _soft drop_, where _rows_ = number of descended rows while _the soft drop_ was active.


[Arduino Software]: https://www.arduino.cc/en/main/software
[Tetris Wiki]: http://tetris.wikia.com/wiki/SRS
[Documentation]: https://github.com/ACatalin14/Arduino_Tetris-Game/wiki/Documentation
[Gameplay Video]: https://youtu.be/GOFtpmN9A7Y
[Tetris Shapes]: http://tetris.wikia.com/wiki/Tetromino