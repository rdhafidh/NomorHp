#include "sqlfetch.h"
namespace dbbase {

SqlFetch::SqlFetch() {
#ifdef DEBUGB
  //qDebug() << "QThread::currentThreadId();: " << QThread::currentThreadId()
           << __FILE__ << __LINE__;
#endif
  db.reset();
}

SqlFetch::~SqlFetch() {
#ifdef DEBUGB
  //qDebug() << "QThread::~currentThreadId();: " << QThread::currentThreadId()
           << __FILE__ << __LINE__;
#endif
  if (db) {
    if (db->isOpen()) {
      db->close();
    }
  }
}

void SqlFetch::setConfig(const configdb &c) {
  /*
   * struktur config
   * 1. dbbackend
   * 2. hostname
   * 3. dbname
   * 4. username
   * 5. password
   * 6. connectopt
   * 7. port
   * 8. conname
   **/
  cf = c;
  if (cf.contains("conname")) {
#ifdef DEBUGB
    //qDebug() << "conname: " << cf["conname"];
#endif
    db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase(
        cf["dbbackend"].toString(), cf["conname"].toString()));
  } else {
    db = std::make_unique<QSqlDatabase>(
        QSqlDatabase::addDatabase(cf["dbbackend"].toString()));
  }
  db->setDatabaseName(cf["dbname"].toString());

  if (cf["dbbackend"].toString().compare(QLatin1String("QPSQL"),
                                         Qt::CaseSensitive) == 0 ||
      cf["dbbackend"].toString().compare(QLatin1String("QMYSQL"),
                                         Qt::CaseSensitive) == 0) {
    db->setHostName(cf["hostname"].toString());
    db->setUserName(cf["username"].toString());
    db->setPassword(cf["password"].toString());
    db->setPort(cf["port"].toInt());
    if (cf.contains("connectopt")) {
      db->setConnectOptions(cf["connectopt"].toString());
    }
  }
  db->open();
}

bool SqlFetch::open() const {
  if (db) {
    return (db->isOpen() && db->isValid() && !db->isOpenError());
  }
  return false;
}

sqlerror SqlFetch::lastError() const {
  if (db) {
    if (db->lastError().isValid()) {
      return std::make_tuple(
          QObject::tr("Galat : %1").arg(db->lastError().text()),
          db->lastError().type(), db->lastError().nativeErrorCode());
    }

    return error;
  }
  sqlerror errdata;
  errdata = std::make_tuple(
      QStringLiteral("Galat internal error koneksi database : kode -1000e3"),
      QSqlError::UnknownError, QStringLiteral("-1000e3"));
  return errdata;
}

