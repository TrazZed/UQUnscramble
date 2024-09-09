#include <stdlib.h>
#include <stdio.h>
#include <csse2310a1.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_ARGC_COUNT 7
#define MAX_WORD_LENGTH 60
#define DICT_ERROR_EXIT 2
#define FORMAT_ERROR_EXIT 1
#define LENMIN_ERROR_EXIT 8
#define DEFAULT_LEN_MIN 3
#define LEN_MIN_UPPER_BOUND 6
#define LEN_MIN_LOWER_BOUND 2
#define DEFAULT_LETT_COUNT 7
#define DEFAULT_DICT_PATH "/local/courses/csse2310/etc/words"
#define MAX_LETT_COUNT 12
#define TOO_MANY_LETTERS_ERROR_EXIT 14
#define TOO_FEW_LETTERS_ERROR_EXIT 5
#define INVALID_LETTERS_ERROR_EXIT 3
#define MAX_WORD_BONUS 10
#define SUCCESSFUL_EXIT 0
#define NO_WORDS_EXIT 12

void command_line_check(int argc, char** argv, char** dict, const char** let,
        int* lenMin, bool* inputDict, bool* inputLett, bool* inputMin);
void parameter_checking(bool inputMin, bool inputLett, bool inputDict,
        int* lenMin, const char** let, char** dict);
void command_line_error();
void len_min_error();
void check_letters(const char* letters, int lenMin);
char** read_dict(char* dict, int* numOfWords);
int play_game(const char* let, int lenMin, int lenMax, int numberOfWords,
        char** words);
int only_letters(char* input);
int formable_string(char* input, const char* letters);
bool check_valid_input(char* input, int inputLength, int lenMin, int lenMax,
        const char* let, char** foundWords, int numFoundWords, char** words,
        int numberOfWords);
int found_already(char* input, char** foundWords, int numWordsFound);
int exists_in_dictionary(char* input, char** dict, int dictLength);
int sort_string(const void* word1, const void* word2);
void print_all_words(char** words, int numberOfWords, int lenMin, int lenMax,
        const char* let);
void end_game(int score);

int main(int argc, char** argv)
{
    // Check if number of arguments is even less than or equal to 6
    if ((argc + 1) % 2 != 0 || argc > MAX_ARGC_COUNT) {
        // Error
        command_line_error();
    }
    // Booleans to keep track of if one of the parameters has already been
    //  assigned a value
    bool inputDict = false;
    bool inputLett = false;
    bool inputMin = false;
    char* dict;
    const char* let;
    int lenMin;
    // Check command line arguments and parameters
    command_line_check(argc, argv, &dict, &let, &lenMin, &inputDict, &inputLett,
            &inputMin);
    parameter_checking(inputMin, inputLett, inputDict, &lenMin, &let, &dict);
    int numberOfWords;
    char** words = read_dict(dict, &numberOfWords);
    // All checks were successful - begin the game
    int lenMax = strlen(let);
    printf("Welcome to UQunscramble!\n");
    printf("Enter words of length %d to %d made from the letters \"%s\"\n",
            lenMin, lenMax, let);
    int score = play_game(let, lenMin, lenMax, numberOfWords, words);
    for (int i = 0; i < numberOfWords; i++) {
        free(words[i]);
    }
    free(words);
    end_game(score);
}

/** Checks the command line arguments to ensure that there are the correct
 * format. For all argc arguments stored in argv, if the user passes the
 * appropriate parameter, assign the value to the corresponding vairable, and
 * changes the associated boolean value to ensure multiple occurences dont
 * happen
 * If --dict is specified, assign associated value with dict and set variable
 * inputDict true
 * If --len-min is specified, assign associated value with let and set variable
 * inputLett true
 * If --len-min is specified, assign associated value with lenMin and set
 * variable inputMin true
 * If any error occurs, process the correct error processing step
 */
