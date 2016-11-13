
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "qlib/qencrypt.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

void shorttest()
{
    unsigned char plaintext[8];
    unsigned char encrytext[8];
    unsigned char decrytext[8];
    unsigned char key[8];
    unsigned char buffer[10];
    int index, tmp;
    char c;

    srand(time(0));
    for(index = 0; index < 8; index++)
    {
        tmp = rand()%26;
        c = tmp + 'a';
        plaintext[index] = c;

        tmp = rand()%10;
        c = tmp + '0';
        key[index] = c;
    }

    printf("=====Plain   key ======\n");
    snprintf(buffer, 9, "%s", key); //one more space for '\0'
    printf("%s\n", buffer);
    QDes_keygen(key);
    printf("=====Plain   Text======\n");
    snprintf(buffer, 9, "%s", plaintext);
    printf("%s\n", buffer);
    printf("=====Encrypt Text======\n");
    QDes_encrypt(plaintext, encrytext);
    snprintf(buffer, 9, "%s", encrytext);
    printf("%s\n", buffer);
    printf("=====Decrypt Text======\n");
    QDes_decrypt(encrytext, decrytext);
    snprintf(buffer, 9, "%s", decrytext);
    printf("%s\n", buffer);

}

int main()
{
    shorttest();

    return 0;
}

