
// *-----------------------------------------------------------------
//
// 大数辅助类, 基于openssl
//
// *-----------------------------------------------------------------

#pragma once

#include "Util/ByteBuffer.h"

struct bignum_st;

class BigNumber
{
public:
    BigNumber();
    BigNumber(const BigNumber &bn);
    BigNumber(unsigned long);
    ~BigNumber();

    void SetDword(unsigned long);
    void SetQword(unsigned __int64);
    void SetBinary(const unsigned char *bytes, int len);
    void SetHexStr(const char *str);

    void SetRand(int numbits);

    BigNumber operator=(const BigNumber &bn);
    

    BigNumber operator+=(const BigNumber &bn);
    BigNumber operator+(const BigNumber &bn)
    {
        BigNumber t(*this);
        return t += bn;
    }
    BigNumber operator-=(const BigNumber &bn);
    BigNumber operator-(const BigNumber &bn)
    {
        BigNumber t(*this);
        return t -= bn;
    }
    BigNumber operator*=(const BigNumber &bn);
    BigNumber operator*(const BigNumber &bn)
    {
        BigNumber t(*this);
        return t *= bn;
    }
    BigNumber operator/=(const BigNumber &bn);
    BigNumber operator/(const BigNumber &bn)
    {
        BigNumber t(*this);
        return t /= bn;
    }
    BigNumber operator%=(const BigNumber &bn);
    BigNumber operator%(const BigNumber &bn)
    {
        BigNumber t(*this);
        return t %= bn;
    }

    BigNumber ModExp(const BigNumber &bn1, const BigNumber &bn2);
    BigNumber Exp(const BigNumber &);

    int GetNumBytes(void);

    struct bignum_st *BN() { return _bn; }

    unsigned long AsDword();
    unsigned char* AsByteArray();
    ByteBuffer AsByteBuffer();
    std::vector<unsigned char> AsByteVector();

    const char *AsHexStr();
    const char *AsDecStr();

private:
    struct bignum_st *_bn;
    unsigned char *_array;
};