void command_line_check(int argc, char** argv, char** dict, const char** let,
        int* lenMin, bool* inputDict, bool* inputLett, bool* inputMin)
{
    // For each input command line argument, check if it is valid and if it has
    // not already been input, assign the associated value to a variable
    for (int i = 1; i < argc; i = i + 2) {
        if (strcmp(argv[i], "--dict") == 0) {
            if (*inputDict) {
                command_line_error();
            }
            *dict = argv[i + 1];
            *inputDict = true;
        } else if (strcmp(argv[i], "--lett") == 0) {
            if (*inputLett) {
                command_line_error();
            }
            *let = argv[i + 1];
            *inputLett = true;
        } else if (strcmp(argv[i], "--len-min") == 0) {
            if (*inputMin) {
                command_line_error();
            }
            // Check that the supplied argument is a single int
            if (!(isdigit(*argv[i + 1]))) {
                command_line_error();
            }
            *lenMin = atoi(argv[i + 1]);
            *inputMin = true;
        } else {
            command_line_error();
        }
    }
}

/** Checks if a parameter that has been input by the user in the command line,
 * identifiable by the booleans inputMin, inputLett, inputDict, is valid to the
 * requirements, and if not, proceed with the corresponding error. If the user
 * did not specify a parameter on the command line, assign values
 */
void parameter_checking(bool inputMin, bool inputLett, bool inputDict,
        int* lenMin, const char** let, char** dict)
{
    // If minimum length is specified on the command line, check that it is
    // valid, and if not, exit with status 8
    if (!inputMin) {
        *lenMin = DEFAULT_LEN_MIN;
    } else if (inputMin
            && (*lenMin > LEN_MIN_UPPER_BOUND
                    || *lenMin < LEN_MIN_LOWER_BOUND)) {
        len_min_error();
    }
    // Check if the specified letters is valid
    if (!inputLett) {
        *let = get_random_letters(DEFAULT_LETT_COUNT);
    } else {
        check_letters(*let, *lenMin);
    }
    // Check if the specified dictionary is valid
    if (!inputDict) {
        *dict = DEFAULT_DICT_PATH;
    }
}

/* Terminates the game and prints a message to stderr detailing the correct
 * format for command line arguments
 */
void command_line_error()
{
    fprintf(stderr,
            "Usage: uqunscramble [--dict dictfile] [--lett letters] [--len-min "
            "numchars]\n");
    exit(FORMAT_ERROR_EXIT);
}

/** Terminates the game and prints a message to stderr detailing the required
 * bounds for minimum length
 */
void len_min_error()
{
    fprintf(stderr, "uqunscramble: minimum length must be between 2 and 6\n");
    exit(LENMIN_ERROR_EXIT);
}

/** Check whether the user input letters is valid
 */
void check_letters(const char* letters, int lenMin)
{
    int len = strlen(letters);
    if (len > MAX_LETT_COUNT) {
        fprintf(stderr,
                "uqunscramble: too many letters - no more than 12 expected\n");
        exit(TOO_MANY_LETTERS_ERROR_EXIT);
    } else if (len < lenMin) {
        fprintf(stderr,
                "uqunscramble: too few letters for the given minimum length "
                "(%d)\n",
                lenMin);
        exit(TOO_FEW_LETTERS_ERROR_EXIT);
    }
    for (int i = 0; i < len; i++) {
        if (!(isalpha(letters[i]))) {
            fprintf(stderr, "uqunscramble: invalid letter set\n");
            exit(INVALID_LETTERS_ERROR_EXIT);
        }
    }
}

/**
 * Read the dictionary file line by line, and store the word in the array words
 * dynamically allocating memory at each point in time, and also storing the
 * number of words in the dictionary. If there is an error reading the
 * dictionary, exit with the appropriate steps
 */
char** read_dict(char* dict, int* numOfWords)
{
    // Read lines from dictionary and add to an array of strings
    FILE* dictWords;
    // Initialise the array of strings to store one string
    char** words = malloc(sizeof(char*));
    int i = 0;
    char line[MAX_WORD_LENGTH];
    int wordLen;
    bool valid;
    // If there is an error reading the file, exit with the appropriate message
    // to stderr
    if (!(dictWords = fopen(dict, "r"))) {
        fprintf(stderr,
                "uqunscramble: dictionary file named \"%s\" cannot be opened\n",
                dict);
        exit(DICT_ERROR_EXIT);
    }
    // For each word in the dictionary
    while (fgets(line, MAX_WORD_LENGTH, dictWords) != NULL) {
        valid = true;
        // Get the length of the word
        wordLen = strlen(line);
        // Convert all to upper-case and ensure the word is valid
        for (int i = 0; i < wordLen - 1; i++) {
            line[i] = toupper(line[i]);
            if (!(isalpha(line[i]))) {
                valid = false;
                break;
            }
        }
        if (valid) {
            // Allocate memory for the word
            words[i] = malloc((wordLen + 1) * sizeof(char));
            // Copy the word into the array
            strcpy(words[i], line);
            i++;
            // Allocate more memory for the next word
            words = realloc(words, (i + 1) * sizeof(char*));
        }
    }
    fclose(dictWords);
    *numOfWords = i;
    return words;
}

