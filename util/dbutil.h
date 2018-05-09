#ifndef DBUTIL_H
#define DBUTIL_H
#include <future>
#include <memory>
#include <thread>
#include <tuple>
#include <utility>
#include "invalidkoneksi.h"
#include "settinghandler.h"

#include "uiutil.h"
namespace filedb {
struct dbapp {
  static bool checkdbfolder(QWidget *parent, const QString &p) {
    auto pg_hba = QDir::toNativeSeparators(p + "/pg_hba.conf");
    auto pg_ident = QDir::toNativeSeparators(p + "/pg_ident.conf");
    auto postgresql = QDir::toNativeSeparators(p + "/postgresql.conf");
    QFile p1(pg_hba), p2(pg_ident), p3(postgresql);
#ifdef DEBUGB
    qDebug() << "hba: " << pg_hba;
    qDebug() << "ident: " << pg_ident;
    qDebug() << "postgresql: " << postgresql;
#endif
    if (!p1.exists() || !p2.exists() || !p3.exists()) {
      error::dialog::emitdialogerror(
          parent, QObject::tr("Invalid database folder"),
          QObject::tr("Kami menemukan database folder yang tidak sah. Anda "
                      "bisa memilih folder yang benar lainnya atau re-install "
                      "database folder yang baru."));
      return false;
    }
    return true;
  }
  bool removerecfd(const QString &d) {
    QDir dr(d);

    if (!dr.removeRecursively()) {
      return false;
    }
    return true;
  }
  bool removerecfdready(const QString &d) {
    std::packaged_task<bool(const QString &)> task(
        [this](const QString &d) { return removerecfd(d); });
    std::future<bool> result1 = task.get_future();
    try {
      std::thread task_td(std::move(task), std::cref(d));
      task_td.join();
    } catch (const std::system_error &) {
      return false;
    } catch (...) {
      return false;
    }
    return result1.get();
  }

  static bool checkcommonapp(const QString &p) {
    if (!ex_initdb(p) || !ex_pg_ctl(p) || !ex_pg_dump(p) || !ex_pg_isready(p) ||
        !ex_pg_restore(p) || !ex_postgres(p) || !ex_psql(p)) {
      return false;
    }
    return true;
  }

