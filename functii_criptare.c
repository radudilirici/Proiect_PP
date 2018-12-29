#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned char *header;
    unsigned char *pixel;
}imagine;

void swap_u(unsigned int *a,unsigned int *b)
{
    unsigned int aux = *a;
    *a = *b;
    *b = aux;
}

unsigned int *XoRshift32(unsigned int seed, unsigned int n)
{
    unsigned int *to_fill, r = seed;
    int i;
    to_fill = (unsigned int*) malloc(n * sizeof(unsigned int));
    if (to_fill == NULL)
    {
        printf("Nu mai e memorie pentru vectorul de numere pseudo-randmo");
        exit(1);
    }

    for (i = 0; i < n; i++)
    {
        r ^= r<<13;
        r ^= r>>17;
        r ^= r<<5;
        to_fill[i] = r;
    }

    return to_fill;
}

unsigned int width_from_header(unsigned char *header)
{
    typedef union{
        unsigned int width;
        unsigned char a[4];
    } tip;

    tip x;

    x.a[0] = header[18];
    x.a[1] = header[19];
    x.a[2] = header[20];
    x.a[3] = header[21];

    return x.width;
}

unsigned int height_from_header(unsigned char *header)
{
    typedef union{
        unsigned int height;
        unsigned char a[4];
    } tip;

    tip x;

    x.a[0] = header[22];
    x.a[1] = header[23];
    x.a[2] = header[24];
    x.a[3] = header[25];

    return x.height;
}

unsigned int width_value(FILE *f)
{
    unsigned int width;

    fseek(f, 18, SEEK_SET);
    fread(&width, sizeof(unsigned int), 1, f);
    fseek(f, 0, SEEK_SET);

    return width;
}

unsigned int height_value(FILE *f)
{
    unsigned int height;

    fseek(f, 22, SEEK_SET);
    fread(&height, sizeof(unsigned int), 1, f);
    fseek(f, 0, SEEK_SET);

    return height;
}

imagine new_imagine(unsigned int width, unsigned int height)
{
    imagine to_return;
    to_return.header = (unsigned char*) malloc(54);
    to_return.pixel = (unsigned char*) malloc(3 * width * height);
    if (to_return.header == NULL || to_return.pixel == NULL)
    {
        printf("Nu mai e memorie pentru imagine noua");
        exit(1);
    }
    return to_return;
}

imagine citire_imagine(char *path)
{
    FILE *f = fopen(path, "rb");

    if (f == 0)
    {
        printf("Nu s-a putut deschide imaginea %s\n", path);
        fclose(f);
        exit(1);
    }

    unsigned int width = width_value(f);
    unsigned int height = height_value(f);

    imagine img_test = new_imagine(width, height);

    fread(img_test.header, 1, 54, f);

    int padding;
    if (width % 4 != 0)
        padding = 4 - (3 * width) % 4;
    else
        padding = 0;

    int i;
    for (i = 0; i < height; i++)
    {
        fread(&img_test.pixel[3 * width * i], 1, 3 * width , f);
        fseek(f, padding, SEEK_CUR);
    }

    fclose(f);
    return img_test;
}

void scriere_imagine(char *path, imagine img_test)
{
    FILE *g = fopen(path, "wb");

    if (g == NULL)
    {
        printf("Nu s-a putut crea imaginea %s\n", path);
        fclose(g);
        exit(1);
    }

    unsigned int width = width_from_header(img_test.header);
    unsigned int height = height_from_header(img_test.header);

    fwrite(img_test.header, 1, 54, g);

    int padding;
    if (width % 4 != 0)
        padding = 4 - (3 * width) % 4;
    else
        padding = 0;

    unsigned char zero = 0;
    int i;
    for (i = 0; i < height; i++)
    {
        fwrite(&img_test.pixel[3 * width * i], 1, 3 * width , g);
        fflush(g);
        fwrite(&zero, 1, padding, g);
    }

    fclose(g);
}

unsigned int *generare_permutare(unsigned int random_keys[], int n)
{
	unsigned int *permutare;
	int i, r;
	permutare = (unsigned int*) malloc(n * sizeof(unsigned int));
	if (permutare == NULL)
    {
        printf("Nu mai e memorie pentru permutare");
        exit(1);
    }

    for (i = 0; i < n; i++)
        permutare[i] = i;

    for (i = n-1; i > 0; i--)
    {
        r = random_keys[n-i-1] % (i+1);
        swap_u(&permutare[i], &permutare[r]);
    }
    return permutare;
}

imagine permutare_pixeli(imagine img_test, unsigned int random_keys[], int width, int height)
{
    unsigned int k, *permutare;
    int i;

    permutare = generare_permutare(random_keys, width * height);

    imagine img_perm = new_imagine(width, height);

    for (i = 0; i < 54; i++)
        img_perm.header[i] = img_test.header[i];

    for (i = 0; i < width * height; i++)
    {
        k = permutare[i];
        img_perm.pixel[3 * i] = img_test.pixel[3 * k];
        img_perm.pixel[3 * i + 1] = img_test.pixel[3 * k + 1];
        img_perm.pixel[3 * i + 2] = img_test.pixel[3 * k + 2];
    }

    free(permutare);
    return img_perm;
}

