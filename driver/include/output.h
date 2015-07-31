// 29 July 2015

#ifndef OUTPUT_H
#define OUTPUT_H

int output_init();
void output_destroy();
void output_noteon(char track, char note);
void output_noteoff(char track, char note);
void output_update();
void output_panic();

#endif // ORGAN_H
