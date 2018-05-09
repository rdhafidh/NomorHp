#include "resetdatanomor.h"
#include <utilproduk.h>

ResetDataNomor::ResetDataNomor() {}

ResetDataNomor::~ResetDataNomor() {}

std::pair<bool, QString> ResetDataNomor::askresetDataNomor(
    const dbbase::configdb &mdb) {
  utilproduk::sqlmodelwork wk1;
  QString sql = "DELETE FROM nomor_telp ";
  return wk1.crdelalterReady(mdb, sql);
}

std::pair<bool, QString> ResetDataNomor::askresetDataNomorNonDuplicate(const dbbase::configdb &mdb)
{
    utilproduk::sqlmodelwork wk1;
    QString sql = "DELETE FROM nomor_nonduplicate ";
    return wk1.crdelalterReady(mdb, sql);
}
