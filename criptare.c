#include <stdio.h>
#include <stdlib.h>
#include "functii_criptare.h"

int main()
{
    char *original, *criptat, *decriptat, *key, *hi;
    original = (char *) malloc(100);
    criptat = (char*) malloc(100);
    decriptat = (char*) malloc(100);
    key = (char*) malloc(100);
    hi = (char*) malloc(100);

    printf("Scrieti numele fisierului cu imaginea sursa: ");
    scanf("%s", original);
    printf("Scrieti numele fisierului pentru imaginea criptata: ");
    scanf("%s", criptat);
    printf("Scrieti numele fisierului cu cheia secreta: ");
    scanf("%s", key);

    criptare(original, criptat, key);

    printf("Criptare reusita!\n");

    printf("Scrieti numele fisierului cu imaginea criptata: ");
    scanf("%s", criptat);
    printf("Scrieti numele fisierului pentru imaginea decriptata: ");
    scanf("%s", decriptat);
    printf("Scrieti numele fisierului cu cheia secreta: ");
    scanf("%s", key);

    decriptare(criptat, decriptat, key);

    printf("Decriptare reusita!\n");

    printf("Scrieti numele fisierului pentru calcularea testului: ");
    scanf("%s", hi);

    print_hi(hi);

    free(original);
    free(criptat);
    free(decriptat);
    free(hi);

    return 0;
}