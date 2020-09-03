/********************************************************
 * Course: ENGR476
 * Student Name: Taylor Artunian
 * Student Number: 920351715
 * Assignment: 1
 * Date Submitted: 9/3/20
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <readline/readline.h>
#include <string.h>

#define MAX_FILENAME_LENGTH   128
#define MAX_NAME_LENGTH       64
#define MAX_STUDENTS          8
#define ENTRY_BUFFER_SIZE     256
#define PROMPT_BUFFER_SIZE    64
#define MAX_COMMAND_SIZE      8
#define MAX_COMMAND_DESC_SIZE 64

typedef enum { FT_NEW, FT_EXISTING } FILENAME_TYPE;

const char I_FILENAME_PROMPT[PROMPT_BUFFER_SIZE] =
  "Please enter input file name:";

const char O_FILENAME_PROMPT[PROMPT_BUFFER_SIZE] =
  "Please enter output file name:";

const char COMMAND_PROMPT[PROMPT_BUFFER_SIZE] = 
  "Please enter a command (enter h for help): ";

typedef struct {
  char CommandString[MAX_COMMAND_SIZE];
  int (*Function)();
  char CommandDescription[MAX_COMMAND_DESC_SIZE];
} Command;

typedef struct {
  char    StudentName[MAX_NAME_LENGTH];
  int     StudentNumber;
  double  SubjectAScore;
  double  SubjectBScore;
} StudentEntry;

StudentEntry studentEntries[MAX_STUDENTS];
int studentEntryCount = 0;

/* Input handling function declarations. */

/* Prompts the user to enter either an input filename or output filename 
 * depending on type parameter (FT_NEW means only accept filename that does
 * not exist, FT_EXISTING means only accept a filename that does exist). Once
 * the filename has been accepted, it is copied to fileNameBuffer for use by
 * the caller. */
int promptForFileName(char[MAX_FILENAME_LENGTH],
  const char[PROMPT_BUFFER_SIZE], FILENAME_TYPE);

/* Command routine declarations. */
void printStudentEntryHeaderNormal(FILE*);
void printStudentEntryHeaderAVGSTDDEV(FILE*);
void printStudentEntryNormal(FILE*, StudentEntry*);
void printStudentsWithDiploma(FILE*);
void printSortSubjectAScoreAscending(FILE*);
void printCalculateAvgAndStdDev(FILE*);

/* Command declarations. */
int cmd_studentsWithDiploma();
int cmd_sortSubjectAScoreAscending();
int cmd_calculateAvgAndStdDev();
int cmd_saveAll();
int cmd_exit();
int cmd_help();

Command dispatchTable[] = {
  { "a", cmd_studentsWithDiploma,         "All students that receive diploma." },
  { "b", cmd_sortSubjectAScoreAscending,  "Subject A in ascending order." },
  { "c", cmd_calculateAvgAndStdDev,       "Average and standard deviation." },
  { "d", cmd_saveAll,                     "Save all output to file." },
  { "e", cmd_exit,                        "Exit." },
  { "h", cmd_help,                        "Help." }
};

const int NUM_COMMANDS = sizeof(dispatchTable)/sizeof(Command);

/* Command routine definitions.
 *
 * These include a FILE* parameter 
 * so that output can be directed either to the screen (stdout) or 
 * an output file. */

/* Prints the header for all fields except STUDENT NO. */
void printStudentEntryHeaderNormal(FILE* file) {
  fprintf(file, "%-18s %-12s %-9s\n",
    "STUDENT NAME", "SUBJECT A", "SUBJECT B");
}

/* Prints the header for average and standard deviation. */
void printStudentEntryHeaderAVGSTDDEV(FILE* file) {
  fprintf(file, "%-18s %-12s %-9s\n",
    "STUDENT NAME", "AVERAGE", "STDDEV");
}

/* Prints a StudentEntry to the specified output. */
void printStudentEntryNormal(FILE* file, StudentEntry* entry) {
  fprintf(file, "%-18s %-12.1f %-9.1f\n",
    entry->StudentName, entry->SubjectAScore, entry->SubjectBScore);
}

