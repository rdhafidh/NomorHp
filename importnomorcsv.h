#ifndef IMPORTNOMORCSV_H
#define IMPORTNOMORCSV_H

#include <QDialog>
#include <sqlfetch.h>
#include <prosesbar.h>
#include <QScopedPointer>

namespace Ui {
class ImportNomorCSV;
}
class  CSVNumberThreadParser;

class ImportNomorCSV : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImportNomorCSV(QWidget *parent = 0);
    ~ImportNomorCSV();
    void setconfigdb(const dbbase::configdb &config);
    
private slots:
    void on_pushButton_upload_clicked();
    
    void on_pushButton_batal_clicked();
    
    void on_pushButton_pilih_csv_clicked();
    
    void parseinitDoneEmitLine(int rows);
    void parseFailed(const QString &msg);
    void parsedLine(int at);
    void parseDone();
    
    void on_checkBox_use_header_toggled(bool checked);
    
private:
    QString fname="";
    dbbase::configdb mdb;
    Ui::ImportNomorCSV *ui;
    CSVNumberThreadParser *parser;
    prosesbar *pb;
};

#endif // IMPORTNOMORCSV_H
