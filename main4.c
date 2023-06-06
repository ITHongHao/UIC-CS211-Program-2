#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

typedef char* String;

//Functions:
void displayStart();
short int inDict(String* word, String** word_bank, int* word_bank_size);
short int wordCheck(String* word, int* word_length, String** word_bank, int* word_bank_size);
void freeBank(String** word_bank, int* word_bank_size);
void splitString(String* user_input, String* start_word, String* end_word);
void fillNull(String** word_bank, int* word_bank_size);
void addWordToBank(String** word_bank, String* input, int* index, int* word_bank_size, int* word_length);
void loadWords(int* word_length, String** word_bank, int* word_bank_size, String file_name);
short int charCheck(String* word, String* match);
short int validInput(String* word, String* match, String** word_bank, int* word_bank_size);
void game(String* start_word, String* end_word, String** word_bank, int* word_bank_size);
void endMenu(short int* state);


//displays starting message when game is first booted up
void displayStart(){
    printf("Weaver is a game where you try to find a way to get from the starting word to the ending word.\n");
	printf("You can change only one letter at a time, and each word along the way must be a valid word.\n");
	printf("Enjoy!\n\n");
}

//free all allocated memory for that word_bank
//basically resets word_bank_size and word_bank without memory leaks
void freeBank(String** word_bank, int* word_bank_size){
    for(int i = 0; i < (*word_bank_size); i++){
        free((*word_bank)[i]);      //erase Strings
    }
    (*word_bank_size) = 0;          //reset amount
    free(*word_bank);               //reset bank pointer
}

//splits user_input into start_word and end_word
//check if user only inputted one word
void splitString(String* user_input, String* start_word, String* end_word){
    int string_size = (strchr((*user_input), '\n') - *user_input) / sizeof(char);   //find the length of input string
    int start_length = (strchr((*user_input), ' ') - *user_input) / sizeof(char);   //find first string length
    int end_length = string_size - start_length - 1;                                //find last string length
    int j = 0;     

    for(int i = 0; i < string_size; i++){       //build the word character by character using predetermined string sizes
        if(i < start_length){
            (*start_word)[j++] = (*user_input)[i];
        }
        else if(i > start_length){
            (*end_word)[j++] = (*user_input)[i];
        }
        else{
            j = 0;      //reset placement index
        }
    }
    //finish the string off with null character
    if(strlen(*start_word) != 0) {(*start_word)[start_length] = '\0';}  
    if(strlen(*end_word) != 0) {(*end_word)[end_length] = '\0';}
}

//returns 1 if word is found in word_bank using binary search algorithm
short int inDict(String* word, String** word_bank, int* word_bank_size){
    int first = 0;
    int last = (*word_bank_size);
    int mid = last / 2;

    //binary search algorithm 
    //variable names tell you most of the logic
    while(first != last){
        mid = (last + first) / 2;  
        if(strcmp((*word), (*word_bank)[mid]) == 0){        //target equal to mid
            return 1;
        }
        else if(strcmp((*word), (*word_bank)[mid]) > 0){    //target less than mid
            first = mid + 1;
        }
        else{           //target greater than mid
            last = mid;
        }
    }
    if(strcmp((*word), (*word_bank)[first]) == 0) return 1; //if last unchecked element is target
    
    return 0;
}

//returns 1 if a input is valid, return 0 if not valid
short int wordCheck(String* word, int* word_length, String** word_bank, int* word_bank_size){
    if(strcmp((*word), "r") == 0){      //user wants random words from word_bank
        strcpy((*word), (*word_bank)[rand() % (*word_bank_size)]);
        return 1;
    }
    else if(strlen(*word) != (*word_length)){  //start word wrong length
        printf("Your word, '%s', is not a %d-letter word. Try again.\n", (*word), (*word_length));
        return 0;
    }
    else if(!inDict(word, word_bank, word_bank_size)){  //start word not word
        printf("Your word, '%s', is not a valid dictionary word. Try again.\n", (*word));
        return 0;
    }
    else{   //passes all conditions
        return 1;
    }
}