  static bool ex_clusterdb(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/clusterdb.exe");
#else
    QString re(p + "/clusterdb");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }
  static bool ex_createdb(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/createdb.exe");
#else
    QString re(p + "/createdb");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }
  static bool ex_initdb(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/initdb.exe");
#else
    QString re(p + "/initdb");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_pg_ctl(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/pg_ctl.exe");
#else
    QString re(p + "/pg_ctl");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_pg_dump(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/pg_dumpall.exe");
#else
    QString re(p + "/pg_dumpall");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_pg_isready(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/pg_isready.exe");
#else
    QString re(p + "/pg_isready");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_pg_restore(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/pg_restore.exe");
#else
    QString re(p + "/pg_restore");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_postgres(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/postgres.exe");
#else
    QString re(p + "/postgres");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool ex_psql(const QString &p) {
#ifdef Q_OS_WIN
    QString re(p + "/psql.exe");
#else
    QString re(p + "/psql");
#endif
    QFile f(re);
    if (!f.exists()) {
      return false;
    }
    return true;
  }

  static bool isfileordirempty(const QString &r) {
    QDir s(r);
    qint64 i = 0;
    foreach (QFileInfo in,
             s.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Files |
                             QDir::AllDirs | QDir::AllEntries)) {
      if (in.isDir()) {
#ifdef DEBUGB
        qDebug() << "found dir i: " << i;
#endif
        ++i;
      }
      if (in.isFile()) {
        ++i;
#ifdef DEBUGB
        qDebug() << "found file i: " << i;
#endif
      }
    }
    if (i > 0) {
#ifdef DEBUGB
      qDebug() << "return false: " << i;
#endif
      return false;
    }
#ifdef DEBUGB
    qDebug() << "return true: " << i;
#endif

    return true;
  }
};
}

namespace koneksi {
class dbutil {
 public:
  dbutil() {}
  ~dbutil() {}

  static bool testkoneksi(const dbbase::configdb &c) {
    dbbase::SqlFetch *s = new dbbase::SqlFetch();

    auto cc = fixconname(c);

    if (invalidconfig(cc)) {
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        //             QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << " c isempty: " << c.isEmpty();
#endif
      return false;
    }

    s->setConfig(cc);
    if (!s->open()) {
      s->close();
      delete s;
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__;
#endif
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return false;
    }
    s->close();
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__;
#endif
    return true;
  }
  std::pair<QStringList, QString> daftardatabase(const dbbase::configdb &c) {
    dbbase::SqlFetch *s = new dbbase::SqlFetch();

    QStringList aa;

    auto cc = fixconname(c);

    if (invalidconfig(cc)) {
      auto r =
          std::make_pair(aa, QObject::tr("Galat, konfigurasi login invalid"));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        //              QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return r;
    }

    s->setConfig(cc);
    if (!s->open()) {
      // if qstring is not empty then error happen here
      auto r =
          std::make_pair(aa, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return r;
    }
    auto rets = s->rawSelect(sqlstr::daftardatabase::data());
    if (!rets.first) {
      auto r =
          std::make_pair(aa, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return r;
    }
    for (size_t k = 0; k < rets.second.size(); k++) {
      aa.append(std::move(rets.second.at(k).field(0).value().toString()));
    }
    QString dummy("");
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return std::make_pair(aa, dummy);
  }
  std::pair<QStringList, QString> daftardatabaseready(
      const dbbase::configdb &c) {
    std::packaged_task<std::pair<QStringList, QString>(
        const dbbase::configdb &)>
    task([this](const dbbase::configdb &e) { return daftardatabase(e); });
    std::future<std::pair<QStringList, QString> > result1 = task.get_future();
    try {
      std::thread task_td(std::move(task), std::cref(c));
      task_td.join();
    }

    catch (const std::system_error &) {
      return std::make_pair(QStringList(),
                            QObject::tr("Daftar database system error"));
    } catch (...) {
      return std::make_pair(
          QStringList(),
          QObject::tr("Daftar database error thread tidak diketahui. Periksa "
                      "kembali, pastikan komputer ini memiliki memori ram yg "
                      "cukup untuk dijalankan."));
    }

    return result1.get();
  }
  std::pair<bool, QString> changedbname(const dbbase::configdb &c,
                                        const QString &from,
                                        const QString &to) {
    dbbase::SqlFetch *s = new dbbase::SqlFetch;

    auto cc = fixconname(c);

    if (invalidconfig(cc) || from.isEmpty() || to.isEmpty()) {
      auto ret =
          std::make_pair(false, QObject::tr("Invalid konfigurasi login."));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        //               QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }

    s->setConfig(cc);
    if (!s->open()) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }
    if (!s->crdelalter(sqlstr::renamedatabase::data(from, to))) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }
    auto ret = std::make_pair(true, QLatin1String(""));
    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  std::pair<bool, QString> changedbnameready(const dbbase::configdb &c,
                                             const QString &from,
                                             const QString &to) {
    std::packaged_task<std::pair<bool, QString>(
        const dbbase::configdb &, const QString &, const QString &)>
    task([this](const dbbase::configdb &c, const QString &from,
                const QString &to) { return changedbname(c, from, to); });
    std::future<std::pair<bool, QString> > result1 = task.get_future();
    try {
      std::thread task_td(std::move(task), std::cref(c), std::cref(from),
                          std::cref(to));
      task_td.join();
    }

    catch (const std::system_error &) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki memori ram yang "
                             "cukup untuk menjalankan app ini."));
    } catch (...) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki memori ram yang "
                             "cukup untuk menjalankan app ini."));
    }

    return result1.get();
  }

  std::pair<bool, QString> createdb(const dbbase::configdb &c,
                                    const QString &dbbaru) {
    dbbase::SqlFetch *s = new dbbase::SqlFetch;

    auto cc = fixconname(c);

    if (invalidconfig(cc) || dbbaru.isEmpty()) {
      auto ret =
          std::make_pair(false, QObject::tr("invalid konfigurasi login"));

      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        //               QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }

    s->setConfig(cc);
    if (!s->open()) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }
    if (!s->crdelalter(sqlstr::tambahdatabase::data(dbbaru))) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }

    auto ret = std::make_pair(true, QLatin1String(""));

    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  std::pair<bool, QString> createdbready(const dbbase::configdb &c,
                                         const QString &dbbaru) {
    std::packaged_task<std::pair<bool, QString>(const dbbase::configdb &,
                                                const QString &)>
    task([this](const dbbase::configdb &c, const QString &dbbaru) {
      return createdb(c, dbbaru);
    });
    std::future<std::pair<bool, QString> > result1 = task.get_future();
    try {
      std::thread task_td(std::move(task), std::cref(c), std::cref(dbbaru));
      task_td.join();
    } catch (const std::system_error &) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki memori ram yang "
                             "cukup untuk menjalankan app ini."));
    } catch (...) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki memori ram yang "
                             "cukup untuk menjalankan app ini."));
    }

    return result1.get();
  }
  std::pair<bool, QString> hapusdb(const dbbase::configdb &c,
                                   const QString &dbname) {
    dbbase::SqlFetch *s = new dbbase::SqlFetch;

    auto cc = fixconname(c);

    if (invalidconfig(cc) || dbname.isEmpty()) {
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        //               QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return std::make_pair(false,
                            QObject::tr("Invalid sesi login ditemukan, anda "
                                        "bisa melakukan login kembali."));
    }

    s->setConfig(cc);
    if (!s->open()) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }
    if (!s->crdelalter(sqlstr::dropdatabase::data(dbname))) {
      auto ret = std::make_pair(
          false, dbbase::SqlFetch::errortostring(s->lastError()));
      delete s;
      if (!cc["conname"].toString().isEmpty()) {
        QSqlDatabase::removeDatabase(cc["conname"].toString());
      }
      return ret;
    }
    auto ret = std::make_pair(true, QLatin1String(""));

    delete s;
    if (!cc["conname"].toString().isEmpty()) {
      QSqlDatabase::removeDatabase(cc["conname"].toString());
    }
    return ret;
  }
  std::pair<bool, QString> hapusdbready(const dbbase::configdb &c,
                                        const QString &dbname) {
    std::packaged_task<std::pair<bool, QString>(const dbbase::configdb &,
                                                const QString &)>
    task([this](const dbbase::configdb &c, const QString &dbname) {
      return hapusdb(c, dbname);
    });
    std::future<std::pair<bool, QString> > result1 = task.get_future();
    try {
      std::thread task_td(std::move(task), std::cref(c), std::cref(dbname));
      task_td.join();
    } catch (const std::system_error &) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
                             "untuk menjalankan app ini."));
    } catch (...) {
      return std::make_pair(
          false, QObject::tr("Galat, pastikan anda memiliki cukup memori ram "
                             "untuk menjalankan app ini."));
    }

    return result1.get();
  }
};

