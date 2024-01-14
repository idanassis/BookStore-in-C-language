// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#include "Crypto.h"

#define KEY "p!JLx=t]4W8g@"

//encrypting the employee data with xor operation
void encryptEmployee(Employee* emp) {
    int i;
    char* data = (char*)emp;
    for (i = 0; i < sizeof(Employee); i++) {
        data[i] ^= KEY[i % strlen(KEY)];
    }
}

//decrypting the employee data with xor operation
void decryptEmployee(Employee* emp) {
    int i;
    char* data = (char*)emp;
    for (i = 0; i < sizeof(Employee); i++) {
        data[i] ^= KEY[i % strlen(KEY)];
    }
}