#ifndef CRIPTARE_H_INCLUDED
#define CRIPTARE_H_INCLUDED

typedef struct
{
    unsigned char *header;
    unsigned char *pixel;
}imagine;

unsigned int width_from_header(unsigned char *header);
unsigned int height_from_header(unsigned char *header);
imagine new_imagine(unsigned int width, unsigned int height);
imagine citire_imagine(char *path);
void scriere_imagine(char *path, imagine img_test);
void criptare(char *original, char *criptat, char *cheie);
void decriptare(char *criptat, char *decriptat, char *cheie);
void print_chi(char *imagine);

#endif // CRIPTARE_H_INCLUDED