class server : public QObject {
  Q_OBJECT
 public:
  server() : modes(false) { mp.reset(new PWrapper()); }
  ~server() {
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  }
  void setdeatach(bool t)
  {
    mp->setdeatach(t);
  }

  void serve(const QString &rootdb, const QString &rootdbdata, bool mode) {
    // read config file, assume it is valid using default config

    modes = mode;
    connect(mp.data(), SIGNAL(emitError(QString)), this,
            SLOT(onError(QString)));
    connect(mp.data(), SIGNAL(emitOutput(QString)), this,
            SLOT(onOutput(QString)));
    connect(mp.data(), SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp.data(), SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));

    QDir pyhome(rootdb);
#ifndef Q_OS_WIN
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10, false);
      return;
    }
#endif
#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_ctl.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_ctl");
#endif
    if (mode) {  // on
      QStringList args;
      QDateTime tm = QDateTime::currentDateTime();
      auto log =
          QDir::toNativeSeparators(rootdbdata +
                                   QString("/logfile_koneksi_%1.log")
                                       .arg(tm.toString("dddd_dd_MM_yyyy")));
      args << "start"<<"-W"<<"-c"
           << "-D" << QDir::toNativeSeparators(rootdbdata) << "-l" << log;

      mp->start(prog, args, pyhome.path());
    } else {  // off
      // force shutdown
      QStringList args;
      args << "stop"
           <<"-W"
           << "-D" << QDir::toNativeSeparators(rootdbdata) << "-m"
           << "fast";

      mp->start(prog, args, pyhome.path());
    }
  }

 Q_SIGNALS:
  void emitfinish(int e, bool mode);
  void onoutput(const QString &o, bool mode);
  void onstart(bool mode);
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o, modes); }
  void onStart() { Q_EMIT onstart(modes); }
  void onFinish(int e, QProcess::ExitStatus) { Q_EMIT emitfinish(e, modes); }

 private:
  bool modes;
  QScopedPointer<PWrapper> mp;
};

