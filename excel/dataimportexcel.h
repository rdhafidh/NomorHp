#ifndef DATAIMPORTEXCEL_H
#define DATAIMPORTEXCEL_H

#include <uiutil.h>
#include <utility>
#include "sqlfetch.h"
#include <tuple>
struct DataImportExcel {
  QString tblname;
  QString tblsqlname;
  QString fname;
  std::vector<QPair<QString, QVariant::Type> > colnamesandtypes;
  //importexcel this behaivor is that:
  //bool jika true maka column itu buat idsistem jika false
  //maka kolumn tadi diBUAT USER kode default input type
  std::vector<std::tuple<int,bool,QVariant> > ignorecolumns;

  std::vector<QPair<QString,QString> > colcellranges;
  dbbase::configdb infologin;
};

struct DataImportExcel4SetupWizard
{
    QString tblname;
    QString tblsqlname;
    dbbase::configdb infologin;
    //importexcel this behaivor is that:
    //bool jika true maka column itu buat idsistem jika false
    //maka kolumn tadi diBUAT USER kode default input type
    std::vector<std::tuple<int,bool,QVariant> > ignorecolumns;

    std::vector<QPair<QString, QVariant::Type> > colnamesandtypes;
};


enum class ImportExcelResult {
  importexcelsuccess = 0x100,
  importexcelnotreadible,
  importexcelcontainformula,
  importexcelsqlerror,
    importexcellemptycellrange,
    importexcellinvalidcellrange,
    importexcellrangecellerror,
    importexcelfilenotfound

};

#endif  // DATAIMPORTEXCEL_H
