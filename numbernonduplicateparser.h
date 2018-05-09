#ifndef NUMBERNONDUPLICATEPARSER_H
#define NUMBERNONDUPLICATEPARSER_H

#include <sqlfetch.h>
#include <QThread>

class NumberProcessWork:public QObject
{
    Q_OBJECT
public:
    explicit NumberProcessWork(QObject*parent=0);
    ~NumberProcessWork();
    void setup(const QString &mfile,const dbbase::configdb &db,bool m_is_use_header);
    
Q_SIGNALS:
    void emitImportFailed(const QString &msg);
    void emitProgress(qint64 line);
    void emitMaxProgress(qint64 max);
    void emitDone();
    
public Q_SLOTS:
    void doimport();
    
private:
    bool doimportsql(const QByteArray &in);
    
    bool is_use_header=false;
    dbbase::configdb config;
    QString file;
    dbbase::SqlFetch *sql;
};

class NumberNonDuplicateParser:public QObject
{
    Q_OBJECT
public:
   explicit NumberNonDuplicateParser(QObject *parent=0);
    ~NumberNonDuplicateParser();
    void setup(const QString &mfile,const dbbase::configdb &db,bool m_is_use_header);
    void selesai();
    
    
Q_SIGNALS:
    void emitdoimport();
    
    void emitImportFailed(const QString &msg);
    void emitProgress(qint64 line);
    void emitMaxProgress(qint64 max);
    void emitDone();
    
public Q_SLOTS:
    void doimport();
    
private:
    NumberProcessWork *np;
    QThread th;
};

#endif // NUMBERNONDUPLICATEPARSER_H