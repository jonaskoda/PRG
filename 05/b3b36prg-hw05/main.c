#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum 
{
    INPUT_ERROR = 100,
    SIGN_ERROR = 101,
    MALLOC_ERROR = 102,
 };
typedef struct matrix  
{
    int rows;
    int cols;
    int *values;
} matrix;

// functions
static void *my_malloc(size_t size);
_Bool load_size(int *r_cnt, int *c_cnt);
_Bool read_matrix(int rows, int cols, int matrix[rows][cols]);
matrix *read_matrix2(void);
static matrix *allocate_matrix(void);
void free_matrix(matrix **m);
_Bool load_sign(char *ch);
_Bool check_sizes(matrix *m1, matrix *m2, char operator);
void return_error(void);
void return_sign_error(void);
void print_size(int rows, int cols);
void print_matrix(const struct matrix* const m);
matrix *create_matrix(matrix *m1, matrix *m2);
matrix *calculate(matrix *m1, matrix *m2, char sign);
matrix *matrix_sum(matrix *m1, matrix *m2, char sign);
matrix *matrix_multiply(matrix *m1, matrix *m2, char sign);
void clean(matrix *temp, char *sign);
matrix *manage_matrices(void);

// The main program 
int main(int argc, char *argv[])
{
    matrix *result = NULL;
    result = manage_matrices();

    if (!result)
    {
        fprintf(stderr, "Cannot read input\n");
        exit(INPUT_ERROR);
    }
    else
    {
        print_size(result->rows, result->cols);
        print_matrix(result);
    }

    free_matrix(&result);

    return EXIT_SUCCESS;
}

_Bool load_size(int *r_cnt, int *c_cnt)  // scan the size info from the input
{                                        // return true if successful
    _Bool ret = true;
    if (scanf("%d %d", r_cnt, c_cnt) != 2)
    {
        ret = false;
    }
    return ret;
}

_Bool read_matrix(int rows, int cols, int matrix[rows][cols])   // function for reading
{                                                               // a matrix from the input
    _Bool ret = true;                                           // and saving values into a new one 
    for (int r = 0; r < rows; ++r)                              // with allocated size
    {
        for (int c = 0; c < cols; ++c)
        {
            if (scanf("%d", &matrix[r][c]) != 1)
            {
                ret = false;
            }
        }
    }
    return ret;
}

static void *my_malloc(size_t size)       // allocate array of a certain 
{                                         // size and return it
    void *ret = malloc(size);
    if (!ret)
    {
        fprintf(stderr, "Error, malloc\n");
        exit(MALLOC_ERROR);
    }
    return ret;
}

static matrix *allocate_matrix(void)      // "initialize" matrix and its components
{
    matrix *ret = my_malloc(sizeof(matrix));
    ret->rows = ret->cols = 0;
    ret->values = NULL;
    return ret;
}

void free_matrix(matrix **m)       // deallocate the matrix comletely
{
    if (m && *m)                   // if m is a matrix
    {
        if ((*m)->values)          // if m does have values
        {
            free((*m)->values);
        }
        free(*m);         // frees the matrix
        *m = NULL;        // frees the pointer
    }
}

matrix *read_matrix2(void)
{
    matrix *ret = allocate_matrix();             // first allocate the matrix
    if (!load_size(&(ret->rows), &(ret->cols)))  // load the measurements and 
    {                                            // assign them to the matrix struct
        free_matrix(&ret);
        return_error();
    }
    if (ret)
    {
        ret->values = my_malloc(ret->rows * ret->cols * sizeof(int));          // allocate 1D array for values
        if (!read_matrix(ret->rows, ret->cols, (int (*)[ret->cols])ret->values)) // fill the array with values
        {
            free_matrix(&ret);                   //sets ret to NULL
            return_error();
        }
    }
    return ret;
}

_Bool load_sign(char *ch)                        // load the operator between matrices, return true 
{                                                // in case of success
    _Bool ret = true;
    getchar();                                   // get rid of a new line character
    if ((*ch = getchar()) == EOF)                // in case of end of file, return false
    {
        ret = false;
    }
    else if (!(*ch == '+' || *ch == '-' || *ch == '*')) // if anything else than these specific operators 
    {                                                   // is read, return error
        return_sign_error();
    }
    return ret;
}

matrix *create_matrix(matrix *m1, matrix *m2)     // create a new matrix based on two matrices as inputs
{
    matrix *result = NULL;
    result = allocate_matrix();
    result->values = my_malloc(m1->rows * m2->cols * sizeof(int));
    return result;
}

_Bool check_sizes(matrix *m1, matrix *m2, char operator)  // check whether a certain operation can be performed 
{                                                         // based on sizes of matrices
    _Bool ret = true;
    if (operator == '*')                                  // case for multiplication
    {
        if (!(m1 && m1->values && m2 && m2->values &&
        m1->cols == m2->rows))
        {
            ret = false;
        }
    }
    else if (operator == '+' || operator == '-')          // case for addition / subtraction
    {
        if (!(m1 && m1->values && m2 && m2->values &&
        m1->rows == m2->rows && m1->cols == m2->cols))
        {
            ret = false;
        }
    }
    if (!ret)
    {
        free_matrix(&m1);
        free_matrix(&m2);
        return_error();
    }
    return ret;
}

