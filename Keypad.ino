

#define USE_ALB_LCD_I2C // LCD Library
#include <Keypad.h>
#include "ArduinoLearningBoard.h"

ALB_LCD_I2C lcd; // Instantiating with no arguments defaults to Address 0x27, 16 Columns, 2 Rows

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

const int led = 13;

void setup(){

  Serial.begin(9600);
  lcd.init(); // Initialize the LCD
  lcd.clear(); // Clear the LCD (set cursor to 0)
  lcd.backlight(); // Turn on the LCD backlight

  pinMode(led,OUTPUT); //Set LED pin to output
  
}

String password = "1234"; //Current Password
String passwordConfig = "000000"; //Password to change current password

String input = ""; //Current password attempt by user
String inputHidden = ""; //Display user input as * on LCD

String msgFL = ""; //First line to display on LCD
String msgSL = ""; //Second line to display on LCD

bool inputComplete = false; //When user finishes password attempt with #
bool inputChange = false; //Whether the user is in config
bool inputNewPassword = false; //Whether user is inputting first new password

bool confirmOldPassword = false; //Whether confirming old password was correct
bool confirmNewPassword = false; //Whether user is inputting new password again

String newPassword = ""; //First input of new password
String newPasswordConfirmation = ""; //Second input of new password

int timeout = 0; //Timer that removes user input after x amount of seconds
const int timeoutMax = 2000;  
void loop(){
  
  if (input == "") { //Display respective info if user hasn't input anything yet

    //Regular input instructions
    msgFL = "Enter PIN";
    msgSL = "End with #";
      
    if (confirmOldPassword) { //When user has inputted config password
      msgFL = "Confirm old PIN";  
      msgSL = "to change";
    } 
    if (inputNewPassword) { //When user has confirmed old password in config screen
      msgFL = "Enter new PIN"; 
      msgSL = "finish with #";
    }
    if (confirmNewPassword){ //When user has entered first new password in config screen
      msgFL = "Re-enter new PIN"; 
      msgSL = "to confirm";
    } 

    //Prints the first and second lines on LCD
    lcd.setCursor(0,0);
    lcd.print(msgFL);
    
    lcd.setCursor(0,1);
    lcd.print(msgSL);
  } else {
    lcd.setCursor(0,0);

    //Creates a hidden password to display that corresponds with actual input's length
    inputHidden = "";
    for(int i = 0; i < input.length(); i ++) {
      inputHidden += "*";
    }

    lcd.print(inputHidden); //Prints hidden input to LCD

    //Erase input if timer reaches above 10 seconds roughly
    if (timeout > timeoutMax) {
      timeout = 0;
      input = "";
    }
    timeout ++; //Increment timer
  }
  
  char customKey = customKeypad.getKey();
  
  if (customKey){
    timeout = 0; //Reset timeout every time a key is pressed
    if (customKey == '#') {
      if (input.length() > 0) {
        inputComplete = true; //Submit password attempt when # is pressed with at least 1 letter
      }
    } else if (customKey == '*') { 
      //Backspace
      if (input.length() > 0) {
        input = input.substring(0,input.length()-1);
        lcd.clear();
      }
    } else {
      //Add every other type of keypress to our password attempt
      input += customKey;
      lcd.clear();
    }
    
  }
  

  if (inputComplete) {

    newPasswordCheck:
    
    //Set new password once both inputs are complete
    if (newPassword != "" && newPasswordConfirmation != "") {
      if (newPassword == newPasswordConfirmation && newPassword != passwordConfig) {
        password = newPassword;
        lcd.clear();
        msgFL = "NEW PIN SET!";
        msgSL = "";
      } else if (newPassword == newPasswordConfirmation && newPassword == passwordConfig) {
        msgFL = "NEW PIN CANNOT";
        msgSL = "BE CONFIG PIN";
      } else {
        msgFL = "PINS DO NOT";
        msgSL = "MATCH";
      }

      //Print corresponding outcome
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(msgFL);

      lcd.setCursor(0,1);
      lcd.print(msgSL);
      
      delay(3000);
      
      //Reset flag variables
      confirmOldPassword = false;
      confirmNewPassword = false;
      inputNewPassword = false;
      newPassword = "";
      newPasswordConfirmation = "";

      lcd.clear();
      input = "";
      inputComplete = false;
      
    } else {
      if (inputNewPassword) {
          newPassword = input;
          inputNewPassword = false;
          confirmNewPassword = true;
      } else if (confirmNewPassword){
          newPasswordConfirmation = input;
          goto newPasswordCheck;
      } else {
        if (input == password) {
          
          if (confirmOldPassword) {
            confirmOldPassword = false;
            inputNewPassword = true;
          } else {
            lcd.clear();
            lcd.print("PIN OK!");
            
            digitalWrite(led,HIGH);
            delay(5000);
            digitalWrite(led,LOW);
          }   
        } else if (input == passwordConfig) {
          //Confirm old password afterwards when config password is submitted
          confirmOldPassword = true;
        } else {
          lcd.clear();
          lcd.print("INCORRECT PIN!");
  
          confirmOldPassword = false; //Return to default screen, otherwise stuck in config unless correct password used
          delay(2000);
        }
      }
      
      lcd.clear();
      input = "";
      inputComplete = false;
    }
  }

  
 
}
