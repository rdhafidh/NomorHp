#include "setupfeedimportexcel.h"
#include <algorithm>
#include "ui_setupfeedimportexcel.h"

SetupFeedImportExcel::SetupFeedImportExcel(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::SetupFeedImportExcel) {
  ui->setupUi(this);
  pb = new prosesbar;
  pb->setWindowFlags(pb->windowFlags() & ~Qt::WindowCloseButtonHint);
  setPixmap(QWizard::BannerPixmap,QPixmap(":/importdialog/banner_excel.png"));
  setPixmap(QWizard::LogoPixmap,QPixmap(":/importdialog/export_excel_logo.png"));
  mlabels.clear();
  medits1.clear();
  medits2.clear();
  di.colcellranges.clear();
  di.colnamesandtypes.clear();
  di.fname.clear();
  di.ignorecolumns.clear();
  di.infologin.clear();
  di.tblname.clear();
  di.tblsqlname.clear();
}

SetupFeedImportExcel::~SetupFeedImportExcel() {
  pb->deleteLater();
  foreach (QLabel *lbl, mlabels) {
    if (lbl != nullptr) {
      lbl->deleteLater();
#ifdef DEBUGB
      qDebug() << "widget deleted QLabel";
#endif
    }
  }
  foreach (QLineEdit *edts1, medits1) {
    if (edts1 != nullptr) {
      edts1->deleteLater();
#ifdef DEBUGB
      qDebug() << "widget deleted QLineEdit 2";
#endif
    }
  }
  foreach (QLineEdit *edts2, medits2) {
    if (edts2 != nullptr) {
      edts2->deleteLater();
#ifdef DEBUGB
      qDebug() << "widget deleted QLineEdit 1";
#endif
    }
  }

  delete ui;
}

void SetupFeedImportExcel::setTableInfos(
    const DataImportExcel4SetupWizard &inf) {
  di.colnamesandtypes = inf.colnamesandtypes;
  di.ignorecolumns = inf.ignorecolumns;
  di.tblname = inf.tblname;
  di.tblsqlname = inf.tblsqlname;
  di.infologin = inf.infologin;

}

void SetupFeedImportExcel::maxbarimportexcel(int n) { pb->setmax(n); }

void SetupFeedImportExcel::valuebarimportexcel(int n) {
  pb->setprogressStep(n);
}

void SetupFeedImportExcel::resultimportexcel(ImportExcelResult ret,
                                             const QString &msg) {
  finishedornot = true;
  Q_EMIT completeChanged();

  qApp->processEvents();
  pb->setWindowTitle(tr("Proses selesai..."));
  pb->accept();
  di.colcellranges.clear();
  qApp->processEvents();
  switch (ret) {
    case ImportExcelResult::importexcelcontainformula:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcelfilenotfound:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcellemptycellrange:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcellinvalidcellrange:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcellrangecellerror:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcelnotreadible:
      error::dialog::emitdialogwarning(this, tr("Galat"), msg);
      break;
    case ImportExcelResult::importexcelsqlerror:
      error::dialog::emitdialogerror(this, tr("Galat"), tr("Terjadi galat ketika melakukan proses import data dari "
                                                           "excel, klik selengkapnya untuk mengetahui penyebabnya."),msg);
      break;
    case ImportExcelResult::importexcelsuccess:
      error::dialog::emitdialoginfo(this, tr("Berhasil"), msg);
      break;
    default:
      break;
  }
//prevent for doing second time
 ui->toolButton_doimportexcel->setEnabled(false);
 ui->toolButton_namafiletoimport->setEnabled(false);
  Q_EMIT requestcleanupthread();
}

void SetupFeedImportExcel::showmsgbar() {
  pb->setWindowTitle(tr("Proses berlangsung..."));
  pb->setmsg(tr("Proses import data excel...").toStdString().c_str());
  pb->open();
}

void SetupFeedImportExcel::setmsgcontent(const QString &dat)
{
    pb->setmsg(dat.toStdString().c_str());
}

int SetupFeedImportExcel::nextId() const { return -1; }

