#ifndef NUMBERPARSERTYPE_H
#define NUMBERPARSERTYPE_H

#include <iostream>
#include <QtGlobal>

namespace nomorutil {
enum class KategoriNomor{
    NOMOR_UNKNOWN=-1,
    NOMOR_10_DIGIT=1,
    NOMOR_CANTIK=2,
    NOMOR_ACAK=3
};

enum class ProviderNomor{
    PROVIDER_UNKNOWN=-1,
    PROVIDER_TELKOMSEL=1,
    PROVIDER_INDOSAT=2,
    PROVIDER_XL=3,
    PROVIDER_3=4,
    PROVIDER_AXIS=5
};

struct ReturnDeterminedNumber{
    std::string nomor_string_diprocess="";
    bool isnumbervalid=false;
    KategoriNomor km=KategoriNomor::NOMOR_UNKNOWN;
    ProviderNomor pn=ProviderNomor::PROVIDER_UNKNOWN;
};

class NumberParserType
{
public:
    NumberParserType();
    ~NumberParserType();
    
  static  ReturnDeterminedNumber determineNumber(const std::string &number);
  
  static bool isNomorCantikKah(const std::string &nomorBerawalan08x);
  
private:
    
};

}
#endif // NUMBERPARSERTYPE_H