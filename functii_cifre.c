#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "functii_criptare.h"

int f_gri, f_medie, f_omega, f_corr;

imagine grayscale(imagine img_test)
{
    f_gri++;
	unsigned int width = width_from_header(img_test.header);
	unsigned int height = height_from_header(img_test.header);

	imagine gray = new_imagine(width, height);

	int k;

	for (k = 0; k < 54; k++)
		gray.header[k] = img_test.header[k];

	for (k = 0; k < width * height; k++)
	{
		double B = img_test.pixel[3 * k];
		double G = img_test.pixel[3 * k + 1];
		double R = img_test.pixel[3 * k + 2];

		R = 0.229 * R + 0.587 * G + 0.114 * B;

		gray.pixel[3 * k] = gray.pixel[3 * k + 1] = gray.pixel[3 * k + 2] = R;
	}
	return gray;
}

double medie_pixeli(imagine img_test)
{
    f_medie++;
	double medie = 0;
	unsigned int width = width_from_header(img_test.header);
	unsigned int height = height_from_header(img_test.header);

	int k, n = width * height;
	for (k = 0; k < n; k++)
		medie += img_test.pixel[3 * k];

	medie /= n;
	return medie;
}

double omega(imagine img_test, unsigned int width, unsigned int height, double medie)
{
    f_omega++;
	double o = 0;

	int k, i, j, pozitie;
	int n = width * height;
	unsigned int full_width = width_from_header(img_test.header);

	for (k = 0; k < n; k++)
	{
		i = k / width;
		j = k % width;
		pozitie = i * full_width + j;

		o += (img_test.pixel[3 * pozitie] - medie) * (img_test.pixel[3 * pozitie] - medie);
	}

	o /= n-1;
	o = sqrt(o);

	return o;
}

double corr(imagine *img_test, imagine sablon, int shift, double omega_f, double omega_S, double f_barat, double S_barat)
{
    f_corr++;
	unsigned width_test = width_from_header((*img_test).header);
	unsigned height_test = height_from_header((*img_test).header);
	unsigned width_sablon = width_from_header(sablon.header);
	unsigned height_sablon = height_from_header(sablon.header);

	double corelatie = 0;

	int k, i, j, pozitie = 0;
	int n = width_sablon * height_sablon;
	for (k = 0; k < n; k++)
	{
		i = k / width_sablon;
		j = k % width_sablon;
		pozitie = i * width_test + j + shift;

		if (pozitie > width_test * height_test)
            break;

		corelatie += ((*img_test).pixel[3 * pozitie] - f_barat) * (sablon.pixel[3 * k] - S_barat);
	}

	corelatie /= (omega_f * omega_S);
	corelatie /= n;
	return corelatie;
}

typedef struct
{
	int coord, width, height, cifra;
	double corelatie;
}fereastra;

typedef struct
{
	int nr_elem;
	fereastra *fi;
}detectii;

detectii template_matching(imagine img_test, imagine sablon, double prag, int cifra_crt)
{
	unsigned width_test = width_from_header(img_test.header);
	unsigned height_test = height_from_header(img_test.header);
	unsigned width_sablon = width_from_header(sablon.header);
	unsigned height_sablon = height_from_header(sablon.header);

	double f_barat = medie_pixeli(img_test);
	double S_barat = medie_pixeli(sablon);

	double omega_f = omega(img_test, width_sablon, height_sablon, f_barat);
	double omega_S = omega(sablon, width_sablon, height_sablon, S_barat);

	detectii D;
	int MAX_DET = 10000;
	D.fi = (fereastra*) malloc(MAX_DET * sizeof(fereastra));

	int i, k = 0;
	for (i = 0; i < width_test * height_test; i++)
    {
        if (k >= MAX_DET)
            break;
        double corelatie = corr(&img_test, sablon, i, omega_f, omega_S, f_barat, S_barat);

        if (corelatie >= prag)
        {
            D.fi[k].coord = i;
            D.fi[k].width = width_sablon;
            D.fi[k].height = height_sablon;
            D.fi[k].corelatie = corelatie;
            D.fi[k].cifra = cifra_crt;
            k++;
        }
    }

	D.nr_elem = k;
	return D;
}

void colorare_chenar(imagine *img_test, fereastra fi, unsigned char culoare[3])
{
    unsigned int width = width_from_header((*img_test).header);
    unsigned int height = height_from_header((*img_test).header);

    int i, j, k;
    for (i = fi.coord; i - fi.coord < fi.width; i++)
    {
        if (i < width * height)
        {
            (*img_test).pixel[3 * i] = culoare[0];
            (*img_test).pixel[3 * i + 1] = culoare[1];
            (*img_test).pixel[3 * i + 2] = culoare[2];
        }

        j = i + width * fi.height;

        if (j < width * height)
        {
            (*img_test).pixel[3 * j] = culoare[0];
            (*img_test).pixel[3 * j + 1] = culoare[1];
            (*img_test).pixel[3 * j + 2] = culoare[2];
        }
    }

    for (i = fi.coord, k = 0; k < fi.height; i += width, k++)
    {
        if (i < width * height)
        {
            (*img_test).pixel[3 * i] = culoare[0];
            (*img_test).pixel[3 * i + 1] = culoare[1];
            (*img_test).pixel[3 * i + 2] = culoare[2];
        }

        j = i + fi.width - 1;

        if (j < width * height)
        {
            (*img_test).pixel[3 * j] = culoare[0];
            (*img_test).pixel[3 * j + 1] = culoare[1];
            (*img_test).pixel[3 * j + 2] = culoare[2];
        }
    }
}

