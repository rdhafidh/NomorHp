#ifndef UTILPRODUK_H
#define UTILPRODUK_H
#include "invalidkoneksi.h"
#include <utility>

namespace utilproduk {

class selectdatahelper {
  dbbase::ret retval;
  QString msg;
  QString errorcode;

public:
  void seterrorcode(const QString &ed) { errorcode = ed; }

  explicit selectdatahelper() { retval.second.clear(); }
  ~selectdatahelper() {}
  void setval(dbbase::ret a, const QString &b) {
    retval = std::move(a);
    msg = b;
  }
  dbbase::ret data() const { return retval; }
  QString message() const { return msg; }
  bool avoiddeepcopy() {
    retval.second.clear();
    return (retval.second.size() == 0);
  }
};
class selectcounthelper {
  qint64 count = -1;
  QString msg = "";
  bool valid = false;
  QString errorcode;

public:
  void seterrorcode(const QString &ed) { errorcode = ed; }
  selectcounthelper() {}
  ~selectcounthelper() {}
  void setvalue(qint64 c, const QString &m, bool validity) {
    count = c;
    msg = m;
    valid = validity;
  }
  qint64 countrecord() const { return count; }
  QString message() const { return msg; }
  bool validity() const { return valid; }
};

class sqlmodelwork {
public:
  sqlmodelwork() {}
  ~sqlmodelwork() {}

  std::tuple<bool,QString,QString> testlogin(const dbbase::configdb &c);

  std::tuple<bool,QString,QString> testloginReady(const dbbase::configdb &c);

  QSqlRecord typecolumn(const dbbase::configdb &c
                        ,const QString &tblname);
/*
 * return jumlah kolom dan tipe kolom apa saja dari setiap kolom
 * menganut kepada tipe data qvariant.type() function.
 * */
  QSqlRecord typecolumnReady(const dbbase::configdb &c
                             ,const QString &tblname
                             ,bool asy=true);

  utilproduk::selectdatahelper selectnolmt(const dbbase::configdb &c,
                                           const QString &sql); 
  
  utilproduk::selectdatahelper selectnolmtReady(const dbbase::configdb &c,
                                                const QString &sql,
                                                bool asy = true) ;

  utilproduk::selectdatahelper select(const dbbase::configdb &c,
                                      const QString &sql,
                                      const QPair<qint64, qint64> &limit);

  utilproduk::selectdatahelper selectReady(const dbbase::configdb &c,
                                           const QString &sql,
                                           const QPair<qint64, qint64> &limit,
                                           bool asy = true) ;

  std::pair<bool, QString> crdelalter(const dbbase::configdb &c,
                                      const QString &sql) ;

  std::pair<bool, QString> crdelalterReady(const dbbase::configdb &c,
                                           const QString &sql,
                                           bool asy = true) ;
  
  std::pair<bool, QString> edit(const dbbase::configdb &c, const QString &sql,
                                const QVariantList &row);
  std::pair<bool, QString> editReady(const dbbase::configdb &c,
                                     const QString &sql,
                                     const QVariantList &row, bool asy = true) ;
  
  std::pair<bool, QString> hapus(const dbbase::configdb &c, const QString &sql,
                                 const QString &id);
  std::pair<bool, QString> hapusReady(const dbbase::configdb &c,
                                      const QString &sql, const QString &id,
                                      bool asy = true);
  
  utilproduk::selectcounthelper
  countUnik2VariantAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql,const QVariant &filter1,
                         const QVariant &filter2);
  utilproduk::selectcounthelper
  countUnik2VariantAndVariantfilterrecordReady(const dbbase::configdb &c, const QString &sql,const QVariant &filter1,
                         const QVariant &filter2);
  utilproduk::selectdatahelper
  filterequalby2groupVariantAndVariant(const dbbase::configdb &c, const QString &sql,const QVariant &filter1,
                           const QVariant &filter2,
                           const QPair<qint64, qint64> &limit) ;
  utilproduk::selectdatahelper
  filterequalby2groupVariantAndVariantReady(const dbbase::configdb &c, const QString &sql,const QVariant &filter1,
                           const QVariant &filter2,
                           const QPair<qint64, qint64> &limit) ;
  
  utilproduk::selectcounthelper
  countUnik2LikeStringAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql,const QString &like,
                         const QVariant &groupfilter);
  
  utilproduk::selectcounthelper countUnik2LikeStringAndVariantfilterrecordReady(const dbbase::configdb &c, const QString &sql,const QString &like,
                                                                                const QVariant &groupfilter);
  
  utilproduk::selectdatahelper
  filterequalby2groupStringAndVariant(const dbbase::configdb &c, const QString &sql,const QString &like,
                           const QVariant &group,
                           const QPair<qint64, qint64> &limit) ;
  
  utilproduk::selectdatahelper
  filterequalby2groupStringAndVariantReady(const dbbase::configdb &c, const QString &sql,const QString &like,
                           const QVariant &group,
                           const QPair<qint64, qint64> &limit) ;

  utilproduk::selectdatahelper
  filterequalbysinglegroup(const dbbase::configdb &c, const QString &sql,
                           const QVariant &group,
                           const QPair<qint64, qint64> &limit) ;
  
  utilproduk::selectdatahelper
  filterequalbysinglegroupReady(const dbbase::configdb &c, const QString &sql,
                                const QVariant &group,
                                const QPair<qint64, qint64> &limit, bool asy = true);

  utilproduk::selectdatahelper
  filterbysinglegroup(const dbbase::configdb &c, const QString &sql,
                      const QString &group, const QPair<qint64, qint64> &limit) ;
  
  utilproduk::selectdatahelper
  filterbysinglegroupReady(const dbbase::configdb &c, const QString &sql,
                           const QString &group, const QPair<qint64,qint64> &limit,
                           bool asy = true) ;

  utilproduk::selectcounthelper countrecordjoin(const dbbase::configdb &c,const QString &sql);
  
  utilproduk::selectcounthelper countrerordjoinReady(const dbbase::configdb &c, const QString &sql,
                                                     bool asy=true);

  utilproduk::selectcounthelper countrecord(const dbbase::configdb &c,
                                            const QString &tblname);
  
  utilproduk::selectcounthelper countrecordReady(const dbbase::configdb &c,
                                                 const QString &tblname,
                                                 bool asy = true);

  utilproduk::selectcounthelper
  countequalfilternonerecord(const dbbase::configdb &c,const QString &sql);
  
  utilproduk::selectcounthelper
  countequalfilternonerecordReady(const dbbase::configdb &c,const QString &sql
                                  ,bool asy=true);
  
  utilproduk::selectcounthelper
  countequalfilterrecord(const dbbase::configdb &c, const QString &sql,
                         const QVariant &groupfilter);
  
  utilproduk::selectcounthelper
  countequalfilterrecordReady(const dbbase::configdb &c, const QString &sql,
                              const QVariant &groupfilter, bool asy = true); 

  utilproduk::selectcounthelper countfilterrecord(const dbbase::configdb &c,
                                                  const QString &sql,
                                                  const QString &groupfilter); 

  utilproduk::selectcounthelper
  countfilterrecordReady(const dbbase::configdb &c, const QString &sql,
                         const QString &groupfilter, bool asy = true) ;

private:
};
}

#endif // UTILPRODUK_H