/** This function provides the main functionality for the game, will read in
 * words from the user constantly and performs the necessary actions to continue
 * the game. Requires the letters let, min and max length of words lenMin and
 * lenMax, the number of words in the dictionary numberOfWords, and the list of
 * words words
 */
int play_game(const char* let, int lenMin, int lenMax, int numberOfWords,
        char** words)
{
    char buffer[MAX_WORD_LENGTH];
    char* input;
    int score = 0;
    int inputLength;
    char** foundWords = malloc(sizeof(char*));
    int numFoundWords = 0;
    // While the user hasn't terminated the program
    while (!(feof(stdin))) {
        // Ask for user input
        input = fgets(buffer, MAX_WORD_LENGTH, stdin);
        // If the user terminates, the input is null, so break out of the loop
        if (input == NULL) {
            break;
        }
        if (strcmp(input, "q\n") == 0) {
            print_all_words(words, numberOfWords, lenMin, lenMax, let);
            break;
        }
        inputLength = strlen(input) - 1;
        bool validInput = check_valid_input(input, inputLength, lenMin, lenMax,
                let, foundWords, numFoundWords, words, numberOfWords);
        if (validInput) {
            foundWords[numFoundWords]
                    = malloc((inputLength + 2) * sizeof(char));
            strcpy(foundWords[numFoundWords], input);
            numFoundWords++;
            foundWords
                    = realloc(foundWords, (numFoundWords + 1) * sizeof(char*));
            if (inputLength == lenMax) {
                score += inputLength + MAX_WORD_BONUS;
            } else {
                score += inputLength;
            }
            printf("Good! Score so far is %d\n", score);
        }
    }
    // Free all the memory used by the dictionary and stored founds words
    // arrays
    for (int i = 0; i < numFoundWords; i++) {
        free(foundWords[i]);
    }
    free(foundWords);
    return score;
}

/** Checks whether a user input string contains characters that exist in the
 * specified list letters, returning 0 if it contains a letter that does not
 * exist, or 1 if all letters exist
 */
int formable_string(char* input, const char* letters)
{
    int inputLength = strlen(input) - 1;
    int numLetters = strlen(letters);
    // Creating a duplicate array of the letters for iterating
    char* available = (char*)malloc((numLetters + 1) * sizeof(char));
    strcpy(available, letters);
    bool exists = false;
    for (int i = 0; i < inputLength; i++) {
        for (int j = 0; j < numLetters; j++) {
            if (input[i] == toupper(available[j])) {
                // Letter found, set it in the available array to NULL so it
                // can't be used again
                available[j] = '0';
                exists = true;
                break;
            }
        }
        if (!exists) {
            free(available);
            return 0;
        }
        exists = false;
    }
    free(available);
    return 1;
}

/** Checks if the input by the user of length inputLength is valid, i.e. it
 * only contains characters. If this is valid, then it will check against the
 * other conditions set, ensuring the inputLength is greater than the minimum
 * lenMin, but less than the max lenMax. It will check if the input is a
 * formable string from the given letters let, check if the input has been
 * already found in the foundWords array with numFoundWords words, and then
 * finally check if the input is already in the dictionary words, with
 * numberOfWords words in the dictionary. Returns false if any condition is not
 * met, and true if all conditions are met
 */