/* Prints all students that will receive a diploma (both scores >= 50). */
void printStudentsWithDiploma(FILE* file) {
  printStudentEntryHeaderNormal(file);
  for(int i=0; i<studentEntryCount; i++) {
    StudentEntry* entry = &studentEntries[i];
    if(entry->SubjectAScore >= 50 && entry->SubjectBScore >= 50) {
      printStudentEntryNormal(file, entry);
    }
  }
}

/* Comparison function used to sort StudentEntry's by SubjectAScore. */
int compareStudentEntrySubjectA(const void* a, const void* b) {
  return ((StudentEntry*)b)->SubjectAScore > ((StudentEntry*)a)->SubjectAScore;
}

/* Prints all students in ascending order of SubjectA score. */
void printSortSubjectAScoreAscending(FILE* file) {
  printStudentEntryHeaderNormal(file);
  qsort(&studentEntries, studentEntryCount,
    sizeof(StudentEntry), compareStudentEntrySubjectA);
  for(int i=0; i<studentEntryCount; i++) {
    printStudentEntryNormal(file, &studentEntries[i]);
  }
}

/* Prints all students' average and standard deviation of scores.
 * Each student's average is the average of their two scores.
 * Standard deviation is based on the average of all scores for all students. */
void printCalculateAvgAndStdDev(FILE* file) {

  printStudentEntryHeaderAVGSTDDEV(file);

  double totalAverage = 0;
  double averages[studentEntryCount];
  double deviations[studentEntryCount];

  //Calculate each student's average and totalAverage of all scores for all students.
  for(int i=0; i<studentEntryCount; i++) {
    StudentEntry* entry = &studentEntries[i];

    //Calculate the average of both scores for the student.
    averages[i] = (entry->SubjectAScore + entry->SubjectBScore) / 2;

    //Add the average to the totalAverage.
    totalAverage += averages[i];

    //Average the totalAverage with the previous totalAverage after the 1st entry.
    if(i>0) {
      totalAverage /= 2;
    }
  }

  //Calculate the standard deviations.
  for(int i=0; i<studentEntryCount; i++) {
    deviations[i] = (averages[i] - totalAverage) / totalAverage;
  }

  //Print the output.
  for(int i=0; i<studentEntryCount; i++) {
    StudentEntry* entry = &studentEntries[i];
    fprintf(file, "%-18s %-12.2f %-9.4f\n",
      entry->StudentName, averages[i], deviations[i]);
  }
}

/* Command definitions. */
int cmd_studentsWithDiploma() {
  printStudentsWithDiploma(stdout);
}

int cmd_sortSubjectAScoreAscending() {
  printSortSubjectAScoreAscending(stdout);
}

int cmd_calculateAvgAndStdDev() {
  printCalculateAvgAndStdDev(stdout);
}

int cmd_saveAll() {
  char fileName[MAX_FILENAME_LENGTH];

  //Prompt for an output filename. File must not already exist.
  promptForFileName(fileName, O_FILENAME_PROMPT, FT_NEW);
  FILE* file = fopen(fileName, "wrb");

  //Send the 3 different output tables to the output file.
  printStudentsWithDiploma(file);
  printSortSubjectAScoreAscending(file);
  printCalculateAvgAndStdDev(file);

  fclose(file);
}

int cmd_exit() {
  exit(0);
}

int cmd_help() {
  for(int i=0; i<NUM_COMMANDS; i++) {
    Command* command = &dispatchTable[i];
    printf("%4s - %s\n", command->CommandString, command->CommandDescription);
  }
}

/* Input handling function definitions. */
int parseLine(char* data, StudentEntry* entryOut) {

  char* savePointer;
  char* token = strtok_r(data, " ", &savePointer);

  /* 0 - StudentName    - char*
   * 1 - StudentNumber  - int
   * 2 - SubjectAScore  - double
   * 3 - SubjectBScore  - double
   */
  int tokenNumber = 0;

  //Loop through each token.
  while(token != NULL && *token != 0) {

    //Handle each column of the input line separately.
    switch(tokenNumber) {
      case 0: {
        strncpy(entryOut->StudentName, token, MAX_NAME_LENGTH);
        break;
      }
      case 1: {
        entryOut->StudentNumber = atoi(token);
        break;
      }
      case 2: {
        entryOut->SubjectAScore = atof(token);
        break;
      }
      case 3: {
        entryOut->SubjectBScore = atof(token);
        break;
      }
      default: {
        printf("Unexpected token.\n");
      }
    }

    //Increment the column/token number and fetch next token.
    tokenNumber++;
    token = strtok_r(0, " ", &savePointer);

  }

  return 1;

}