void return_error(void)
{
    fprintf(stderr, "Error: Chybny vstup!\n");
    exit(INPUT_ERROR);
}

void return_sign_error()
{
    fprintf(stderr, "Error, invalid sign of math operator\n");
    exit(SIGN_ERROR);
}

void print_size(int rows, int cols)           // prints the size of matrix printed in the terminal
{
    printf("%d %d\n", rows, cols);
}

void print_matrix(const struct matrix* const m)
{
    if (m && m->values)
    {
        for (int r = 0; r < m->rows; ++r)
        {
            for (int c = 0; c < m->cols-1; ++c)
            {
                printf("%d ", m->values[r * m->cols + c]);
            }
        printf("%d", m->values[r * m->cols + m->cols-1]);  // print the last number without space
        printf("\n");
        }
    }
}

matrix *calculate(matrix *m1, matrix *m2, char sign)  // function for managing the calculation
{
    matrix *result = NULL;
    if (sign == '+' || sign == '-')                   // in case of summation, call the sum function
    {                                                 // and pass it the + or - sign accordingly
        result = matrix_sum(m1, m2, sign);
    }
    else if (sign == '*')
    {
        result = matrix_multiply(m1, m2, sign); 
    }
    return result;
}

matrix *matrix_sum(matrix *m1, matrix *m2, char sign)  
{   
    matrix *result = NULL;
    int operator = sign == '+'? 1:-1;  // addition or subtraction switch 

    if (check_sizes(m1, m2, sign))     // check whether both matrices have the same measurements and
    {                                  // thus can be added / subtracted
        result = allocate_matrix();    // create a new matrix with a specified size
        result->rows = m1->rows;
        result->cols = m1->cols;
        result->values = my_malloc(m1->rows * m1->cols * sizeof(int));  // allocate an array for values
        
        int *v1 = m1->values;
        int *v2 = m2->values;
        int *r = result->values;
        for (int i = 0; i < m1->rows * m1->cols; ++i)    // the actual calculation
        {
            *r++ = *v1++ + operator * (*v2++);
        } 
    }
    return result;
}

matrix *matrix_multiply(matrix *m1, matrix *m2, char sign)
{
    matrix *result = NULL;
    if (check_sizes(m1, m2, sign))   // check whether the matrix multiplication is possible
    {
        result = allocate_matrix();
        result->rows = m1->rows;
        result->cols = m2->cols;
        result->values = my_malloc(m1->rows * m2->cols * sizeof(int));

        int *v1 = m1->values;
        int *v2 = m2->values;
        int *r = result->values;
        
        for (int i = 0; i < m1->rows; i++)           // actual calculation
        {
            for (int j = 0; j < m2->cols; j++)
            {
                int sum = 0;
                for (int k = 0; k < m1->cols; k++)   // summing row and column of two matrices
                {                            
                    sum += ((v1[i * m1->cols + k]) * (v2[k * m2->cols + j]));   
                }
                *r++ = sum;                // resulting value for the new matrix position 
            }
        }
    }
    return result;
}

void clean(matrix *temp, char *sign)
{
    temp = NULL;
    *sign = '\0';
} 

matrix *manage_matrices(void)    // function that manages the proces of reading matrices
{                                // and determining what should be done next, returns the final mtx
    matrix *m1 = read_matrix2(); // read the first matrix before the loop
    matrix *m2 = NULL;           
    matrix *m3 = NULL;
    matrix *temp = NULL;
    char sign1 = '\0';
    char sign2 = '\0';

    for (;;)                         // infinite loop
    {  
        if (!sign1)                  // if sign is not read
        {
            if (!load_sign(&sign1))  // try to read sign
            {
                break;
            }
        }
        if (sign1 == '*')            // if the sign is '*', do multiplication of the first two matrices
        {
            m2 = read_matrix2();
            temp = calculate(m1, m2, sign1);
            free_matrix(&m1);
            free_matrix(&m2);
            m1 = temp;
            clean(temp, &sign1);
        }
        else if (sign1 == '+' || sign1 == '-')   // else check for m2 and read another matrix
        {
            if (m2 == NULL)
            {
                m2 = read_matrix2();
            }
            if (!load_sign(&sign2))              // try to load a second sign
            {
                temp = calculate(m1, m2, sign1); 
                free_matrix(&m1);
                free_matrix(&m2);
                m1 = temp;
                clean(temp, &sign1);
                break;                           // terminating condition based on  
            }                                    // whether there is another sign on the
        }                                        // input or not
        if (sign2 == '*')                        // continue in the loop by multiplying second and third matrix
        {
            m3 = read_matrix2();
            temp = calculate(m2, m3, sign2);
            free_matrix(&m2);
            free_matrix(&m3);
            m2 = temp;
            clean(temp, &sign2);
        }
        else if (sign2 == '+' || sign2 == '-')   // continue in the loop by summing first and second matrix
        {
            temp = calculate (m1, m2, sign1);
            free_matrix(&m1);
            free_matrix(&m2);
            m1 = temp;
            temp = NULL;
            sign1 = sign2;
            sign2 = '\0';
        }
    } 
    return m1;           // return the resulting matrix
} 

