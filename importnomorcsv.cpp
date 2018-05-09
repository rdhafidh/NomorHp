#include "importnomorcsv.h"
#include <csvnumberparser.h>
#include <uiutil.h>
#include <QFileDialog>
#include <QtConcurrent>
#include "ui_importnomorcsv.h"

ImportNomorCSV::ImportNomorCSV(QWidget *parent)
    : QDialog(parent), ui(new Ui::ImportNomorCSV) {
  ui->setupUi(this);
  parser = new CSVNumberThreadParser(this);
  connect(this, &ImportNomorCSV::rejected, [this]() {
    this->parser->selesai();
    this->parser->deleteLater();
  });
  connect(this, &ImportNomorCSV::accepted, [this]() {
    this->parser->selesai();
    this->parser->deleteLater();
  });
  connect(parser, SIGNAL(parsedLine(int)), this, SLOT(parsedLine(int)));
  connect(parser, SIGNAL(parseDone()), this, SLOT(parseDone()));
  connect(parser, SIGNAL(parseFailed(QString)), this,
          SLOT(parseFailed(QString)));
  connect(parser, SIGNAL(parseinitDoneEmitLine(int)), this,
          SLOT(parseinitDoneEmitLine(int)));
  ui->pushButton_upload->setEnabled(false);
  pb = new prosesbar(this);
  pb->setmsg(
      tr("Sedang Melakukan proses upload data nomor..").toStdString().c_str());
  pb->setWindowFlags(pb->windowFlags() & ~Qt::WindowCloseButtonHint);
}

ImportNomorCSV::~ImportNomorCSV() {
  pb->deleteLater();
  delete ui;
}

void ImportNomorCSV::setconfigdb(const dbbase::configdb &config) {
  mdb = config;
}

void ImportNomorCSV::on_pushButton_upload_clicked() {
  parser->doimport();
  pb->setprogressStep(0);
  pb->setWindowTitle(tr("Proses berlangsung..."));
  pb->open();
}

void ImportNomorCSV::on_pushButton_batal_clicked() { reject(); }

void ImportNomorCSV::on_pushButton_pilih_csv_clicked() {
  fname = QFileDialog::getOpenFileName(
      this, tr("Pilih File CSV format nomor & cluster"), tr("."), "*.csv");
  if (fname.isEmpty()) {
    ui->pushButton_upload->setEnabled(false);
    return;
  }
  QFile ftest(fname);
  if (!ftest.open(QIODevice::ReadOnly | QIODevice::Text)) {
    error::dialog::emitdialogerror(
        this, tr("Galat"), tr("File %1 tidak bisa dibuka. Pastikan anda "
                              "memiliki hak izin akses ke file tersebut.")
                               .arg(fname));
    ui->pushButton_upload->setEnabled(false);
    return;
  }
  ftest.close();
  ui->lineEdit_fileupload->setText(fname);
  ui->pushButton_upload->setEnabled(true);
  parser->setup(fname, mdb, ui->checkBox_use_header->isChecked());
}

void ImportNomorCSV::parseinitDoneEmitLine(int rows) { pb->setmax(rows); }

void ImportNomorCSV::parseFailed(const QString &msg) {
  pb->setWindowTitle(tr("selesai"));
  pb->accept();
  error::dialog::emitdialogerror(this, tr("Galat"), msg);
  reject();
}

void ImportNomorCSV::parsedLine(int at) { pb->setprogressStep(at); }

void ImportNomorCSV::parseDone() {
  pb->setWindowTitle(tr("selesai"));
  pb->accept();
  this->accept();
}

void ImportNomorCSV::on_checkBox_use_header_toggled(bool checked) {
  if (!ui->lineEdit_fileupload->text().isEmpty()) {
    parser->setup(fname, mdb, checked);
  }
}
