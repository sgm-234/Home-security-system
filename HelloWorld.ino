
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5


int done = 0;

MFRC522 rfid(SS_PIN, RST_PIN);

byte keyTagUID[][4] = {{0x15, 0xB2, 0x7A, 0x32},{0xF5, 0x1D, 0xD3, 0x5C},{0xC5, 0x7D, 0x61, 0x5B}}; //F1 44 BC F0
String name[] = {"Alan","Ashwin Karthik","Gowtham"}; 


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4; // Number of rows on the keypad
const byte COLS = 4; // Number of columns on the keypad

// Define the keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};


int i=0;
int j=0;
int reset = 2;


byte  colPins[COLS]= {6, 8, 7, 14}; 
byte rowPins[ROWS] = {16,3,4,5};
// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define the expected password
const char passcode[] = "1235";
char enteredCode[sizeof(passcode)] = ""; // To store entered passcode
int relay = 15;
void setup() {
  Serial.begin(9600);
  pinMode(relay,OUTPUT);
  pinMode(reset,OUTPUT);
  digitalWrite(relay,HIGH);
  lcd.init();
  SPI.begin(); // init SPI bu
  rfid.PCD_Init(); // init MFRC522
  // Turn on the backlight
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(i,j);
  lcd.print("Enter password");
}

void loop() {

  char key = keypad.getKey();
  if (key != NO_KEY) { // Check if a key is pressed
    Serial.println(key);
    if (strlen(enteredCode) < sizeof(passcode) - 2) { // Check if entered code is not complete yet
      Serial.println(key);
      strcat(enteredCode, &key); // Append the pressed key to the entered code
      lcd.setCursor(0,1);
      lcd.print(enteredCode); // Print the entered code so far
      
    } else { // Entered code is complete
      strcat(enteredCode, &key); // Append the pressed key to the entered code
      Serial.println(enteredCode);
      if (strcmp(enteredCode, passcode) == 0) { // Compare entered code with the expected passcode
      
        lcd.clear();
        i=0;
        j=0;
        lcd.setCursor(i,j);
        lcd.print("Access Granted!"); // If the entered code matches, print Access Granted
        digitalWrite(relay,LOW);
        delay(5000);
        digitalWrite(relay,HIGH);
        lcd.clear();
        lcd.setCursor(i,j);
        lcd.print("Enter password");
        i++;
      } else {
        i=0;j=0;
        lcd.clear();
        lcd.setCursor(i,j);
        lcd.print("Access Denied!"); // If the entered code doesn't match, print Access Denied
        delay(2000);
        lcd.clear();
        lcd.setCursor(i,j);
        lcd.print("Enter password");
        
        i++;
      }
      memset(enteredCode, 0, sizeof(enteredCode)); // Clear the entered code
    }
  }
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { 
   
      for(int i=0;i<sizeof(keyTagUID)/sizeof(keyTagUID[0]);i++){
        if (rfid.uid.uidByte[0] == keyTagUID[i][0] &&
            rfid.uid.uidByte[1] == keyTagUID[i][1] &&
            rfid.uid.uidByte[2] == keyTagUID[i][2] &&
            rfid.uid.uidByte[3] == keyTagUID[i][3] ) {
          Serial.print("Access is granted - ");
          Serial.println(name[i]);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access Granted!");
          delay(5000);
          lcd.clear();
          digitalWrite(relay,LOW);
          delay(5000);
          digitalWrite(relay,HIGH);
          delay(1000);
          lcd.setCursor(0,0);
          lcd.print("Enter password");
          done=1;
        }
      }
      if (done==0){
        i=0;j=0;
        lcd.clear();
        lcd.setCursor(i,j);
        lcd.print("Access Denied!"); // If the entered code doesn't match, print Access Denied
        delay(2000);
        lcd.clear();
        lcd.setCursor(i,j);
        lcd.print("Enter password");
        i++;
      }
    
    
      
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
   
  }
  

   

  done=0; 
  
  

}
