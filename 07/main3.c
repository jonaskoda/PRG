#include <stdio.h>
#include <stdlib.h>

int little_big(int v);

int main() {
    FILE* f = fopen("input.bin", "wb");  // otviram soubor, abych do nej mohl neco zapsat
    if (f == NULL) {
        fprintf(stderr, "Soubor se nepodarilo otevrit.\n");
        exit(1);
    }
    int i = little_big(256);
    
    fwrite(&i, sizeof(int), 1, f);
    if (fclose(f) == EOF) {   // kdyz zaviram a nepovede se mi to, tak to vraci EOF
        fprintf(stderr, "Soubor se nepodarilo uzavrit.\n");
        exit(1);
    }    

    f = fopen("input.bin", "rb");
    if (f == NULL) {
        fprintf(stderr, "Soubor se nepodarilo otevrit.\n");
        exit(1);
    }

    while (!feof(f))
    {
        fread(&i, sizeof(int), 1, f);
        if (!feof(f))
        {
            printf("%d\n", i);
        }
    }

    if (fclose(f) == EOF) {
        fprintf(stderr, "Soubor se nepodarilo uzavrit.\n");
        exit(1);
    } 
    
    return 0;
}


int little_big(int v)
{
    int result = 0;
    for (int i = 0; i < 4; i++);
    {
            result |= (v & 0xff) << ((sizeof(int) - (i+1)) *8);
            v >>= 8;
    }
}

