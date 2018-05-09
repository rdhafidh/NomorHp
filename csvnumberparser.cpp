#include "csvnumberparser.h"
#include <QStringList>

CSVNumberParser::CSVNumberParser(QObject *parent)
    : QObject(parent), sql(nullptr) {
    sql = new dbbase::SqlFetch();
    connect(this,SIGNAL(emitImport()),this,SLOT(doimport()));
    connect(this,SIGNAL(emitSetup(QString,dbbase::configdb,bool)),this,SLOT(setup(QString,dbbase::configdb,bool)));
}

CSVNumberParser::~CSVNumberParser() {
    if(sql !=nullptr){
        delete sql;
    }
}

void CSVNumberParser::setup(const QString &filepath,
                            const dbbase::configdb &mdb, bool m_use_header) {
  fname = filepath;
  configdb = mdb;

  use_header_csv=m_use_header;
  qDebug()<<"fname"<<fname<<"use_header"<<use_header_csv;
  sql->setConfig(configdb);
}

QList<std::pair<QByteArray, QByteArray> > CSVNumberParser::getcsvComma(const QString &path, bool use_header)
{
    QFile fn(path);
    QList<std::pair<QByteArray, QByteArray> >ret;
    if(!fn.open (QIODevice::ReadOnly|QIODevice::Text)){
         qDebug()<<__FILE__<<__LINE__;
        return ret;
    }
    qint64 line=0;
    while(!fn.atEnd ()){
        if(line==0 && use_header){
            ++line;
            fn.readLine ();
            continue;
        }
        
        QByteArray data=fn.readLine ();
        QList<QByteArray> row=data.split (',');
        if(row.size ()>=2){
            ret<<std::make_pair(row.at (0),row.at (1));  
        }
        ++line;
    }
    fn.close ();
    return ret;
}

QList<std::pair<QByteArray, QByteArray> > CSVNumberParser::getcsvDotComma(const QString &path, bool use_header)
{
    QFile fn(path);
    QList<std::pair<QByteArray, QByteArray> >ret;
    if(!fn.open (QIODevice::ReadOnly|QIODevice::Text)){
        return ret;
    }
    qint64 line=0;
    while(!fn.atEnd ()){
        if(line==0 && use_header){
            ++line;
            fn.readLine ();
            continue;
        }
        
        QByteArray data=fn.readLine ();
        QList<QByteArray> row=data.split (';');
        if(row.size ()>=2){
            ret<<std::make_pair(row.at (0),row.at (1));  
        }
        ++line;
    }
    fn.close ();
    return ret;
}

void CSVNumberParser::doimport() {
    qDebug()<<"doimport called";
 
      qDebug()<<"exec getcsv";
      bool isok= sql->open();
      if(!isok){
          this->parseFailed (tr("Koneksi ke server database tidak terhubung. Dimohon untuk menghidupkan server database mysql."));
  //        failedimport<<qMakePair(datasinglerow,retnum);
          //fatal;
         return ;
      }
  QList<std::pair<QByteArray,QByteArray>> file=getcsvComma(fname,use_header_csv);
  qDebug()<<"exec after file";
  bool isfailed=false;
  if(file.size ()==0){
      isfailed=true;
  }
  if(isfailed){
      file=getcsvDotComma(fname,use_header_csv);
  }
  if (file.size ()== 0) {
         qDebug()<<"file.size ()== 0";
    this->parseFailed(tr("Konten data yang diimport tidak boleh kosong."));
    return;
  }
  qDebug()<<"found"<<file.size ();
  this->parseinitDoneEmitLine(file.size());
  for (int m = 0; m < file.size(); m++) {
      QStringList row;
      row<<QString::fromLocal8Bit (file.at (m).first);
      row<<QString::fromLocal8Bit (file.at (m).second);
     auto retstat=doimportdb(row);
     if(!retstat.first){
         break;
     }
     this->parsedLine (m+1);
  }
  this->parseDone ();
}