imagine permutare_inversa_pixeli(imagine img_test, unsigned int random_keys[], int width, int height)
{
    unsigned int k, *permutare;
    int i;

    permutare = generare_permutare(random_keys, width * height);

    imagine img_perm_inv = new_imagine(width, height);

    for (i = 0; i < 54; i++)
        img_perm_inv.header[i] = img_test.header[i];

    for (i = 0; i < width * height; i++)
    {
        k = permutare[i];
        k = permutare[i];
        img_perm_inv.pixel[3 * k] = img_test.pixel[3 * i];
        img_perm_inv.pixel[3 * k + 1] = img_test.pixel[3 * i + 1];
        img_perm_inv.pixel[3 * k + 2] = img_test.pixel[3 * i + 2];
    }

    free(permutare);
    return img_perm_inv;
}

unsigned char *u_int_to_3_u_char(unsigned int x)
{
    unsigned char *a, x2, x1, x0;
    a = (unsigned char*) &x;
    a = a + 1;
    x0 = *a;
    a = a + 1;
    x1 = *a;
    a = a + 1;
    x2 = *a;

    a = (unsigned char*) malloc(3);
    a[0] = x0;
    a[1] = x1;
    a[2] = x2;

    return a;
}

unsigned char *XOR_pixeli(unsigned char P1[3], unsigned char P2[3])
{
    unsigned char *Pxor = (unsigned char*) malloc(3);
    Pxor[0] = P1[0] ^ P2[0];
    Pxor[1] = P1[1] ^ P2[1];
    Pxor[2] = P1[2] ^ P2[2];
    return Pxor;
}

imagine XOR_culori(imagine img_test, unsigned int SV, unsigned int random_keys[], int width, int height)
{
    imagine criptata = new_imagine(width, height);

    int k;

    for (k = 0; k < 54; k++)
        criptata.header[k] = img_test.header[k];

    for (k = 0; k < width * height; k++)
        criptata.pixel[k] = img_test.pixel[k];

    unsigned char *cSV = (unsigned char*) malloc(3);
    unsigned char *Rwh = (unsigned char*) malloc(3);
    unsigned char *Ck1 = (unsigned char*) malloc(3);
    unsigned char *Pk = (unsigned char*) malloc(3);
    unsigned char *aux = (unsigned char*) malloc(3);

    cSV = u_int_to_3_u_char(SV);
    Rwh = u_int_to_3_u_char(random_keys[width * height]);

    Pk[0] = img_test.pixel[0];
    Pk[1] = img_test.pixel[1];
    Pk[2] = img_test.pixel[2];

    aux = XOR_pixeli(cSV, Pk);
    aux = XOR_pixeli(aux, Rwh);

    criptata.pixel[0] = aux[0];
    criptata.pixel[1] = aux[1];
    criptata.pixel[2] = aux[2];

    for (k = 1; k < width * height; k++)
    {
        Rwh = u_int_to_3_u_char(random_keys[width * height + k]);

        Ck1[0] = criptata.pixel[3 * (k - 1)];
        Ck1[1] = criptata.pixel[3 * (k - 1) + 1];
        Ck1[2] = criptata.pixel[3 * (k - 1) + 2];

        Pk[0] = img_test.pixel[3 * k];
        Pk[1] = img_test.pixel[3 * k + 1];
        Pk[2] = img_test.pixel[3 * k + 2];

        aux = XOR_pixeli(Ck1, Pk);
        aux = XOR_pixeli(aux, Rwh);

        criptata.pixel[3 * k] = aux[0];
        criptata.pixel[3 * k + 1] = aux[1];
        criptata.pixel[3 * k + 2] = aux[2];
    }

    free(cSV);
    free(Rwh);
    free(Ck1);
    free(Pk);
    free(aux);
    return criptata;
}

