#ifndef IMPORTHAPUSNOMORONLYPARSER_H
#define IMPORTHAPUSNOMORONLYPARSER_H

#include <QThread>
#include <QStringList>
#include <vector>
#include <qbytearray.h>
#include <sqlfetch.h>

class ImportHapusNomorOnlyParserWork:public QObject
{
    Q_OBJECT
public:
    explicit ImportHapusNomorOnlyParserWork(QObject *parent=0);
    ~ImportHapusNomorOnlyParserWork();
    
    void setup(const QString &fname,bool use_header,dbbase::configdb &db);
    
Q_SIGNALS:
    
    void emitImportFailed(const QString &msg);
    void emitImportDone(const std::vector<QByteArray> &datas);
    void emitImportManyCheckSize(qint64 n);
    void emitImportCheckStep(qint64 n);
    
public Q_SLOTS:
    void doimport();
    
private:
    QString file;
    bool is_use_header=false;
    dbbase::SqlFetch *sql;
    dbbase::configdb mdb;
};

class ImportHapusNomorOnlyParser:public QObject
{
    Q_OBJECT
public:
    explicit ImportHapusNomorOnlyParser(QObject *parent=0);
    ~ImportHapusNomorOnlyParser();
    void setup(const QString &fname,bool use_header,dbbase::configdb &db);
    
Q_SIGNALS:
    void emitImportFailed(const QString &msg);
    void emitImportDone(const std::vector<QByteArray> &datas);
    void emitImportManyCheckSize(qint64 n);
    void emitImportCheckStep(qint64 n);
    
    void startimporting();
    
public Q_SLOTS:
    void doimport();
    void selesai();
    
private:
    QThread th;
    ImportHapusNomorOnlyParserWork *wk;
};

#endif // IMPORTHAPUSNOMORONLYPARSER_H