QPair<bool, bool> CSVNumberParser::doimportdb(const QStringList& datasinglerow) {
  
  nomorutil::ReturnDeterminedNumber retnum=nomorutil::NumberParserType::determineNumber (datasinglerow.at (0).toStdString());
  if(!retnum.isnumbervalid){
      failedimport<<qMakePair(datasinglerow,retnum);
      //tidak fatal tapi tetap galat
      return qMakePair(true,false);
  }

//  QString sqldelete="delete from nomor_telp where nomor = :0";
 dbbase::placeholderprepareinsert prep;
//  prep.push_back (std::make_pair(":0",retnum.nomor_string_diprocess.c_str ()));
//  isok= sql->crdelalterWithParam(sqldelete,prep);
//  if(!isok){
//      failedimport<<qMakePair(datasinglerow,retnum);
//      //fatal;
//     return qMakePair(false,false);
//  }
  
  prep.clear ();
  auto current = QDateTime::currentDateTime();
  auto fmt = current.toString("ddMMyyyyhhmmsszzz");
  if(retnum.km==nomorutil::KategoriNomor::NOMOR_10_DIGIT){
      fmt.prepend ("10DGT_");
  }
  if(retnum.km==nomorutil::KategoriNomor::NOMOR_ACAK){
      fmt.prepend ("ACK_");
  }
  if(retnum.km==nomorutil::KategoriNomor::NOMOR_CANTIK){
      fmt.prepend ("CTK_");
  }
  QVariantList row;
  row << QString::fromStdString (retnum.nomor_string_diprocess);
  row << fmt;
  row << static_cast<int>(retnum.pn);
  row << datasinglerow.at (1);
  row << static_cast<int>(retnum.km);
  row << QDate::currentDate();
 
  QString insert =
      "insert or replace into nomor_telp "
      "(`nomor`,`nama`,`id_provider`,`cluster`,`id_kategori`,`tanggal_dibuat`"
      ") values "
      "(:0,:1,:2,:3,:4,:5);";
  prep.push_back (std::make_pair(":0",row.at (0)));
  prep.push_back (std::make_pair(":1",row.at (1)));
  prep.push_back (std::make_pair(":2",row.at (2)));
  prep.push_back (std::make_pair(":3",row.at (3)));
  prep.push_back (std::make_pair(":4",row.at (4)));
  prep.push_back (std::make_pair(":5",row.at (5)));
 bool isok=sql->crdelalterWithParam (insert,prep);
//  if(!isok){
//      failedimport<<qMakePair(datasinglerow,retnum);
//     //fatal;
//     return qMakePair(false,false);
//  }
 return qMakePair(true,true);
}

CSVNumberThreadParser::CSVNumberThreadParser(QObject *parent):QObject(parent)
{
    qRegisterMetaType<dbbase::configdb>("dbbase::configdb");
    parser=new CSVNumberParser;
    parser->moveToThread (&th);
    connect(parser,SIGNAL(parsedLine(int)),this,SIGNAL(parsedLine(int)));
    connect(parser,SIGNAL(parseDone()),this,SIGNAL(parseDone()));
    connect(parser,SIGNAL(parseFailed(QString)),this,SIGNAL(parseFailed(QString)));
    connect(parser,SIGNAL(parseinitDoneEmitLine(int)),this,SIGNAL(parseinitDoneEmitLine(int)));
    connect(&th,SIGNAL(finished()),parser,SLOT(deleteLater()));
    connect(this,SIGNAL(startimporting()),parser,SIGNAL(emitImport()));
    connect(this,SIGNAL(setupSetting(QString,dbbase::configdb,bool)),parser,SIGNAL(emitSetup(QString,dbbase::configdb,bool)));
    th.start ();
}

CSVNumberThreadParser::~CSVNumberThreadParser()
{
}

void CSVNumberThreadParser::selesai()
{
    if(th.isRunning ()){
        th.requestInterruption ();
        th.quit ();
        th.wait (3000);
    }
}

void CSVNumberThreadParser::setup(const QString &filepath, const dbbase::configdb &mdb, bool use_header)
{
   this->setupSetting (filepath,mdb,use_header);
}

void CSVNumberThreadParser::doimport()
{
  this->startimporting ();
}
