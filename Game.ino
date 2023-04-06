
#include <MD_MAX72xx.h>

#include <RH_ASK.h> //Library was changed so Rx pin is 9
#include <SPI.h>

#define MAX_DEVICES 1 // 1 blocks
#define CS_PIN 3

MD_MAX72XX ledMat = MD_MAX72XX(MD_MAX72XX::GENERIC_HW, 3, 1);
RH_ASK driver;

//Hold values to represent the players chips and where they are located
uint8_t board[8][8] = 
{
  { 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0}
};


//Custom boards used for win screen

// P1
uint8_t p1_board[8][8] = 
{
  { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1},
  { 1 , 0 , 0 , 1 , 0 , 0 , 0 , 0},
  { 1 , 0 , 0 , 1 , 0 , 0 , 0 , 0},
  { 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0},
  { 0 , 1 , 1 , 0 , 0 , 0 , 1 , 1},
  { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1},
  { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1},
  { 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1}
};

// P2
uint8_t p2_board[8][8] = 
{
  { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1},
  { 1 , 0 , 0 , 1 , 0 , 0 , 0 , 0},
  { 1 , 0 , 0 , 1 , 0 , 0 , 0 , 0},
  { 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0},
  { 1 , 1 , 0 , 0 , 1 , 1 , 1 , 1},
  { 1 , 1 , 0 , 1 , 1 , 1 , 1 , 1},
  { 1 , 1 , 0 , 1 , 1 , 0 , 1 , 1},
  { 1 , 1 , 1 , 1 , 0 , 0 , 1 , 1}
};


// W
uint8_t w_board[8][8] = 
{
  { 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0},
  { 0 , 0 , 1 , 1 , 1 , 1 , 0 , 0},
  { 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1},
  { 0 , 0 , 0 , 1 , 1 , 1 , 1 , 0},
  { 0 , 0 , 0 , 1 , 1 , 1 , 1 , 0},
  { 0 , 0 , 0 , 0 , 0 , 1 , 1 , 1},
  { 0 , 0 , 1 , 1 , 1 , 1 , 0 , 0},
  { 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0}
};




bool player_turn = false; //False is player 1 turn (solid LED) and True is player 2 turn (blinking LED)
uint8_t blink_delay = 200; //delay between when p2 "pieces" turn off and on
//*You can adjust the above value to make the pieces blink slower or faster but if blink_delay is too high you may
// have difficulties getting readings from the controller*
uint8_t spot = 0; //current column the user is hovering
uint8_t buf[1]; //buffer for RX transmission

void setup() {
  Serial.begin(9600);
  driver.init();
  ledMat.begin();
}

//Resets the board
void resetBoard(uint8_t del = 0){
  //if del > 0 we turn on all the LEDs before doing the clear
  if(del > 0){
    for(uint8_t i = 0; i < 7; i++){
      for(uint8_t j = 1; j < 8; j++){
        ledMat.setPoint(i,j,true);
      }
    }
  }
  for(uint8_t i = 0; i < 8; i++){
    for(uint8_t j = 0; j < 8; j++){
       ledMat.setPoint(i,j,false);
       board[i][j] = 0;
       delay(del);
    }
  }
  delay(del*4);
  if(player_turn){
    board[0][0] = 2;
  }
  else{
    board[0][0] = 1;
  }
  spot = 0;
  uint8_t len = 1;
  //Calls this to clear last controller input
  driver.recv(buf, &len);
}

//Starts if board is full or game has been finished
void gameOver(bool gameWon, uint8_t show_pieces[8] = {0}){
  if(gameWon){
    delay(250);
    //reset board for winners show
    resetBoard();
    //show winning pieces flashing
    ledMat.setPoint(show_pieces[0],show_pieces[1],true);
    ledMat.setPoint(show_pieces[2],show_pieces[3],true);
    ledMat.setPoint(show_pieces[4],show_pieces[5],true);
    ledMat.setPoint(show_pieces[6],show_pieces[7],true);
    delay(2000);
    ledMat.setPoint(show_pieces[0],show_pieces[1],false);
    ledMat.setPoint(show_pieces[2],show_pieces[3],false);
    ledMat.setPoint(show_pieces[4],show_pieces[5],false);
    ledMat.setPoint(show_pieces[6],show_pieces[7],false);
    delay(500);
    //blink 3 times
    for(uint8_t i = 0; i < 3; i++){
      ledMat.setPoint(show_pieces[0],show_pieces[1],true);
      ledMat.setPoint(show_pieces[2],show_pieces[3],true);
      ledMat.setPoint(show_pieces[4],show_pieces[5],true);
      ledMat.setPoint(show_pieces[6],show_pieces[7],true);
      delay(500);
      ledMat.setPoint(show_pieces[0],show_pieces[1],false);
      ledMat.setPoint(show_pieces[2],show_pieces[3],false);
      ledMat.setPoint(show_pieces[4],show_pieces[5],false);
      ledMat.setPoint(show_pieces[6],show_pieces[7],false);
      delay(500);  
    }
    
    //if player 1's won
    if(player_turn){
      //Update LED Matrix to P1
      for(uint8_t i = 0; i < 8; i++){
        for(uint8_t j = 0; j < 8; j++){
          if(p1_board[i][j] > 0){
            ledMat.setPoint(i,j,true);
          }
          else{
            ledMat.setPoint(i,j,false);
          }
        }
      }
    }
    else{
      //Update LED Matrix to P2
      for(uint8_t i = 0; i < 8; i++){
        for(uint8_t j = 0; j < 8; j++){
          if(p2_board[i][j] > 0){
            ledMat.setPoint(i,j,true);
          }
          else{
            ledMat.setPoint(i,j,false);
          }
        }
      }
    }
    delay(3000);
    //Update LED Matrix to W
    for(uint8_t i = 0; i < 8; i++){
      for(uint8_t j = 0; j < 8; j++){
        if(w_board[i][j] > 0){
          ledMat.setPoint(i,j,true);
        }
        else{
          ledMat.setPoint(i,j,false);
        }
      }
    }
    delay(3000);
    //resets for next game
    resetBoard();
  }
  else{
    //resets for new game (sets delay for interesting visual clear)
    resetBoard(100);
  }
  
  
}



