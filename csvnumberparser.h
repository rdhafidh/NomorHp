#ifndef CSVNUMBERPARSER_H
#define CSVNUMBERPARSER_H

#include <QThread>
#include <vector>
#include <utility>
#include <sqlfetch.h>
#include <numberparsertype.h>

class CSVNumberParser:public QObject
{
    Q_OBJECT
public:
    explicit CSVNumberParser(QObject*parent=nullptr);
    ~CSVNumberParser();
    
    
Q_SIGNALS:
    void parseinitDoneEmitLine(int rows);
    void parseFailed(const QString &msg);
    void parsedLine(int at);
    void parseDone();
    
    //to child thread
    void emitImport();
    void emitSetup(const QString &filepath,const dbbase::configdb &mdb,bool m_use_header);
    
    
public Q_SLOTS:
    void doimport();
    void setup(const QString &filepath,const dbbase::configdb &mdb,bool m_use_header);
    
private:
    QList<std::pair<QByteArray, QByteArray> > getcsvComma(const QString &path,bool use_header=true);
    QList<std::pair<QByteArray, QByteArray> > getcsvDotComma(const QString &path,bool use_header=true);
    
    QPair<bool,bool> doimportdb(const QStringList &datasinglerow);
    dbbase::SqlFetch *sql;
    dbbase::configdb configdb;
    QString fname;
    bool use_header_csv=true;
   QList<QPair<QStringList,nomorutil::ReturnDeterminedNumber>> failedimport;
};

class CSVNumberThreadParser:public QObject{
    Q_OBJECT
public:
    explicit  CSVNumberThreadParser(QObject *parent=nullptr);
    ~CSVNumberThreadParser();
    
    void selesai();
        void setup(const QString &filepath,const dbbase::configdb &mdb,bool use_header=true);
    
Q_SIGNALS:
    void parseinitDoneEmitLine(int rows);
    void parseFailed(const QString &msg);
    void parsedLine(int at);
    void parseDone();
    void setupSetting(const QString &filepath,const dbbase::configdb &mdb,bool use_header=true);
    
    //to worker
    void startimporting();
    
public Q_SLOTS:
    void doimport();
    
private:
    CSVNumberParser *parser;
    QThread th;
};



#endif // CSVNUMBERPARSER_H