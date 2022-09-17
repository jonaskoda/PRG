#include <stdio.h>
#include <stdlib.h>

#define WRONG_INPUT 100
#define OUT_OF_INTERVAL 101
#define WIDTH_NOT_ODD 102
#define WRONG_FENCE_SIZE 103

//functions
void print_pattern(int line,int j);
void print_inside_walls (int width, int line);
void print_fence (int fence, int row);
void print_walls_fence(int width, int height, int fence);
void print_roof (int width);
void print_walls (int width, int height);
void print_base (int width, int height);
void end_of_line(void);
int checker100 (int input_count);
int checker101 (int width, int height);
int checker102 (int width);
int checker103 (int height, int fence);

// The main program 
int main(int argc, char *argv[])
{
  int width, height, input_count;

  input_count = scanf("%d %d", &width, &height);
  
  if (checker100(input_count) == WRONG_INPUT)
  {
    return WRONG_INPUT;
  }
  if (checker101(width, height) == OUT_OF_INTERVAL)
  {
    return OUT_OF_INTERVAL;
  }
  if (checker102(width) == WIDTH_NOT_ODD)
  {
    return WIDTH_NOT_ODD;
  }
  
  if (width == height) //pripad pro dum s plotem
  {
    int input_counter, fence;
    input_counter = scanf("%d", &fence);

    if (input_counter != 1)
    {
      fprintf(stderr, "Error: Chybny vstup!\n");
      return WRONG_INPUT;
    }
    if (checker101(width, height) == 101)
    {
      return OUT_OF_INTERVAL;
    }
    if (checker103(height, fence) == 103)
    {
      return WRONG_FENCE_SIZE;
    }
    
    print_roof(width);
    print_base(width, height);
    end_of_line();
    print_walls_fence(width, height, fence);
    print_base(width, height);
    print_fence(fence, 0);
    end_of_line();

  }
  else //pripad pro dum bez plotu
  {
  print_roof(width);
  print_base(width, height);
  end_of_line();
  print_walls(width, height);
  print_base(width, height);
  end_of_line();
  }

  return EXIT_SUCCESS;
}

//funkce pro tisk vzoru uvnitr domecku, "*" a "o"
//vytiskne vzdy jen jeden znak na jedno zavolani 
void print_pattern(int line,int j)       
{                                        
  if (((line % 2) == 0 && (j % 2) == 1) || ((line % 2) == 1 && (j % 2) == 0))         
  {                         //jedna se bud o sudou line a liche policko                                                      
    printf("o");            //nebo o lichou line a sude policko     
  }
  if (((line % 2) == 0 && (j % 2) == 0) || ((line % 2) == 1 && (j % 2) == 1))        
  {                         //jedna se bud o sudou line a sude policko                                                         
    printf("*");            //nebo o lichou line a liche policko
  }
}

void print_inside_walls (int width, int line) //funkce pro tisk steny s vyplni
{
  for (int j = 0; j < width; j++)
  {
    if (j == 0 || j == (width-1))
    {
      printf("X");
    }
    else
    {
      print_pattern(line, j);
    }
  }
}

void print_fence (int fence, int row) //funkce pro tisk plotu
{
  int k;
  if (fence % 2 == 0)  //ma plot lichy nebo sudy pocet bloku?
  {
    k = 0;
  }
  else
  {
    k = 1;
  }
  for (int i = 0; i < fence; i++)
  {
    if ((i+k) % 2 == 0)
    {
      if (row == 0)    //prvni radek plotu
      {
        printf("-");  
      }
      else
      {
        printf(" ");
      }
    }
    else
    {
      printf("|");
    }
  }
}

void print_walls_fence(int width, int height, int fence) //funkce pro tisk domu s plotem
{
  for (int i = 0; i < height-2; i++)
  {
    if (i < ((height-1) - fence))   //pokud se jedna o patra nad plotem, tiskni stenu s vyplni
    {
      print_inside_walls(width, i);
      printf("\n");
    }
    if (i >= ((height-1) - fence))   //pokud se jedna o vysku, ve ktere zacina plot
    {                               
      if (i == ((height-1) - fence)) //vytiskni hranicni cast plotu
      {
        print_inside_walls(width, i);
        print_fence(fence, 0);
      }
      else                           //vytiskni stredni cast plotu
      {
        print_inside_walls(width, i);
        print_fence(fence, 1);
      }
      printf("\n");
    }
  }
}

void print_roof (int width) //funkce pro tisk strechy
{
  int mid = (width+1)/2;

  for (int i = 0; i < (width-1)/2; i++) 
  {
    for (int j = 0; j < mid+i; j++)
    {
      if (j == (mid-1)+i || j == (mid-1)-i) //pokud jde o stred nebo symetricke souradnice
      {
        printf("X");    
      }
      else
      {
        printf(" ");
      }
    }
    printf("\n");
  }
}

void print_walls (int width, int height) //funkce pro tisk sten standardniho domu bez vyplne
{
  for (int i = 0; i < height-2; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (j == 0 || j == (width-1))
      {
        printf("X");
      }
      else
      {
        printf(" ");
      }
    }
    printf("\n");
  }
}

void print_base (int width, int height) //funkce pro tisk stropu a podlahy
{
  for (int i = 0; i < width; i++)
    {
      printf("X");
    }
}   

void end_of_line(void) 
{
  printf("\n");
}

//funkce pro kontrolu vstupu
int checker100 (int input_count) 
{
  if (input_count != 2)
  {
    fprintf(stderr,"Error: Chybny vstup!\n");
    return WRONG_INPUT;
  }
  else
  {
    return 0;
  }
}

int checker101 (int width, int height)
{
  if ((width > 69 || width < 3) || (height > 69 || height < 3))
  {
    fprintf(stderr,"Error: Vstup mimo interval!\n");
    return OUT_OF_INTERVAL;
  }
  else
  {
    return 0;
  }
}

int checker102 (int width)
{
  if (width % 2 == 0)
  {
    fprintf(stderr,"Error: Sirka neni liche cislo!\n");
    return WIDTH_NOT_ODD;
  }
  else
  {
    return 0;
  }
}

int checker103 (int height, int fence)
{
  if (fence < 1 || fence >= height)
    {
      fprintf(stderr, "Error: Neplatna velikost plotu!\n");
      return WRONG_FENCE_SIZE;
    }
  else
  {
    return 0;
  }
}


