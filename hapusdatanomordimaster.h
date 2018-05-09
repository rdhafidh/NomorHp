#ifndef HAPUSDATANOMORDIMASTER_H
#define HAPUSDATANOMORDIMASTER_H

#include <QDialog>
#include <vector>
#include <QByteArray>
#include <sqlfetch.h>

namespace Ui {
class HapusDataNomorDiMaster;
}
class ImportHapusNomorOnlyParser;
class HapusByImportDataNomorDiMaster;

class prosesbar;

class HapusDataNomorDiMaster : public QDialog
{
    Q_OBJECT
public:
    explicit HapusDataNomorDiMaster(QWidget *parent = 0);
    ~HapusDataNomorDiMaster();
    void setconfigdb(const dbbase::configdb &mdb);
    
private slots:
    void on_pushButton_upload_nomor_hapus_clicked();
    
    void on_pushButton_jalankan_hapus_nomor_clicked();
    
    void on_pushbutton_tidak_jadi_clicked();
    
    void dorealhapus();
    
    void progresstep(qint64 n);
    void progressdone();
    void progressfailed(const QString &msg);
    void handleImportManyCheckSize(qint64 n);
    void handleImportCheckStep(qint64 n);
    
private:
    Ui::HapusDataNomorDiMaster *ui;
    QString fname;
    ImportHapusNomorOnlyParser *np;
    std::vector<QByteArray> datas;
    dbbase::configdb config;
    HapusByImportDataNomorDiMaster *dm;
   prosesbar *bar;
};

#endif // HAPUSDATANOMORDIMASTER_H