int comp(const void *a, const void *b)
{
    fereastra x = *(fereastra*) a;
    fereastra y = *(fereastra*) b;
    if (x.corelatie > y.corelatie)
        return -1;
    return 1;
}

void sortare(detectii *D)
{
    qsort((*D).fi, (*D).nr_elem - 1, sizeof(fereastra), comp);
}

double arie(fereastra fi)
{
    return (double) (fi.width * fi.height);
}

double suprapunere(fereastra fi1, fereastra fi2, unsigned int image_width)
{
    int i1, i2, j1, j2;
    i1 = fi1.coord / image_width;
    j1 = fi1.coord % image_width;
    i2 = fi2.coord / image_width;
    j2 = fi2.coord % image_width;

    fereastra intersectie;

    if (i1 > i2)
        intersectie.height = i2 + fi2.height - i1;
    else
        intersectie.height = i1 + fi1.height - i2;

    if (j1 > j2)
        intersectie.width = j2 + fi2.width - j1;
    else
        intersectie.width = j1 + fi1.width - j2;

    if (intersectie.width < 0 || intersectie.height < 0)
        return 0;

    double arie_suprapunere = arie(intersectie) / (arie(fi1) + arie(fi2) - arie(intersectie));
    return arie_suprapunere;
}

void eliminare_non_maxime(detectii *D, unsigned int image_width)
{
    int i, j, k;
    for (i = 0; i < (*D).nr_elem - 1; i++)
        for (j = i + 1; j < (*D).nr_elem; j++)
            if (suprapunere((*D).fi[i], (*D).fi[j], image_width) > 0.2)
            {
                for (k = j; k < (*D).nr_elem - 1; k++)
                {
                    (*D).fi[k].coord = (*D).fi[k + 1].coord;
                    (*D).fi[k].width = (*D).fi[k + 1].width;
                    (*D).fi[k].height = (*D).fi[k + 1].height;
                    (*D).fi[k].cifra = (*D).fi[k + 1].cifra;
                    (*D).fi[k].corelatie = (*D).fi[k + 1].corelatie;
                }
                j--;
                (*D).nr_elem--;
            }
}

void adaugare_detectii(detectii *D1, detectii D2)
{
    (*D1).fi = (fereastra*) realloc((*D1).fi, ((*D1).nr_elem + D2.nr_elem) * sizeof(fereastra));
    int i;
    for (i = 0; i < D2.nr_elem; i++)
        (*D1).fi[i + (*D1).nr_elem] = D2.fi[i];
    (*D1).nr_elem += D2.nr_elem;
}

void colorare_imagine(imagine *img_test, double prag)
{
    char **pathuri_sablon = (char**) malloc(10 * sizeof(char*));
    int i;
    for (i = 0; i < 10; i++)
        pathuri_sablon[i] = (char*) malloc(10 * sizeof(char));
    strcpy(pathuri_sablon[0], "cifra0.bmp");
    strcpy(pathuri_sablon[1], "cifra1.bmp");
    strcpy(pathuri_sablon[2], "cifra2.bmp");
    strcpy(pathuri_sablon[3], "cifra3.bmp");
    strcpy(pathuri_sablon[4], "cifra4.bmp");
    strcpy(pathuri_sablon[5], "cifra5.bmp");
    strcpy(pathuri_sablon[6], "cifra6.bmp");
    strcpy(pathuri_sablon[7], "cifra7.bmp");
    strcpy(pathuri_sablon[8], "cifra8.bmp");
    strcpy(pathuri_sablon[9], "cifra9.bmp");

    unsigned char **culoare = (unsigned char**) malloc(10 * sizeof(unsigned char*));
    for (i = 0; i < 10; i++)
        culoare[i] = (unsigned char*) (malloc(3 * sizeof(unsigned char)));

	culoare[0][2] = 255;
	culoare[0][1] = 0;
	culoare[0][0] = 0;

	culoare[1][2] = 255;
	culoare[1][1] = 255;
	culoare[1][0] = 0;

	culoare[2][2] = 0;
	culoare[2][1] = 255;
	culoare[2][0] = 0;

	culoare[3][2] = 0;
	culoare[3][1] = 255;
	culoare[3][0] = 255;

	culoare[4][2] = 255;
	culoare[4][1] = 0;
	culoare[4][0] = 255;

	culoare[5][2] = 0;
	culoare[5][1] = 0;
	culoare[5][0] = 255;

	culoare[6][2] = 192;
	culoare[6][1] = 192;
	culoare[6][0] = 192;

	culoare[7][2] = 255;
	culoare[7][1] = 140;
	culoare[7][0] = 0;

	culoare[8][2] = 128;
	culoare[8][1] = 0;
	culoare[8][0] = 128;

	culoare[9][2] = 128;
	culoare[9][1] = 0;
	culoare[9][0] = 0;

	imagine img_gray = grayscale(*img_test);

	imagine sablon;
    detectii D, D_aux;
    D.fi = (fereastra*) malloc(0);
    D.nr_elem = 0;

    for (i = 0; i < 10; i ++)
    {

        sablon = citire_imagine(pathuri_sablon[i]);
        D_aux = template_matching(img_gray, sablon, prag, i);
        adaugare_detectii(&D, D_aux);
        free(D_aux.fi);
        free(sablon.header);
        free(sablon.pixel);
    }
    free(img_gray.header);
    free(img_gray.pixel);

    sortare(&D);

    unsigned int width = width_from_header((*img_test).header);
    eliminare_non_maxime(&D, width);

    for (i = 0; i < D.nr_elem; i++)
        colorare_chenar(img_test, D.fi[i], culoare[D.fi[i].cifra]);

    free(D.fi);
}
