#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEMLIMIT 128
#define HASHBUCKETS 128

struct command {

  char *commandName;
  char *arg[MEMLIMIT];
  char *val[MEMLIMIT];
  int argLength;

};

struct hashValue {
  
  int first;
  int second;

};

void info();
void prompt();
void listen(int, char *);
void parse(char* , struct command *);
int commandHandler(struct command *);
struct hashValue getHashValue(char *);
void handleCreateCommand(struct command *);
void handleAssignCommand(struct command *);
void handlePrintCommand(struct command *);
void handleJoinCommand(struct command *);
void handleCopyCommand(struct command *);
void handleRemoveCommand(struct command *);
void handleCompareCommand(struct command *);
int handleFindCommand(struct command *);
void handleReplaceCommand(struct command *);
void conditionalHandler(struct command *, struct command *);
int findMode(int);

char *stringVariable[MEMLIMIT][HASHBUCKETS];

int main(int argc, char *argv[]) {

  int mode = findMode(argc);
  if(mode == 0) {
    argv[1] = "";
  }
  info();
  if(mode == 0) 
    prompt();
  listen(mode, argv[1]);
  
  return 0;
}

int findMode(int argc) {
  if(argc == 1) {
    //returns 0 if shell mode
    return 0;
  }

  else {
    //returns 1 if file mode
    return 1;
  }
}

void info() {
  char info[] = "NICE (NICE Interpreted Computer English) v0. [March 21, 2015]\n";
  printf(info);
}

void prompt() {
  printf(">>> ");
}

void listen(int mode, char *fileName) {

  char line[MEMLIMIT];
  
  if(mode == 1) {
    
    FILE *scriptFile = fopen(fileName, "r");
    int i = 0;
    while(fgets(line, sizeof(line), scriptFile)) {
      if(!strcmp(line, "bye")) {
	break;
      }
      struct command commandData;
      parse(line, &commandData);
      
      int conditional = commandHandler(&commandData);
      
      if(conditional == 1) {
	struct command commandDataConditionalStatement;
	fgets(line, sizeof(line), scriptFile);
	parse(line, &commandDataConditionalStatement);
	conditionalHandler(&commandData, &commandDataConditionalStatement);
      }
    }
    fclose(scriptFile);
    if(strcmp("bye", line) != 0) {
      mode = 0;
      prompt();
    }
  }

  if(mode == 0) {
    
    while(scanf(" %[^\n]s", line)) {
      if(!strcmp(line, "bye")) {
	break;
      }

      struct command commandData;
      parse(line, &commandData);
      int conditional = commandHandler(&commandData);
      
      if(conditional == 1) {
	struct command commandDataConditionalStatement;
	printf("...    ");
	scanf(" %[^\n]s", line);
	parse(line, &commandDataConditionalStatement);
	conditionalHandler(&commandData, &commandDataConditionalStatement);
      }      
      prompt();
    }
  }
}

void parse(char *line, struct command *commandData) {  
  
  int commandNameLength = 0;
  for(commandNameLength = 0; line[commandNameLength] != ' '; commandNameLength++)
    ;

  int j = 0;
  commandData->commandName = malloc(commandNameLength * sizeof(char));
  
  for(j = 0; j < commandNameLength; j++) {
    commandData->commandName[j] = line[j];
  }
  commandData->commandName[j] = '\0';

  char *var;
  commandData->argLength = 0;
  int numberOfValues = -1;
  //argLength refers to number of variables
  for(j = commandNameLength; j < strlen(line); j++) {
    if(line[j] == '(') {
      j++;
      commandData->argLength++;
      commandData->arg[commandData->argLength] = malloc(MEMLIMIT);
      int i = 0;
      while(line[j] != ')') {
	commandData->arg[commandData->argLength][i] = line[j];
	j++;
	i++;
      }
    }

    if(line[j] == '"') {
      j++;
      numberOfValues++;
      commandData->val[numberOfValues] = malloc(MEMLIMIT);
      int i = 0;
      while(line[j] != '"') {
	commandData->val[numberOfValues][i] = line[j];
	j++;
	i++;
      }
    }
  }

  /* Debugging 
  printf("commandName: %s\n", commandData->commandName);
  for(j = 1; j <= commandData->argLength; j++) {
    printf("arg[%d]: %s\n", j, commandData->arg[j]);
  }
  for(j = 0; j <= numberOfValues; j++)
  printf("Command Value %s\n", commandData->val[j]);
  /* End of Debugging */
}

struct hashValue getHashValue(char *var) {
  
  struct hashValue hv;
  int sum = 0;
  int i = 0;
  for(i = 0; i < strlen(var); i++) {
    sum = sum + (int)var[i];
  }

  hv.first = var[0];
  hv.second = sum % HASHBUCKETS;
  return hv;

}

void conditionalHandler(struct command *commandData, struct command *commandDataCS) {
  
  int returnValue = handleFindCommand(commandData);
  if(returnValue != -1) {
    commandHandler(commandDataCS);
  }
}   