class isready : public QObject {
  Q_OBJECT
 public:
  isready() { mp.reset(new PWrapper()); }
  ~isready() {}
  void check(const QString &rootdb, const QString &host, int port) {
    connect(mp.data(), SIGNAL(emitError(QString)), this,
            SLOT(onError(QString)));
    connect(mp.data(), SIGNAL(emitOutput(QString)), this,
            SLOT(onOutput(QString)));
    connect(mp.data(), SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp.data(), SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_isready.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_isready");
#endif

    QStringList args;
    args << "-h" << host << "-p" << QString("%1").arg(port);

    mp->start(prog, args, pyhome.path());
  }
  void checkwithusername(const QString &rootdb, const QString &user,
                         const QString &host, int port) {
    mp.reset(new PWrapper());
    connect(mp.data(), SIGNAL(emitError(QString)), this,
            SLOT(onError(QString)));
    connect(mp.data(), SIGNAL(emitOutput(QString)), this,
            SLOT(onOutput(QString)));
    connect(mp.data(), SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp.data(), SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_isready.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_isready");
#endif

    QStringList args;
    args << "-U" << user << "-h" << host << "-p" << QString("%1").arg(port);

    mp->start(prog, args, pyhome.path());
  }

 Q_SIGNALS:
  void emitfinish(int e);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o); }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) { Q_EMIT emitfinish(e); }

 private:
  QScopedPointer<PWrapper> mp;
};

class createdb : public QObject {
  Q_OBJECT
 public:
  createdb() { mp.reset(new PWrapper()); }
  ~createdb() {}
  void create(const QString &rootdb, const dbbase::configdb &c,
              const QString dbname) {
    /* dbbase::configdb c;
     c["dbbackend"]="QPSQL";
     c["hostname"]=SettingHandler::getSetting("database","alamatdb");
     c["dbname"]=SettingHandler::getSetting("database","namadb");
     c["username"]=SettingHandler::getSetting("database","namauser");
     c["password"]=SettingHandler::getSetting("database","password");
     c["port"]=SettingHandler::getSetting("database","port");
     QDateTime tm=QDateTime::currentDateTime();
     c["conname"] =
     QString("koneksi-%1").arg(tm.toString("dddd_dd_MM_yyyy::HH_mm_ss"));

     */
    // assume c had been validated
    connect(mp.data(), SIGNAL(emitError(QString)), this,
            SLOT(onError(QString)));
    connect(mp.data(), SIGNAL(emitOutput(QString)), this,
            SLOT(onOutput(QString)));
    connect(mp.data(), SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp.data(), SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10, dbname);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/createdb.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/createdb");
#endif
    //        valid cmd ex:
    //        createdb -h localhost -p 5432 -U admin -E UTF8 -T template0
    //        databasebaru
    QStringList args;
    args << "-U" << c["username"].toString() << "-h" << c["hostname"].toString()
         << "-p" << c["port"].toString() << "-E"
         << "UTF8"
         << "-T"
         << "template0" << dbname;
    //        supply password set in temp env
    dbbaru = dbname;
    mp->startwithpwd(prog, args, pyhome.path(), c["password"].toString());
  }

