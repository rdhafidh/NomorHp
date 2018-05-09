#include "hapusbyimportdatanomordimaster.h"

HapusByImportDataNomorDiMaster::HapusByImportDataNomorDiMaster(QObject *parent)
    : QObject(parent) {
    mw=new HapusByImportDataNomorDiMasterWork;
    mw->moveToThread (&th);
    connect(this,SIGNAL(startprocessing()),mw,SLOT(doprocessing()));
    connect(mw,SIGNAL(processDone()),SIGNAL(processDone()));
    connect(mw,SIGNAL(processFailed(QString)),SIGNAL(processFailed(QString)));
    connect (mw,SIGNAL(processStep(qint64)),SIGNAL(processStep(qint64)));
    connect(&th,SIGNAL(finished()),mw,SLOT(deleteLater()));
}

HapusByImportDataNomorDiMaster::~HapusByImportDataNomorDiMaster() {
    if(!askclosed){
        mw->disconnect ();
       delete mw;
    }
    qDebug()<<"askclosed"<<askclosed;
}

void HapusByImportDataNomorDiMaster::setconfigdb(const dbbase::configdb &db, const std::vector<QByteArray> &datas)
{
    mw->setconfigdb (db,datas);
}

void HapusByImportDataNomorDiMaster::doprocessing()
{
    if(!th.isRunning ()){
        th.start ();
    }
    this->startprocessing ();
}

void HapusByImportDataNomorDiMaster::selesai()
{
    if(th.isRunning ()){
        th.requestInterruption ();
        th.quit ();
        th.wait (3000);
        askclosed=true;
    }
}

HapusByImportDataNomorDiMasterWork::HapusByImportDataNomorDiMasterWork(
    QObject *parent) :QObject(parent){
  sql = new dbbase::SqlFetch;
}

HapusByImportDataNomorDiMasterWork::~HapusByImportDataNomorDiMasterWork() {
    qDebug()<<Q_FUNC_INFO<<"called";
    if(sql!=nullptr){
       delete sql;
         QSqlDatabase::removeDatabase (mdb["conname"].toString ());
    }
}

void HapusByImportDataNomorDiMasterWork::setconfigdb(
    const dbbase::configdb &db, const std::vector<QByteArray> &datas) {
  mdb = db;
  auto dt=QDate::currentDate ();
  mdb["conname"]=dt.toString ("ddMMyyhhmmsszzz")+dt.toString ("211");
  nomor = datas;
  sql->setConfig(mdb);
}

void HapusByImportDataNomorDiMasterWork::doprocessing() {
  if (!sql->open()) {
    this->processFailed(
        tr("Koneksi ke file database galat. %1.")
            .arg(dbbase::SqlFetch::errortostring(sql->lastError())));
    return;
  }
  for (decltype(nomor.size()) x = 0; x < nomor.size(); x++) {
    auto sqldata = "delete from nomor_telp where nomor = :0 ";
    dbbase::placeholderprepareinsert prep;
    prep.push_back(std::make_pair(":0", QString::fromStdString (nomor.at(x).toStdString())));
    bool isok = sql->crdelalterWithParam(sqldata, prep);
    if (!isok) {
      this->processFailed(
          tr("Terjadi galat ketika sedang melakukan penghapusan data dalam "
             "database. %1.")
              .arg(dbbase::SqlFetch::errortostring(sql->lastError())));
      return;
    }
    qDebug()<<"nomor.at(x)"<<nomor.at(x);
    this->processStep(x);
  }
  this->processDone();
}
