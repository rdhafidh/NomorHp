#ifndef RESETDATANOMOR_H
#define RESETDATANOMOR_H

#include <QString>
#include <utility>
#include <sqlfetch.h>

class ResetDataNomor
{
public:
    explicit ResetDataNomor();
    ~ResetDataNomor();
    
    static std::pair<bool,QString> askresetDataNomor(const dbbase::configdb &mdb);
    static std::pair<bool,QString> askresetDataNomorNonDuplicate(const dbbase::configdb &mdb);
};

#endif // RESETDATANOMOR_H