bool check_valid_input(char* input, int inputLength, int lenMin, int lenMax,
        const char* let, char** foundWords, int numFoundWords, char** words,
        int numberOfWords)
{
    // Make all input upper case
    for (int i = 0; i < inputLength; i++) {
        input[i] = toupper(input[i]);
    }
    bool validInput = true;
    for (int i = 0; i < inputLength; i++) {
        if (!isalpha(input[i])) {
            validInput = false;
            break;
        }
    }
    if (!validInput) {
        printf("Your word must contain only letters\n");
    } else if (inputLength < lenMin) {
        printf("Word too short - it must have at least %d characters\n",
                lenMin);
    } else if (inputLength > lenMax) {
        printf("Word must have a length of no more than %d characters\n",
                lenMax);
    } else if (!formable_string(input, let)) {
        printf("Word can't be formed from available letters\n");
    } else if (found_already(input, foundWords, numFoundWords)) {
        printf("Word has already been guessed\n");
    } else if (!exists_in_dictionary(input, words, numberOfWords)) {
        printf("Word can't be found in dictionary file\n");
    } else {
        return true;
    }
    return false;
}

/** Determine if the word input has already been found in the array foundWords,
 * with numWordsFound number of words already found
 */
int found_already(char* input, char** foundWords, int numWordsFound)
{
    for (int i = 0; i < numWordsFound; i++) {
        if (strcmp(input, foundWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/** Determine if the input word exists in the dictionary of words dict with
 * dictLength words, and if it does, return 1, else return 0
 */
int exists_in_dictionary(char* input, char** dict, int dictLength)
{
    for (int i = 0; i < dictLength; i++) {
        if (strcmp(input, dict[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/** A function to be passed into qsort to compare words and sort them from
 * shortest to longest first, then alphabetically
 */
int sort_string(const void* word1, const void* word2)
{
    char* const* pointer1 = word1;
    char* const* pointer2 = word2;
    int lengthDifference = strlen(*pointer1) - strlen(*pointer2);
    if (lengthDifference != 0) {
        return lengthDifference;
    }
    return strcmp(*pointer1, *pointer2);
}

/** If the user enters 'q', will print out all of the possible words the user
 * could have entered, alongside the maximum possible score for the game.
 * Requires the dictionary of words, number of words numberOfWords, the minimum
 * and maximum length lenmin and lenMax, and the available letters let
 */
void print_all_words(char** words, int numberOfWords, int lenMin, int lenMax,
        const char* let)
{
    int maxScore = 0;
    int wordLength;
    // Keep track of all printed words already
    char** printedWords = malloc(sizeof(char*));
    int numPrintedWords = 0;
    bool alreadyPrinted;
    // Sort the list of words
    qsort(words, numberOfWords, sizeof(*words), sort_string);
    for (int i = 0; i < numberOfWords; i++) {
        alreadyPrinted = false;
        // Check if the word is formable with the given letters
        if (formable_string(words[i], let)) {
            // Check if the word has already been printed
            for (int j = 0; j < numPrintedWords; j++) {
                if (strcmp(words[i], printedWords[j]) == 0) {
                    alreadyPrinted = true;
                    break;
                }
            }
            if (!alreadyPrinted) {
                // Allocate memory to store the word
                wordLength = strlen(words[i]) - 1;
                printedWords[numPrintedWords]
                        = malloc((wordLength + 2) * sizeof(char));
                strcpy(printedWords[numPrintedWords], words[i]);
                numPrintedWords++;
                printedWords = realloc(
                        printedWords, (numPrintedWords + 1) * sizeof(char*));
                if (wordLength >= lenMin) {
                    printf("%s", words[i]);
                    // Determine the score of the word
                    if (wordLength == lenMax) {
                        maxScore += wordLength + MAX_WORD_BONUS;
                    } else {
                        maxScore += wordLength;
                    }
                }
            }
        }
    }
    // Free the memory used to store the printed words
    for (int i = 0; i < numPrintedWords; i++) {
        free(printedWords[i]);
    }
    free(printedWords);
    printf("Maximum possible score was %d\n", maxScore);
}

/** Ends the game and prints out the score that the user achieve to stdout, and
 * exit with the correct exit code
 */
void end_game(int score)
{
    if (score > 0) {
        printf("Game over. You scored %d\n", score);
        exit(SUCCESSFUL_EXIT);
    } else {
        printf("No words guessed!\n");
        exit(NO_WORDS_EXIT);
    }
}
