#ifdef BUILDRPCMODE
#include <cmds/askcountequalfilterrecord.h>
#include <cmds/askcountrecordjoin.h>
#include <cmds/askcrdelalter.h>
#include <cmds/askedit.h>
#include <cmds/askfilterequalbysinglegroup.h>
#include <cmds/askhapus.h>
#include <cmds/askselectlimit.h>
#include <cmds/askselectnolimit.h>
#include <cmds/asktypecolumn.h>
#include <cmds/testlogininfo.h>
#include <util/encodedecodervariant.h>
#else
#include <utilproduk.h>
#endif
#include <future>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <thread>

std::tuple<bool, QString, QString>
utilproduk::sqlmodelwork::testlogin(const dbbase::configdb &c) {
#ifdef BUILDRPCMODE
  /*
   * Require username password sama hostname & port 
   * */
  base::TestLoginInfoParam param;
  auto tmpalamat=QString("%1:%2");
  param.alamat=tmpalamat.arg (c["hostname"].toString ())
      .arg (c["port"].toString ()).toStdString ();
  param.myusername=c["username"].toString ().toStdString ();
  param.mypassword=c["password"].toString ().toStdString ();
  work::TestLoginInfo inf(param);
  base::ResponseData myval=inf.process (); 
 return std::make_tuple( myval.selecthelper.data ().first,
                         myval.selecthelper.message (),
                         "");
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    return std::make_tuple(false, QObject::tr("Invalid sesi login"),
                           QStringLiteral(""));
  }
  s->setConfig(cc);
  if (!s->open()) {
    auto ret =
        std::make_tuple(false, dbbase::SqlFetch::errortostring(s->lastError()),
                        std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return std::make_tuple(true, QStringLiteral(""), QStringLiteral(""));
#endif
}

std::tuple<bool, QString, QString> helperFunc_testlogin(const dbbase::configdb &c){
    utilproduk::sqlmodelwork wk1;
    return wk1.testlogin (c);
}

std::tuple<bool, QString, QString>
utilproduk::sqlmodelwork::testloginReady(const dbbase::configdb &c) {
  // only execute at different thread
//  std::packaged_task<std::tuple<bool, QString, QString>(
//      const dbbase::configdb &)>
//  task([this](const dbbase::configdb &c) { return testlogin(c); });
//  std::future<std::tuple<bool, QString, QString>> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return std::make_tuple(false, QStringLiteral(""), QStringLiteral(""));
//  } catch (...) {
//    return std::make_tuple(false, QStringLiteral(""), QStringLiteral(""));
//  }
//  return result1.get();
    QFuture<std::tuple<bool, QString, QString>> ret=QtConcurrent::run(helperFunc_testlogin,c);
    ret.waitForFinished ();
    return ret.result ();
}

QSqlRecord utilproduk::sqlmodelwork::typecolumn(const dbbase::configdb &c,
                                                const QString &tblname) {
  #ifdef BUILDRPCMODE
  base::AskTypeColumnParam param;
  auto tmpalamat=QString("%1:%2");
  param.alamat=tmpalamat.arg (c["hostname"].toString ())
      .arg (c["port"].toString ()).toStdString ();
  param.username=c["username"].toString ().toStdString ();
  param.password=c["password"].toString ().toStdString ();
  param.tablename=tblname.toStdString ();
  work::AskTypeColumn ask(param);
  base::ResponseData myval= ask.process ();
  if(myval.recordtypes.isvalid ()){
     return myval.recordtypes.getRecord ();
  }
  QSqlRecord rec;
  return rec;
#else
  QSqlRecord dummyrec;
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    return dummyrec;
  }
  s->setConfig(cc);
  if (!s->open()) {
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }

    return dummyrec;
  }
  dummyrec = s->record(tblname);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return dummyrec;
#endif
}
QSqlRecord helperFunc_typecolumn(const dbbase::configdb &c,
                                 const QString &tblname){
    utilproduk::sqlmodelwork wk1;
    return wk1.typecolumn (c,tblname);
}

QSqlRecord utilproduk::sqlmodelwork::typecolumnReady(const dbbase::configdb &c,
                                                     const QString &tblname,
                                                     bool asy)

{
    Q_UNUSED(asy)
//  if (asy) {
//    std::future<QSqlRecord> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, tblname]() { return typecolumn(c, tblname); });
//      return ret.get();
//    } catch (...) {
//      QSqlRecord emptyrec;
//      return emptyrec;
//    }
//  }
//  std::packaged_task<QSqlRecord(const dbbase::configdb &, const QString &)>
//  task([this](const dbbase::configdb &c, const QString &tblname) {
//    return typecolumnReady(c, tblname);
//  });
//  QSqlRecord emptyrec;

//  std::future<QSqlRecord> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(tblname));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return emptyrec;
//  } catch (...) {
//    return emptyrec;
//  }

//  return result1.get();
    QFuture<QSqlRecord> ret=QtConcurrent::run(helperFunc_typecolumn,c,tblname);
    ret.waitForFinished ();
    return ret.result ();
}

utilproduk::selectdatahelper
utilproduk::sqlmodelwork::selectnolmt(const dbbase::configdb &c,
                                      const QString &sql) {
#ifdef BUILDRPCMODE
base::AskSelectnolimitParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString ();
param.sql=sql.toStdString ();
work::AskSelectnolimit ask(param);
base::ResponseData myval=ask.process ();
return myval.selecthelper;
#else
  utilproduk::selectdatahelper retvaluefail;
  std::deque<QSqlRecord> emptyrec;
  emptyrec.clear();
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                    "melakukan login kembali."));
    return retvaluefail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }

    return retvaluefail;
  }
  dbbase::ret ret = s->rawSelect(sql);
  if (!ret.first) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvaluefail;
  }
  utilproduk::selectdatahelper retvalue;
  QString empty("");
  retvalue.setval(std::move(ret), empty);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retvalue;
