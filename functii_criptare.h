#ifndef CRIPTARE_H_INCLUDED
#define CRIPTARE_H_INCLUDED

#include "functii_criptare.c"

void criptare(char *original, char *criptat, char *cheie);

void decriptare(char *criptat, char *decriptat, char *cheie);

void print_hi(char *imagine);

#endif // CRIPTARE_H_INCLUDED