//Checks to see if there is a connect 4 present
//Calculates the number of repeating player "pieces" in a certain direction (like diagonally left down to up right)
//and sees if it adds to 4. It will only look at the spot given for a connect 4.
//If y value is for the top row (y = 1) then it will check if the board is full after checking for win.

void checkForFour(uint8_t x, uint8_t y){
  
  uint8_t player = board[x][y]; //number we are checking for
  uint8_t count = 1; //number of pieces in a row counted
  uint8_t winning_pieces[8]; //holds the x and y cords for the winning spots
  winning_pieces[0] = x;
  winning_pieces[1] = y;
  //Checking left to right
  
  //Left
  uint8_t copyX = x - 1;
  while(copyX >= 0){
    if(board[copyX][y] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = y;
      }
      count ++;
    }
    else{
      break;
    }
    copyX --;
  }
  
  //Right
  copyX = x + 1;
  while(copyX <= 6){
    if(board[copyX][y] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = y;
      }
      count ++;
    }
    else{
      break;
    }
    copyX ++;
  }
  Serial.print("Left to Right Count: ");
  Serial.println(count);
  if(count >= 4){
    gameOver(true,winning_pieces);
  }
  count = 1;


  //Checking up to down
  
  //Up
  uint8_t copyY = y - 1;
  while(copyY >= 1){
    if(board[x][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = x;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyY --;
  }
  
  //Down
  copyY = y + 1;
  while(copyX <= 7){
    if(board[x][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = x;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyY ++;
  }
  Serial.print("Up to Down Count: ");
  Serial.println(count);
  if(count >= 4){
    gameOver(true,winning_pieces);
  }
  count = 1;

  
  //Checking bottom-left to top-right
  
  //Bottom-left
  copyX = x - 1;
  copyY = y + 1;
  while(copyX >= 0 && copyY <= 7){
    if(board[copyX][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyX --;
    copyY ++;
  }
  
  //Top-right
  copyX = x + 1;
  copyY = y - 1;
  while(copyX <= 6 && copyY >= 1){
    if(board[copyX][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyX ++;
    copyY --;
  }
  Serial.print("Bottom-left to Top-right Count: ");
  Serial.println(count);
  if(count >= 4){
    gameOver(true,winning_pieces);
  }
  count = 1;



  //Checking top-left to bottom-right

  //Top-left
  copyX = x - 1;
  copyY = y - 1;
  while(copyX >= 0 && copyY >= 1){
    if(board[copyX][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyX --;
    copyY --;
  }
  
  //bottom-right
  copyX = x + 1;
  copyY = y + 1;
  while(copyX <= 6 && copyY <= 7){
    if(board[copyX][copyY] == player){
      if(count < 4){
        winning_pieces[count*2] = copyX;
        winning_pieces[count*2 + 1] = copyY;
      }
      count ++;
    }
    else{
      break;
    }
    copyX ++;
    copyY ++;
  }
  Serial.print("Top-left to Bottom-right Count: ");
  Serial.println(count);
  if(count >= 4){
    gameOver(true,winning_pieces);
  }
  count = 1;

  //checking if board is full
  bool check = true;
  for(uint8_t i = 0; i <=6; i++){
    if(board[i][1] == 0){
      check = false;
    }
  }
  if(check){
    gameOver(false);
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  uint8_t len = 1;
  if (driver.recv(buf, &len)) {
    char letter= buf[0];
    Serial.println(letter);
    if(letter == 'L' and spot != 0){
      Serial.print("Left from position ");
      Serial.println(spot);
      board[spot][0] = 0;
      ledMat.setPoint(spot,0,false);
      spot --;
      if(player_turn){
        board[spot][0] = 2;
      }
      else{
        board[spot][0] = 1;
      }
    }
    else if(letter == 'R' and spot != 6){
      Serial.print("Right from position ");
      Serial.println(spot);
      board[spot][0] = 0;
      ledMat.setPoint(spot,0,false);
      spot ++;
      if(player_turn){
        board[spot][0] = 2;
      }
      else{
        board[spot][0] = 1;
      }
    }
    else if(letter == 'B' and board[spot][1] == 0){
      for(uint8_t i = 7; i > 0; i--){
        if(board[spot][i] == 0){
          if(player_turn){
            board[spot][i] = 2;
            board[spot][0] = 1;
            player_turn = false;
          }
          else{
            board[spot][i] = 1;
            board[spot][0] = 2;
            player_turn = true;
          }
          checkForFour(spot, i);
          break;
        }
      }
    }
  }
  //Update LED Matrix
  for(uint8_t i = 0; i < 8; i++){
    for(uint8_t j = 0; j < 8; j++){
      if(board[i][j] > 0){
        ledMat.setPoint(i,j,true);
      }
    }
  }
  delay(blink_delay);
  for(uint8_t i = 0; i < 8; i++){
    for(uint8_t j = 0; j < 8; j++){
      if(board[i][j] > 1){
        ledMat.setPoint(i,j,false);
      }
    }
  }
  delay(blink_delay);
}