#endif
}


utilproduk::selectdatahelper helperFunc_selectnolmt(const dbbase::configdb &c,
                                                    const QString &sql){
    utilproduk::sqlmodelwork wk1;
    return wk1.selectnolmt (c,sql);
}

utilproduk::selectdatahelper
utilproduk::sqlmodelwork::selectnolmtReady(const dbbase::configdb &c,
                                           const QString &sql, bool asy) {
    Q_UNUSED(asy)
//  if (asy) {
//    std::future<utilproduk::selectdatahelper> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, sql]() { return selectnolmt(c, sql); });
//      return ret.get();
//    } catch (...) {
//      std::deque<QSqlRecord> emptyrec;
//      emptyrec.clear();
//      auto pair = std::make_pair(false, emptyrec);
//      utilproduk::selectdatahelper retfail;
//      retfail.setval(pair,
//                     QObject::tr("Galat, pastikan anda memiliki cukup memori "
//                                 "ram untuk menjalankan app ini."));
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectdatahelper(const dbbase::configdb &,
//                                                  const QString &)>
//  task([this](const dbbase::configdb &c, const QString &sql) {
//    return selectnolmt(c, sql);
//  });
//  std::deque<QSqlRecord> emptyrec;
//  emptyrec.clear();
//  auto pair = std::make_pair(false, emptyrec);
//  utilproduk::selectdatahelper retfail;
//  retfail.setval(pair, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."));

//  std::future<utilproduk::selectdatahelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
    QFuture<utilproduk::selectdatahelper> ret=QtConcurrent::run(helperFunc_selectnolmt,c,sql);
    ret.waitForFinished ();
    return ret.result ();
}


utilproduk::selectdatahelper
utilproduk::sqlmodelwork::select(const dbbase::configdb &c, const QString &sql,
                                 const QPair<qint64, qint64> &limit) {
#ifdef BUILDRPCMODE
base::AskSelectlimitParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString ();
param.sql=sql.toStdString ();
param.offset=limit.first;
param.limit=limit.second;
work::AskSelectlimit ask(param);
base::ResponseData myval=ask.process ();
return myval.selecthelper;
#else
    utilproduk::selectdatahelper retvaluefail;
    std::deque<QSqlRecord> emptyrec;
    emptyrec.clear();
    dbbase::SqlFetch *s = new dbbase::SqlFetch;
    auto cc = fixconname(c);
    if (invalidconfig(cc)) {
      delete s;
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                      "melakukan login kembali."));
      return retvaluefail;
    }
    s->setConfig(cc);
    if (!s->open()) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }

      return retvaluefail;
    }
    dbbase::placeholderprepareinsert prep;
    prep.push_back(std::make_pair(":0", limit.first));
    prep.push_back(std::make_pair(":1", limit.second));
    std::pair<bool, std::deque<QSqlRecord>> ret =
        s->prepareSelectFilter(sql, prep);
    if (!ret.first) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retvaluefail;
    }
    utilproduk::selectdatahelper retvalue;
    QString empty("");
    retvalue.setval(ret, empty);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvalue;
#endif
}

utilproduk::selectdatahelper helperFunc_select(const dbbase::configdb &c, const QString &sql,
                                    const QPair<qint64, qint64> &limit){
    utilproduk::sqlmodelwork wk1;
    return wk1.select (c,sql,limit);
}

utilproduk::selectdatahelper
utilproduk::sqlmodelwork::selectReady(const dbbase::configdb &c,
                                      const QString &sql,
                                      const QPair<qint64, qint64> &limit, bool asy) {
    Q_UNUSED(asy)
    QFuture<utilproduk::selectdatahelper> ret=QtConcurrent::run(helperFunc_select,c,sql,limit);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectdatahelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql, limit]() {
//        return select(c, sql, limit);
//      });
//      return ret.get();
//    } catch (...) {
//      std::deque<QSqlRecord> emptyrec;
//      emptyrec.clear();
//      auto pair = std::make_pair(false, emptyrec);
//      utilproduk::selectdatahelper retfail;
//      retfail.setval(pair,
//                     QObject::tr("Galat, pastikan anda memiliki cukup memori "
//                                 "ram untuk menjalankan app ini."));
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectdatahelper(
//      const dbbase::configdb &, const QString &, const QPair<qint64, qint64> &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QPair<qint64, qint64> &l) { return select(c, sql, l); });
//  std::deque<QSqlRecord> emptyrec;
//  emptyrec.clear();
//  auto pair = std::make_pair(false, emptyrec);
//  utilproduk::selectdatahelper retfail;
//  retfail.setval(pair, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."));

//  std::future<utilproduk::selectdatahelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(limit));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
    
}


std::pair<bool, QString>
utilproduk::sqlmodelwork::crdelalter(const dbbase::configdb &c,
                                     const QString &sql) {
#ifdef BUILDRPCMODE
base::AskCrdelalterParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.myusername=c["username"].toString ().toStdString ();
param.mypassword=c["password"].toString ().toStdString ();
param.mysql=sql.toStdString () ;
work::AskCrdelalter ask(param);
base::ResponseData myval=ask.process ();
return std::make_pair(myval.selecthelper.data ().first
                      ,myval.selecthelper.message ());
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    return std::make_pair(false,
                          QObject::tr("Invalid sesi login ditemukan, anda "
                                      "bisa melakukan login kembali."));
  }
  s->setConfig(cc);
  if (!s->open()) {
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  if (!s->crdelalter(sql)) {
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  auto ret = std::make_pair(true, QString(""));
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return ret;
#endif
}

std::pair<bool, QString> helperFunc_crdelalter(const dbbase::configdb &c,
                                                   const QString &sql){
    utilproduk::sqlmodelwork wk1;
    return wk1.crdelalter (c,sql);
}

std::pair<bool, QString>
utilproduk::sqlmodelwork::crdelalterReady(const dbbase::configdb &c,
                                          const QString &sql, bool asy) {
    Q_UNUSED(asy)
    QFuture<std::pair<bool, QString>> ret=QtConcurrent::run(helperFunc_crdelalter,c,sql);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<std::pair<bool, QString>> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, sql]() { return crdelalter(c, sql); });
//      return ret.get();
//    } catch (...) {
//      return std::make_pair(
//          false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                             "untuk menjalankan app ini."));
//    }
//  }
//  std::packaged_task<std::pair<bool, QString>(const dbbase::configdb &,
//                                              const QString &)>
//  task([this](const dbbase::configdb &c, const QString &sql) {
//    return crdelalter(c, sql);
//  });
//  std::future<std::pair<bool, QString>> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  } catch (...) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  }

//  return result1.get();
}

