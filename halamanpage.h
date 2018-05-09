#ifndef HALAMANPAGE_H
#define HALAMANPAGE_H

#include <sqlfetch.h>
#include <QWidget>
#include <numberparsertype.h>

namespace Ui {
class HalamanPage;
}

class   QRegExpValidator ;
class HalamanPage : public QWidget {
  Q_OBJECT

 public:
  explicit HalamanPage(QWidget *parent = 0);
  ~HalamanPage();

  void setdbconfig(dbbase::configdb *db);
  void askrefresh();
  
Q_SIGNALS:
  void asktoinputdata();
  
 private slots:
  void on_lineEdit_inputcari_returnPressed();
  
  void addentrybaru();
  
  void removeEntry(const QString&id);
  void editEntry(const QString &id);

  void on_pushButton_cari_clicked();

  void on_comboBox_kategori_currentIndexChanged(int index);
  
  void on_pushButton_cancel_clicked();
  
  void on_pushButton_simpan_clicked();
  
  void on_lineEdit_inputnomor_textChanged(const QString &arg1);
  
  void on_lineEdit_kota_textChanged(const QString &arg1);
  
  void setDataViewExportNameKategori();
  
private:
  dbbase::configdb *mdb;
  QRegExpValidator *numvalid;
    //mode add default
  qint64 last_id_edit=0;
  bool modeadd_or_edit=true;
  std::deque<std::pair<int,QString>>m_kategori_nomor;
  Ui::HalamanPage *ui;
  QString placeholdernomor;
  QString placeholdercluster;
};

#endif  // HALAMANPAGE_H