int parseFile(char fileName[MAX_FILENAME_LENGTH]) {

  printf("Opening file: %s\n", fileName);

  FILE* file = fopen(fileName, "rb");
  char buffer[ENTRY_BUFFER_SIZE];

  //Discard the first line of headers.
  fgets(buffer, ENTRY_BUFFER_SIZE, file);

  while(fgets(buffer, ENTRY_BUFFER_SIZE, file)) {
    
    // Remove the trailing '\n'
    buffer[strlen(buffer)-1] = '\0';

    printf("Parsing line: %s\n", buffer);

    //Try to parse the new line of input into a StudentEntry struct.
    if(parseLine(buffer, &studentEntries[studentEntryCount])) {
      studentEntryCount++;
      printf("Line successfully parsed.\n");
    } else {
      printf("Line could not be parsed.\n");
    }

    printf("\n");

  }

  fclose(file);

  return 0;

}

int promptForFileName(char fileNameBuffer[MAX_FILENAME_LENGTH],
  const char promptMessage[PROMPT_BUFFER_SIZE], FILENAME_TYPE type) {

  char fileName[MAX_FILENAME_LENGTH];

  while(1) {

    printf("%s ", promptMessage);

    // Get input from stdin
    fgets(fileName, MAX_FILENAME_LENGTH, stdin);

    // Accept and return the input if the fileName is not empty.
    if(strcmp(fileName, "")) {

      // Remove the trailing '\n'
      fileName[strlen(fileName)-1] = '\0';

      /* Check if the file exists. 0 if it exists, -1 otherwise.
       * Different actions will be taken depending on the FILENAME_TYPE provided. */
      int accessResult = access(fileName, F_OK);

      //Looking for file that does exist.
      if(type == FT_EXISTING) {
        if(accessResult) {
          printf("File '%s' does not exist.\n", fileName);
          continue;
        } else {
          // Copy fileName to output buffer
          strncpy(fileNameBuffer, fileName, MAX_FILENAME_LENGTH);
          return 1;
        }

      //Trying to get new file. File should not exist.
      } else {
        if(accessResult) {
          // Copy fileName to output buffer
          strncpy(fileNameBuffer, fileName, MAX_FILENAME_LENGTH);
          return 1;
        } else {
          printf("File '%s' already exists.\n", fileName);
          continue;
        }
      }

    } else {
      printf("Invalid file name.\n");
      continue;
    }
  }

  return 0;

}

Command* getCommand(char command[MAX_COMMAND_SIZE]) {

  //Check if the command string exists in the dispatchTable
  for(int i=0; i<NUM_COMMANDS; i++) {
    if(!strcmp(dispatchTable[i].CommandString, command)) {
      return &dispatchTable[i];
    }
  }

  return 0;

}

Command* promptForCommand() {

  char* commandBuffer;
  Command* command;

  /* Infinite loop for reading commands.
   * Will exit once a valid command is entered. */
  while(1) {
    commandBuffer = readline(COMMAND_PROMPT);
    command = getCommand(commandBuffer);
    if(command) {
      return command;
    } else {
      printf("Invalid command.\n");
    }
  }

  return 0;

}

void executeCommand(Command* command) {
  printf("%s\n", command->CommandDescription);
  command->Function();
}

int main(int argc, char* argv[]) {

  char fileName[MAX_FILENAME_LENGTH];
  Command* command = NULL;

  //Get the input file to read.
  promptForFileName(fileName, I_FILENAME_PROMPT, FT_EXISTING);

  //Parse the file into the array of StudentEntry structs.
  parseFile(fileName);

  //Infinite loop for reading commands.
  while(1) {
    command = promptForCommand();
    executeCommand(command);
  }

  return 0;

}