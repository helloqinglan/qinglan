
// *-----------------------------------------------------------------
//
// Sha1散列算法实现, 基于openssl
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Util/Sha1.h"
#include <stdarg.h>

Sha1Hash::Sha1Hash()
{
    SHA1_Init(&mC);
}

Sha1Hash::~Sha1Hash()
{
    SHA1_Init(&mC);
}

void Sha1Hash::UpdateData(const unsigned char *dta, int len)
{
    SHA1_Update(&mC, dta, len);
}

void Sha1Hash::UpdateData(const std::string &str)
{
    UpdateData((unsigned char *)str.c_str(), (int)str.length());
}

void Sha1Hash::UpdateBigNumbers(BigNumber *bn0, ...)
{
    va_list v;
    BigNumber *bn;

    va_start(v, bn0);
    bn = bn0;
    while (bn)
    {
        UpdateData(bn->AsByteArray(), bn->GetNumBytes());
        bn = va_arg(v, BigNumber *);
    }
    va_end(v);
}

void Sha1Hash::Initialize()
{
    SHA1_Init(&mC);
}

void Sha1Hash::Finalize(void)
{
    SHA1_Final(mDigest, &mC);
}
