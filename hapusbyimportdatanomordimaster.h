#ifndef HAPUSBYIMPORTDATANOMORDIMASTER_H
#define HAPUSBYIMPORTDATANOMORDIMASTER_H

#include <sqlfetch.h>
#include <QThread>

class HapusByImportDataNomorDiMasterWork : public QObject {
  Q_OBJECT
 public:
  explicit HapusByImportDataNomorDiMasterWork(QObject *parent = 0);
  ~HapusByImportDataNomorDiMasterWork();

  void setconfigdb(const dbbase::configdb &db,
                   const std::vector<QByteArray> &datas);

 Q_SIGNALS:
  void processStep(qint64 n);
  void processFailed(const QString &msg);
  void processDone();

 public Q_SLOTS:
  void doprocessing();

 private:
  dbbase::SqlFetch *sql;
  dbbase::configdb mdb;
  std::vector<QByteArray> nomor;
};

class HapusByImportDataNomorDiMaster : public QObject {
  Q_OBJECT
 public:
  explicit HapusByImportDataNomorDiMaster(QObject *parent = 0);
  ~HapusByImportDataNomorDiMaster();
  void setconfigdb(const dbbase::configdb &db,
                   const std::vector<QByteArray> &datas);

 Q_SIGNALS:
  void processStep(qint64 n);
  void processFailed(const QString &msg);
  void processDone();
  
  void startprocessing();

 public slots:
    void doprocessing();
    void selesai();

 private:
  HapusByImportDataNomorDiMasterWork *mw;
  QThread th;
  bool askclosed=false;
};

#endif  // HAPUSBYIMPORTDATANOMORDIMASTER_H