std::pair<bool, QString>
utilproduk::sqlmodelwork::edit(const dbbase::configdb &c, const QString &sql,
                               const QVariantList &row) {
#ifdef BUILDRPCMODE
base::AskEditParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.myusername=c["username"].toString ().toStdString ();
param.mypassword=c["password"].toString ().toStdString ();
param.mysql=sql.toStdString ();
param.row=row; 
work::AskEdit ask(param);
base::ResponseData myval=ask.process ();
return std::make_pair(myval.selecthelper.data().first,
                      myval.selecthelper.message());
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    return std::make_pair(false,
                          QObject::tr("Invalid sesi login ditemukan, anda "
                                      "bisa melakukan login kembali."));
  }
  s->setConfig(cc);
  if (!s->open()) {
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  dbbase::placeholderprepareinsert prep;
  for (int rwlist = 0; rwlist < row.size(); rwlist++) {
    QString plc(":");
    QTextStream strm(&plc);
    strm << rwlist;
    prep.push_back(std::make_pair(plc, row.at(rwlist)));
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << " " << __FILE__ << ":" << __LINE__;
 
#endif
  if (!s->preparedStm(sql, prep)) {
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << " " << __FILE__ << ":" << __LINE__
             << " query data: " << row;
#endif
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  auto ret = std::make_pair(true, QString(""));
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << " " << __FILE__ << ":" << __LINE__;
#endif
  return ret;
#endif
}


std::pair<bool, QString> helperFunc_edit(const dbbase::configdb &c,
                                                   const QString &sql,const QVariantList &row){
    utilproduk::sqlmodelwork wk1;
    return wk1.edit (c,sql,row);
}

std::pair<bool, QString>
utilproduk::sqlmodelwork::editReady(const dbbase::configdb &c,
                                    const QString &sql, const QVariantList &row,
                                    bool asy) {
    Q_UNUSED(asy)
    QFuture<std::pair<bool, QString>>ret=QtConcurrent::run(helperFunc_edit,c,sql,row);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<std::pair<bool, QString>> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, sql, row]() { return edit(c, sql, row); });
//      return ret.get();
//    } catch (...) {
//      return std::make_pair(
//          false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                             "untuk menjalankan app ini."));
//    }
//  }
//  std::packaged_task<std::pair<bool, QString>(
//      const dbbase::configdb &, const QString &, const QVariantList &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QVariantList &perrows) { return edit(c, sql, perrows); });
//  std::future<std::pair<bool, QString>> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(row));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  } catch (...) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  }

//  return result1.get();
}

