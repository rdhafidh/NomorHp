#include "importhapusnomoronlyparser.h"
#include <numberparsertype.h>
#include <QtCore>

ImportHapusNomorOnlyParser::ImportHapusNomorOnlyParser(QObject *parent)
    : QObject(parent) {
  wk = new ImportHapusNomorOnlyParserWork;
  wk->moveToThread(&th);
  connect(this, SIGNAL(startimporting()), wk, SLOT(doimport()));
  connect(&th, SIGNAL(finished()), wk, SLOT(deleteLater()));
  connect(wk, SIGNAL(emitImportDone(std::vector<QByteArray>)),
          SIGNAL(emitImportDone(std::vector<QByteArray>)));
  connect(wk, SIGNAL(emitImportFailed(QString)),
          SIGNAL(emitImportFailed(QString)));
  connect(wk,SIGNAL(emitImportCheckStep(qint64)),SIGNAL(emitImportCheckStep(qint64)));
  connect(wk,SIGNAL(emitImportManyCheckSize(qint64)),SIGNAL(emitImportManyCheckSize(qint64)));
}

ImportHapusNomorOnlyParser::~ImportHapusNomorOnlyParser() {}

void ImportHapusNomorOnlyParser::setup(const QString &fname, bool use_header, dbbase::configdb &db) {
  wk->setup(fname, use_header,db);
}

void ImportHapusNomorOnlyParser::doimport() {
  if (!th.isRunning()) {
    th.start();
  }
  this->startimporting();
}

void ImportHapusNomorOnlyParser::selesai() {
  if (th.isRunning()) {
    th.requestInterruption();
    th.quit();
    th.wait(3000);
  }
}

ImportHapusNomorOnlyParserWork::ImportHapusNomorOnlyParserWork(QObject *parent)
    : QObject(parent) {
    sql=new dbbase::SqlFetch;
}

ImportHapusNomorOnlyParserWork::~ImportHapusNomorOnlyParserWork() {
    if(sql !=nullptr){
        delete sql;
        QSqlDatabase::removeDatabase (mdb["conname"].toString ());
    }
}

void ImportHapusNomorOnlyParserWork::setup(const QString &fname,
                                           bool use_header, dbbase::configdb &db) {
  file = fname;
  is_use_header = use_header;
  mdb=db;
  auto dt=QDate::currentDate ();
  mdb["conname"]=dt.toString ("ddMMyyhhmmsszzz")+dt.toString ("111");
}

void ImportHapusNomorOnlyParserWork::doimport() {
  QFile fn(file);
  sql->setConfig (mdb);
  if(!sql->open ()){
      this->emitImportFailed (tr("Koneksi ke database gagal. %1").arg (dbbase::SqlFetch::errortostring (sql->lastError ())));
      return;
  }
  qDebug()<<"is_use_header"<<is_use_header<<"file"<<file;
  if (!fn.open(QIODevice::ReadOnly | QIODevice::Text)) {
    this->emitImportFailed(tr("File %1 tidak bisa dibaca.").arg(file));
    return;
  }
  qint64 line = 0;
  std::vector<QByteArray> datas;
  std::vector<QString> tmp_data;
  while (!fn.atEnd()) {
    if (line == 0 && is_use_header) {
      ++line;
      // still consume but do nothing
      fn.readLine();
      continue;
    }
    QByteArray data = fn.readLine();
//    data = data.simplified();
//    nomorutil::ReturnDeterminedNumber retnum =
//        nomorutil::NumberParserType::determineNumber(data.toStdString());
//    if (!retnum.isnumbervalid) {
//      ++line;
//      continue;
//    }
    tmp_data.push_back (QString::fromUtf8 (data));
    ++line;
  }
  fn.close();
  this->emitImportManyCheckSize (tmp_data.size ());
  
  for(decltype(tmp_data.size ())x=0;x<tmp_data.size ();x++){
      if(QThread::currentThread ()->isInterruptionRequested ()){
          return;
      }
      QString sqldata="select nomor from nomor_telp where nomor = :0 ";
      dbbase::placeholderprepareinsert prep;
      prep.push_back (std::make_pair(":0",tmp_data.at (x)));
      auto ret=sql->prepareSelectFilter (sqldata,prep);
      if(!ret.first){
          this->emitImportFailed (tr("Terjadi galat pada database ketika sedang melakukan pengecekan data nomor. %1").arg (dbbase::SqlFetch::errortostring (sql->lastError ())));
          return;
      }
      if(ret.second.size ()>0){
          datas.push_back(tmp_data.at (x).toUtf8());
      }
      this->emitImportCheckStep (x+1);
  }
  tmp_data.clear ();
  this->emitImportDone(datas);
}
