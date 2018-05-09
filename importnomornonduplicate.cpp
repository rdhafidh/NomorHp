#include "importnomornonduplicate.h"
#include <numbernonduplicateparser.h>
#include <prosesbar.h>
#include "ui_importnomornonduplicate.h"
#include <uiutil.h>

ImportNomorNonDuplicate::ImportNomorNonDuplicate(QWidget *parent)
    : QDialog(parent), mdb(nullptr), ui(new Ui::ImportNomorNonDuplicate) {
  ui->setupUi(this);
  pr = new NumberNonDuplicateParser;
  connect(this, &ImportNomorNonDuplicate::accepted, [this]() {
    this->pr->selesai();
    this->pr->deleteLater();
  });
  connect(this, &ImportNomorNonDuplicate::rejected, [this]() {
    this->pr->selesai();
    this->pr->deleteLater();
  });
  connect(pr, SIGNAL(emitDone()), SLOT(workdone()));
  connect(pr, SIGNAL(emitImportFailed(QString)), SLOT(importfailed(QString)));
  connect(pr, SIGNAL(emitMaxProgress(qint64)), SLOT(emitMaxProgress(qint64)));
  connect(pr, SIGNAL(emitProgress(qint64)), SLOT(emitProgressStep(qint64)));
  bar = new prosesbar(this);
  bar->setWindowTitle(tr("Sedang melakukan import"));
  bar->setWindowFlags(bar->windowFlags() & ~Qt::WindowCloseButtonHint);
  bar->setmsg(
      tr("Sedang melakukan import data nomor...").toStdString().c_str());
}

ImportNomorNonDuplicate::~ImportNomorNonDuplicate() { delete ui; }

void ImportNomorNonDuplicate::setconfigdb(dbbase::configdb *db) { mdb = db; }

void ImportNomorNonDuplicate::on_pushButton_tutup_clicked() { reject(); }

void ImportNomorNonDuplicate::on_pushButton_jalankan_upload_clicked() {
    pr->setup(ui->lineEdit->text(), *mdb,
              ui->checkBox_is_using_header->isChecked());
    bar->setprogressStep (0);
   pr->doimport ();
   bar->open ();
}

void ImportNomorNonDuplicate::on_pushButton_pilih_file_csv_clicked() {
    fname=QFileDialog::getOpenFileName (this,tr("Pilih File CSV format nomor saja"),tr("."),"*.csv");
    if(fname.isEmpty ()){
        ui->pushButton_jalankan_upload->setEnabled (false);
        ui->lineEdit->clear ();
        return;
    }
    QFile ftest(fname);
    if(!ftest.open (QIODevice::ReadOnly|QIODevice::Text)){
        error::dialog::emitdialogerror (this,tr("Galat")
                                        ,tr("File %1 tidak bisa dibuka. Pastikan anda memiliki hak izin akses ke file tersebut.").arg (fname));
        ui->pushButton_jalankan_upload->setEnabled (false);
              ui->lineEdit->clear ();
        return;
    }
    ftest.close ();
    ui->pushButton_jalankan_upload->setEnabled (true);
    ui->lineEdit->setText (fname);
    
}

void ImportNomorNonDuplicate::workdone() { accept(); }

void ImportNomorNonDuplicate::importfailed(const QString &file) {
   bar->accept ();
   error::dialog::emitdialogerror (this,tr("Galat"),file);
   reject ();
}

void ImportNomorNonDuplicate::emitProgressStep(qint64 step) {
    bar->setprogressStep (step);
}

void ImportNomorNonDuplicate::emitMaxProgress(qint64 max) {
    bar->setmax (max);
}

void ImportNomorNonDuplicate::on_checkBox_is_using_header_toggled(bool checked)
{
//    pr->setup(ui->lineEdit->text(), *mdb,checked);
}
