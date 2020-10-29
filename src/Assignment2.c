#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define     PAYLOAD_BUF_SIZE                      256

const char  FLAG_SEQ[9]                       =   "01111110";
const int   FLAG_SIZE                         =   sizeof(FLAG_SEQ);
const char  TERMINATOR                        =   27;                 //ESC character
      
      char  inputBuffer[9]                    =   "        ";         //Hold the 8 most recent characters entered.
      char  payloadBuffer[PAYLOAD_BUF_SIZE+1] =   "";
      
      int   flagDetected                      =   0;
      int   bufferFull                        =   0;

//Used to shift a character into the right side of an array.
void shiftInRight(char buf[], size_t size, char c) {
  for(int i=0; i<size-2; i++) {
    buf[i] = buf[i+1];
  }
  buf[size-2] = c;
}

int main(int argc, char* argv[]) {

  //Turn of echo and canonical mode to prevent buffering of input before it reaches the program.
  system("/bin/stty -icanon -echo");

  //Holds the last key pressed.
  char input;

  //Used to track how many characters have been entered in the payload portion
  //of the sequence. Should not exceed PAYLOAD_BUF_SIZE.
  int payloadCount = 0;

  //Fetch each character and store in 'input';
  while((input = getchar()) != TERMINATOR) {

    //Discard newline aand carriage return
    if(input == '\n' || input == '\r') {
      continue;
    }

    //Shift input char to right side of inputBuffer array.
    //This is where the latest 8 characters are stored for detecting 
    //the flag.
    shiftInRight(inputBuffer, 9, input);
    putchar(input);

    //Check if the previous character toggled flagDetected to true.
    if(flagDetected) {

      //If the start flag was detected, we shift in the input to another array
      //(payloadBuffer) and increment the payload count (for 
      //detecting a full buffer).
      shiftInRight(payloadBuffer, PAYLOAD_BUF_SIZE+1, input);
      payloadCount++;
    }

    //Check if the 8 chars in the inputBuffer match the FLAG_SEQ.
    if(!strcmp(inputBuffer, FLAG_SEQ)) {

      //Since the inputBuffer matched the FLAG_SEQ, we toggle flagDetected.
      flagDetected = !flagDetected;

      //If flagDetected is now true, it was a start flag.
      if(flagDetected) {
        puts("\nStart flag detected.");

      //Otherwise it is an end flag.
      } else {
        puts("\nEnd flag detected.");

        //Print the contents of the payloadBuffer from the first character after the start flag to the last character
        //before the end flag.
        printf("Payload: %.*s\n", payloadCount-FLAG_SIZE+1, &payloadBuffer[PAYLOAD_BUF_SIZE-payloadCount]);

        //Reset flagDetected to 0, payloadBuffer to a null string, and count to 0.
        flagDetected = 0;
        payloadBuffer[0] = '\0';
        payloadCount = 0;
      }
    }

    //Check if the buffer is full.
    bufferFull = payloadCount == PAYLOAD_BUF_SIZE;

    //If the buffer is full we do the same thing as when the end flag is detected.
    if(bufferFull) {
      puts("\nBuffer full.");
      puts(&payloadBuffer[PAYLOAD_BUF_SIZE-payloadCount]);
      flagDetected = 0;
      payloadBuffer[0] = '\0';
      payloadCount = 0;
    }

  }

  //Restore echo and canonical mode.
  system("/bin/stty icanon echo");

}