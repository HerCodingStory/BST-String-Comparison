
/*------------------------------------------
* Author: Cristina E Villarroel
*
* Panther ID:    5923243
*
* COP 4338 - Programming III
*
* Summer 2017
*
* Assignment #1
*
* Plagiarism Statement: I certify that this
* assignment is my own work and that I have
* not copied in part or whole or otherwise
* plagiarized the work of other students
* and/or persons.
*----------------------------------------*/

/*------------------------------------------
* Program Description: This program sorts the line of an input file (or from standard input)
* and print the sorted lines to an output file (or standart output). The program uses a Binary
* search tree to sort (Nodes) using inOrder Traversal. Also, it checks for whitespaces and empty
* input. At the end of the program, the memory is set free.
*----------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

//Struture of a node for the binary search tree
typedef struct TreeNode
{
    int count; // count how many instances
    char *string; // actual sentence
    struct TreeNode *leftChild; // left child is less than the root
    struct TreeNode *rightChild; // right child is greater than the root
}TreeNode;

// Functions
void Add(char *key, struct TreeNode **node, bool caseSensitiveFlag);
void CheckForWhiteSpaces(char *input);
void InOrder(struct TreeNode *, FILE *);
void DeleteBST(struct TreeNode **root);
void ProcessUserInput(struct TreeNode **root, bool caseSensitiveFlag);
bool CheckEmptyString(const char *input);
int StringComparisons(const char string1[], const char string2[], bool caseSensitiveFlag);

int main(int argc, char **argv)
{
    extern char *optarg;       // optarg = optional argument
    extern int optind;         // optind = optional index
    extern int errno;          // errno will  print our error code for each case and error occurs
    FILE *inputFile        = stdin; // file to read if there is no file just use standard input
    FILE *outputFile       = stdout; // file to write if there is no file just use standard output
    int input              = 0;
    int outputFlag         = 0; // flags
    int inputFlag          = 0;
    bool caseSensitiveFlag = false; // boolean flag
    char inputFileName[128]; // filenames
    char outputFileName[128];
    char bufferReader[400];   // buffer to dynamically allocate strings
    TreeNode *root = NULL; // Node holds the root

    static char usage[] = "Usage: %s bstsort [-c] [-o output_file_name] [input_file_name]\n";

    // When getopt return -1, indicates no more options are present and loop ends.
    while ((input = getopt(argc,argv, "co:")) != -1)
    {
        // use to dispatch on the return value from getopt
        switch (input)
        {
            case 'c':
                caseSensitiveFlag = true; // activates case sensitive flag
                break;
            case 'o':
                strcpy(outputFileName, optarg); // gets optional argument of file output
                outputFlag = 1;
                break;
            case '?':
                // Error handling
                fprintf(stderr,usage,argv[0]);
                exit(0);
        }
    }

    // Make sure the optind is correct and the input file was given
    if (optind != argc)
    {
        strcpy(inputFileName, argv[optind]); // Gets the last input word which is the filename
        inputFlag = 1; // input file was given
    }

    // if input and output files were given
    if(outputFlag && inputFlag)
    {
        // Reading File
        if (inputFileName != NULL)
        {
            errno = 0; // set errno to 0 everytime it might get an error code

            inputFile = fopen(inputFileName, "r"); // Try to open and read inputFileName.txt

            if(!inputFile) // if it fails
            {
                fprintf(stderr, "Error: Couldn't open the file: %s\n", strerror(errno));
                exit(errno); // Failing to open a file will close the program
            }

            // Reads the file until it gets to the end
            while(fgets(bufferReader, 400, inputFile) != NULL)
            {
                CheckForWhiteSpaces(bufferReader); // function trims white spaces

                if (!CheckEmptyString(bufferReader)) // function check for empty lines
                {
                    bufferReader[strcspn(bufferReader, "\n")] = '\0'; // removing new line and add the new character
                    Add(bufferReader,  &root, caseSensitiveFlag); // function adds the string to a node
                }
            }
            fclose(inputFile); // close the file
        }

        // Writing File
        errno = 0;

        FILE *outputFile = fopen(outputFileName, "w"); // Try to create and write a file

        if(!outputFile) // if it fails
        {
            fprintf(stderr, "Error: Couldn't write the file: %s\n", strerror(errno));
            exit(errno); // Failing to write a file will close the program
        }

        InOrder(root, outputFile); // function for inorder traversal of the tree

        fclose(outputFile); // Close the file
    }
    else if (outputFlag == 1) // if only outputfile was given
    {
        ProcessUserInput(&root, caseSensitiveFlag); // function to process standard input form console

        errno = 0;

        FILE *outputFile = fopen(outputFileName, "w"); // Try to create and write a file

        if(!outputFile) // if it fails
        {
            fprintf(stderr, "Error: Couldn't open the file: %s\n", strerror(errno));
            exit(errno); // Failing to write a file will close the program
        }

        InOrder(root, outputFile); // function for inorder traversal of the tree

        fclose(outputFile); // Close the file
    }
    else if (inputFlag == 1)  // if only input file was given, read and print to console
    {
        // Reading file
        if (inputFileName != NULL)
        {
            errno = 0;

            inputFile = fopen(inputFileName, "r"); // Try to open and read inputFileName.txt

            if(!inputFile) // if it fails
            {
                fprintf(stderr, "Error: Couldn't open the file: %s\n", strerror(errno));
                exit(errno); // Failing to open a file will close the program
            }

            // Reads the file until it gets to the end
            while(fgets(bufferReader, 400, inputFile) != NULL)
            {
                bufferReader[strcspn(bufferReader, "\n")] = '\0'; // removing new line and add the new character
                Add(bufferReader,  &root, caseSensitiveFlag); // function adds the string to a node
            }
            fclose(inputFile); // Close the file
        }

        printf("\n"); // print new line

        InOrder(root, outputFile); // function for inorder traversal of the tree
    }
    else // no files given just process from stdin and print to console
    {
        ProcessUserInput(&root, caseSensitiveFlag); // function to process standard input form console
        printf("\n"); // print new line
        InOrder(root, outputFile); // function for inorder traversal of the tree
    }

    DeleteBST(&root); // free space in memory by deleting the tree

    return 0;
}

// function makes comparisons of strings for both cases (sensitive or insensitive)
int StringComparisons(const char *string1, const char *string2, bool caseSensitiveFlag)
{
    int length1 = strlen(string1) + 1; // adding 1 makes space for '/0'
    int length2 = strlen(string2) + 1;
    int i;

    char string1Copy[length1]; // creates a string with the same size as parameter string
	char string2Copy[length2];

	strcpy(string1Copy,string1); // copy string1
	strcpy(string2Copy,string2); // copy string2

    // if caseSensitiveFlag is false, just lower the case to make comparisons
    if (!caseSensitiveFlag)
    {
        for(i = 0; i < length1; i++)
        {
            string1Copy[i] = tolower(string1[i]); // lower every single char
        }

        for(i = 0; i < length2; i++)
        {
            string2Copy[i] = tolower(string2[i]); // lower every single char
        }
    }

    i = 0; // set to 0 after loop

    while(string1Copy[i] != '\0' && (string1Copy[i] == string2Copy[i]))
    {
        i++; // increment i to make correct comparison
    }

    if (string1Copy[i] == '\0') // strings are the same
        return 0;
    else if (string1Copy[i] < string2Copy[i]) // string2 is greater than string1
        return -1;
    else // string1 is greater than string2
        return 1;
}

// This method traverses the tree in-order, and then outputs, respectively using recursion.
void InOrder(struct TreeNode *root, FILE *stream)
{
    if (root != NULL) // root has to exist
    {
        InOrder(root->leftChild, stream); // print left child first

        int i;

        for (i = 0; i < root->count; i++) // print root
        {
            fprintf(stream, "%s\n", root->string); //print the lines 'count' many times.
        }

        InOrder(root->rightChild, stream); // print right child
    }
}

// This function reads from the console and then adds it to the tree
void ProcessUserInput(struct TreeNode **root, bool isCaseSensitive)
{
  char input[129]; // one more because the '\0'

  printf("\nEnter sentence or Press enter to finish: ");

  //gets user input until new line
  while ((fgets(input, sizeof(input), stdin) != NULL) && (input[0] != '\n'))
  {
      CheckForWhiteSpaces(input); // fucntion for trimming white spaces

      if (!CheckEmptyString(input)) // fucntion to check if empty line
      {
        input[strcspn(input, "\n")] = '\0'; // removing new line and add the new character
        Add(input,  root, isCaseSensitive); // add string to tree
        printf("\nEnter another sentence or Press enter to finish: "); // prompt user for next
      }
      else
        printf("\nYou entered an empty line. Try again or Press enter to finish: "); // empty lines are check
  }
}

// This function traverse the tree in post order(children first), and then free the nodes and strings, using recursion.
void DeleteBST(struct TreeNode **root)
{
    if(*root != NULL) // root has to exist
    {
        DeleteBST(&(*root)->leftChild); // remove left first
        DeleteBST(&(*root)->rightChild); // remove right second
        free((*root)->string); // free string
        free((*root)); // free root
    }

}

// This function adds a node to the bst using recursion
void Add(char *key, struct TreeNode **node, bool isCaseSensitive)
{
    if(*node == NULL) // if it is empty
    {
        *node = malloc(sizeof(struct TreeNode)); // malloc is allocating space in memory for our node
        (*node)->string = malloc(sizeof(char) * strlen(key) + 1); // malloc is allocating space in memory for our string
        memcpy((*node)->string, key, strlen(key) + 1); // copy key into string node
        (*node)->count = 1; // count instance
        (*node)->leftChild = NULL; // set children to null
        (*node)->rightChild = NULL;
    }
    else
    {
        int result;

        result = StringComparisons(key, (*node)->string, isCaseSensitive); // result from comparison function

        if(result < 0) // add it to the left child
            Add(key, &(*node)->leftChild, isCaseSensitive);
        else if (result > 0) // add it to the right child
            Add(key, &(*node)->rightChild, isCaseSensitive);
        else
            (*node)->count += 1; // if they are equal increase counter
    }
}

// in Case Funtion for trimming all the leading and trailing spaces
void CheckForWhiteSpaces(char *input)
{
    int i;
    int startingPoint = 0;
    int endingPoint = strlen(input) - 1;

    while(isspace((unsigned char)input[startingPoint])) // count how many spaces you find
        startingPoint++; // keep checking

    // check again but from the end and taake into account starting point
    while((endingPoint >= startingPoint) && isspace((unsigned char) input[endingPoint]))
        endingPoint--;

    for(i = startingPoint; i <= endingPoint; i++)
        input[i - startingPoint] = input[i]; // replace spaces with correct char

    input[i - startingPoint] = '\0'; // last char has to be \0
}

// in Case function for checking empty lines in input
bool CheckEmptyString(const char *input)
{
    while(*input != '\0') // unitl input is not \0
    {
        if(!isspace((unsigned char) *input)) // check if input is a space
            return false; // it is not an empty line

        input++; // else increase counter to keep checking
    }
    return true; // if it is an empty line
}