 Q_SIGNALS:
  void emitfinish(int e, const QString &f);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o); }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) { Q_EMIT emitfinish(e, dbbaru); }

 private:
  QString dbbaru;
  QScopedPointer<PWrapper> mp;
};

class serverdbisready : public QObject {
  Q_OBJECT
 public:
  serverdbisready() { mp.reset(new PWrapper()); }
  ~serverdbisready() {}
  void isdbready(const QString &rootdb, const QString &dbfolder) {
    connect(mp.data(), SIGNAL(emitError(QString)), this,
            SLOT(onError(QString)));
    connect(mp.data(), SIGNAL(emitOutput(QString)), this,
            SLOT(onOutput(QString)));
    connect(mp.data(), SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp.data(), SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
    QDir pyhome(rootdb);
#ifndef Q_OS_WIN
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }
#endif

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_ctl.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_ctl");
#endif
    QStringList args;
    args << "status"
         << "-D" << QDir::toNativeSeparators(dbfolder);

    mp->start(prog, args, pyhome.path());
    return;
  }

 Q_SIGNALS:
  void emitfinish(int e);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o); }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) {
    //         if ret is 0 then ok, server is already running in that valid
    //         dbfolder directory
    //        if ret is 3 then no server is running in that valid dbfolder
    //        directory
    //        if ret is 4 then that is invalid dbfolder directory
    Q_EMIT emitfinish(e);
  }

 private:
  QScopedPointer<PWrapper> mp;
};

class restoredb : public QObject {
  Q_OBJECT
 public:
  restoredb() { mp = new PWrapper(); }
  ~restoredb() { mp->deleteLater(); }
  void restore(const QString &rootdb, const dbbase::configdb &c,
               const QString dbbaru, const QString fromsqlfile) {
    /* dbbase::configdb c;
     c["dbbackend"]="QPSQL";
     c["hostname"]=SettingHandler::getSetting("database","alamatdb");
     c["dbname"]=SettingHandler::getSetting("database","namadb");
     c["username"]=SettingHandler::getSetting("database","namauser");
     c["password"]=SettingHandler::getSetting("database","password");
     c["port"]=SettingHandler::getSetting("database","port");
     QDateTime tm=QDateTime::currentDateTime();
     c["conname"] =
     QString("koneksi-%1").arg(tm.toString("dddd_dd_MM_yyyy::HH_mm_ss"));

     */
    // assume c had been validated
    connect(mp, SIGNAL(emitError(QString)), this, SLOT(onError(QString)));
    connect(mp, SIGNAL(emitOutput(QString)), this, SLOT(onOutput(QString)));
    connect(mp, SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp, SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/psql.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/psql");
#endif
    //        valid cmd ex:
    //        psql -h localhost -p 5432 -d databasebaru -U admin -f foo.sql
    QStringList args;
    args << "-U" << c["username"].toString() << "-h" << c["hostname"].toString()
         << "-p" << c["port"].toString() << "-d" << dbbaru << "-f"
         << QDir::toNativeSeparators(fromsqlfile);
    //        supply password set in temp env
    mp->startwithpwd(prog, args, pyhome.path(), c["password"].toString());
  }

 Q_SIGNALS:
  void emitfinish(int e);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o); }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) { Q_EMIT emitfinish(e); }

 private:
  PWrapper *mp;
};

