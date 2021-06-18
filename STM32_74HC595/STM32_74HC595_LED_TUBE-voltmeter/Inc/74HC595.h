#ifndef _74HC595_H
#define _74HC595_H

void send(unsigned char X);
void pre_send(unsigned char X, unsigned char value);
void digit4_3(int n, int replay, bool showZero);
void digit4_2(int n, int replay);
void digit4_1(int n);
void digit4showZero_2(int n, int replay);
void digit4showZero_1(int n);
void digit2_3(int n, int value, int replay);
void digit2_2(int n, int value);

#endif /* _74HC595_H */
