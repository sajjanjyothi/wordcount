#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <getopt.h>
#include    <errno.h>

#define MAX_WORD_LENGTH 255 //Tunable

typedef struct _wordMap
{
    char word[MAX_WORD_LENGTH];
    int frequency;
    struct _wordMap *next;
}wordMap;

wordMap     *wordMapHead = NULL;
wordMap     *currItr = NULL;
int         wordCount = 0;

/**
 * @brief Add word to the internal linked list map
 * 
 * @param word Actual word
 * @return int return failure/success
 */
static int addWordToMap( char *word )
{
    wordMap *itr = wordMapHead;

    while(itr)
    {
        if(strcasecmp(itr->word, word) == 0) //Match
        {
            itr->frequency++;
            break;
        }
        itr = (wordMap *)itr->next;
    }

    if(!itr) //Didn't find it - Add it
    {
        if(wordMapHead == NULL)
        {
            wordMapHead =  (wordMap *)malloc(sizeof(wordMap));
            if (wordMapHead)
            {
                strcpy(wordMapHead->word, word);
                wordMapHead->frequency = 1;
                wordMapHead->next = NULL;
                currItr = wordMapHead;
                wordCount++;
            }
        }
        else
        {   
            wordMap *node;
            node = (wordMap *)malloc(sizeof(wordMap));
            if (node)
            {
                strcpy(node->word, word);
                node->frequency = 1;
                node->next = NULL;
                currItr->next = (wordMap *)node;
                currItr = node;
                wordCount++;
            }
        }
    }

    return 0;
}

/**
 * @brief Fill up word map from file
 * 
 * @param filename Actual text file name
 * @return int return success/failure
 */
static int fillupWordMap(char *filename)
{
    FILE *fp;
    char word[MAX_WORD_LENGTH];
    char *token;

    fp = fopen(filename, "r");
    if (fp)
    {
        while (fgets(word, sizeof(word), fp))
        {
            if (strlen(word) > 0)
            {
                if (strchr(word, '\n'))
                {
                    word[strlen(word) - 1] = '\0'; //remove new line if any
                }
                //Tokenize word if more than one - detect space
                token = strtok(word," ");
                while(token)
                {
                    addWordToMap(word);
                    token = strtok(NULL," ");
                }
            }
        }

        fclose(fp);
    }
    else
    {
        fprintf(stderr,"Invalid file : %s\n", strerror(errno));
    }
    
    return 0;
}

/**
 * @brief Quick sort compare function
 * 
 * @param item1 
 * @param item2 
 * @return int 
 */
static int compareFrequency(const void *item1, const void *item2)
{
    int ret = -1;

    wordMap *ptr1 = *(wordMap * const*)item1;
    wordMap *ptr2 = *(wordMap * const*)item2;
    ret = ptr1->frequency < ptr2->frequency;
    return (ret);
}

/**
 * @brief Print the words and its counts after sorting
 * 
 * @return int success/failure
 */
static int printFrequentlyUsedWords()
{
    wordMap     *itr;
    wordMap     **ptrList;
    int         idx = 0;

    itr             =  wordMapHead;
    ptrList         = malloc((sizeof(wordMap *))*wordCount); 

    while(itr) //Copy only pointers to an array, This is for using quick sort algorithm to sort the words based on frequency
    {
        ptrList[idx] = itr;
        idx++;
        itr = itr->next;
    }

    qsort(ptrList, wordCount, sizeof(wordMap*),compareFrequency);//Sort it with frequency
    idx = 0;

    while( ((idx < 20) && (idx < wordCount)) ) //20 Words most frequent
    {
       printf("%d --- %s\n", ptrList[idx]->frequency, ptrList[idx]->word);
       idx++;
    }
    
    free(ptrList);
    return 0;
}

/**
 * @brief Free all the allocated resources
 * 
 * @return int sucess/failure
 */
static int freeResources()
{
    wordMap     *itr = wordMapHead;
    wordMap     *tmp;
    while(itr)
    {
        tmp = itr;
        itr = itr->next;
        if(tmp)
        {
            free(tmp); //Free the node
        }
    }
    return 0;
}

static void printUsage(char *exec)
{
    fprintf(stderr, "Usage %s <file name>\n", exec);
    exit(1);
}
/**
 * @brief Main - Entry point
 * 
 * @param argc 
 * @param argv 
 * @return int sucess/failure
 */
int main(int argc, char *argv[])
{
    int cmOp = -1;
    char fileName[MAX_WORD_LENGTH];
    const char *short_opt = "hf:";
    struct option long_opt[] =
        {
            {"help", no_argument, NULL, 'h'},
            {"file", required_argument, NULL, 'f'},
            {NULL, 0, NULL, 0}
        };

    while ((cmOp = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) //To get the command line options
    {
        switch (cmOp)
        {
        case 'f':
            strcpy(fileName, optarg);
            break;
        case 'h':
            printUsage(argv[0]);
            break;
        default:
            printUsage(argv[0]);
            break;
        }
    }
    fillupWordMap(fileName);
    printFrequentlyUsedWords();
    freeResources();
    return 0;
}
