
// *-----------------------------------------------------------------
//
// Sha1散列算法实现, 基于openssl
//
// *-----------------------------------------------------------------

#pragma once

#include <openssl/sha.h>
#include "Util/BigNumber.h"

class Sha1Hash
{
public:
    Sha1Hash();
    ~Sha1Hash();

    void UpdateFinalizeBigNumbers(BigNumber *bn0, ...);
    void UpdateBigNumbers(BigNumber *bn0, ...);

    void UpdateData(const unsigned char *dta, int len);
    void UpdateData(const std::string &str);

    void Initialize();
    void Finalize();

    unsigned char *GetDigest(void) { return mDigest; };
    int GetLength(void) { return SHA_DIGEST_LENGTH; };

    BigNumber GetBigNumber();

private:
    SHA_CTX mC;
    unsigned char mDigest[SHA_DIGEST_LENGTH];
};
