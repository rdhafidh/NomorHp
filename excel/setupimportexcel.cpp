#include "setupimportexcel.h"
#include <QAbstractButton>
#include <algorithm>
#include "importwizard.h"
#include "ui_setupimportexcel.h"

SetupImportExcel::SetupImportExcel(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::SetupImportExcel) {
  ui->setupUi(this);
  setPixmap(QWizard::BannerPixmap,QPixmap(":/importdialog/banner_excel.png"));
  setPixmap(QWizard::LogoPixmap,QPixmap(":/importdialog/export_excel_logo.png"));
}

SetupImportExcel::~SetupImportExcel() { delete ui; }

void SetupImportExcel::setTableInfos(const DataImportExcel4SetupWizard &inf) {
  info.colnamesandtypes = inf.colnamesandtypes;
  info.tblname = inf.tblname;
  info.ignorecolumns = inf.ignorecolumns;
#ifdef DEBUGB
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`";
  qDebug() << Q_FUNC_INFO << "called";
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`";
#endif
}

int SetupImportExcel::nextId() const {
  if (!foundinvalidcol) {
    return ImportWizard::setupfeedimportexcel;
  }
  return -1;
}

void SetupImportExcel::initializePage() {
  ui->label_headerket->setText(
      ui->label_headerket->text()
          .arg(info.tblname)
          .arg(static_cast<int>(info.colnamesandtypes.size() -
                                info.ignorecolumns.size())));

  if (info.colnamesandtypes.size() == 0) {
    QString ret =
        tr("Proses import excel dihentikan karena kami tidak\ndapat"
           "mendeteksi jumlah kolom dalam tabel.");
    ui->label_kolomrincian->setText(ret);
    setFinalPage(true);
    foundinvalidcol = true;
#ifdef DEBUGB
    // make sure setTableInfos had been called here.
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`";
    qDebug() << Q_FUNC_INFO << "called";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`";
    qDebug() << __FILE__ << __LINE__ << "executed";
#endif
    return;
  }

  // possible wrong here ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // FIXME
  // note that it is impossible if type is qvariant::invalid import
  // so we break here,telling that we couldnt continue simply because there is
  // invalid qvariant.
  bool invalidvariant = false;
  QString invalidvariantcolname = "";
  int invalidvariantcolindex;
  QString ket;
//
// copy first ,any improve here?

#ifdef DEBUGB
  qDebug() << __FILE__ << __LINE__ << "executed";
  qDebug() << "info.colnamesandtypes.size():" << info.colnamesandtypes.size();
  qDebug() << "info.ignorecolumns.size():" << info.ignorecolumns.size();
#endif
  for (int col = 0; col < info.colnamesandtypes.size(); col++) {
#ifdef DEBUGB
    qDebug() << "ket:";
    qDebug() << ket;
#endif
#ifdef DEBUGB
    qDebug() << __FILE__ << __LINE__ << "executed";
#endif
    if (std::any_of(info.ignorecolumns.begin(), info.ignorecolumns.end(),
                    [&col](std::tuple<int, bool, QVariant> i) {
                      return col == std::get<0>(i);
                    })) {
      QString line;
      switch (info.colnamesandtypes.at(col).second) {
        case QVariant::Date:
          line.append(tr("%1. Kolom %2 Pengisi default sistem hari ini.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::DateTime:
          line.append(
              tr("%1. Kolom %2 Pengisi default sistem hari jam sekarang.\n")
                  .arg(col + 1)
                  .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::Bool:
          line.append(tr("%1. Kolom %2 Pengisi default sistem false.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::Int:
          line.append(tr("%1. Kolom %2 Pengisi default sistem 0.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::Time:
          line.append(tr("%1. Kolom %2 Pengisi default sistem jam sekarang.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::Double:
          line.append(tr("%1. Kolom %2 Pengisi default sistem 0,0.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
        case QVariant::Invalid:
          invalidvariant = true;
          invalidvariantcolindex = col + 1;
          invalidvariantcolname = info.colnamesandtypes.at(col).first;
          break;
        default:
          line.append(tr("%1. Kolom %2 Pengisi default sistem aplikasi ini.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first));
          break;
      }
      ket.append(line);
    } else {
      QString line;
      switch (info.colnamesandtypes.at(col).second) {
        case QVariant::Date:
          line.append(
              tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                  .arg(col + 1)
                  .arg(info.colnamesandtypes.at(col).first)
                  .arg(tr("format tanggal semisal 10-10-2016(dd-MM-yyyy)")));
          break;
        case QVariant::DateTime:
          line.append(tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first)
                          .arg(tr("format hari jam  seperti 10-03-2016 10:05 "
                                  "(dd-MM-yyyy hh:mm)")));
          break;
        case QVariant::Bool:
          line.append(tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first)
                          .arg(tr("TRUE atau FALSE")));
          break;
        case QVariant::Int:
          line.append(tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first)
                          .arg(tr("angka bilangan digit tanpa koma")));
          break;
        case QVariant::Time:
          line.append(
              tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                  .arg(col + 1)
                  .arg(info.colnamesandtypes.at(col).first)
                  .arg(tr("waktu jam format 24 jam semisal 23::05(hh::mm)")));
          break;
        case QVariant::Double:
          line.append(tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first)
                          .arg(tr("angka bilangan digit dengan koma")));
          break;
        case QVariant::Invalid:
          invalidvariant = true;
          invalidvariantcolindex = col + 1;
          invalidvariantcolname = info.colnamesandtypes.at(col).first;
          break;
        default:
          line.append(tr("%1. Kolom %2 membutuhkan tipe data %3.\n")
                          .arg(col + 1)
                          .arg(info.colnamesandtypes.at(col).first)
                          .arg(tr("huruf a-z, A-Z atau unicode text, bisa "
                                  "dengan campuran digit")));
          break;
      }
      ket.append(line);
    }
  }
  if (invalidvariant) {
    ket.clear();
    ket.append(
        tr("Kami menemukan tipe data yang tidak bisa dikenali oleh sistem\n"
           "pada kolom ke %1 dan nama kolom %2. Proses import kedelam "
           "database\n"
           "tidak bisa diteruskan. Klik selesai untuk menutup program pemandu.")
            .arg(invalidvariantcolindex)
            .arg(invalidvariantcolname));

    setFinalPage(true);
    foundinvalidcol = true;
  }
#ifdef DEBUGB
  qDebug() << "ket:";
  qDebug() << ket;
#endif
  ui->label_kolomrincian->setText(ket);
}
