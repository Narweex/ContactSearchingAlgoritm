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
#define MAX_SEARCH_INPUT 15 //maximum characters as an input
#define MAX_AMOUNT_OF_CONTACTS 50 //maximum amount of contacts
#define MAX_PHONE_LENGTH 15 //max length of a single phone number
#define MAX_NAME_LENGTH 25 //maximum length of a person's name

enum errors{//program error codes and return values
    Success = 0,
    InvalidArguments = 100,
    NoArguments = 101,
    ExceededSearchLimit = 102,
    NoContactsFound = 103,
    NoFilePassed = 104,

};

int validateArguments(int argc, char **argv){//determine whether passed arguments are a valid array of integers
    if(argc < 2){//ensure user submitted an input
        printf("No input arguments, search results all apply to these filters\n");//warn the user that there are no arguments
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

    return Success;
}

 typedef struct{//structure for storing the found contacts
        char name[MAX_NAME_LENGTH];
        char number[MAX_PHONE_LENGTH];
        bool valid;
    }Contact;

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

int main(int argc, char **argv){//main function, creates structs, loads data into them, triggers search functions, prints out results

    if(isatty(fileno(stdin))){
        fprintf(stderr, "No file was passed to search in, error code %d", NoFilePassed);
        return NoFilePassed;
    }
    int argumentsResult = validateArguments(argc, argv);
    if(argumentsResult != Success && argumentsResult != NoArguments){//ensure all arguments are valid
       exit(InvalidArguments);
    }

    //initialize the structure for storing contacts
    Contact contacts[MAX_AMOUNT_OF_CONTACTS];

    //buffer for storing the input
    char buffer[MAX_NAME_LENGTH];

    for(int i = 0; i < MAX_AMOUNT_OF_CONTACTS; i++){//clear the memory used for storing the users data
        strcpy(contacts[i].name, "0");
        strcpy(contacts[i].number, "0");
        contacts[i].valid = false;
    }

    int i = 0;//while loop iteration to load the data
    while(fgets(buffer, sizeof(buffer), stdin)){//open file
        buffer[strcspn(buffer, "\n")] = 0;

        if(sizeof(buffer) > 121){//if contact name or number > 100 characters
            return InvalidArguments;
        }

        //load from file into a struct
        if(isalpha(buffer[0])){
            strcpy(contacts[i].name, buffer);   
        }
        else if(isdigit(buffer[0])){
            strcpy(contacts[i].number, buffer);            
            i++;//increment -> move onto next struct
        }
    }

    if(argumentsResult == NoArguments){//case where there are no arguments entered
         for(int g = 0; g < MAX_AMOUNT_OF_CONTACTS; g++){
            int numberLength = strlen(contacts[g].number);

            if(numberLength > 2){
                printf("%s, %s\n", contacts[g].name, contacts[g].number);
            }
        }
    }
    else{//in case arguments are found begin searching

        //SEARCHING FOR NUMBERS
        searchNumbers(contacts, argv);

        /*SEARCH FOR NAMES*/
        searchNames(contacts, argv);

        /*PRINT OUT THE FOUND CONTACTS*/
        int ContactFound = NoContactsFound;
        for(int g = 0; g < MAX_AMOUNT_OF_CONTACTS; g++){
            if(contacts[g].valid == true){
                    printf("%s, %s\n", contacts[g].name, contacts[g].number);
                    ContactFound = 0;
            }   
        }

        /*CHECK FOR NOT FOUND*/
        if(ContactFound == NoContactsFound){
            for(int g = 0; g < MAX_AMOUNT_OF_CONTACTS; g++){
            if(contacts[g].valid != true){
                    printf("No contacts were found");
                    break;
                }   
            }
        }
    }
    
    //end of the program
    return Success;
}