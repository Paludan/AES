#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes_matrixes.h"

void subBytes (uint8_t input[4][4]);
void shiftRows (uint8_t input[4][4]);
void mixColumns (uint8_t input[4][4]);
uint8_t b02mul (uint8_t x);
uint8_t mul (uint8_t x, uint8_t y);

int gf2n_multiply(int a, int b) {
    int sum = 0;
    while (b > 0) {
        if (b & 1) sum = sum ^ a;        // if last bit of b is 1, add a to the sum
        b = b >> 1;                      // divide b by 2, discarding the last bit
        a = a << 1;                      // multiply a by 2
        if (a & 0x100) a = a ^ 0x11b;    // reduce a modulo the AES polynomial
    }
    return sum;
}

int main(int argc, const char * argv[]) {
    uint8_t input[4][4] =
    {
        0x0, 0x1, 0x2, 0x3,
        0x4, 0x3, 0x2, 0x1,
        0x2, 0x4, 0x1, 0x3,
        0x0, 0x3, 0x1, 0x4
    };
    
    subBytes(input);
    mixColumns(input);
    
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            printf("%02x, ", input[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}

uint8_t b02mul (uint8_t x) {
    uint8_t R_p = 0x11b % 256; // 2^8 + 2^4 + 2^3 + 2 + 1 mod 256
    int remainder = x << 1; // x << 1 = 2^8 + remainder

    if (remainder > 256) // if remainder is larger than 2^8 then we have an overflow
        return remainder ^ R_p;
    else
        return x << 1;
}

uint8_t mul (uint8_t x, uint8_t y) {
    switch (y) {
        case 0x01:
            return x;
        case 0x02:
            return b02mul(x);
        case 0x03:
            return b02mul(x) ^ x;
        default:
            printf("How did you end up here with AES?");
            exit(1);
    }
}

void subBytes (uint8_t input[4][4]) {
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            input[i][j] = S[input[i][j]];
        }
    }
}

void shiftRows (uint8_t input[4][4]) {
    uint8_t col_copy[4], i, col, row, shift = 3;
    
    for (col = 1; col < 4; col++) {
        // Make temporary copy of current col
        for (i = 0; i < 4; i++) {
            memcpy(&col_copy[i], &input[i][col], 1); // Overvej assignment i stedet
        }
        
        // Shift row
        for (row = 0; row < 4; row++) {
            input[(row + shift) % 4][col] = col_copy[row];
        }
        
        shift--;
    }
}

void mixColumns (uint8_t input[4][4]) {
    uint8_t i, j, k;
    uint8_t ret[4][4];
    
    for (i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (j = 0; j < (sizeof(uint8_t) * 4); j++) {
            uint8_t elem = 0;
            
            for (k = 0; k < (sizeof(uint8_t) * 4); k++) {
                elem ^= mul(input[k][i], M[j][k]);
            }
            ret[j][i] = elem;
        }
    }
    
    memcpy(input, ret, sizeof(ret));
}
