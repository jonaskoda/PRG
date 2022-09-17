#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 50
#define WRONG_INPUT 100
#define WRONG_LEN 101
#define SHIFT_RANGE 52
#define SHIFT 6

const char *const error_input = "Error: Chybny vstup!\n";
const char *const error_len = "Error: Chybna delka vstupu!\n";

// Functions
char *load_input(int *input_len);
char *create_arr_unknown_size(void);
char *create_arr_known_size(int len);
void malloc_checker(char *text);
void input_checker(char ch, char *text);
void len_checker(char *string1, char *string2, int len1, int len2);
char *compare(char *string1, char *string2, int len1);
int find_key(char *string1, char *string2, int len1);
char *shift_array(char *temp_arr, int array_len, int key);
char shift_letter(char ch, int key);
int count_similarities(char *shifted, char *original, int len);
int max_at_index(int *array, int len);
void print_string(char *array, int len);

// The main program 
int main(int argc, char *argv[])
{
    char *string1, *string2, *matching_string;
    int string1_len, string2_len;

    string1 = string2 = matching_string = NULL;
    string1_len = string2_len = 0; 

    string1 = load_input(&string1_len);    // reading input + saving into dynamically allocated arr (decoded)
    string2 = load_input(&string2_len);    // (original message with some letters wrong)

    len_checker(string1, string2, string1_len, string2_len); // checking whether both strings are the same length

    matching_string = compare(string1, string2, string1_len);  // the best matching result of a string
    
    print_string(matching_string, string1_len); 

    free(string1);
    free(string2);
    free(matching_string);

    return EXIT_SUCCESS;
}

char *load_input(int *input_len)
{
    char *text, ch;
    int len = 0, j = 2, i = 1;

    text = create_arr_unknown_size();
    
    while ((ch=getchar()) != '\n')  // reading input of unknown length
    {
        input_checker(ch, text);                 // checking for wrong inputs

        if (len >= N*i)                          // if the size is not enough -> enlarge array
        { 
            char *temp_text =(char* ) realloc(text, (N*j++)*sizeof(char));
            if (temp_text == NULL)
            {
                fprintf(stderr, "Error: realloc failed\n");
                free(text);
                text = NULL;
                len = 0;
                exit(EXIT_FAILURE);
            }
            text = temp_text;
            i++;                                 // enlarging the check size
        }  
        text[len++] = ch;                        // add char into array
    }
    text[len++] = '\0';                          // create a string at the end

    *input_len = len;
    return text;
}

char *create_arr_unknown_size(void)
{
    char *text;

    text = (char *) malloc((N+1) * sizeof(char));
    malloc_checker(text);
    return text;
}

char *create_arr_known_size(int len)
{
    char *text;

    text = (char *) malloc((len+1) * sizeof(char));
    malloc_checker(text);
    return text;
}

void malloc_checker(char *text)
{
    if (text == NULL)
    {
        fprintf(stderr, "Error: malloc failed\n");
        free(text);
        exit(EXIT_FAILURE);
    }
}

void input_checker(char ch, char *text)
{
    if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '\n')))
    {
        fprintf(stderr, error_input);
        free(text);
        exit(WRONG_INPUT);
    }
}

void len_checker(char *string1, char *string2, int len1, int len2)
{
    if (!(len1 == len2))
    {
        fprintf(stderr, error_len);
        free(string1);
        free(string2);
        exit(WRONG_LEN);
    }
}

char *compare(char *string1, char *string2, int len1)  // function returning final result
{
    char *matching_string;
    int key;
    
    matching_string = create_arr_known_size(len1);
    strcpy(matching_string, string1);                  // fill the array 

    key = find_key(string1, string2, len1);            // find the best key (number of shifts)

    matching_string = shift_array(matching_string, len1, key); // create the final string

    return matching_string;
}

int find_key(char *string1, char *string2, int len1)   // find the best key (number of shifts)
{
    char *temp_arr;
    int key_return; 
    int key_arr[SHIFT_RANGE];

    for (int key = 0; key < SHIFT_RANGE; key++)        // for each key create an array, shift every
    {                                                  // letter by key (1, 2,...) and count the 
        temp_arr = create_arr_known_size(len1);        // number of matching letters -> then put them
        strcpy(temp_arr, string1);                     // into key array
        
        temp_arr = shift_array(temp_arr, len1, key);

        key_arr[key] = count_similarities(temp_arr, string2, len1);

        free(temp_arr);
    }
    key_return = max_at_index(key_arr, SHIFT_RANGE);
    
    return key_return;
}

char *shift_array(char *temp_arr, int array_len, int key)  // shift all the letters in the array
{   
    for (int i = 0; i < array_len; i++)
    {
        temp_arr[i] = shift_letter(temp_arr[i], key);
    }
    
    return temp_arr;
}

char shift_letter(char ch, int key)  // shift every letter by a given key
{   
    if (ch == '\0')                  // if you encounter null, ignore
    {
        return ch;
    }
    int num_ch = (int) ch;           // convert the char into int

    if (num_ch > 'Z')                // case for non-capital letters
    {
        num_ch -= SHIFT;             // group the ascii ranges (65 - 116)
    }
    num_ch += key;                   // add the key
    if (num_ch > ('z'-SHIFT))        // case for big shifts
    {
        num_ch = (num_ch - ('z'-SHIFT) + 'A'-1); 
    }

    if (num_ch > 'Z')                
    {
        num_ch += SHIFT;
    }
    return num_ch;
}

int count_similarities(char *shifted, char *original, int len)
{
    int counter = 0;                // compare all elements of two arrays
                                    // at same indexes
    for (int i = 0; i < len; i++)
    {
        if (shifted[i] == original[i])
        {
            counter++;
        }
    }
    return counter;
}

void print_string(char *array, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (!(array[i] == '\0'))
        {
            putchar(array[i]);   
        }
    }
    putchar('\n');
}

int max_at_index(int *array, int len)  // return the index of the biggest number in arr
{
    int largest_idx = 0, largest = array[0];

    for (int i = 0; i < len; i++)
    {
        if (array[i] > largest)
        {
            largest = array[i];
            largest_idx = i;
        }
    }
    return largest_idx;
}