//prompts user to go from word x to word y and sets it up
void wordChoice(String* start_word, String* end_word, int* word_length, String** word_bank, int* word_bank_size){
    String user_input = malloc(92 * sizeof(char));  
    size_t size = 92;
    int loop;
    do{
        loop = 0;   //check whether both words are valid
        printf("Enter starting and ending words, or 'r' for either for a random word: ");
        fflush(stdin);
        fgets(user_input, size, stdin);                                         //read user inputs
        splitString(&user_input, start_word, end_word);                         //split user input into two strings, start_word and end_word

        loop += wordCheck(start_word, word_length, word_bank, word_bank_size);  //check if start_word is correct
        if(loop != 1) continue;                                                 //stops if the first word is not correct
        loop += wordCheck(end_word, word_length, word_bank, word_bank_size);
    }while(loop != 2);                                                          //run until both words are valid
    printf("Your starting word is: %s.\n", (*start_word));                       
    printf("Your ending word is: %s.\n\n", (*end_word));        
    free(user_input);   
}

//Fill storage with NULL to prevent any double freeing
void fillNull(String** word_bank, int* word_bank_size){
    for(int i = (*word_bank_size) - 25; i < (*word_bank_size); i++){
        (*word_bank)[i] = NULL;
    }
}

//adds input to the end of *word_bank and increments word_bank_size properly
void addWordToBank(String** word_bank, String* input, int* index, int* word_bank_size, int* word_length){
    String add = malloc(1 + (*word_length) * sizeof(char)); 
    strcpy(add, *input);                    //copy input string and store its own value
    if(*word_bank_size == 0){               //initial creation of the array
        (*word_bank_size) += 25;
        (*word_bank) = malloc((*word_bank_size)* sizeof(String));
        fillNull(word_bank,word_bank_size); //fill all values with NULL to prevent double free
    }
    else if((*index) >= (*word_bank_size)){ //Dynamically grow the array by 25 strings
        (*word_bank_size) += 25;
        (*word_bank) = realloc((*word_bank),(*word_bank_size)* sizeof(String)); //increase string space by 25
        fillNull(word_bank,word_bank_size);
    }
    String* location = (*word_bank) + (*index); //go to the index of to store new word
    (*location) = add;                      //store new word
}

//Makes word_bank an array containing all words of word_length characters from file file_name
//word_bank_size is updated depending on the amount of elements stored in the word bank
void loadWords(int* word_length, String** word_bank, int* word_bank_size, String file_name){
    int index = 0;
	FILE *filePtr  = fopen(file_name, "r");     // "r" means we open the file for reading
	if(filePtr == NULL) {                       // Check that the file was able to be opened       
		printf("Error: could not open %s for reading\n", file_name);
		exit(-1);
	}
	// Read each word from file, and print them one by one
	String input_string = malloc(45 * sizeof(char));    //freed
	while(fscanf(filePtr, "%s", input_string) != EOF) {
		if(strlen(input_string) == (*word_length)){     //check if word in file is of the correct length
            addWordToBank(word_bank, &input_string, &index, word_bank_size, word_length);
            index++;
        }
	}
    free(input_string);
	// Close the file
	fclose(filePtr);
    (*word_bank_size) = index;  //reassign word_bank_size
}

//returns 0 if the words ONLY have 1 different character, returns 1 if they have more character differences
short int charCheck(String* word, String* match){
    int difference = 0;
    for(int i = 0; (*word)[i] != '\0'; i++){        //go through every char in word
        if((*word)[i] != (*match)[i]) difference++; //check for char differences
    }
    if(difference == 1) return 0;
    return 1;
}

//returns 0 if invalid input, returns 1 if valid
short int validInput(String* word, String* match, String** word_bank, int* word_bank_size){
    if(strlen(*word) != strlen(*match)){  //word is of wrong length
        printf("Your word, '%s', is not a %d-letter word. Try again.\n\n", (*word), (int)strlen(*match));
        return 0;
    }
    else if(!inDict(word, word_bank, word_bank_size)){  //word not a word
        printf("Your word, '%s', is not a valid dictionary word. Try again.\n\n", (*word));
        return 0;
    }
    else if(charCheck(word, match)){    //word has more than 1 char difference or 0 differences
        printf("Your word, '%s', is not exactly 1 character different. Try again.\n\n", (*word));
        return 0;
    }
    else{
        return 1;
    }
}

