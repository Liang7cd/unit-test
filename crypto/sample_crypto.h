/*
 * crypto_api_demo.h
 *
 *  Created on: Dec 6, 2016
 *      Author: duobao
 */

#ifndef CRYPTO_API_DEMO_H_
#define CRYPTO_API_DEMO_H_

#include "fh_crypto_api.h"

typedef struct FH_CRYPTO_CASE
{
	CRYPTO_CTRL_S stCryptoCtrl;
	uint8_t* encrypto_buf;
	uint8_t* decrypto_buf;
	uint32_t crypto_size;

	uint8_t* key_buf;
	uint8_t* iv_buf;
	uint32_t key_size;
	uint32_t iv_size;

	char     name[8];
} CRYPTO_CASE;

#endif /* CRYPTO_API_DEMO_H_ */