std::pair<bool, QString>
utilproduk::sqlmodelwork::hapus(const dbbase::configdb &c, const QString &sql,
                                const QString &id) {
#ifdef BUILDRPCMODE
base::AskHapusParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.myusername=c["username"].toString ().toStdString ();
param.mypassword=c["password"].toString ().toStdString ();
param.mysql=sql.toStdString ();
param.id=id.toStdString (); 
work::AskHapus ask(param);
base::ResponseData myval=ask.process ();
return std::make_pair(myval.selecthelper.data().first,
                      myval.selecthelper.message());
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);

  if (invalidconfig(cc)) {
    delete s;
    return std::make_pair(false,
                          QObject::tr("Invalid sesi login ditemukan, anda "
                                      "bisa melakukan login kembali."));
  }
  s->setConfig(cc);
  if (!s->open()) {
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  dbbase::placeholderprepareinsert prep;
  prep.push_back(std::make_pair(":0", id));
  if (!s->preparedStm(sql, prep)) {
    auto ret =
        std::make_pair(false, dbbase::SqlFetch::errortostring(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  auto ret = std::make_pair(true, QString(""));
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return ret;
#endif
}

std::pair<bool,QString> helperFunc_hapus(const dbbase::configdb &c, const QString &sql,
                                         const QString &id){
    utilproduk::sqlmodelwork wk1;
    return wk1.hapus (c,sql,id);
}

std::pair<bool, QString>
utilproduk::sqlmodelwork::hapusReady(const dbbase::configdb &c,
                                     const QString &sql, const QString &id,
                                     bool asy) {
    Q_UNUSED(asy)
    QFuture<std::pair<bool,QString>> ret=QtConcurrent::run( helperFunc_hapus,c,sql,id);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<std::pair<bool, QString>> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, sql, id]() { return hapus(c, sql, id); });
//      return ret.get();
//    } catch (...) {
//      return std::make_pair(
//          false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                             "untuk menjalankan app ini."));
//    }
//  }
//  std::packaged_task<std::pair<bool, QString>(const dbbase::configdb &,
//                                              const QString &, const QString &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QString &id) { return hapus(c, sql, id); });
//  std::future<std::pair<bool, QString>> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(id));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  } catch (...) {
//    return std::make_pair(
//        false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
//                           "untuk menjalankan app ini."));
//  }

    //  return result1.get();
}

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countUnik2VariantAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2)
{
    dbbase::SqlFetch *s = new dbbase::SqlFetch;
    auto cc = fixconname(c);
    utilproduk::selectcounthelper retfail;
    if (invalidconfig(cc)) {
      delete s;
      retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                       "bisa melakukan login kembali."),
                       false);
      return retfail;
    }
    s->setConfig(cc);
    if (!s->open()) {
      retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                       false);
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retfail;
    }
    dbbase::placeholderprepareinsert prep;
    prep.push_back(std::make_pair(":0", filter1));
    prep.push_back (std::make_pair(":1",filter2));
    qint64 ret = s->prepareSelectFilterCount(sql, prep);
    if (ret < 0) {
      retfail.setvalue(ret, dbbase::SqlFetch::errortostring(s->lastError()),
                       false);
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retfail;
    }
    utilproduk::selectcounthelper retval;
    QString dummy("");
    retval.setvalue(ret, dummy, true);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retval;  
}

utilproduk::selectcounthelper  helperFunc_countUnik2VariantAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2){
    utilproduk::sqlmodelwork wk1;
    return wk1.countUnik2VariantAndVariantfilterrecord (c,sql,filter1,filter2);
}

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countUnik2VariantAndVariantfilterrecordReady(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2)
{
    QFuture<utilproduk::selectcounthelper> f=QtConcurrent::run(helperFunc_countUnik2VariantAndVariantfilterrecord,c,sql,filter1,filter2);
    f.waitForFinished ();
    return f.result ();
}

utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterequalby2groupVariantAndVariant(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2, const QPair<qint64, qint64> &limit)
{
    utilproduk::selectdatahelper retvaluefail;
    std::deque<QSqlRecord> emptyrec;
    emptyrec.clear();
    dbbase::SqlFetch *s = new dbbase::SqlFetch;
    auto cc = fixconname(c);
    if (invalidconfig(cc)) {
      delete s;
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                      "melakukan login kembali."));
      return retvaluefail;
    }
    s->setConfig(cc);
    if (!s->open()) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retvaluefail;
    }
    dbbase::placeholderprepareinsert prep;
    prep.push_back (std::make_pair(":0",filter1));
    prep.push_back(std::make_pair(":1", filter2));
    prep.push_back(std::make_pair(":2", limit.first));
    prep.push_back(std::make_pair(":3", limit.second));
    std::pair<bool, std::deque<QSqlRecord>> ret =
        s->prepareSelectFilter (sql, prep);
    if (!ret.first) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retvaluefail;
    }
    utilproduk::selectdatahelper retvalue;
    QString empty("");
    retvalue.setval(std::move(ret), empty);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvalue;
}

 utilproduk::selectdatahelper helperFunc_filterequalby2groupVariantAndVariant(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2, const QPair<qint64, qint64> &limit)
{
    utilproduk::sqlmodelwork wk1;
    return wk1.filterequalby2groupVariantAndVariant (c,sql,filter1,filter2,limit);
}
 
 utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterequalby2groupVariantAndVariantReady(const dbbase::configdb &c, const QString &sql, const QVariant &filter1, const QVariant &filter2, const QPair<qint64, qint64> &limit)
 {
     QFuture< utilproduk::selectdatahelper> f=QtConcurrent::run(helperFunc_filterequalby2groupVariantAndVariant,c,sql,filter1,filter2,limit);
     f.waitForFinished ();
     return f.result ();
 }

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countUnik2LikeStringAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &groupfilter)
{
    dbbase::SqlFetch *s = new dbbase::SqlFetch;
    auto cc = fixconname(c);
    utilproduk::selectcounthelper retfail;
    if (invalidconfig(cc)) {
      delete s;
      retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                       "bisa melakukan login kembali."),
                       false);
      return retfail;
    }
    s->setConfig(cc);
    if (!s->open()) {
      retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                       false);
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retfail;
    }
    dbbase::placeholderprepareinsert prep;
    QString m_id = like;
    m_id.prepend("%");
    m_id.append("%");
    prep.push_back(std::make_pair(":0", m_id));
    prep.push_back (std::make_pair(":1",groupfilter));
    qint64 ret = s->prepareSelectFilterCount(sql, prep);
    if (ret < 0) {
      retfail.setvalue(ret, dbbase::SqlFetch::errortostring(s->lastError()),
                       false);
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retfail;
    }
    utilproduk::selectcounthelper retval;
    QString dummy("");
    retval.setvalue(ret, dummy, true);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retval;  
}


