#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functii_criptare.h"
#include "functii_cifre.h"

int main()
{
    char *original, *criptat, *decriptat, *key, *marcat;
    original = (char *) malloc(30);
    criptat = (char*) malloc(30);
    decriptat = (char*) malloc(30);
    key = (char*) malloc(30);
	marcat = (char*) malloc(30);

	float precizie;

    FILE *f = fopen("date.txt", "r");
    if (f == NULL)
    {
        printf("Nu s-a putut gasi fisierul \"date.txt\"");
        exit(1);
    }

    fscanf(f, "%s", original);
    fscanf(f, "%s", criptat);
    fscanf(f, "%s", decriptat);
    fscanf(f, "%s", marcat);
    fscanf(f, "%s", key);
    fscanf(f, "%f", &precizie);

    fclose(f);

    // criptare

    criptare(original, criptat, key);

    decriptare(criptat, decriptat, key);

    print_chi(original);
    print_chi(criptat);

    free(original);
    free(criptat);
    free(key);

    // template matching

	imagine img_test = citire_imagine(decriptat);

    colorare_imagine(&img_test, precizie);

	scriere_imagine(marcat, img_test);

	free(decriptat);
	free(img_test.header);
    free(img_test.pixel);
	free(marcat);

    return 0;
}
