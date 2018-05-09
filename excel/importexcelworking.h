#ifndef IMPORTEXCELWORKING_H
#define IMPORTEXCELWORKING_H

#include "dataimportexcel.h"
#include <deque>
class ImportExcelWorking : public QObject
{
    Q_OBJECT
public:
    explicit ImportExcelWorking(QObject *parent = 0);
    ~ImportExcelWorking();

    void importsetup(const DataImportExcel &inp);

Q_SIGNALS:
    void maxbarimportexcel(int n);
    void valuebarimportexcel(int n);
    void resultimportexcel(ImportExcelResult ret, const QString &msg);
     void setmsgcontent(const QString &dat);

public Q_SLOTS:
  void doimport();

private:
  std::tuple<bool, QByteArray, qint64> parsecellnamehorizontal(
        const QString &name);

    QVariant parseexcelvalue(const QString& fname,const QString &cellname);
    DataImportExcel in;
};

class ImportExcelThread:public QObject
{
    Q_OBJECT
public:
    explicit ImportExcelThread(QObject*parent=0);
    ~ImportExcelThread();

    void importsetup(const DataImportExcel &inp);

Q_SIGNALS:
    void maxbarimportexcel(int n);
    void valuebarimportexcel(int n);
    void resultimportexcel(ImportExcelResult ret, const QString &msg);
    void startdoing();
     void setmsgcontent(const QString &dat);

public Q_SLOTS:
    void doimport();
    void selesai();

private:
    QThread th;
    QScopedPointer<ImportExcelWorking> wk;

};

#endif // IMPORTEXCELWORKING_H