ret SqlFetch::prepareSelectFilter(const QString &s,
                                  const placeholderprepareinsert &p) {
  std::deque<QSqlRecord> rec;
  if (!open()) {
    error = this->lastError();
    return std::make_pair(false, rec);
  }
  QSqlQuery qr(*db);
  qr.prepare(s);
  for (size_t i = 0; i < p.size(); i++) {
#ifdef DEBUGB
    //qDebug() << "" << __FILE__ << __LINE__;
#endif
    //qDebug()<<"p.at(i).first, p.at(i).second"<<p.at(i).first<< p.at(i).second;
    qr.bindValue(p.at(i).first, p.at(i).second);
  }
  bool ok = qr.exec();
#ifdef DEBUGB
  //qDebug() << "qr.lastQuery(): " << qr.lastQuery();
#endif
  if (!ok) {
    error = std::make_tuple(
        QObject::tr("query select error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return std::make_pair(false, rec);
  } else {
     //qDebug() << " qr.size ()"<< qr.size ();
    while (qr.next()) {
      rec.push_back(qr.record());
    }
  }
  //qDebug() << "qr.lastQuery(): " << qr.lastQuery();
  qr.finish();
  return std::make_pair(true, rec);
}

ret SqlFetch::rawSelect(const QString &s) {
  std::deque<QSqlRecord> rec;
  if (!open()) {
    error = this->lastError();
    return std::make_pair(false, rec);
  }
  QSqlQuery qr(*db);
  bool r = qr.exec(s);
  if (!r) {
    error = std::make_tuple(
        QObject::tr("query select error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return std::make_pair(false, rec);
  } else {
    while (qr.next()) {
      rec.push_back(qr.record());
    }
  }
  qr.finish();
  return std::make_pair(true, rec);
}

bool SqlFetch::checkrowidexist(const QString &sql,
                               const placeholderprepareinsert &p) {
  if (!open()) {
    error = this->lastError();
    return false;
  }
  QSqlQuery qr(*db);
  qr.prepare(sql);
  for (size_t i = 0; i < p.size(); i++) {
    qr.bindValue(p.at(i).first, p.at(i).second);
  }
  bool ok = qr.exec();
#ifdef DEBUGB
  //qDebug() << "last query: " << qr.lastQuery();
#endif
  if (!ok) {
    error = std::make_tuple(
        QObject::tr("query select error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return ok;
  }
  bool exists = false;
  while (qr.next()) {
    exists = true;
  }
  qr.finish();
  if (!exists) {
    error = std::make_tuple(QObject::tr("Galat id tidak ditemukan"),
                            qr.lastError().type(), "");
  }
  return exists;
}

bool SqlFetch::preparedStm(const QString &sql,
                           const placeholderprepareinsert &p) {
  if (!open()) {
    error = this->lastError();
    return false;
  }
  QSqlQuery qr(*db);
  qr.prepare(sql);
  for (size_t i = 0; i < p.size(); i++) {
    qr.bindValue(p.at(i).first, p.at(i).second);
  }
  bool ok = qr.exec();
#ifdef DEBUGB
  //qDebug() << "last query: " << qr.lastQuery() << __FILE__ << __LINE__;
#endif
  if (!ok) {
    error = std::make_tuple(
        QObject::tr("query select error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return ok;
  }
  qr.finish();
  return ok;
}

bool SqlFetch::preparedbatchstm2(
    const QString &sql, const std::pair<QVariantList, QVariantList> &perrows) {
  if (!open()) {
    error = std::make_tuple(
        QObject::tr("Sesi login galat, koneksi database gagal "),
        QSqlError::ConnectionError, QObject::tr(""));
    return false;
  }

  if (perrows.first.size() != perrows.second.size()) {
#ifdef DEBUGB
    //qDebug() << "ERROR MISMATCH execbatch binvalues: " << __FILE__ << ":"
             << __LINE__;
#endif
    return false;
  }
  QSqlQuery qr(*db);
  qr.prepare(sql);
  for (int rw1 = 0; rw1 < perrows.first.size(); rw1++) {
    qr.addBindValue(perrows.first.at(rw1));
    qr.addBindValue(perrows.second.at(rw1));
  }
  if (!qr.execBatch()) {
    qr.finish();
    return false;
  }
  qr.finish();
  return true;
}

bool SqlFetch::crdelalter(const QString &sql) {
  if (!open()) {
    error = this->lastError();
    return false;
  }
  QSqlQuery qr(*db);
  bool ok = qr.exec(sql);
#ifdef DEBUGB
  //qDebug() << "last query: " << qr.lastQuery() << __FILE__ << __LINE__;
#endif
  if (!ok) {
    error = std::make_tuple(
        QObject::tr("query error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return ok;
  }
  qr.finish();
  return ok;
}

std::pair<bool, QVariant> SqlFetch::insertWithParam(const QString &sql, const placeholderprepareinsert &p)
{
    if (!open()) {
      error = this->lastError();
      return std::make_pair(false,QVariant());
    }
    QSqlQuery qr(*db);
   if(!qr.prepare (sql)){
       error = std::make_tuple(
           QObject::tr("query error: %1").arg(qr.lastError().text()),
           qr.lastError().type(), qr.lastError().nativeErrorCode());
       return  std::make_pair(false,QVariant());
   }
   for(decltype(p.size())x=0;x<p.size();x++){
     qr.bindValue (p.at (x).first,p.at(x).second); 
   }
    
    bool ok = qr.exec();
  #ifdef DEBUGB
    //qDebug() << "last query: " << qr.lastQuery() << __FILE__ << __LINE__;
  #endif
    if (!ok) {
      error = std::make_tuple(
          QObject::tr("query error: %1").arg(qr.lastError().text()),
          qr.lastError().type(), qr.lastError().nativeErrorCode());
      qr.finish();
      return std::make_pair(false,QVariant()); 
    }
    auto lastid=qr.lastInsertId ();
    qr.finish();
    return std::make_pair(true,lastid);  
}

bool SqlFetch::crdelalterWithParam(const QString &sql, const placeholderprepareinsert &p)
{
    if (!open()) {
      error = this->lastError();
      return false;
    }
    QSqlQuery qr(*db);
   if(!qr.prepare (sql)){
       error = std::make_tuple(
           QObject::tr("query error: %1").arg(qr.lastError().text()),
           qr.lastError().type(), qr.lastError().nativeErrorCode());
       return false;
   }
   for(decltype(p.size())x=0;x<p.size();x++){
     qr.bindValue (p.at (x).first,p.at(x).second); 
   }
    
    bool ok = qr.exec();
  #ifdef DEBUGB
    //qDebug() << "last query: " << qr.lastQuery() << __FILE__ << __LINE__;
  #endif
    if (!ok) {
      error = std::make_tuple(
          QObject::tr("query error: %1").arg(qr.lastError().text()),
          qr.lastError().type(), qr.lastError().nativeErrorCode());
      qr.finish();
      return ok;
    }
    qr.finish();
    return true;
}

qint64 SqlFetch::selectCountJoin(const QString &sql) {
  if (!open()) {
    error = this->lastError();
    return -2;
  }
  QSqlQuery qr(*db);
  QString psql = sql;
  psql.remove(";");
  std::string prep = std::string("select count(*) from ( ") +
                     psql.toStdString() + std::string(" ) as baris");
  bool r = qr.exec(prep.c_str());

  qint64 n = -1;
  if (!r) {
    error = std::make_tuple(
        QObject::tr("query select count error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return n;
  }
  while (qr.next()) {
    n = qr.value(0).toLongLong();
  }
  qr.finish();
  return n;
}

qint64 SqlFetch::selectCount(const QString &tblname) {
  if (!open()) {
    error = this->lastError();
    return -2;
  }
  QSqlQuery qr(*db);
  std::string prep =
      std::string("select count(*) from ") + tblname.toStdString();
  bool r = qr.exec(prep.c_str());

  qint64 n = -1;
  if (!r) {
    error = std::make_tuple(
        QObject::tr("query select count error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return n;
  }
  while (qr.next()) {
    n = qr.value(0).toLongLong();
  }
  qr.finish();
  return n;
}

qint64 SqlFetch::prepareSelectFilterCount(const QString &s,
                                          const placeholderprepareinsert &p) {
  if (!open()) {
    error = this->lastError();
    return -2;
  }
  QSqlQuery qr(*db);
  qr.prepare(s);
  for (size_t i = 0; i < p.size(); i++) {
    qr.bindValue(p.at(i).first, p.at(i).second);
  }
  bool ok = qr.exec();
  qint64 n = -1;
  if (!ok) {
    error = std::make_tuple(
        QObject::tr("query select error: %1").arg(qr.lastError().text()),
        qr.lastError().type(), qr.lastError().nativeErrorCode());
    qr.finish();
    return n;
  } else {
    n = 0;
    while (qr.next()) {
      n++;
    }
  }
  //qDebug()<<"last genertaed sql: "<<qr.lastQuery ();
  qr.finish();
  return n;
}

QSqlRecord SqlFetch::record(const QString &tblname) {
  if (db) {
    return db->record(tblname);
  }
  QSqlRecord dummy;
  return dummy;
}

bool SqlFetch::checkreopenifnot() {
  if (db) {
    if (!db->isOpen() || !this->open() || !this->testvalidopen()) {
#ifdef DEBUGB
      //qDebug() << "internal db handle is not open re-opening again..";
#endif
      db->close();
      db.reset();
      if (!cf["conname"].toString().isEmpty()) {
#ifdef DEBUGB
        //qDebug() << "removing old db connection name: "
                 << cf["conname"].toString();
#endif
        QSqlDatabase::removeDatabase(cf["conname"].toString());
      }
      QDateTime tm = QDateTime::currentDateTime();
      cf["conname"] = "koneksi-" + tm.toString("dddd-MM-yyyy.hh.ss.zzz");
      setConfig(cf);
      return this->testvalidopen();
    }
    return true;
  }
  return false;
}

bool SqlFetch::testvalidopen() {
  ret result;
  result.first = false;
  if (cf["dbbackend"].toString().compare(QLatin1String("QPSQL"),
                                         Qt::CaseSensitive) == 0 ||
      cf["dbbackend"].toString().compare(QLatin1String("QMYSQL"),
                                         Qt::CaseSensitive) == 0) {
    result = this->rawSelect("select CURRENT_DATE;");
  }
  return result.first;
}
}
