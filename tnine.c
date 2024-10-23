/****************************************/
//      Jakub Kříž - October 2024       // 
//   Contact searching algorithm - IZP  //
//              xkrizja00               //
//                                      //
/****************************************/
//COMPILE ONLY WITH:    gcc -std=c11 -Wall -Wextra -Werror tnine.c -o tnine

/*PREPROCESSORS*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#define MAX_SEARCH_INPUT 100 //maximum characters as an input
#define MAX_AMOUNT_OF_CONTACTS 50 //maximum amount of contacts
#define MAX_PHONE_LENGTH 15 //max length of a single phone number
#define MAX_NAME_LENGTH 100 //maximum length of a person's name

enum errors{//program error codes and return values
    Success = 0,
    InvalidArguments = 100,
    NoArguments = 101,
    ExceededSearchLimit = 102,
    NoContactsFound = 103,
    NoFilePassed = 104,
    TooManyContacts = 105,
    ContactTooLarge = 106,
    TooManyArguments = 107,

};

int validateArguments(int argc, char **argv){//determine whether passed arguments are valid digits
   
   if(argc > 2){//ensure not too many arguments are passed
        fprintf(stderr, "Too many arguments passed");
        return TooManyArguments;
    }
    else if(argc < 2){//ensure user submitted an input
        return NoArguments;
    }

    if (strlen(argv[1]) > MAX_SEARCH_INPUT) {//maximum input is MAX_SEARCH_INPUT characters
        fprintf(stderr, "Ensure you input no more than %d digits, error code: %d\n", MAX_SEARCH_INPUT, ExceededSearchLimit);        
        return ExceededSearchLimit;
    }

    for (int i = 0; argv[1][i] != '\0'; i++) {//remove the null termination and make sure there are only digits in the search field
        if (!isdigit(argv[1][i])) {
        fprintf(stderr, "Make sure to only input digits, error code: %d\n", InvalidArguments);
            return InvalidArguments;
        }
    }
     // Transform the first character if it is '0'
    if (argv[1][0] == '0') {
        argv[1][0] = '+';  // Transform '0' to '+'
    }

    return Success;
}

 typedef struct{//structure for storing the found contacts
        char name[MAX_NAME_LENGTH];
        char number[MAX_PHONE_LENGTH];
        bool valid;
    }Contact;

int initializeContacts(Contact contacts[]){
     for(int i = 0; i < MAX_AMOUNT_OF_CONTACTS; i++){//clear the memory used for storing the users data
        strcpy(contacts[i].name, "0");
        strcpy(contacts[i].number, "0");
        contacts[i].valid = false;
    }
    //buffer for storing the input
    char buffer[MAX_NAME_LENGTH];

    int i = 0;//while loop iteration to load the data
    while(fgets(buffer, sizeof(buffer), stdin)){//open file
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) > MAX_NAME_LENGTH) {
            fprintf(stderr, "Exceeded maximum length of line, maximum %d characters allowed.\n", MAX_NAME_LENGTH);
            exit(ContactTooLarge);  
        }
        if(i > MAX_AMOUNT_OF_CONTACTS){
            fprintf(stderr, "Exceeded maximum amount of contacts, input max %d", MAX_AMOUNT_OF_CONTACTS);
            return TooManyContacts; 
        }

        //load from buffer into a struct
        if(isalpha(buffer[0])){
            strcpy(contacts[i].name, buffer);   
        }
        else if(isdigit(buffer[0]) || buffer[0] == '+'){
            strcpy(contacts[i].number, buffer);            
            i++;//increment -> move onto next struct
        }
    }
    return Success;
}

int searchNumbers(Contact contact[], char* argv[]){//searches the phone numbers for substrings and validates the passed structs
    for (int i = 0; i < MAX_AMOUNT_OF_CONTACTS; i++) {//iteration for each contact
        int numberLength = strlen(contact[i].number);
        int searchingLength = strlen(argv[1]);
        char buffer[MAX_SEARCH_INPUT];
       for (int j = 0; j <= numberLength - searchingLength; j++) {//if there is a matching substring set the contact as valid
            strncpy(buffer, &contact[i].number[j], searchingLength);
            buffer[searchingLength] = '\0';//null termination
            

            if(!strcmp(buffer, argv[1])){//if there's a match break of the loop and continue to the next contact
                contact[i].valid = true;
                break;
            }
       }
    }
  
    return Success;
}

int searchNames(Contact contact[], char *argv[]) {//searches the names and marks contact as valid if it contains uninterrupted sequence
   //define the keypads
    char *keypads[10] = {"+", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
    
    
    for (int i = 0; i < MAX_AMOUNT_OF_CONTACTS; i++) {// iterate through all contacts
        int numberLength = strlen(contact[i].number);
        int nameLength = strlen(contact[i].name);
        int searchLength = strlen(argv[1]);

        if (numberLength < 2 || searchLength == 0) {//check for empty number
            continue;  
        }
       
        for (int j = 0; j <= nameLength - searchLength; j++) {// Iterate through each character in the contact's name
            int match = 1; 

            for (int k = 0; k < searchLength; k++) {// check if an uninterrupted substring exists
                if (isdigit(argv[1][k])) {
                    int index = argv[1][k] - '0';  // casting to integer
                    if (strchr(keypads[index], tolower(contact[i].name[j + k])) == NULL) {// Check if the character in the contact's name is in the corresponding keypad set
                        match = 0;  
                        break;
                    }
                }
            }

           
            if (match) {//if an uninterrupted match is found, mark the contact as valid
                contact[i].valid = true;
                break;  
            }
        }
    }

    return Success;
}

int checkFileInput(){//check if file was passed as standard input
    if(isatty(STDIN_FILENO)){
        return NoFilePassed;
    }
    else {
        return Success;
    }
}

int printAllContacts(Contact contact[]){//prints all the contacts whether they are valid or invalid
    for(int g = 0; g < MAX_AMOUNT_OF_CONTACTS; g++){
        int contactLength = strlen(contact[g].name);
            if(contactLength > 2){
                for(int j = 0; j < contactLength; j++){
                printf("%c", tolower(contact[g].name[j]));
                }
            printf(", %s\n", contact[g].number);
        }
    }
    return Success;
}

int printFoundContacts(Contact contact[]){//prints out all valid contacts and return if some were valid
     int ContactFound = NoContactsFound;
        for(int g = 0; g < MAX_AMOUNT_OF_CONTACTS; g++){
            if(contact[g].valid == true){
            int contactLength = strlen(contact[g].name);
                for(int j = 0; j < contactLength; j++){
                printf("%c", tolower(contact[g].name[j]));
                }
            printf(", %s\n", contact[g].number);
            ContactFound = 0;
            }
        
        }
    return ContactFound;
}

int noSearchResults(int ContactFound){//check if all contacts were invalid and print out not found
    if(ContactFound == NoContactsFound){
            printf("Not found");
    }
    return Success;
}

int main(int argc, char **argv){//main function, creates structs, loads data into them, triggers search functions, prints out results

    /*CHECK IF A FILE WAS PASSED AS STDIN*/
    if(checkFileInput() != Success){
        fprintf(stderr, "No file was passed to search in, error code %d", NoFilePassed);
        exit(NoFilePassed);
    }

    /*ENSURE ARGUMENTS ARE VALID DIGITS*/
    int argumentsResult = validateArguments(argc, argv);
    if(argumentsResult != Success && argumentsResult != NoArguments){
       exit(argumentsResult);
    }

    /*INITIALIZE THE STRUCTURE FOR STORING CONTACTS*/
    Contact contacts[MAX_AMOUNT_OF_CONTACTS];

    /*LOAD ALL INFORMATION INTO STRUCTS*/
    initializeContacts(contacts);

    if(argumentsResult == NoArguments){//case where there are no arguments entered
        printAllContacts(contacts);
        return Success;
    }
    else{//in case arguments are found begin searching

        //SEARCHING FOR NUMBERS
        searchNumbers(contacts, argv);

        /*SEARCH FOR NAMES*/
        searchNames(contacts, argv);

        /*PRINT OUT THE FOUND CONTACTS*/
        int ContactFound = printFoundContacts(contacts);

        /*PRINT OUT NOTHING WAS FOUND*/
        noSearchResults(ContactFound);
    }
    
    /*END OF THE PROGRAM*/
    return Success;
}