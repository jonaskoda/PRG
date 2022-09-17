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
int **create_temp_mtx(int rows1, int cols1, int rows2, int cols2, char sign);
bool check_mtx_sizes(int rows1, int cols1, int rows2, int cols2, char sign);
void calculate(int m1[], int m2[], char sign, int temp_matrix[]);
int **allocate_matrix(int rows, int cols);

// add
// subtract
// mutiply
void print_matrix(int rows, int cols, int matrix[rows][cols]); 


// The main program 
int main(int argc, char *argv[])
{
    int rows1, cols1;
    int rows2, cols2;
    char sign;
    rows1 = cols1 = rows2 = cols2 = 1;
    int **temp_matrix;

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
    temp_matrix = create_temp_mtx(rows1, cols1, rows2, cols2, sign);
    
    //calculate(m1, m2, sign, temp_matrix); //return a modified temp matrix

    print_matrix(rows1, cols1, m1);
    print_matrix(rows2, cols2, m2);

    return EXIT_SUCCESS;
}

void load_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (scanf("%d", &matrix[r][c]) != 1)
            {
                return_matrix_error();
            }
            getchar();
        }
    }
}

void load_size(int *r_cnt, int *c_cnt)
{
    if (scanf("%d %d", r_cnt, c_cnt) != 2)
    {
        return_size_error();
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

void load_sign(char *ch)
{
    if (scanf("%c", ch) != 1 || *ch != '+' || *ch != '-' || *ch != '*')
    {
        return_sign_error();
    }
}

int **create_temp_mtx(int rows1, int cols1, int rows2, int cols2, char sign)
{
    int *temp_rows, *temp_cols, **new_matrix;
    temp_rows = temp_cols = 0;

    if (sign == '+' || sign == '-')
    {
        if (check_mtx_sizes(rows1, cols1, rows2, cols2, sign))
        {
            *temp_rows = rows1;
            *temp_cols = cols1; 
        }
    }
    else  // sign == '*'
    {
        if (check_mtx_sizes(rows1, cols1, rows2, cols2, sign))
        {
            *temp_rows = cols1;
            *temp_cols = rows2;
        }
    }
    // create array and return a pointer to an array
    new_matrix = allocate_matrix(*temp_rows, *temp_cols);
    return new_matrix;
}

bool check_mtx_sizes(int rows1, int cols1, int rows2, int cols2, char sign)
{
    if (sign == '+' || sign == '-')
    {
        if (rows1 == rows2 && cols1 == cols2)
        {
            return true;
        }
    }
    else if (sign == '*')
    {
        if (rows1 == cols2 && cols1 == rows2)
        {
            return true;
        }
    }
    else 
    {
        return false;
    }
}

int **allocate_matrix(int rows, int cols)
{

}

void print_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (printf("%d ", matrix[r][c]) != 1)
            {
                return_matrix_error();
            }
        }
        printf("\n");
    }
}

void calculate(int m1[], int m2[], char sign, int temp_matrix[])
{
    switch (sign)
    {
        case '+':
            //add(m1, m2);
            break;
        case '-':
            //subtract(m1,m2);
            break;
        case '*':
            //multiply(m1, m2);
            break; 
        default:
            return_switch_error();
            break;
    }
}

