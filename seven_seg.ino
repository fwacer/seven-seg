/*
 * July 25th, 2017
 * 
 * Seven Segment Display Program
 * By: Bryce Dombrowski
 * 
 *    Takes numbers fed by the Serial Monitor and
 *    outputs them on a single seven segment display
 *      Note: NL or CR must be on
 *     _____
 *   _|__B__|_
 *  | |     | |     This is the layout that this
 *  |A|     |C|     program uses for the display
 *  |_|_____|_|
 *   _|__D__|_
 *  | |     | |
 *  |G|     |E|
 *  |_|_____|_|
 *    |__F__|
 * 
 * 
 */

#define pin_A 2
#define pin_G 8

void writeToLED(byte input){
  for(int i=pin_G; i>=pin_A; i--){
    digitalWrite(i, (input & 1)); //uses logical '&' to determine voltage state
    input = input >> 1;
  }
  
}
void seven_seg(long input, long msecDelay /*Delay between numbers changing*/){
  //Input sanitization
  while (input < 0) {
    Serial.println("Input underflow:: enter a non-negative 32-bit number");
    return;
  }
  while (input > 2147483646) {
    Serial.println("Input overflow:: enter a 32-bit number");
    return;
  }
  
  Serial.print("Input: ");
  Serial.println(input);
  
  byte buf[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //Buffer holds the digits that we will output
  int loopTime = 0;
  convertNumber(input, &loopTime, buf);
  
  for(int i = loopTime; i > 0; i--){
    writeToLED(buf[i]);
    delay(msecDelay); //time between the display changing numbers
  }
  
  writeToLED(0x00); //resets display
}
void convertNumber(long input, int* loopTimePtr, byte* bufPtr){
  byte number[] = {B1110111,B0010100,B0111011,B0111110,B1011100,B1101110,B1101111,B0110100,B1111111,B1111100};
  //A-G pin states for each digit
  
  *loopTimePtr += 1;
  int loopTime = *loopTimePtr;
  
  if (input>9){
    convertNumber(input/10, loopTimePtr, bufPtr); //recursion
    input = input - 10*(input / 10); //uses integer rounding to remove last digit place.
  }

  bufPtr[loopTime] = number[input];
}
void setup() {
  Serial.begin(9600);
  for(int i=pin_A; i <= pin_G; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  Serial.println("Ready: ");
}

long userInput = 0; //global variable to keep a consistent value through the loops
char oldSerialInput; //for checking if both NL and CR are on

void loop() {
  bool output = false;
  char serialInput;
  
  //wait for input
  if (Serial.available() > 0){
    //read the incoming byte
    serialInput = Serial.read();
    if ( (serialInput == 0xA) || (serialInput == 0xD)){ //looking for NL or CR
      output = true;
      if ((oldSerialInput == 0xA) || (oldSerialInput == 0xD)){ //if true, both NL and CR are on; do not output
        output = false;
      }
    } else {
      userInput = ((userInput * 10) + (serialInput-0x30));
    }
    
  }
  if (output){
    seven_seg(userInput, 1000 /* one second delay between digit changing */);
    userInput = 0;
    output = false;
  }
  oldSerialInput = serialInput;
}