void SetupFeedImportExcel::initializePage() {
  // we pay attention to the ignore col

  for (int col = 0; col < static_cast<int>(di.colnamesandtypes.size()); col++) {
    if (std::any_of(di.ignorecolumns.begin(), di.ignorecolumns.end(),
                    [&col](std::tuple<int, bool, QVariant> i) {
                      return std::get<0>(i) == col;
                    })) {
      mlabels[col] = new QLabel;
      mlabels[col]->setText(tr("Kolom %1"
                               ":")
                                .arg(di.colnamesandtypes.at(col).first));

      medits1[col] = new QLineEdit;
      medits1[col]->setClearButtonEnabled(true);
      medits1[col]->setDisabled(true);
      medits1[col]->setToolTip(
          tr("Kolom %1 Pengisi default sistem aplikasi ini")
              .arg(di.colnamesandtypes.at(col).first));
      medits1[col]->setToolTipDuration(8000);

      medits2[col] = new QLineEdit;
      medits2[col]->setClearButtonEnabled(true);
      medits2[col]->setDisabled(true);
      medits2[col]->setToolTip(
          tr("Kolom %1 Pengisi default sistem aplikasi ini")
              .arg(di.colnamesandtypes.at(col).first));
      medits2[col]->setToolTipDuration(8000);

      ui->gridLayout_inputcontrol->addWidget(mlabels[col], col + 1, 0);
      ui->gridLayout_inputcontrol->addWidget(medits1[col], col + 1, 1);
      ui->gridLayout_inputcontrol->addWidget(medits2[col], col + 1, 2);
    } else {
      mlabels[col] = new QLabel;
      mlabels[col]->setText(
          tr("Kolom %1:").arg(di.colnamesandtypes.at(col).first));

      medits1[col] = new QLineEdit;
      medits1[col]->setClearButtonEnabled(true);

      medits1[col]->setToolTip(tr("Jangkauan cell paling atas."));
      medits1[col]->setToolTipDuration(8000);

      medits2[col] = new QLineEdit;
      medits2[col]->setClearButtonEnabled(true);

      medits2[col]->setToolTip(tr("Jangkauan cell paling bawah."));
      medits2[col]->setToolTipDuration(8000);

      ui->gridLayout_inputcontrol->addWidget(mlabels[col], col + 1, 0);
      ui->gridLayout_inputcontrol->addWidget(medits1[col], col + 1, 1);
      ui->gridLayout_inputcontrol->addWidget(medits2[col], col + 1, 2);
    }
  }
  finishedornot = false;
  Q_EMIT completeChanged();
}

bool SetupFeedImportExcel::isComplete() const { return finishedornot; }

void SetupFeedImportExcel::on_toolButton_namafiletoimport_clicked() {
  auto fname = QFileDialog::getOpenFileName(
      this, tr("Pilih Import Nama File Excel"), "", tr("Excel File (*.xlsx)"));
  if (fname.isEmpty()) {
    ui->toolButton_doimportexcel->setDisabled(true);
    return;
  } else {
    ui->toolButton_doimportexcel->setDisabled(false);
  }
  ui->lineEdit_importexcel->setText(fname);
  di.fname = fname;
}

void SetupFeedImportExcel::on_toolButton_doimportexcel_clicked() {
  bool foundcol1 = false;
  bool foundcol2 = false;

  QHashIterator<int, QLineEdit *> eds1(medits1);
  while (eds1.hasNext()) {
    eds1.next();
    if (!std::any_of(di.ignorecolumns.begin(), di.ignorecolumns.end(),
                     [&eds1](std::tuple<int, bool, QVariant> i) {
                       return std::get<0>(i) == eds1.key();
                     })) {
      if (eds1.value()->text().isEmpty()) {
        foundcol1 = true;
#ifdef DEBUGB
        qDebug() << "foundcol1 col:" << eds1.key();
#endif
      }
    }
  }

  if (foundcol1) {
    error::dialog::emitdialogwarning(
        this, tr("Galat"),
        tr("Kami mendeteksi salah satu input cell atas kosong. "
           "Anda harus mengisi jangkuan input cell atas seluruhnya"
           " setiap kolom."));
    return;
  }

  QHashIterator<int, QLineEdit *> eds2(medits2);
  while (eds2.hasNext()) {
    eds2.next();
    if (!std::any_of(di.ignorecolumns.begin(), di.ignorecolumns.end(),
                     [&eds2](std::tuple<int, bool, QVariant> i) {
                       return std::get<0>(i) == eds2.key();
                     })) {
      if (eds2.value()->text().isEmpty()) {
        foundcol2 = true;
#ifdef DEBUGB
        qDebug() << "foundcol2 col:" << eds2.key();
#endif
      }
    }
  }

  if (foundcol2) {
    error::dialog::emitdialogwarning(
        this, tr("Galat"),
        tr("Kami mendeteksi salah satu input cell bawah kosong. "
           "Anda harus mengisi jangkuan input cell bawah seluruhnya setiap "
           "kolom."));
    return;
  }
  // di fname
  di.fname = ui->lineEdit_importexcel->text();
  for (int col = 0; col < static_cast<int>(di.colnamesandtypes.size()); col++) {
    // setiap kolom ada dua input cell= 1 cell range
    if (!std::any_of(di.ignorecolumns.begin(), di.ignorecolumns.end(),
                     [&col](std::tuple<int, bool, QVariant> i) {
                       return std::get<0>(i) == col;
                     })) {
      di.colcellranges.push_back(
          qMakePair(medits1[col]->text(), medits2[col]->text()));
#ifdef DEBUGB
      qDebug() << "colnamesandtypes col:" << col << " left"
               << medits1[col]->text() << "right" << medits2[col]->text();
#endif
    }
  }

#ifdef DEBUGB
    qDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        qDebug() << "colcellranges size:" << di.colcellranges.size();
        qDebug() << "ignorecolumns size:" << di.ignorecolumns.size();
        qDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
#endif

  Q_EMIT importexcelwork(di);

}
