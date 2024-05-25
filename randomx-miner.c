/*
 * Copyright 2024 Simon Liu
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"
#include "miner.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int scanhash_randomx(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
                     uint32_t max_nonce, randomx_vm *vm, unsigned long *hashes_done, uint8_t *out_rx_hash)
{
    const uint32_t Htarg = ptarget[7];
    uint32_t n = pdata[19];
    uint8_t rx_hash[RANDOMX_HASH_SIZE];
    uint8_t rx_cm[RANDOMX_HASH_SIZE];

    // Construct blockheader in serialized little endian form (code from submit work function)
    uint32_t blockHeader[28];
    memset(blockHeader, 0, sizeof(blockHeader)); // make sure rx_hash field is zero for rx hash and cm computation
    for (int i = 0; i < 20; i++)
    {
        be32enc(blockHeader + i, pdata[i]);
    }

    void *blockHeaderPtr = (void *)blockHeader;
    void *noncePtr = &blockHeader[19];
    const uint32_t *cmCheckPtr = &((const uint32_t *)&rx_cm)[7];

    do
    {
        // serialize the nonce into the blockheader in little endian
        le32enc(noncePtr, n);

        randomx_calculate_hash(vm, blockHeaderPtr, sizeof(blockHeader), &rx_hash);

        randomx_calculate_commitment(blockHeaderPtr, sizeof(blockHeader), &rx_hash, &rx_cm);

        if (*cmCheckPtr < Htarg)
        {
            if (fulltest((void *)rx_cm, ptarget))
            {
                *hashes_done = n - pdata[19] + 1;

                // Note: Encode nonce into buffer as big endian, so it gets flipped back to little endian when submitting to node
                be32enc((void *)&pdata[19], n);

                // rx_hash is already in little endian order for uint256, so just copy it into block header for submission
                memcpy(out_rx_hash, rx_hash, RANDOMX_HASH_SIZE);

                return 1;
            }
        }

        n++;

    } while (n < max_nonce && !work_restart[thr_id].restart);

    *hashes_done = n - pdata[19];
    pdata[19] = n;
    return 0;
}


int scanhash_randomx_batch(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
                           uint32_t max_nonce, randomx_vm *vm, unsigned long *hashes_done, uint8_t *out_rx_hash)
{
    const uint32_t Htarg = ptarget[7];
    uint32_t n = pdata[19];
    uint8_t rx_hash[RANDOMX_HASH_SIZE];
    uint8_t rx_cm[RANDOMX_HASH_SIZE];

    // Construct blockheader in serialized little endian form (code from submit work function)
    uint32_t blockHeader[28];
    memset(blockHeader, 0, sizeof(blockHeader)); // make sure rx_hash field is zero for rx hash and cm computation
    for (int i = 0; i < 20; i++)
    {
        be32enc(blockHeader + i, pdata[i]);
    }

    void *blockHeaderPtr = (void *)blockHeader;
    void *noncePtr = &blockHeader[19];
    const uint32_t *cmCheckPtr = &((const uint32_t *)&rx_cm)[7];

    // First round of batch mode
    le32enc(noncePtr, n);
    randomx_calculate_hash_first(vm, blockHeaderPtr, sizeof(blockHeader));

    while (n < max_nonce && !work_restart[thr_id].restart)
    {
        // serialize nonce N+1 into blockheader in little endian
        le32enc(noncePtr, n + 1);

        // prepare to calculate hash for N+1, return hash for N
        //randomx_calculate_hash(vm, blockHeaderPtr, sizeof(blockHeader), &rx_hash);
        randomx_calculate_hash_next(vm, blockHeaderPtr, sizeof(blockHeader), &rx_hash);

        // serialize N into blockheader to compute commitment
        le32enc(noncePtr, n);

        // calculate commitment for nonce N
        randomx_calculate_commitment(blockHeaderPtr, sizeof(blockHeader), &rx_hash, &rx_cm);

        if (*cmCheckPtr < Htarg)
        {
            if (fulltest((void *)rx_cm, ptarget))
            {
                *hashes_done = n - pdata[19] + 1;

                // Note: Encode nonce into buffer as big endian, so it gets flipped back to little endian when submitting to node
                be32enc((void *)&pdata[19], n);

                // rx_hash is already in little endian order for uint256, so just copy it into block header for submission
                memcpy(out_rx_hash, rx_hash, RANDOMX_HASH_SIZE);

                return 1;
            }
        }

        n++;
    }

    *hashes_done = n - pdata[19];
    pdata[19] = n;
    return 0;
}
