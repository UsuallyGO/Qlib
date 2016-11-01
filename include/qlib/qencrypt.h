
#ifndef _QLIB_ENCRYPT_H_
#define _QLIB_ENCRYPT_H_

/*
 * In this encrypt module, I only provide DES encrypt/decrypt algorithm.
 * Need more encrypt algorithms ? I am just too lazy to write the code...
 * DES(Data Encryption Standart) algorithm is a kind of symmetric encryption
 * algorithm. User has to provide the plain key, which used to encrypt and 
 * decrypt the data block. The length of the key should be 8bytes.
 *
 * For the plain text and encrypt text, the length should be both 8bytes.
 *
 * I don't want to explaint the process of encryption and decryption, it's too long...
 */

int QDes_keygen(const unsigned char *key);

int QDes_encrypt(const unsigned char *text, unsigned char *entext);

int QDes_decrypt(const unsigned char *entext, unsigned char *detext);

#endif //!
