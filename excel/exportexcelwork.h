#ifndef EXPORTEXCELWORK_H
#define EXPORTEXCELWORK_H

#include <QObject>
#include <utility>
#include <QByteArray>
#include <QString>
#include <QSqlRecord>
#include "exportexcelresult.h"
#include <QThread>
#include <QScopedPointer>
#include <deque>
#include <sqlfetch.h>

class ExportExcelWork : public QObject
{
    Q_OBJECT
public:
    explicit ExportExcelWork(QObject *parent = 0);
    ~ExportExcelWork();
    void setTableAsliName(const QString&tbl,const dbbase::configdb &mdb);

    void exportSetup(std::deque<QSqlRecord> *mrect, int howmanycol, const QString &fn, const QString &mtitle, const QStringList &colnm,bool m_use_62
                     ,bool is_mode_csv_or_excel);
Q_SIGNALS:
    void result(ExportExcelResult ret);
    //for progressbar
    void emitMaxBar(int n);
    void emitValueBar(int n);

public Q_SLOTS:
    void doexporting();

private:

    void deleteDataById(const QString&id);
    void openemptyandwrite1(const QString &fname, const QString &cell, const QByteArray &cnt);
    void openemptyandwriteheadertable(const QString &fname, const QString &cell, const QString &mergecell, const QByteArray &cnt);

    QString whatcolumnlabel(int col);

    std::deque<QSqlRecord> *mp;
    QString fname_kategori;
    QString tbltitle;
    int manycol=0;
    //csv default
    bool mode_csv_or_excel=true;
    QStringList hddata;
    QString tabel_asliname;
    dbbase::configdb db_config;
    bool use_62=false;
};

class ExportExcelThread:public QObject
{
    Q_OBJECT
public:
    explicit ExportExcelThread(QObject *parent = 0);
    ~ExportExcelThread();
    void exportSetup(std::deque<QSqlRecord> *mrect, int howmanycol,
                      const QString &fn, const QString &mtitle, const QStringList &colnm, bool use_62,
                     bool is_mode_csv_or_excel);
     void setTableAsliName(const QString&tbl,const dbbase::configdb &mdb);
     

Q_SIGNALS:
    //~~FOR FORWARD SIGNAL
    void result(ExportExcelResult ret);
    //for progressbar
    void emitMaxBar(int n);
    void emitValueBar(int n);
    void startdoing();


public Q_SLOTS:
    void doexporting();
    void selesai();

private:
     QThread pth;
    QScopedPointer<ExportExcelWork> wk;

};

#endif // EXPORTEXCELWORK_H
