#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum 
{
    INPUT_ERROR = 100,
    SIGN_ERROR = 101,
};
typedef struct matrix
{
    int rows;
    int cols;
    int *values;
} matrix;
// functions
void load_size(int *r_cnt, int *c_cnt);
void load_matrix(int rows, int cols, int matrix[rows][cols]);
void load_sign(char *ch);
void return_error(void);
void return_sign_error(void);
void pick_measurements(int rows1, int cols1, int rows2, int cols2, 
                       char sign, int *new_rows, int *new_cols);
int check_size(int rows1, int cols1, int rows2, int cols2, char sign);
void print_size(int rows, int cols);
void print_matrix(int rows, int cols, int matrix[rows][cols]); 
void print_matrix2(const struct matrix* const m);
void calculate(int rows1, int cols1, int m1[rows1][cols1], 
               int rows2, int cols2, int m2[rows2][cols2], 
               int new_rows, int new_cols, int new_mtx[new_rows][new_cols],
               char sign);
void sum(int rows1, int cols1, int m1[rows1][cols1], 
         int m2[rows1][cols1], int new_m[rows1][cols1],  char sign);
void multiply(int rows1, int cols1, int m1[rows1][cols1], 
             int rows2, int cols2, int m2[rows2][cols2], int new_m[rows1][cols2]);

// The main program 
int main(int argc, char *argv[])
{
    int rows1, cols1;
    int rows2, cols2;
    int new_rows, new_cols;
    char sign;
    rows1 = cols1 = rows2 = cols2 = new_rows = new_cols = 1;

    // matrix 1
    load_size(&rows1, &cols1);
    int m1[rows1][cols1];
    load_matrix(rows1, cols1, m1);
    // operator
    load_sign(&sign);
    // matrix 2
    load_size(&rows2, &cols2);
    int m2[rows2][cols2];
    load_matrix(rows2, cols2, m2);
    // determine the size of the resultant matrix
    pick_measurements(rows1, cols1, rows2, cols2, sign, &new_rows, &new_cols);
    // create temp matrix
    int new_m[new_rows][new_cols];
    calculate(rows1, cols1, m1, rows2, cols2, m2, new_rows, new_cols, new_m, sign); 

    return EXIT_SUCCESS;
}

void load_size(int *r_cnt, int *c_cnt)  // scan the size info from the input
{
    if (scanf("%d %d", r_cnt, c_cnt) != 2)
    {
        return_error();
    }
}

void load_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (scanf("%d", &matrix[r][c]) != 1)
            {
                return_error();
            }
        }
    }
}

void load_sign(char *ch) // load the operator between matrices
{
    getchar();           // get rid of a new line character
    if ((scanf("%c", ch) != 1) || !(*ch == '+' || *ch == '-' || *ch == '*'))
    {
        return_sign_error();
    }
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


void pick_measurements(int rows1, int cols1, int rows2, int cols2, 
                       char sign, int *new_rows, int *new_cols)
{
    if (check_size(rows1, cols1, rows2, cols2, sign))
    {
        if (sign == '+' || sign == '-')
        {
            *new_rows = rows1; // size of a new matrix does not change
            *new_cols = cols1;
        }
        else if (sign == '*')
        {
            *new_rows = rows1; // size of a new matrix is now set
            *new_cols = cols2;
        }
    }
    else
    {
        return_error();
    }
}

int check_size(int rows1, int cols1, int rows2, int cols2, char sign)
{
    if (sign == '*')
    {
        if (cols1 == rows2)   // do matrices obey the rule for 
        {                     // matrix multiplication?
            return true;
        }
        else 
            return false;
    }
    else if (sign == '+' || sign == '-')
    {
        if (rows1 == rows2 && cols1 == cols2) // do matrices have the 
        {                                     // same measurements?
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void print_size(int rows, int cols)
{
    printf("%d %d\n", rows, cols);
}

void print_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < (cols-1); c++)
        {
            printf("%d ", matrix[r][c]);
        }
        printf("%d", matrix[r][cols-1]); // last number without space

        printf("\n");
    }
}

void print_matrix2(const struct matrix* const m)
{
    if (m && m->values)
    {
        for (int r = 0; r < m->rows; r++)
        {
            for (int c = 0; c < ((m->cols)-1); c++)
            {
                printf("%d ", m->values[r * m->cols + c]);
            }
        printf("\n");
        }
    }
}

void calculate(int rows1, int cols1, int m1[rows1][cols1], 
               int rows2, int cols2, int m2[rows2][cols2], 
               int new_rows, int new_cols, int new_mtx[new_rows][new_cols],
               char sign)
{
    if (sign == '+' || sign == '-')  // in case of summation, call the sum function
    {                                // and pass it the + or - sign accordingly
        sum(rows1, cols1, m1, m2, new_mtx, sign);
        print_size(rows1, cols1);
        print_matrix(rows1, cols1, new_mtx);
    }
    else if (sign == '*')
    {
        multiply(rows1, cols1, m1, rows2, cols2, m2, new_mtx);
        print_size(new_rows, new_cols);
        print_matrix(new_rows, new_cols, new_mtx);
    }
}

void sum(int rows1, int cols1, int m1[rows1][cols1], int m2[rows1][cols1], int new_m[rows1][cols1],  char sign)
{   
    int operator = sign == '+'? 1:-1;  // addition or subtraction switch 

    for (int r = 0; r < rows1; r++)
    {
        for (int c = 0; c < cols1; c++)
        {
            new_m[r][c] = m1[r][c] + (operator * m2[r][c]); 
            
        }
    }
}

void multiply(int rows1, int cols1, int m1[rows1][cols1], 
              int rows2, int cols2, int m2[rows2][cols2], int new_m[rows1][cols2])
{
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < cols2; j++)
        {
            int sum = 0;
            for (int k = 0; k < cols1; k++)   // summing row and column of two matrices
            {                                 
                sum += (m1[i][k] * m2[k][j]);   
            }
            new_m[i][j] = sum;                // resulting value for the new matrix position 
        }
    }
}



