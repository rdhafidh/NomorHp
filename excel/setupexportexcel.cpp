#include "setupexportexcel.h"
#include <QFileDialog>
#include "exportwizard.h"
#include "ui_setupexportexcel.h"
#include "uiutil.h"

SetupExportExcel::SetupExportExcel(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::SetupExportExcel) {
  ui->setupUi(this);
  pb = new prosesbar;
  pb->setWindowFlags(pb->windowFlags() & ~Qt::WindowCloseButtonHint);
  setPixmap(QWizard::BannerPixmap, QPixmap(":/importdialog/banner_excel.png"));
  setPixmap(QWizard::LogoPixmap,
            QPixmap(":/importdialog/export_excel_logo.png"));
  ui->lineEdit_judultabel->setEnabled (false);
  ui->toolButton_doexport->setEnabled (true);
}

SetupExportExcel::~SetupExportExcel() {
  pb->deleteLater();
  delete ui;
}

void SetupExportExcel::maxbarexportexcel(int n) { pb->setmax(n); }

void SetupExportExcel::valuebarexportexcel(int n) { pb->setprogressStep(n); }

void SetupExportExcel::resultexportexcel(ExportExcelResult res) {
  pb->setWindowTitle(tr("Proses selesai..."));
  pb->accept();

  qApp->processEvents();
  exportdone = true;
  Q_EMIT completeChanged();
  qApp->processEvents();
  switch (res) {
    case ExportExcelResult::exportfailnotwritable:
      error::dialog::emitdialogwarning(
          this, tr("Export Excel."),
          tr("Anda tidak memiliki hak izin akses untuk export excel. "
             "Coba Anda ulangi pilih folder lain untuk menyimpan proses export "
             "excel."));
      break;

    case ExportExcelResult::exportsuccess:
      error::dialog::emitdialoginfo(this, tr("Export Excel."),
                                    tr("Proses export excel berhasil dan telah "
                                       "selesai. Klik tombol selesai "
                                       "pada program pemandu."));
      break;
    case ExportExcelResult::exportsuccessemptycol:
      error::dialog::emitdialogwarning(
          this, tr("Export Excel."),
          tr("Proses export excel selesai dengan isi tabel data yang kosong."));
      break;
    case ExportExcelResult::exportfailexportexcelnameempty:
      error::dialog::emitdialogwarning(
          this, tr("Export Excel."),
          tr("Proses export excel terjadi galat. Nama File tujuan excel tidak "
             "boleh kosong."));
      break;
    case ExportExcelResult::exportfailnullpointertableviewdatasetup:
      error::dialog::emitdialogwarning(
          this, tr("Export Excel."),
          tr("Proses export excel terjadi galat. Internal Error data setup. "
             "Dimohon menghubungi "
             "customer service kami lebih lanjut"
             "."));
      break;
    default:
      break;
  }

  Q_EMIT requestcleanupthread();
}

void SetupExportExcel::showmsg() {
  pb->setmsg(tr("Sedang Meng-ekspor...").toUtf8().constData());
  pb->setWindowTitle(tr("Proses berlangsung..."));
  pb->open();
}

int SetupExportExcel::nextId() const { return -1; }

bool SetupExportExcel::isComplete() const {
  // by default we wait until user click doexport button
  return exportdone;
}

void SetupExportExcel::initializePage() {
  exportdone = false;
  Q_EMIT completeChanged();
}

void SetupExportExcel::on_toolButton_doexport_clicked() {
  Q_EMIT exportexcel(ui->lineEdit_judultabel->text(),
                     ui->checkBox_gunakan_62->isChecked(),mode_csv_or_excel);
  ui->toolButton_doexport->setDisabled(true);
}

void SetupExportExcel::on_lineEdit_judultabel_textChanged(const QString &arg1) {
  if (!arg1.isEmpty()) {
    ui->toolButton_doexport->setDisabled(false);
  } else {
    if (ui->comboBox_mode_export_is_csv_or_excel->currentIndex() == 1) {
      ui->toolButton_doexport->setDisabled(true);
    } else {
      ui->toolButton_doexport->setDisabled(false);
    }
  }
}

void SetupExportExcel::
    on_comboBox_mode_export_is_csv_or_excel_currentIndexChanged(int index) {
  if (index == 0) {
    // mode csv
    ui->lineEdit_judultabel->setEnabled(false);
    mode_csv_or_excel=1;
      ui->toolButton_doexport->setDisabled(false);
  } else {
    ui->lineEdit_judultabel->setEnabled(true);
    mode_csv_or_excel=0;
  }
}