//actual gameplay. Controls/prompts user inputs, prints main game ui
void game(String* start_word, String* end_word, String** word_bank, int* word_bank_size){
    int turn = 1;                                       
    String user_input = malloc((45) * sizeof(char));        //holds user's current input
    String temp = malloc((45) * sizeof(char));              //holds user's last input     
    strcpy(temp, (*start_word));    
    while(1){   //just keep looping until internal condition is met
        printf("%d. Previous word is '%s'. Goal word is '%s'. Next word: ", turn, temp, *end_word);
        fgets(user_input, 45, stdin);
        int newline_index = (strchr((user_input), '\n') - user_input) / sizeof(char);   //get length of string
        user_input[newline_index] = '\0';   //complete string with null char

        if(strcmp(user_input, "q") == 0) break; //user quits early
        if(!validInput(&user_input, &temp, word_bank, word_bank_size)) continue;   //check if input is valid
        
        strcpy(temp,user_input);    //if its valid, store it next
        if(strcmp(temp, (*end_word)) == 0){ //user matches the word
            printf("Congratulations! You changed '%s' into '%s' in %d moves.\n", (*start_word), (*end_word), turn);
            break;
        }
        else{printf("\n");}
        turn++;
    }
    free(user_input);
    free(temp);
}

//displays end menu and changes the state to determine the game flow and the next action to 
//perform. Look in main for documentation on what state values correspond to what options
void endMenu(short int* state){
    String user_input = malloc(2*sizeof(char));

    //main message for ending the game
    printf("\nEnter:  1 to play again,\n");
    printf("	2 to change the number of letters in the words and then play again, or \n");
    printf("	3 to exit the program.\n");
    printf("Your choice --> ");
    fflush(stdin);
    fgets(user_input, 2, stdin);

    switch(atoi(user_input)){
        case 1:     //play again
        case 2:     //change letter of characters in word and play again
        case 3:     //quit and terminate
            (*state) = atoi(user_input);
            break;
        default:
            (*state) = 3; //figure out default, but for now it just quits.
    }
    free(user_input);
}

int main() {
    srand(1);
    String file_name = "words.txt";

    String start_word;               //start word
    String end_word;                //destination word
    String* word_bank = NULL;      //storing all valid words
    int word_length = 0;            //length of words being played with
    int word_bank_size = 0;         //amount of words in word_bank
    short int state = 2;        //determines events in main function (game flow)
    //  1 = play again
    //  2 = play again, change letters (DEFAULT)
    //  3 = terminate
    displayStart();             
    do{
        start_word  = malloc(46 * sizeof(char));   //45 - longest word in english dictionary + null char space
        end_word  = malloc(46 * sizeof(char));
        if(state > 1){      //if play again and change letters (default state is 2 so this runs the firts time)
            printf("How many letters do you want to have in the words? ");
            int choice = 0;
            scanf("%d",&choice);    //take user input
            word_length = choice;   //for word-length
            //Gets rid of anything on the stack buffer
            String trash = malloc(45*sizeof(char));
            fgets(trash, 45,  stdin);
            free(trash);
            loadWords(&word_length, &word_bank, &word_bank_size, file_name);    //load valid words into word_bank from file_name
	        printf("Number of %d-letter words found: %d.\n\n", word_length, word_bank_size);
        }
        
        wordChoice(&start_word, &end_word, &word_length, &word_bank, &word_bank_size);  //prompt user what words they want to start and end with
        printf("On each move enter a word of the same length that is at most 1 character different and is also in the dictionary.\n");
        printf("You may also type in 'q' to quit guessing.\n\n");
        game(&start_word, &end_word, &word_bank, &word_bank_size);  //main gameplay engine
        endMenu(&state);    //display end menu and prompt user for action    
        free(start_word);
        free(end_word);
        if(state > 1){  //reset bank if chosen
            freeBank(&word_bank, &word_bank_size);
        }
        //same as lines 282-285
        String trash = malloc(45*sizeof(char));
        fgets(trash, 45,  stdin);
        free(trash);
    }while(state != 3);                         //loop until user wants to quit
    printf("\nThanks for playing!\nExiting...");
	return 0;
}