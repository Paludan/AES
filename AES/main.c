#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes_matrixes.h"

void subBytes (uint8_t input[4][4]);
void shiftRows (uint8_t input[4][4]);
void mixColumns (uint8_t input[4][4]);
void addRoundKey (uint8_t input[4][4], uint8_t key[4][4]);
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
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB,
        0xCC, 0xDD, 0xEE, 0xFF
    };
    
    uint8_t key[4][4] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };
    
    addRoundKey(input, key);
    
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            printf("%02x, ", input[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    subBytes(input);
    
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            printf("%02x, ", input[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    shiftRows(input);
    
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            printf("%02x, ", input[i][j]);
        }
        printf("\n");
    }
    printf("\n");
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

void addRoundKey (uint8_t input[4][4], uint8_t key[4][4]) {
    for (uint8_t i = 0; i < (sizeof(uint8_t) * 4); i++) {
        for (uint8_t j = 0; j < (sizeof(uint8_t) * 4); j++) {
            input[i][j] = input[i][j] ^ key[i][j];
        }
    }
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
                elem ^= mul(input[i][k], M[j][k]);
            }
            ret[i][j] = elem;
        }
    }
    
    memcpy(input, ret, sizeof(ret));
}
