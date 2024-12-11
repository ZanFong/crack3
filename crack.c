#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"

#if __has_include("fileutil.h")
#include "fileutil.h"
#endif

#define PASS_LEN 50     // Maximum length any password will be.
#define HASH_LEN 33     // Length of hash plus one for null.


//provided function outline
// Given a target plaintext word, use it to try to find
// a matching hash in the hashFile.
char * tryWord(char * plaintext, char ** hashes, int hashSize)
{
    char *password = md5(plaintext, strlen(plaintext));
    
    for(int i = 0; i < hashSize; i++)
    {
        char *hash = hashes[i];

        if(!strcmp(password, hash))
        {
            free(password);
            return hashes[i];
        }
    }

    free(password);
    return NULL;
}

// Function to load a file into an array of strings
// This function loads the contents of a file into a dynamically allocated array of strings.
char ** loadFile(char *filename, int *size, int maxLen)
{
    // Open the file
    FILE *in = fopen(filename, "r");
    if (!in)
    {
        perror("Can't open file");
        exit(1);
    }

    // Allocate memory for the array of strings
    int capacity = 1000; // initial capacity
    char ** arr = malloc(capacity * sizeof(char *));
    char line[maxLen];
    int lineNum = 0;

    // Read the file line by line.
    while(fgets(line, PASS_LEN, in))
    {
        // Trim newline if present
        if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        // Check if capacity is exceeded, expand if needed
        if(lineNum == capacity)
        {
            capacity += 1000;
            arr = realloc(arr, capacity * sizeof(char *));
        }

        // Allocate memory to store the current line
        arr[lineNum] = malloc(strlen(line) + 1);
        strcpy(arr[lineNum], line); // Copy line into the array

        lineNum++;
    }

    // Close the file
    fclose(in);

    // Set the size to the number of lines in the array
    *size = lineNum;

    // Return the array of strings
    return arr;
}

//main function
//takes 2 arguments from the command line
int main(int argc, char *argv[])
{   
    //if too few arguments, exit
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // Load dictionary file into array
    int dictSize = 0;
    char **dictWords = loadFile(argv[2], &dictSize, PASS_LEN);

    //load hashes into array
    int hashSize = 0;
    char **hashes = loadFile(argv[1], &hashSize, HASH_LEN);

    // Initialize var to hold number of successful matches
    int hacked = 0;

    // Loop through the array of dictionary words
    for (int i = 0; i < dictSize; i++)
    {  
        // Store result of tryWord
        char *match = tryWord(dictWords[i], hashes, hashSize);
        
        // If tryWord returned non-NULL value, print the hash and dictionary word
        if(match)
        {
            printf("%s %s\n", match, dictWords[i]); 
            hacked++;
        }
    }

    // Free the memory used by the arrays
    for (int i = 0; i < dictSize; i++)
    {
        free(dictWords[i]);
    }
    free(dictWords);

    for (int i = 0; i < hashSize; i++)
    {
        free(hashes[i]);
    }
    free(hashes);

    // Print total number of successful matches
    printf("%d hashes were cracked.\n", hacked);

    return 0;
}