utilproduk::selectcounthelper helperFunc_countUnik2LikeStringAndVariantfilterrecord(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &groupfilter){
    utilproduk::sqlmodelwork wk1;
    return wk1.countUnik2LikeStringAndVariantfilterrecord (c,sql,like,groupfilter);
}

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countUnik2LikeStringAndVariantfilterrecordReady(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &groupfilter)
{
    QFuture<utilproduk::selectcounthelper> f=QtConcurrent::run(helperFunc_countUnik2LikeStringAndVariantfilterrecord,c,sql,like,groupfilter);
    f.waitForFinished ();
    return f.result ();
}


utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterequalby2groupStringAndVariant(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &group, const QPair<qint64, qint64> &limit)
{
    utilproduk::selectdatahelper retvaluefail;
    std::deque<QSqlRecord> emptyrec;
    emptyrec.clear();
    dbbase::SqlFetch *s = new dbbase::SqlFetch;
    auto cc = fixconname(c);
    if (invalidconfig(cc)) {
      delete s;
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                      "melakukan login kembali."));
      return retvaluefail;
    }
    s->setConfig(cc);
    if (!s->open()) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
  
      return retvaluefail;
    }
    dbbase::placeholderprepareinsert prep;
    QString m_id = like;
    m_id.prepend("%");
    m_id.append("%");
    prep.push_back (std::make_pair(":0",m_id));
    prep.push_back(std::make_pair(":1", group));
    prep.push_back(std::make_pair(":2", limit.first));
    prep.push_back(std::make_pair(":3", limit.second));
    std::pair<bool, std::deque<QSqlRecord>> ret =
        s->prepareSelectFilter (sql, prep);
    if (!ret.first) {
      retvaluefail.setval(std::make_pair(false, emptyrec),
                          dbbase::SqlFetch::errortostring(s->lastError()));
      retvaluefail.seterrorcode(std::get<2>(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return retvaluefail;
    }
    utilproduk::selectdatahelper retvalue;
    QString empty("");
    retvalue.setval(std::move(ret), empty);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvalue;
}

 utilproduk::selectdatahelper helper_filterequalby2groupStringAndVariant(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &group, const QPair<qint64, qint64> &limit){
    utilproduk::sqlmodelwork wk1;
    return wk1.filterequalby2groupStringAndVariant (c,sql,like,group,limit);
}
 
 utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterequalby2groupStringAndVariantReady(const dbbase::configdb &c, const QString &sql, const QString &like, const QVariant &group, const QPair<qint64, qint64> &limit)
 {
     QFuture<utilproduk::selectdatahelper> ret=QtConcurrent::run(helper_filterequalby2groupStringAndVariant,c,sql,like,group,limit);
     ret.waitForFinished ();
     return ret.result ();
 }


utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterequalbysinglegroup(
    const dbbase::configdb &c, const QString &sql, const QVariant &group,
    const QPair<qint64, qint64> &limit) {
#ifdef BUILDRPCMODE
base::FilterequalbysinglegroupParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.myusername=c["username"].toString ().toStdString ();
param.mypassword=c["password"].toString ().toStdString ();
param.mysql=sql.toStdString ();
param.insqlparam=group; 
param.offsetlimit.first=limit.first;
param.offsetlimit.second=limit.second;
work::AskFilterequalbysinglegroup ask(param);
base::ResponseData myval=ask.process ();
return myval.selecthelper;
#else
  utilproduk::selectdatahelper retvaluefail;
  std::deque<QSqlRecord> emptyrec;
  emptyrec.clear();
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                    "melakukan login kembali."));
    return retvaluefail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }

    return retvaluefail;
  }
  dbbase::placeholderprepareinsert prep;
  prep.push_back(std::make_pair(":0", group));
  prep.push_back(std::make_pair(":1", limit.first));
  prep.push_back(std::make_pair(":2", limit.second));
  std::pair<bool, std::deque<QSqlRecord>> ret =
      s->prepareSelectFilter(sql, prep);
  if (!ret.first) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvaluefail;
  }
#ifdef DEBUGB
  qDebug() << "impldofilter: " << __FILE__ << ":" << __LINE__
           << " ret.second.size(): " << ret.second.size();
#endif
  utilproduk::selectdatahelper retvalue;
  QString empty("");
  retvalue.setval(std::move(ret), empty);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retvalue;
#endif
}

utilproduk::selectdatahelper helperFunc_filterequalbysinglegroup(const dbbase::configdb &c, const QString &sql, const QVariant &group,
                                                                 const QPair<qint64, qint64> &limit){
    utilproduk::sqlmodelwork wk1;
    return wk1.filterequalbysinglegroup (c,sql,group,limit);
}

