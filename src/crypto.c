#include "crypto.h"

#include <avr-crypto-lib/hmac-sha1/hmac-sha1.h>

int crypto_pbkdf2(const char *pass, uint16_t passlen,
                  const uint8_t *salt, uint16_t saltlen,
                  uint16_t iter, uint16_t keylen, uint8_t *out) {
  uint8_t digtmp[AVR_DEPO_PBKDF2_DIGEST_BYTES];
  uint8_t *p;
  uint8_t itmp[4];
  uint8_t salt_itmp[AVR_DEPO_PBKDF2_MAX_SALT_BYTES + 4];
  int32_t cplen, j, k, tkeylen, mdlen;
  uint32_t i = 1;
  hmac_sha1_ctx_t hctx; //HMAC_CTX hctx;

  if(saltlen > AVR_DEPO_PBKDF2_MAX_SALT_BYTES)
    return -1;

  memcpy(salt_itmp, salt, saltlen);

  mdlen = AVR_DEPO_PBKDF2_DIGEST_BYTES;

  /*dont think i need to do this*/ /* init hmac context */ //HMAC_CTX_init(&hctx);
  p = out;
  tkeylen = keylen;
  
  while(tkeylen) {
    if(tkeylen > mdlen)
      cplen = mdlen;
    else
      cplen = tkeylen;
    /* We are unlikely to ever use more than 256 blocks (5120 bits!)
     * but just in case...
     */
    itmp[0] = (uint8_t)((i >> 24) & 0xff);
    itmp[1] = (uint8_t)((i >> 16) & 0xff);
    itmp[2] = (uint8_t)((i >> 8) & 0xff);
    itmp[3] = (uint8_t)(i & 0xff);
    hmac_sha1_init(&hctx, pass, passlen*8); //HMAC_Init_ex(&hctx, pass, passlen, digest, NULL);
    /*HMAC_Update(&hctx, salt, saltlen);
     *HMAC_Update(&hctx, itmp, 4);
     */
    memcpy(salt_itmp + saltlen, itmp, 4);
    hmac_sha1_lastBlock(&hctx, salt_itmp, (saltlen+4)*8);

    /*HMAC_Final(&hctx, digtmp, NULL);
     */
    hmac_sha1_final(digtmp, &hctx);
    
    memcpy(p, digtmp, cplen);

    for(j = 1; j < iter; j++) {
      //HMAC(digest, pass, passlen, digtmp, mdlen, digtmp, NULL);

      hmac_sha1(digtmp, pass, passlen*8, digtmp, mdlen*8);
 
      for(k = 0; k < cplen; k++)
        p[k] ^= digtmp[k];
    }

    tkeylen -= cplen;
    i++;
    p += cplen;
  }
  /* dont think i need to do this */ //HMAC_CTX_cleanup(&hctx);

  return 0;
}