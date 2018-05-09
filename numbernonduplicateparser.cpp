#include "numbernonduplicateparser.h"
#include <vector>
#include <numberparsertype.h>
#include <QDebug>

NumberNonDuplicateParser::NumberNonDuplicateParser(QObject *parent)
    : QObject(parent), np(nullptr) {
  np = new NumberProcessWork;
  np->moveToThread(&th);
  connect(this, SIGNAL(emitdoimport()), np, SLOT(doimport()));
  connect(&th, SIGNAL(finished()), np, SLOT(deleteLater()));
  
  connect(np,SIGNAL(emitDone()),this,SIGNAL(emitDone()));
  connect(np,SIGNAL(emitImportFailed(QString)),SIGNAL(emitImportFailed(QString)));
  connect(np,SIGNAL(emitProgress(qint64)),SIGNAL(emitProgress(qint64)));
  connect(np,SIGNAL(emitMaxProgress(qint64)),SIGNAL(emitMaxProgress(qint64)));
  
}

NumberNonDuplicateParser::~NumberNonDuplicateParser() {}

void NumberNonDuplicateParser::setup(const QString &mfile,
                                     const dbbase::configdb &db,
                                     bool m_is_use_header) {
  np->setup(mfile, db, m_is_use_header);
}

void NumberNonDuplicateParser::selesai() {
  if (th.isRunning()) {
    th.requestInterruption();
    th.quit();
    th.wait(3000);
  }
}

void NumberNonDuplicateParser::doimport() {
  if (!th.isRunning()) {
    th.start();
    this->emitdoimport();
  }
}

NumberProcessWork::NumberProcessWork(QObject *parent) : QObject(parent),sql(nullptr) {}

NumberProcessWork::~NumberProcessWork() {
    if(sql!=nullptr){
        delete sql;
    }
}

void NumberProcessWork::setup(const QString &mfile, const dbbase::configdb &db,
                              bool m_is_use_header) {
  file = mfile;
  config = db;
  is_use_header = m_is_use_header;
  sql=new dbbase::SqlFetch;
  sql->setConfig (config);
}

void NumberProcessWork::doimport() {
    if(!sql->open ()){
        this->emitImportFailed (tr("Terjadi galat ketika membuka database. %1").arg (dbbase::SqlFetch::errortostring (sql->lastError ())));
        return;
    }
    qDebug()<<"file"<<file<<"is_use_header"<<is_use_header;
    QFile fn(file);
    if(!fn.open (QIODevice::ReadOnly|QIODevice::Text)){
        this->emitImportFailed (tr("File %1 tidak bisa dibaca.").arg (file));
        return;
    }
    qint64 line=0;
    std::vector<QByteArray> datas;
    
    while(!fn.atEnd ()){
        if(line==0 && is_use_header){
            ++line;
            //still consume but do nothing 
            fn.readLine ();
            continue;
        }
        QByteArray data=fn.readLine ();
        data=data.simplified ();
        datas.push_back (data);
        ++line;
    }
    fn.close ();
    if(datas.size ()==0){
        this->emitImportFailed (tr("Data import yang ditemukan kosong. Pastikan di dalam file %1berisi angka data nomor saja tiap baris, "
                                   "dan opsional pada awal baris berisi header.").arg (file));
        return;
    }
    qDebug()<<"found nomor"<<datas.size ()<<"line asli"<<line;
    this->emitMaxProgress (datas.size ());
    for(decltype(datas.size ())x=0;x<datas.size ();x++){
        if(!doimportsql (datas.at (x))){
            break;
        }
        this->emitProgress (x);
    }
    this->emitDone ();
}

bool NumberProcessWork::doimportsql(const QByteArray &in)
{
    if(in.isEmpty ()){
        //skipped
        return true;
    }
    nomorutil::ReturnDeterminedNumber retnum=nomorutil::NumberParserType::determineNumber (in.toStdString());
    if(!retnum.isnumbervalid){
        //skip import
        qDebug()<<"nomor"<<in<<"skipped";
        return true;
    }
    
    QVariantList row;
    row << QString::fromStdString (retnum.nomor_string_diprocess);
    row << static_cast<int>(retnum.pn);
    row << static_cast<int>(retnum.km);
    row << QDate::currentDate();
   
    QString insert =
        "insert or replace into nomor_nonduplicate "
        "(`nomor`,`id_provider`,`id_kategori`,`tanggal_dibuat`"
        ") values "
        "(:0,:1,:2,:3);";
    dbbase::placeholderprepareinsert prep;
    prep.push_back (std::make_pair(":0",row.at (0)));
    prep.push_back (std::make_pair(":1",row.at (1)));
    prep.push_back (std::make_pair(":2",row.at (2)));
    prep.push_back (std::make_pair(":3",row.at (3)));
   bool isok=sql->crdelalterWithParam (insert,prep);
   if(!isok){
       this->emitImportFailed (tr("Terjadi galat ketika import ke database. %1").arg (dbbase::SqlFetch::errortostring (sql->lastError ())));
       return false;
   }
   return true;
}