utilproduk::selectdatahelper
utilproduk::sqlmodelwork::filterequalbysinglegroupReady(const dbbase::configdb &c, const QString &sql, const QVariant &group,
    const QPair<qint64, qint64> &limit, bool asy) {
    Q_UNUSED(asy)
    QFuture<utilproduk::selectdatahelper> ret=QtConcurrent::run(helperFunc_filterequalbysinglegroup,c,sql,group,limit);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectdatahelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql, group, limit]() {
//        return filterequalbysinglegroup(c, sql, group, limit);
//      });
//      return ret.get();
//    } catch (...) {
//      std::deque<QSqlRecord> emptyrec;
//      emptyrec.clear();
//      auto pair = std::make_pair(false, emptyrec);
//      utilproduk::selectdatahelper retfail;
//      retfail.setval(pair,
//                     QObject::tr("Galat, pastikan anda memiliki cukup memori "
//                                 "ram untuk menjalankan app ini."));
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectdatahelper(
//      const dbbase::configdb &, const QString &, const QVariant &,
//      const QPair<qint64, qint64> &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QVariant &group, const QPair<qint64, qint64> &limit) {
//    return filterequalbysinglegroup(c, sql, group, limit);
//  });
//  std::deque<QSqlRecord> emptyrec;
//  emptyrec.clear();
//  auto pair = std::make_pair(false, emptyrec);
//  utilproduk::selectdatahelper retfail;
//  retfail.setval(pair, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."));

//  std::future<utilproduk::selectdatahelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(group), std::cref(limit));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }
//  return result1.get();
}


utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterbysinglegroup(
    const dbbase::configdb &c, const QString &sql, const QString &group,
    const QPair<qint64, qint64> &limit) {
#ifdef BUILDRPCMODE
base::FilterequalbysinglegroupParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.myusername=c["username"].toString ().toStdString ();
param.mypassword=c["password"].toString ().toStdString ();
param.mysql=sql.toStdString ();
param.insqlparam=group; 
param.offsetlimit.first=limit.first;
param.offsetlimit.second=limit.second;
work::AskFilterequalbysinglegroup ask(param);
base::ResponseData myval=ask.process ();
return myval.selecthelper;
#else
  utilproduk::selectdatahelper retvaluefail;
  std::deque<QSqlRecord> emptyrec;
  emptyrec.clear();
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  if (invalidconfig(cc)) {
    delete s;
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        QObject::tr("Invalid sesi login ditemukan, anda bisa "
                                    "melakukan login kembali."));
    return retvaluefail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }

    return retvaluefail;
  }
  dbbase::placeholderprepareinsert prep;
  QString m_id = group;
  m_id.prepend("%");
  m_id.append("%");
  prep.push_back(std::make_pair(":0", m_id));
  prep.push_back(std::make_pair(":1", limit.first));
  prep.push_back(std::make_pair(":2", limit.second));
  std::pair<bool, std::deque<QSqlRecord>> ret =
      s->prepareSelectFilter(sql, prep);
  if (!ret.first) {
    retvaluefail.setval(std::make_pair(false, emptyrec),
                        dbbase::SqlFetch::errortostring(s->lastError()));
    retvaluefail.seterrorcode(std::get<2>(s->lastError()));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retvaluefail;
  }
#ifdef DEBUGB
  qDebug() << "impldofilter: " << __FILE__ << ":" << __LINE__
           << " ret.second.size(): " << ret.second.size();
#endif
  utilproduk::selectdatahelper retvalue;
  QString empty("");
  retvalue.setval(std::move(ret), empty);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retvalue;
#endif
}

utilproduk::selectdatahelper  helperFunc_filterbysinglegroup(const dbbase::configdb &c, const QString &sql, const QString &group,
                               const QPair<qint64, qint64> &limit){
    utilproduk::sqlmodelwork wk1;
    return wk1.filterbysinglegroup (c,sql,group,limit);
}

utilproduk::selectdatahelper utilproduk::sqlmodelwork::filterbysinglegroupReady(const dbbase::configdb &c, const QString &sql, const QString &group,
    const QPair<qint64, qint64> &limit, bool asy) {
  Q_UNUSED(asy)
    QFuture<utilproduk::selectdatahelper> ret=QtConcurrent::run(helperFunc_filterbysinglegroup,c,sql,group,limit);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectdatahelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql, group, limit]() {
//        return filterbysinglegroup(c, sql, group, limit);
//      });
//      return ret.get();
//    } catch (...) {
//      std::deque<QSqlRecord> emptyrec;
//      emptyrec.clear();
//      auto pair = std::make_pair(false, emptyrec);
//      utilproduk::selectdatahelper retfail;
//      retfail.setval(pair,
//                     QObject::tr("Galat, pastikan anda memiliki cukup memori "
//                                 "ram untuk menjalankan app ini."));
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectdatahelper(
//      const dbbase::configdb &, const QString &, const QString &,
//      const QPair<qint64, qint64> &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QString &group, const QPair<qint64, qint64> &limit) {
//    return filterbysinglegroup(c, sql, group, limit);
//  });
//  std::deque<QSqlRecord> emptyrec;
//  emptyrec.clear();
//  auto pair = std::make_pair(false, emptyrec);
//  utilproduk::selectdatahelper retfail;
//  retfail.setval(pair, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."));

//  std::future<utilproduk::selectdatahelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(group), std::cref(limit));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }
//  return result1.get();
}


utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countrecordjoin(const dbbase::configdb &c,
                                          const QString &sql) {
#ifdef BUILDRPCMODE
base::AskCountrecordjoinParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString ();
param.sql=sql.toStdString (); 
work::AskCountrecordjoin ask(param);
base::ResponseData myval=ask.process ();
return myval.selectcounthelper;
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  utilproduk::selectcounthelper retfail;
  if (invalidconfig(cc)) {
    delete s;
    retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                     "bisa melakukan login kembali."),
                     false);
    return retfail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  qint64 ret = s->selectCountJoin(sql);
  if (ret == -1) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  utilproduk::selectcounthelper retval;
  QString dummy("");
  retval.setvalue(ret, dummy, true);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retval;
#endif
}

utilproduk::selectcounthelper helperFunc_countrecordjoin(const dbbase::configdb &c,
                           const QString &sql){
    utilproduk::sqlmodelwork wk1;
    return wk1.countrecordjoin(c,sql);
}

utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countrerordjoinReady(const dbbase::configdb &c,
                                               const QString &sql, bool asy)
{
    Q_UNUSED(asy)
    QFuture<utilproduk::selectcounthelper> ret=QtConcurrent::run(helperFunc_countrecordjoin,c,sql);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectcounthelper> ret;
//    try {
//      ret = std::async(std::launch::async,
//                       [this, c, sql]() { return countrecordjoin(c, sql); });
//      return ret.get();
//    } catch (...) {
//      utilproduk::selectcounthelper retfail;
//      retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                       "memori ram untuk menjalankan app ini."),
//                       false);
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectcounthelper(const dbbase::configdb &,
//                                                   const QString &)>
//  task([this](const dbbase::configdb &c, const QString &tblname) {
//    return countrecordjoin(c, tblname);
//  });
//  utilproduk::selectcounthelper retfail;
//  retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."),
//                   false);

//  std::future<utilproduk::selectcounthelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
}


utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countrecord(const dbbase::configdb &c,
                                      const QString &tblname) {
#ifdef BUILDRPCMODE
base::AskCountrecordjoinParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString ();
QString sql("select count(*) from %1");
param.sql=sql.arg (tblname).toStdString (); 
work::AskCountrecordjoin ask(param);
base::ResponseData myval=ask.process ();
return myval.selectcounthelper;
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  utilproduk::selectcounthelper retfail;
  if (invalidconfig(cc)) {
    delete s;
    retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                     "bisa melakukan login kembali."),
                     false);
    return retfail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  qint64 ret = s->selectCount(tblname);
  if (ret == -1) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  utilproduk::selectcounthelper retval;
  QString dummy("");
  retval.setvalue(ret, dummy, true);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retval;
#endif
}


utilproduk::selectcounthelper helperFunc_countrecord(const dbbase::configdb &c,
                                                     const QString &tblname){
    utilproduk::sqlmodelwork wk1;
    return wk1.countrecord(c,tblname);
}

utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countrecordReady(const dbbase::configdb &c,
                                           const QString &tblname, bool asy) {
    Q_UNUSED(asy)
    QFuture<utilproduk::selectcounthelper> ret=QtConcurrent::run(helperFunc_countrecord,c,tblname);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectcounthelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, tblname]() {
//        return countrecord(c, tblname);
//      });
//      return ret.get();
//    } catch (...) {
//      utilproduk::selectcounthelper retfail;
//      retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                       "memori ram untuk menjalankan app ini."),
//                       false);
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectcounthelper(const dbbase::configdb &,
//                                                   const QString &)>
//  task([this](const dbbase::configdb &c, const QString &tblname) {
//    return countrecord(c, tblname);
//  });
//  utilproduk::selectcounthelper retfail;
//  retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                   "memori ram untuk menjalankan app ini."),
//                   false);

//  std::future<utilproduk::selectcounthelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(tblname));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
}


utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countequalfilternonerecord(const dbbase::configdb &c,
                                                     const QString &sql) {
#ifdef BUILDRPCMODE
base::AskCountrecordjoinParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString (); 
param.sql=sql.toStdString (); 
work::AskCountrecordjoin ask(param);
base::ResponseData myval=ask.process ();
return myval.selectcounthelper;
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  utilproduk::selectcounthelper retfail;
  if (invalidconfig(cc) || s == nullptr) {
    delete s;
    retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                     "bisa melakukan login kembali."),
                     false);
    return retfail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  dbbase::placeholderprepareinsert prepdummy;
  qint64 ret = s->prepareSelectFilterCount(sql, prepdummy);
  if (ret < 0) {
    retfail.setvalue(ret, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  utilproduk::selectcounthelper retval;
  QString dummy("");
  retval.setvalue(ret, dummy, true);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retval;
#endif
}

utilproduk::selectcounthelper helperFunc_countequalfilternonerecord(const dbbase::configdb &c,
                                                     const QString &tblname){
    utilproduk::sqlmodelwork wk1;
    return wk1.countequalfilternonerecord(c,tblname);
}

utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countequalfilternonerecordReady(
    const dbbase::configdb &c, const QString &sql, bool asy)

{
    Q_UNUSED(asy)
    QFuture<utilproduk::selectcounthelper> ret=QtConcurrent::run(helperFunc_countequalfilternonerecord,c,sql);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectcounthelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql]() {
//        return countequalfilternonerecord(c, sql);
//      });
//      return ret.get();
//    } catch (...) {
//      utilproduk::selectcounthelper retfail;
//      retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                       "memori ram untuk menjalankan app ini."),
//                       false);
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectcounthelper(const dbbase::configdb &,
//                                                   const QString &)>
//  task([this](const dbbase::configdb &c, const QString &sql) {
//    return countequalfilternonerecord(c, sql);
//  });
//  utilproduk::selectcounthelper retfail;
//  retfail.setvalue(-10, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                    "memori ram untuk menjalankan app ini."),
//                   false);
//  std::future<utilproduk::selectcounthelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
}

utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countequalfilterrecord(const dbbase::configdb &c,
                                                 const QString &sql,
                                                 const QVariant &groupfilter) {
#ifdef BUILDRPCMODE
base::AskCountequalfilterrecordParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString (); 
param.sql=sql.toStdString (); 
param.id=groupfilter;
work::AskCountequalfilterrecord ask(param);
base::ResponseData myval=ask.process ();
return myval.selectcounthelper;
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  utilproduk::selectcounthelper retfail;
  if (invalidconfig(cc)) {
    delete s;
    retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                     "bisa melakukan login kembali."),
                     false);
    return retfail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  dbbase::placeholderprepareinsert prep;
  prep.push_back(std::make_pair(":0", groupfilter));
  qint64 ret = s->prepareSelectFilterCount(sql, prep);
  if (ret < 0) {
    retfail.setvalue(ret, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  utilproduk::selectcounthelper retval;
  QString dummy("");
  retval.setvalue(ret, dummy, true);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retval;
#endif
}

utilproduk::selectcounthelper helperFunc_countequalfilterrecord(const dbbase::configdb &c,
                                                                const QString &sql,
                                                                const QVariant &groupfilter){
    utilproduk::sqlmodelwork wk1;
    return wk1.countequalfilterrecord(c,sql,groupfilter);
}

utilproduk::selectcounthelper
utilproduk::sqlmodelwork::countequalfilterrecordReady(
    const dbbase::configdb &c, const QString &sql, const QVariant &groupfilter,
    bool asy) {
    Q_UNUSED(asy)
    QFuture<utilproduk::selectcounthelper> ret=QtConcurrent::run(helperFunc_countequalfilterrecord,c,sql,groupfilter);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectcounthelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql, groupfilter]() {
//        return countequalfilterrecord(c, sql, groupfilter);
//      });
//      return ret.get();
//    } catch (...) {
//      utilproduk::selectcounthelper retfail;
//      retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                       "memori ram untuk menjalankan app ini."),
//                       false);
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectcounthelper(
//      const dbbase::configdb &, const QString &, const QVariant &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QVariant &groupfilter) {
//    return countequalfilterrecord(c, sql, groupfilter);
//  });
//  utilproduk::selectcounthelper retfail;
//  retfail.setvalue(-10, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                    "memori ram untuk menjalankan app ini."),
//                   false);
//  std::future<utilproduk::selectcounthelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(groupfilter));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
}

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countfilterrecord(
    const dbbase::configdb &c, const QString &sql, const QString &groupfilter) {
#ifdef BUILDRPCMODE
base::AskCountequalfilterrecordParam param;
auto tmpalamat=QString("%1:%2");
param.alamat=tmpalamat.arg (c["hostname"].toString ())
    .arg (c["port"].toString ()).toStdString ();
param.username=c["username"].toString ().toStdString ();
param.password=c["password"].toString ().toStdString (); 
param.sql=sql.toStdString (); 
param.id=groupfilter;
work::AskCountequalfilterrecord ask(param);
base::ResponseData myval=ask.process ();
return myval.selectcounthelper;
#else
  dbbase::SqlFetch *s = new dbbase::SqlFetch;
  auto cc = fixconname(c);
  utilproduk::selectcounthelper retfail;
  if (invalidconfig(cc)) {
    delete s;
    retfail.setvalue(-1, QObject::tr("Invalid sesi login ditemukan, anda "
                                     "bisa melakukan login kembali."),
                     false);
    return retfail;
  }
  s->setConfig(cc);
  if (!s->open()) {
    retfail.setvalue(-1, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  dbbase::placeholderprepareinsert prep;
  QString m_id = groupfilter;
  m_id.prepend("%");
  m_id.append("%");
  prep.push_back(std::make_pair(":0", m_id));
  qint64 ret = s->prepareSelectFilterCount(sql, prep);
  if (ret < 0) {
    retfail.setvalue(ret, dbbase::SqlFetch::errortostring(s->lastError()),
                     false);
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return retfail;
  }
  utilproduk::selectcounthelper retval;
  QString dummy("");
  retval.setvalue(ret, dummy, true);
  delete s;
  if (!cc["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(cc["conname"].toString());
  }
  return retval;
#endif
}

utilproduk::selectcounthelper helperFunc_countfilterrecord(
        const dbbase::configdb &c, const QString &sql, const QString &groupfilter){
    utilproduk::sqlmodelwork wk1;
    return wk1.countfilterrecord(c,sql,groupfilter);
}

utilproduk::selectcounthelper utilproduk::sqlmodelwork::countfilterrecordReady(
    const dbbase::configdb &c, const QString &sql, const QString &groupfilter,
    bool asy) {
    Q_UNUSED(asy)
    QFuture<utilproduk::selectcounthelper> ret=QtConcurrent::run(helperFunc_countfilterrecord,c,sql,groupfilter);
    ret.waitForFinished ();
    return ret.result ();
//  if (asy) {
//    std::future<utilproduk::selectcounthelper> ret;
//    try {
//      ret = std::async(std::launch::async, [this, c, sql, groupfilter]() {
//        return countfilterrecord(c, sql, groupfilter);
//      });
//      return ret.get();
//    } catch (...) {
//      utilproduk::selectcounthelper retfail;
//      retfail.setvalue(-1, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                       "memori ram untuk menjalankan app ini."),
//                       false);
//      return retfail;
//    }
//  }
//  std::packaged_task<utilproduk::selectcounthelper(
//      const dbbase::configdb &, const QString &, const QString &)>
//  task([this](const dbbase::configdb &c, const QString &sql,
//              const QString &groupfilter) {
//    return countfilterrecord(c, sql, groupfilter);
//  });
//  utilproduk::selectcounthelper retfail;
//  retfail.setvalue(-10, QObject::tr("Galat, pastikan anda memiliki cukup "
//                                    "memori ram untuk menjalankan app ini."),
//                   false);
//  std::future<utilproduk::selectcounthelper> result1 = task.get_future();
//  try {
//    std::thread task_td(std::move(task), std::cref(c), std::cref(sql),
//                        std::cref(groupfilter));
//    task_td.join();
//  } catch (const std::system_error &) {
//    return retfail;
//  } catch (...) {
//    return retfail;
//  }

//  return result1.get();
}