int commandHandler(struct command *commandData) {
  
  if(strcmp(commandData->commandName, "if") == 0) {
    return 1;
  }

  else if(strcmp(commandData->commandName, "create") == 0) {
    handleCreateCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "assign") == 0) {
    handleAssignCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "print") == 0) {
    handlePrintCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "join") == 0) {
    handleJoinCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "copy") == 0) {
    handleCopyCommand(commandData);
  }
  
  else if(strcmp(commandData->commandName, "remove") == 0) {
    handleRemoveCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "compare") == 0) {
    handleCompareCommand(commandData);
  }

  else if(strcmp(commandData->commandName, "find") == 0) {
    int returnValue = handleFindCommand(commandData);
    printf("%d\n", returnValue);
  }

  else if(strcmp(commandData->commandName, "replace") == 0) {
    handleReplaceCommand(commandData);
  }

  return 0;
}

void handleCreateCommand(struct command *commandData) {
  
  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);
  stringVariable[hv.first][hv.second] = malloc(MEMLIMIT);

}

void handleAssignCommand(struct command *commandData) {

  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);
  stringVariable[hv.first][hv.second] = commandData->val[0];

}
  
void handlePrintCommand(struct command *commandData) {
  
  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);
  printf("%s\n", stringVariable[hv.first][hv.second]);

}

void handleJoinCommand(struct command *commandData) {
  
  struct hashValue hv1, hv2;
  hv1 = getHashValue(commandData->arg[1]);
  hv2 = getHashValue(commandData->arg[2]);

  strcat(stringVariable[hv1.first][hv1.second], stringVariable[hv2.first][hv2.second]);
}

void handleCopyCommand(struct command *commandData) {
  
  struct hashValue hv1, hv2;
  hv1 = getHashValue(commandData->arg[1]);
  hv2 = getHashValue(commandData->arg[2]);
  strcpy(stringVariable[hv1.first][hv1.second], stringVariable[hv2.first][hv2.second]);

}

void handleRemoveCommand(struct command *commandData) {

  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);
  char temp[MEMLIMIT];
  int matchingLength = 0, iCopy;
  int tempIndex = 0;
  char *sVCopy = stringVariable[hv.first][hv.second];

  for(int i = 0; i < strlen(sVCopy);) {
    iCopy = i;
    if(commandData->val[0][0] != sVCopy[i]) {
      temp[tempIndex] = sVCopy[i];
      tempIndex++;
      i++;
    }
    else {
      matchingLength = 0;
      while(commandData->val[0][matchingLength] == sVCopy[iCopy]) {
	iCopy++;
	matchingLength++;
	if(matchingLength == strlen(commandData->val[0])) {
	  i = iCopy;
	  break;
	}
      }
      if(matchingLength < strlen(sVCopy)) {
	  temp[tempIndex] = sVCopy[i];
	  tempIndex++;
	  i++;
      }
    }
  }

  int i = 0;
  for(i = 0; i < strlen(temp); i++) {
    stringVariable[hv.first][hv.second][i] = temp[i];
  }
  stringVariable[hv.first][hv.second][i] = '\0';
  
}

void handleCompareCommand(struct command *commandData) {
  
  struct hashValue hv1, hv2;
  hv1 = getHashValue(commandData->arg[1]);
  hv2 = getHashValue(commandData->arg[2]);

  int result = strcmp(stringVariable[hv1.first][hv1.second], stringVariable[hv2.first][hv2.second]);

  if(result > 0) {
    printf("%s greater than %s (lexicographically)\n", commandData->arg[1], commandData->arg[2]);
  }

  else if (result == 0) {
    printf("%s equals %s\n", commandData->arg[1], commandData->arg[2]);
  }

  else if (result < 0) {
    printf("%s less than %s (lexicographically)\n", commandData->arg[1], commandData->arg[2]);
  }

}

int handleFindCommand(struct command *commandData) {
  
  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);

  char *sVCopy = stringVariable[hv.first][hv.second];
  int location = -1;
  int i = 0;

  for(i = 0; i < strlen(sVCopy); i++) {
    int matchingLen = 0;
    int iCopy = i;

    if(sVCopy[iCopy] == commandData->val[0][0]) {
      while(commandData->val[0][matchingLen] == sVCopy[iCopy]) {
	iCopy++;
	matchingLen++;
	if(iCopy == strlen(sVCopy)){
	  break;
	}
      }
      if(matchingLen == strlen(commandData->val[0])) {
	location = i;
	break;
      }
    }
   
    if(location != -1) {
      break;
    }
  }
  
  return location;
}


void handleReplaceCommand(struct command *commandData) {

  struct hashValue hv;
  hv = getHashValue(commandData->arg[1]);

  int removeStartingIndex = handleFindCommand(commandData);
  handleRemoveCommand(commandData);

  char *temp = stringVariable[hv.first][hv.second];
  char new[MEMLIMIT];
  
  int i = 0, j = 0;
  for(i = 0; i < removeStartingIndex; i++) {
    new[i] = temp[i];
  }
  
  new[i] = '\0';
  
  for(; j < strlen(commandData->val[1]); j++, i++) {
    new[i] = commandData->val[1][j];
  }

  new[i] = '\0'; 

  for(j = removeStartingIndex; j < strlen(temp); j++, i++) {
    new[i] = temp[j];
  }

  new[i] = '\0';

  for(i = 0; i < strlen(new); i++) {
    stringVariable[hv.first][hv.second][i] = new[i];
  }
  
  stringVariable[hv.first][hv.second][i] = '\0';

}
