#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum 
{
    SIZE_ERROR = 101,
    MATRIX_ERROR = 102,
    SIGN_ERROR = 103,
    SWITCH_ERROR = 104,
};
// functions
void load_size(int *r_cnt, int *c_cnt);
void load_matrix(int rows, int cols, int matrix[rows][cols]);
void return_size_error(void);
void return_matrix_error();
void return_sign_error();
void return_switch_error();
void load_sign(char *ch);
int check_size(int rows1, int cols1, int rows2, int cols2, char sign);
void calculate(int rows1, int cols1, int m1[rows1][cols1], 
               int rows2, int cols2, int m2[rows2][cols2], 
               int new_rows, int new_cols, int new_mtx[new_rows][new_cols],
               char sign);
void sum(int rows1, int cols1, int m1[rows1][cols1], 
         int m2[rows1][cols1], int new_m[rows1][cols1],  char sign);
void multiply(int rows1, int cols1, int m1[rows1][cols1], 
             int rows2, int cols2, int m2[rows2][cols2], int new_m[rows1][cols2]);
void print_matrix(int rows, int cols, int matrix[rows][cols]); 


// The main program 
int main(int argc, char *argv[])
{
    int rows1, cols1;
    int rows2, cols2;
    int new_rows, new_cols;
    char sign;
    rows1 = cols1 = rows2 = cols2 = new_rows = new_cols =1;

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
    // create temp matrix
    if (check_size(rows1, cols1, rows2, cols2, sign))
    {
        if (sign == '+' || sign == '-')
        {
            new_rows = rows1;
            new_cols = cols1;
        }
        else if (sign == '*')
        {
            new_rows = rows1;
            new_cols = cols2;
        }
    }
    else
    {
        return_size_error();
    }
    int new_m[new_rows][new_cols];
    calculate(rows1, cols1, m1, rows2, cols2, m2, new_rows, new_cols, new_m, sign); 

    return EXIT_SUCCESS;
}

void load_size(int *r_cnt, int *c_cnt)
{
    if (scanf("%d %d", r_cnt, c_cnt) != 2)
    {
        return_size_error();
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
                return_matrix_error();
            }
        }
    }
}

void load_sign(char *ch)
{
    getchar(); // get rid of new line character
    if ((scanf("%c", ch) != 1) || !(*ch == '+' || *ch == '-' || *ch == '*'))
    {
        return_sign_error();
    }
}


void return_size_error(void)
{
    fprintf(stderr, "Invalid size parameters\n");
    exit(SIZE_ERROR);
}

void return_matrix_error()
{
    fprintf(stderr, "Invalid value as a matrix input\n");
    exit(MATRIX_ERROR);
}

void return_sign_error()
{
    fprintf(stderr, "Error, invalid sign of math operator\n");
    exit(SIGN_ERROR);
}

void return_switch_error()
{
    fprintf(stderr, "Error, no matching operator\n");
    exit(SWITCH_ERROR);
}

int check_size(int rows1, int cols1, int rows2, int cols2, char sign)
{
    if (sign == '*')
    {
        if (cols1 == rows2)
        {
            return true;
        }
        else 
            return false;
    }
    else if (sign == '+' || sign == '-')
    {
        if (rows1 == rows2 && cols1 == cols2)
        {
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
    printf("\n");
}

void calculate(int rows1, int cols1, int m1[rows1][cols1], 
               int rows2, int cols2, int m2[rows2][cols2], 
               int new_rows, int new_cols, int new_mtx[new_rows][new_cols],
               char sign)
{
    if (sign == '+' || sign == '-')
    {
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
    int token = sign == '+'? 1:-1;  // addition or subtraction switch 

    for (int r = 0; r < rows1; r++)
    {
        for (int c = 0; c < cols1; c++)
        {
            new_m[r][c] = m1[r][c] + (token * m2[r][c]); 
            
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
            for (int k = 0; k < cols1; k++)
            {
                sum += (m1[i][k] * m2[k][j]);
            }
            new_m[i][j] = sum;
        }
    }
}