imagine XOR_culori_invers(imagine criptata, unsigned int SV, unsigned int random_keys[], int width, int height)
{
    imagine decriptata = new_imagine(width, height);

    int k;

    for (k = 0; k < 54; k++)
        decriptata.header[k] = criptata.header[k];

    for (k = 0; k < width * height; k++)
        decriptata.pixel[k] = criptata.pixel[k];

    unsigned char *cSV = (unsigned char*) malloc(3);
    unsigned char *Rwh = (unsigned char*) malloc(3);
    unsigned char *Ck1 = (unsigned char*) malloc(3);
    unsigned char *Pk = (unsigned char*) malloc(3);
    unsigned char *aux = (unsigned char*) malloc(3);

    cSV = u_int_to_3_u_char(SV);
    Rwh = u_int_to_3_u_char(random_keys[width * height]);

    Pk[0] = criptata.pixel[0];
    Pk[1] = criptata.pixel[1];
    Pk[2] = criptata.pixel[2];

    aux = XOR_pixeli(cSV, Pk);
    aux = XOR_pixeli(aux, Rwh);

    decriptata.pixel[0] = aux[0];
    decriptata.pixel[1] = aux[1];
    decriptata.pixel[2] = aux[2];

    for (k = 1; k < width * height; k++)
    {
        Rwh = u_int_to_3_u_char(random_keys[width * height + k]);

        Ck1[0] = criptata.pixel[3 * (k - 1)]; // diferenta e ca pe Ck1m il luam din imaginea initiala
        Ck1[1] = criptata.pixel[3 * (k - 1) + 1];
        Ck1[2] = criptata.pixel[3 * (k - 1) + 2];

        Pk[0] = criptata.pixel[3 * k];
        Pk[1] = criptata.pixel[3 * k + 1];
        Pk[2] = criptata.pixel[3 * k + 2];

        aux = XOR_pixeli(Ck1, Pk);
        aux = XOR_pixeli(aux, Rwh);

        decriptata.pixel[3 * k] = aux[0];
        decriptata.pixel[3 * k + 1] = aux[1];
        decriptata.pixel[3 * k + 2] = aux[2];
    }

    free(cSV);
    free(Rwh);
    free(Ck1);
    free(Pk);
    free(aux);
    return decriptata;
}

void criptare(char *original, char *criptat, char *cheie)
{
    imagine img_test = citire_imagine(original);

    unsigned width = width_from_header(img_test.header);
    unsigned height = height_from_header(img_test.header);

    FILE *k = fopen(cheie, "r");
    if (k == NULL)
    {
        printf("Nu s-a putut deschide imaginea %s in functia criptare\n", cheie);
        fclose(k);
        exit(1);
    }

    unsigned int seed, SV;
    fscanf(k, "%u", &seed);
    fscanf(k, " %u", &SV);
    fclose(k);

    unsigned int *random_keys = XoRshift32(seed, 2 * width * height);

    imagine img_perm = permutare_pixeli(img_test, random_keys, width, height);
    free(img_test.header);
    free(img_test.pixel);
    imagine img_xor = XOR_culori(img_perm, SV, random_keys, width, height);
    free(img_perm.header);
    free(img_perm.pixel);

    free(random_keys);

    scriere_imagine(criptat, img_xor);
    free(img_xor.header);
    free(img_xor.pixel);
}

void decriptare(char *criptat, char *decriptat, char *cheie)
{
    imagine img_test = citire_imagine(criptat);

    unsigned width = width_from_header(img_test.header);
    unsigned height = height_from_header(img_test.header);

    FILE *k = fopen(cheie, "r");
    if (k == NULL)
    {
        printf("Nu s-a putut deschide imaginea %s in functia decriptare", cheie);
        exit(1);
    }

    unsigned int seed, SV;
    fscanf(k, "%u", &seed);
    fscanf(k, "%u", &SV);
    fclose(k);
    unsigned int *random_keys = XoRshift32(seed, 2 * width * height);

    imagine img_xor = XOR_culori_invers(img_test, SV, random_keys, width, height);
    free(img_test.header);
    free(img_test.pixel);
    imagine img_perm = permutare_inversa_pixeli(img_xor, random_keys, width, height);
    free(img_xor.header);
    free(img_xor.pixel);

    free(random_keys);

    scriere_imagine(decriptat, img_perm);
    free(img_perm.header);
    free(img_perm.pixel);
}

void print_chi(char *imagine)
{
    FILE *f = fopen(imagine, "rb");
    if (f == NULL)
    {
        printf("Nu s-a putut deschide imaginea %s in functia de calculare a testului\n", imagine);
        fclose(f);
        exit(1);
    }
    unsigned int width = width_value(f);
    unsigned int height = height_value(f);
    fseek(f, 54, SEEK_SET);

    int k, *fR, *fG, *fB;
    double f_barat, xR, xG, xB;
    unsigned char aux;
    unsigned int i;
    f_barat = (double) (width * height) / 256;
    xR = xG = xB = 0;
    fR = (int*) calloc(256, sizeof(int));
    fG = (int*) calloc(256, sizeof(int));
    fB = (int*) calloc(256, sizeof(int));

    k = 1;
    while (fread(&aux, 1, 1, f) == 1)
    {
        i = (unsigned int) aux;
        if (k == 1) //Blue
            fB[i]++;
        if (k == 2) //Green
            fG[i]++;
        if (k == 3) //Red
            fR[i]++;
        k++;
        if (k == 4)
            k = 1;
    }

    for (k=0; k<256; k++)
    {
        xR += (fR[k]-f_barat)*(fR[k]-f_barat) / f_barat;
        xG += (fG[k]-f_barat)*(fG[k]-f_barat) / f_barat;
        xB += (fB[k]-f_barat)*(fB[k]-f_barat) / f_barat;
    }

    printf("%.2lf %.2lf %.2lf\n", xR, xG, xB);

    free(fR);
    free(fG);
    free(fB);
    fclose(f);
}
