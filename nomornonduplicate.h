#ifndef NOMORNONDUPLICATE_H
#define NOMORNONDUPLICATE_H

#include <QWidget>
#include <sqlfetch.h>

namespace Ui {
class NomorNonDuplicate;
}

class NomorNonDuplicate : public QWidget
{
    Q_OBJECT
    
public:
    explicit NomorNonDuplicate(QWidget *parent = 0);
    ~NomorNonDuplicate();
    void setconfigdb(dbbase::configdb *db);
    
    
private slots:
    void on_comboBox_cari_kategori_nomor_currentIndexChanged(int index);
    
    void on_pushButton_cari_nomor_clicked();
    
    void entryRemove(const QString &id);
    
    void entryRefresh();
    
    
private:
    Ui::NomorNonDuplicate *ui;
    QString placeholdernomor;
    QString placeholdercluster;
    dbbase::configdb *mdb;
};

#endif // NOMORNONDUPLICATE_H
