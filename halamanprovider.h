#ifndef HALAMANPROVIDER_H
#define HALAMANPROVIDER_H

#include <QWidget>
#include <sqlfetch.h>

namespace Ui {
class HalamanProvider;
}

class HalamanProvider : public QWidget
{
    Q_OBJECT
    
public:
    explicit HalamanProvider(QWidget *parent = 0);
    ~HalamanProvider();
    void setdbconfig(dbbase::configdb *db);
    
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    
    void on_pushButton_simpan_clicked();
    
    void on_pushButton_batal_clicked();
    
    void add_data_provider();
    
    void edit_data_byid(const QString &id);
    
    void hapus_edit_byid(const QString&id);
    
    
private:
    dbbase::configdb *mdb;
    bool modeadd_or_edit=true;
    qint64 last_edit_id=-1;
    Ui::HalamanProvider *ui;
};

#endif // HALAMANPROVIDER_H
