#ifndef IMPORTNOMORNONDUPLICATE_H
#define IMPORTNOMORNONDUPLICATE_H

#include <sqlfetch.h>
#include <QDialog>

namespace Ui {
class ImportNomorNonDuplicate;
}
class NumberNonDuplicateParser;
class prosesbar;

class ImportNomorNonDuplicate : public QDialog {
  Q_OBJECT

 public:
  explicit ImportNomorNonDuplicate(QWidget *parent = 0);
  ~ImportNomorNonDuplicate();
  void setconfigdb(dbbase::configdb *db);

 private slots:
  void on_pushButton_tutup_clicked();

  void on_pushButton_jalankan_upload_clicked();

  void on_pushButton_pilih_file_csv_clicked();
  
  void workdone();
  
  void importfailed(const QString &file);
  
  void emitProgressStep(qint64 step);
  
  void emitMaxProgress(qint64 max);
  
  void on_checkBox_is_using_header_toggled(bool checked);
  
private:
  Ui::ImportNomorNonDuplicate *ui;
  dbbase::configdb *mdb;
  NumberNonDuplicateParser *pr;
  prosesbar *bar;
  QString fname;
};

#endif  // IMPORTNOMORNONDUPLICATE_H