class dumpsingledb : public QObject {
  Q_OBJECT
 public:
  dumpsingledb() {
    mp = new PWrapper;
    connect(mp, SIGNAL(emitError(QString)), this, SLOT(onError(QString)));
    connect(mp, SIGNAL(emitOutput(QString)), this, SLOT(onOutput(QString)));
    connect(mp, SIGNAL(emitStart()), this, SLOT(onStart()));
    connect(mp, SIGNAL(emitFinish(int, QProcess::ExitStatus)), this,
            SLOT(onFinish(int, QProcess::ExitStatus)));
  }
  ~dumpsingledb() { mp->deleteLater(); }
  //        note that dump single db, doesnt contain any create database
  //        statement in sql generate file
  void dump(const QString &rootdb, const dbbase::configdb &c,
            const QString dblama, const QString tosqlfile) {
    /* dbbase::configdb c;
     c["dbbackend"]="QPSQL";
     c["hostname"]=SettingHandler::getSetting("database","alamatdb");
     c["dbname"]=SettingHandler::getSetting("database","namadb");
     c["username"]=SettingHandler::getSetting("database","namauser");
     c["password"]=SettingHandler::getSetting("database","password");
     c["port"]=SettingHandler::getSetting("database","port");
     QDateTime tm=QDateTime::currentDateTime();
     c["conname"] =
     QString("koneksi-%1").arg(tm.toString("dddd_dd_MM_yyyy::HH_mm_ss"));

     */
    // assume c had been validated
    //         mp(this);
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_dumpall.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/pg_dumpall");
#endif
    //      valid cmd ex:
    //      pg_dump -h localhost -p 5432 -U admin -d demo -f foo.sql
    QStringList args;
    args << "-U" << c["username"].toString() << "-h" << c["hostname"].toString()
         << "-p" << c["port"].toString() << "-d" << dblama << "-f"
         << QDir::toNativeSeparators(tosqlfile);
    //        supply password set in temp env
    mp->startwithpwd(prog, args, pyhome.path(), c["password"].toString());
  }

 Q_SIGNALS:
  void emitfinish(int e);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) { Q_EMIT onerror(e); }
  void onOutput(const QString &o) { Q_EMIT onoutput(o); }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) { Q_EMIT emitfinish(e); }

 private:
  PWrapper *mp;
};

class reinstalldb : public QObject {
  Q_OBJECT
 public:
  reinstalldb() {
    p = new PWrapper;
    connect(p, SIGNAL(emitStart()), SLOT(onStart()));
    connect(p, SIGNAL(emitFinish(int, QProcess::ExitStatus)),
            SLOT(onFinish(int, QProcess::ExitStatus)));
    connect(p, SIGNAL(emitOutput(QString)), this, SLOT(onOutput(QString)));
    connect(p, SIGNAL(emitError(QString)), this, SLOT(onError(QString)));
  }
  ~reinstalldb() { p->deleteLater(); }
  void doreinstall(const QString &rootdb, const QString &dbfolderbaru,
                   const QString &username, const QString &password) {
    QDir pyhome(rootdb);
    if (!pyhome.cdUp()) {
      Q_EMIT emitfinish(-10);
      return;
    }

#ifdef Q_OS_WIN
    auto prog = QDir::toNativeSeparators(rootdb + "/initdb.exe");
#else
    auto prog = QDir::toNativeSeparators(rootdb + "/initdb");
#endif
    //      valid cmd ex:
    //      initdb --pwfile=filename --auth=trust --auth-host=md5 -D
    //      "D:\masteraplikasi\transferh11nov\mingw64\493\psql1\data" -E UTF8 -U
    //      admin
    QString pwfile(QApplication::applicationDirPath() + "/pgfl");
#ifdef DEBUGB
    qDebug() << "pwfile: " << pwfile;
#endif

    if (!filereader::readwrite::forcewritefile(pwfile, password)) {
      Q_EMIT onoutput(
          QObject::tr("Galat, setting password untuk default database gagal."));
      Q_EMIT emitfinish(-11);
      return;
    }
    QStringList args;
    QString pwarg = QString("--pwfile=%1").arg(pwfile);
    args << pwarg << "--lc-monetary=id_ID"
         << "--auth=trust"
         << "--auth-host=md5"
         << "-D" << QDir::toNativeSeparators(dbfolderbaru) << "-E"
         << "UTF8"
         << "-U" << username;
    //        supply password set in temp env
    pwd = pwfile;
    p->setdeatach(true);
    p->start(prog, args, pyhome.path());
  }

 Q_SIGNALS:
  void emitfinish(int e);
  void onoutput(const QString &o);
  void onstart();
  void onerror(const QString &s);
 private Q_SLOTS:
  void onError(const QString &e) {
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << " e: " << e;
#endif
    Q_EMIT onerror(e);
  }
  void onOutput(const QString &o) {
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << " o: " << o;
#endif
    Q_EMIT onoutput(o);
  }
  void onStart() { Q_EMIT onstart(); }
  void onFinish(int e, QProcess::ExitStatus) {
    QFile rmpwd(pwd);
    rmpwd.remove();
    Q_EMIT emitfinish(e);
  }

 private:
  PWrapper *p;
  QString pwd;
};
}

namespace prepareperkakasdb {
struct dbapp {
  dbapp() {}
  ~dbapp() {}
  bool basepgappready(const std::tuple<QString, QString, QDir> &e) {
    auto a1 = std::get<0>(e);
    auto a2 = std::get<1>(e);
    auto ap = std::get<2>(e);
    if ((a1.compare("galat", Qt::CaseInsensitive) == 0) ||
        (a2.compare("galat", Qt::CaseInsensitive) == 0) ||
        !filedb::dbapp::checkcommonapp(a1) || !ap.exists() ||
        !ap.isReadable()) {
      return false;
    }
    return true;
  }
  bool setuppgappready(const QString &a1, const QString &a2, const QDir &ap) {
    std::packaged_task<bool(const std::tuple<QString, QString, QDir> &)> task(
        [this](const std::tuple<QString, QString, QDir> &e) {
          return basepgappready(e);
        });
    std::future<bool> result1 = task.get_future();
    auto t = std::make_tuple(a1, a2, ap);
    try {
      std::thread task_td(std::move(task), std::cref(t));
      task_td.join();
    } catch (const std::system_error &) {
      return false;
    } catch (...) {
      return false;
    }

    return result1.get();
  }

  bool baseready(
      const std::tuple<dbbase::configdb, QString, QString, QDir> &e) {
    // const dbbase::configdb &c,const QString & a1,const QString & a2,const
    // QDir & ap
    auto a1 = std::get<1>(e);
    auto a2 = std::get<2>(e);
    auto ap = std::get<3>(e);
    if (!koneksi::dbutil::testkoneksi(std::get<0>(e)) ||
        (a1.compare("galat", Qt::CaseInsensitive) != 0) ||
        (a2.compare("galat", Qt::CaseInsensitive) != 0) ||
        !filedb::dbapp::checkcommonapp(a1) || !ap.exists() ||
        !ap.isReadable()) {
      return false;
    }
    return true;
  }
  bool setupready(const dbbase::configdb &c, const QString &a1,
                  const QString &a2, const QDir &ap) {
    std::packaged_task<bool(
        const std::tuple<dbbase::configdb, QString, QString, QDir> &)>
    task([this](const std::tuple<dbbase::configdb, QString, QString, QDir> &e) {
      return baseready(e);
    });
    std::future<bool> result1 = task.get_future();
    auto t = std::make_tuple(c, a1, a2, ap);
    try {
      std::thread task_td(std::move(task), std::cref(t));
      task_td.join();
    } catch (const std::system_error &) {
      return false;
    } catch (...) {
      return false;
    }

    return result1.get();
  }
};
}

#endif  // DBUTIL_H
