#include "importexcelworking.h"
#include <peglib.h>
#include <xlsx/xlsxdocument.h>
#include <algorithm>
#include <iterator>
#include <limits>
#include "utilproduk.h"

ImportExcelWorking::ImportExcelWorking(QObject *parent) : QObject(parent) {}

ImportExcelWorking::~ImportExcelWorking() {}

void ImportExcelWorking::importsetup(const DataImportExcel &inp) {
  in.colcellranges.clear();
  in.colnamesandtypes.clear();
  in.tblname.clear();
  in.tblsqlname.clear();
  in.infologin.clear();
  in.fname.clear();

  std::copy(inp.colcellranges.begin(), inp.colcellranges.end(),
            std::back_inserter(in.colcellranges));
  std::copy(inp.colnamesandtypes.begin(), inp.colnamesandtypes.end(),
            std::back_inserter(in.colnamesandtypes));
  std::copy(inp.ignorecolumns.begin(), inp.ignorecolumns.end(),
            std::back_inserter(in.ignorecolumns));

  in.tblname = inp.tblname;
  in.tblsqlname = inp.tblsqlname;
  in.infologin = inp.infologin;
  in.fname = inp.fname;
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << "colcellranges size:" << inp.colcellranges.size();
  qDebug() << Q_FUNC_INFO
           << "in colcellranges size:" << in.colcellranges.size();
#endif
}

void ImportExcelWorking::doimport() {
  //
  Q_EMIT setmsgcontent(tr("Pengecekan input Excel...").toStdString().c_str());
  if (!QFileInfo::exists(in.fname)) {
    Q_EMIT maxbarimportexcel(100);
    Q_EMIT valuebarimportexcel(100);
    Q_EMIT resultimportexcel(ImportExcelResult::importexcelfilenotfound,
                             tr("Input file excel tidak dapat ditemukan."));
    return;
  }
  QFile fn(in.fname);
  if (!fn.open(QIODevice::ReadOnly)) {
    Q_EMIT maxbarimportexcel(100);
    Q_EMIT valuebarimportexcel(100);
    Q_EMIT resultimportexcel(ImportExcelResult::importexcelnotreadible,
                             tr("Input file excel tidak dapat dibaca."));
    return;
  }
  fn.close();

  // min pasti ada 1 kolom
  if (in.colcellranges.size() == 0) {
    Q_EMIT maxbarimportexcel(100);
    Q_EMIT valuebarimportexcel(100);
    Q_EMIT resultimportexcel(ImportExcelResult::importexcellemptycellrange,
                             tr("Anda harus mengisi cell range yang valid, "
                                "sesuai pada lembar excel."));
    return;
  }
  // assume here
  //  in.colcellranges.size() tidak melebih dari std::numeric_limits<int>::max()
  // so it is safe, because in.colcellranges.size() dari QSqlRecord.count()
  // asumsi nya data user supplied input yang valid itu dari objeck
  // in.colcellranges,belum termasuk yg in.ignorecolumns
  // jd total col in.colcellranges.size()+

  // find out how many rows here?,
  // detect if user submit from the bottom cell to up.
  // the correct one is user submit from up cell to bottom.
  // we use qint64, for detecting integer overflow here
  // we determine everything from  first coloumn. if user submit different
  // cellrange rows for every columns
  // give her assertion error

  std::tuple<bool, QByteArray, qint64> mcelluppest =
      parsecellnamehorizontal(in.colcellranges.at(0).first);
  std::tuple<bool, QByteArray, qint64> mcellbeyond =
      parsecellnamehorizontal(in.colcellranges.at(0).second);
  // semisal b2 - b5 , increment second cell +1 lalu dikurangi value cell
  // pertama, jadi 6-2=4.

  // check first
  if (!std::get<0>(mcelluppest) || !std::get<0>(mcellbeyond)) {
    Q_EMIT maxbarimportexcel(100);
    Q_EMIT valuebarimportexcel(100);
    Q_EMIT resultimportexcel(
        ImportExcelResult::importexcellrangecellerror,
        tr("Kami menemukan kesalahan input cell excel pada kolom %1."
           " Anda bisa memeriksanya kembali. Pastikan jangkauan"
           "cell tiap kolom sama.")
            .arg(1));
    return;
  }

  qint64 rows = (std::get<2>(mcellbeyond) + 1) - std::get<2>(mcelluppest);
  if (rows > (std::numeric_limits<int>::max())) {
    Q_EMIT maxbarimportexcel(100);
    Q_EMIT valuebarimportexcel(100);
    // integer overflow here
    Q_EMIT
    resultimportexcel(ImportExcelResult::importexcellinvalidcellrange,
                      tr("Kami mendeteksi bahwa jumlah baris data pada excel "
                         "tidak boleh melebihi dari "
                         "%1 baris. "
                         "Anda bisa memeriksanya ulang lagi.")
                          .arg((std::numeric_limits<int>::max())));
    return;
  }
  int row32bit = static_cast<int>(rows);

  // detect here if contain mistmatch size of rows for every col
  for (int col = 0; col < static_cast<int>(in.colcellranges.size()); col++) {
    std::tuple<bool, QByteArray, qint64> mcelluppest_loop =
        parsecellnamehorizontal(in.colcellranges.at(col).first);
    std::tuple<bool, QByteArray, qint64> mcellbeyond_loop =
        parsecellnamehorizontal(in.colcellranges.at(col).second);

    if (!std::get<0>(mcelluppest_loop) || !std::get<0>(mcellbeyond_loop)) {
      Q_EMIT maxbarimportexcel(100);
      Q_EMIT valuebarimportexcel(100);
      Q_EMIT resultimportexcel(
          ImportExcelResult::importexcellrangecellerror,
          tr("Kami menemukan invalid reference cell excel pada input kolom %1."
             " Anda bisa memeriksanya kembali pada Input Control Cell Kolom "
             "Excel. ")
              .arg(col + 1));
      return;
    }

    // check if user submit forexample c7-c4 . this is wrong here
    if (std::get<2>(mcelluppest_loop) > std::get<2>(mcellbeyond_loop)) {
      Q_EMIT maxbarimportexcel(100);
      Q_EMIT valuebarimportexcel(100);
      QString atas = QString::fromUtf8(std::get<1>(mcelluppest_loop));
      QString bawah = QString::fromUtf8(std::get<1>(mcellbeyond_loop));
      bawah.append(
          QString::number(static_cast<int>(std::get<2>(mcellbeyond_loop))));
      atas.append(
          QString::number(static_cast<int>(std::get<2>(mcelluppest_loop))));

      Q_EMIT resultimportexcel(
          ImportExcelResult::importexcellrangecellerror,
          tr("Kami menemukan invalid range cell kolom reference excel. "
             " Urutan baris cell setiap kolom harus dimulai semisal dari yang "
             "terendah c4 ke lebih kebawah c10. Contoh urutan yang "
             "tidak sah seperti dari yang terendah c10  ke lebih yang atas c4. "
             "Sedangkan kami mendeteksi bahwasanya anda menginputkan dari cell "
             "%1 ke yang lebih rendah cell %2 pada kolom ke %3.")
              .arg(atas.toUpper())
              .arg(bawah.toUpper())
              .arg(col + 1));
      return;
    }

    // detect if upper cell input c4 and lowest cell input d4, so this is
    // mistmatch here
    if (std::get<1>(mcelluppest_loop).toLower() !=
        std::get<1>(mcellbeyond_loop).toLower()) {
      QByteArray up = std::get<1>(mcelluppest_loop).toUpper();
      QByteArray down = std::get<1>(mcellbeyond_loop).toUpper();
      Q_EMIT maxbarimportexcel(100);
      Q_EMIT valuebarimportexcel(100);
      Q_EMIT resultimportexcel(
          ImportExcelResult::importexcellrangecellerror,
          tr("Kami menemukan invalid kolom reference excel. "
             " Input cell kolom yang anda masukkan paling atas %1x dan input "
             "cell kolom paling bawah %2x, "
             "seharusnya input cell kolom paling atas dan cell kolom paling "
             "bawah sama. "
             "Semisal cell c4 dan c10.")
              .arg(up.constData())
              .arg(down.constData()));
      return;
    }

    // semisal b2 - b5 , increment second cell +1 lalu dikurangi value cell
    // pertama, jadi 6-2=4.

    qint64 rows_loop =
        std::get<2>(mcellbeyond_loop) - std::get<2>(mcelluppest_loop);
    rows_loop += 1;
    if (rows_loop != rows) {
      Q_EMIT maxbarimportexcel(100);
      Q_EMIT valuebarimportexcel(100);
      Q_EMIT resultimportexcel(
          ImportExcelResult::importexcellinvalidcellrange,
          tr("Kami mendeteksi jumlah baris yang tidak sama. Asumsi kami kolom"
             " ke 1 seperti yang anda inputkan ada %1 baris, sedangkan pada "
             "kolom ke %2 terdapat %3 baris. Pastikan jangkauan baris"
             "cell tiap kolom sama.")
              .arg(rows)
              .arg(col + 1)
              .arg(rows_loop));
      return;
    }

    // detect qvariant value if the type is string or qbytearray
    // make sure didnt contains any =value, and loop to every rows.
    // note that its now safe to static cast from qint64 to int
    int rwf = static_cast<int>(std::get<2>(mcelluppest_loop));
    for (int rw = rwf; rw < (row32bit + rwf); rw++) {
      QString cellname = std::get<1>(mcelluppest_loop);
      cellname = cellname.toUpper();
      cellname.append(QString::number(rw));
      QVariant mdat = parseexcelvalue(in.fname, cellname);
#ifdef DEBUGB
      qDebug() << "checked cell:" << cellname;
      qDebug() << "mdat:" << mdat;
#endif
      switch (mdat.type()) {
        case QVariant::ByteArray:
          if (mdat.toByteArray().startsWith("=")) {
            Q_EMIT maxbarimportexcel(100);
            Q_EMIT valuebarimportexcel(100);
            Q_EMIT resultimportexcel(
                ImportExcelResult::importexcelcontainformula,
                tr("Kami mendeteksi pada cell %1 tedapat nilai formula(rumus). "
                   "Anda bisa mengedit copy lalu paste as value dahulu pada MS "
                   "Office, "
                   "atau pada libreoffice copy lalu paste as number.")
                    .arg(cellname));
            return;
          }
          break;
        case QVariant::String:
          if (mdat.toString().startsWith("=")) {
            Q_EMIT maxbarimportexcel(100);
            Q_EMIT valuebarimportexcel(100);
            Q_EMIT resultimportexcel(
                ImportExcelResult::importexcelcontainformula,
                tr("Kami mendeteksi pada cell %1 tedapat nilai formula(rumus). "
                   "Anda bisa mengedit copy lalu paste as value dahulu pada MS "
                   "Office, "
                   "atau pada libreoffice copy lalu paste as number.")
                    .arg(cellname));
            return;
          }
          break;
        default:
          break;
      }
    }
  }
  // this case would be building sql statement from raw excel reading.
  // intrepreted that as empty string
  // note that there would be ignorecolumns, we should make special attention
  // here. Since this step is main point activity

  Q_EMIT setmsgcontent(tr("Memroses import Excel...").toStdString().c_str());
  Q_EMIT valuebarimportexcel(0);
  int rwf = static_cast<int>(std::get<2>(mcelluppest));
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__ << "executed..";
  qDebug() << "rwf:" << rwf;
  qDebug() << "row32bit:" << row32bit;
#endif

  int notignoredcol = 0;

  Q_EMIT maxbarimportexcel((row32bit + rwf) - 1);
  for (int rw = rwf; rw < (row32bit + rwf); rw++) {
    // build raw sql statement here
    Q_EMIT valuebarimportexcel(rw);
    QString ins;
    ins.append(QString("insert into %1 values ( ").arg(in.tblsqlname));
    notignoredcol = 0;  // reset col

    for (int cl = 0; cl < static_cast<int>(in.colcellranges.size() +
                                           in.ignorecolumns.size());
         cl++) {
      // differ which one is ignored and  not one.
      if (!std::any_of(in.ignorecolumns.begin(), in.ignorecolumns.end(),
                       [&cl](std::tuple<int, bool, QVariant> i) {
                         return std::get<0>(i) == cl;
                       })) {
        // column input user
        if (cl ==
            static_cast<int>(in.colcellranges.size() +
                             in.ignorecolumns.size()) -
                1) {
          // mentok tambahin tutup kurawal
          std::tuple<bool, QByteArray, qint64> cell =
              parsecellnamehorizontal(in.colcellranges.at(notignoredcol).first);
          QString cellname = QString::fromUtf8(std::get<1>(cell).toUpper());
          cellname.append(QString::number(rw));
#ifdef DEBUGB
          qDebug() << "checked statement cell not ignored :" << cellname;
#endif
          QVariant mdat = parseexcelvalue(in.fname, cellname);
          // sanity checks
          if (QDate::fromString(mdat.toString(), "dd-MM-yyyy").isValid()) {
            mdat = QVariant::fromValue(
                QDate::fromString(mdat.toString(), "dd-MM-yyyy"));
          }
          if (QDateTime::fromString(mdat.toString(), "dd-MM-yyyy hh:mm")
                  .isValid()) {
            mdat = QVariant::fromValue(
                QDateTime::fromString(mdat.toString(), "dd-MM-yyyy hh:mm"));
          }
          if (QTime::fromString(mdat.toString(), "hh::mm").isValid()) {
            mdat = QVariant::fromValue(
                QTime::fromString(mdat.toString(), "hh::mm"));
          }
          std::string ms;
          switch (mdat.type()) {
            case QVariant::ByteArray:
              if (mdat.toByteArray() == "TRUE" ||
                  mdat.toByteArray() == "FALSE") {
                ms.append(mdat.toByteArray().constData());
                ms.append("' )");
              } else {
                ms.append("'");
                ms.append(mdat.toByteArray().constData());
                ms.append("' )");
              }
              break;
            case QVariant::Bool:
              if (mdat.toBool()) {
                ms.append("TRUE )");
              } else {
                ms.append("FALSE )");
              }
              break;
            case QVariant::Double:
              ms.append(mdat.toString().toStdString().c_str());
              ms.append(" )");
              break;
            case QVariant::Date:
              ms.append("'");
              ms.append(
                  mdat.toDate().toString("dd-MM-yyyy").toStdString().c_str());
              ms.append(" ' )");
              break;
            case QVariant::Invalid:
#ifdef DEBUGB
              qDebug() << "found invalid string line " << __LINE__;
#endif
              ms.append(" '' )");
              break;
            case QVariant::Time:
              ms.append("'");
              ms.append(
                  mdat.toTime().toString("hh:mm").toUtf8().constData());
              ms.append("' )");
              break;
            case QVariant::DateTime:
              ms.append("'");
              ms.append(mdat.toDateTime()
                            .toString("yyyy/MM/dd hh:mm:ss")
                            .toUtf8()
                            .constData());
              ms.append("' )");
              break;
            case QVariant::Int:
              ms.append(mdat.toString().toStdString().c_str());
              ms.append(" )");
              break;
            case QVariant::String:
              // convert into string
              if (mdat.toString().toUtf8() == "TRUE" ||
                  mdat.toString().toUtf8() == "FALSE") {
                ms.append(mdat.toString().toUtf8().constData());
                ms.append(" )");
              } else {
                ms.append("'");
                ms.append(mdat.toString().toUtf8().constData());
                ms.append("' )");
              }
              break;
          }
          ins.append(ms.c_str());
        } else {
          // build string something like 'val_col',
          std::tuple<bool, QByteArray, qint64> cell =
              parsecellnamehorizontal(in.colcellranges.at(notignoredcol).first);
          QString cellname = QString::fromUtf8(std::get<1>(cell).toUpper());
          cellname.append(QString::number(rw));
          QVariant mdat = parseexcelvalue(in.fname, cellname);
          if (QDate::fromString(mdat.toString(), "dd-MM-yyyy").isValid()) {
            mdat = QVariant::fromValue(
                QDate::fromString(mdat.toString(), "dd-MM-yyyy"));
          }
          if (QDateTime::fromString(mdat.toString(), "dd-MM-yyyy hh:mm")
                  .isValid()) {
            mdat = QVariant::fromValue(
                QDateTime::fromString(mdat.toString(), "dd-MM-yyyy hh:mm"));
          }
          if (QTime::fromString(mdat.toString(), "hh::mm").isValid()) {
            mdat = QVariant::fromValue(
                QTime::fromString(mdat.toString(), "hh::mm"));
          }
          std::string ms;
          switch (mdat.type()) {
            case QVariant::ByteArray:
              if ((mdat.toByteArray() == "TRUE") ||
                  (mdat.toByteArray() == "FALSE")) {
                ms.append(mdat.toByteArray().constData());
                ms.append(" ,");
              } else {
                ms.append("'");
                ms.append(mdat.toByteArray().constData());
                ms.append("', ");
              }
              break;
            case QVariant::Bool:
              if (mdat.toBool()) {
                ms.append("TRUE ,");
              } else {
                ms.append("FALSE ,");
              }
              break;
            case QVariant::Double:
              ms.append(mdat.toString().toStdString().c_str());
              ms.append(" ,");
              break;
            case QVariant::Date:
              ms.append("'");
              ms.append(
                  mdat.toDate().toString("dd/MM/yyyy").toStdString().c_str());
              ms.append("' ,");
              break;
            case QVariant::Invalid:
              ms.append("'' ,");
              break;
            case QVariant::Time:
              ms.append("'");
              ms.append(mdat.toTime().toString("hh:mm").toUtf8().constData());
              ms.append("' ,");
              break;
            case QVariant::DateTime:
              ms.append("'");
              ms.append(mdat.toDateTime()
                            .toString("yyyy/MM/dd hh:mm:ss")
                            .toUtf8()
                            .constData());
              ms.append("' ,");
              break;
            case QVariant::Int:
              ms.append(mdat.toString().toStdString().c_str());
              ms.append(" ,");
              break;
            case QVariant::String:
              // convert into string
              if ((mdat.toString().toUtf8() == "TRUE") ||
                  (mdat.toString().toUtf8() == "FALSE")) {
                ms.append(mdat.toString().toUtf8().constData());
                ms.append(" ,");
              } else {
                ms.append("'");
                ms.append(mdat.toString().toUtf8().constData());
                ms.append("', ");
              }
              break;
          }
          ins.append(ms.c_str());
        }
        // detect here if notignoredcol tidak melebihi in.colcellranges.size()
        // user input
        if (notignoredcol != in.colcellranges.size() - 1) {
          notignoredcol++;
        }
#ifdef DEBUGB
        qDebug() << "notignoredcol:" << notignoredcol;
        qDebug() << "cl:" << cl;
#endif
      } else {
        // ignored column

        if (cl ==
            static_cast<int>(in.colcellranges.size() +
                             in.ignorecolumns.size()) -
                1) {
          // mentok tambahin tutup kurawal
          // importexcel this behaivor is that:
          // bool jika true maka column itu buat idsistem jika false
          // maka kolumn tadi diBUAT USER kode default input type
          if (std::any_of(in.ignorecolumns.begin(), in.ignorecolumns.end(),
                          [&cl](std::tuple<int, bool, QVariant> i) {
                            return (std::get<1>(i) == true) &&
                                   (cl == std::get<0>(i));
                          })) {
            // ini kolom buat idsistem
            QByteArray tm;
            auto now=QDateTime::currentDateTime ();
            tm=now.toString ("ddMMyyhhmmsszzz").toLocal8Bit ();
            std::string ms;
            ms.append("'");
            ms.append(tm.toHex().constData());
            ms.append("' )");
            ins.append(ms.c_str());
          } else {
            // ini kolom user supplied default type
            int maxsizeignored = 0;
            int maxsz = static_cast<int>(in.ignorecolumns.size()) - 1;
            std::for_each(
                in.ignorecolumns.begin(), in.ignorecolumns.end(),
                [&cl, &ins, &maxsizeignored,
                 &maxsz](std::tuple<int, bool, QVariant> i) {
                  if (maxsizeignored == maxsz) {
                    return;
                  }
                  if (cl == std::get<0>(i)) {
                    std::string ms;
                    switch (std::get<2>(i).type()) {
                      case QVariant::DateTime:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toDateTime()
                                      .toString("yyyy/MM/dd hh:mm:ss")
                                      .toUtf8()
                                      .constData());
                        ms.append("' )");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::String:
                        if ((std::get<2>(i).toByteArray() == "TRUE") ||
                            (std::get<2>(i).toByteArray() == "FALSE")) {
                          ms.append(std::get<2>(i).toByteArray());
                          ms.append(" ,");
                        } else {
                          ms.append("'");
                          ms.append(
                              std::get<2>(i).toString().toUtf8().constData());
                          ms.append("',");
                          ins.append(ms.c_str());
                        }
                        break;
                      case QVariant::Bool:
                        if (std::get<2>(i).toBool()) {
                          ms.append("TRUE");
                        } else {
                          ms.append("FALSE");
                        }
                        ms.append(" )");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::Date:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toDate()
                                      .toString("dd/MM/yyyy")
                                      .toUtf8()
                                      .constData());
                        ms.append("' )");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::ByteArray:
                        if ((std::get<2>(i).toByteArray() == "TRUE") ||
                            (std::get<2>(i).toByteArray() == "FALSE")) {
                          ms.append(std::get<2>(i).toByteArray());
                          ms.append(" )");
                        } else {
                          ms.append("'");
                          ms.append(std::get<2>(i).toByteArray().constData());
                          ms.append("')");
                          ins.append(ms.c_str());
                        }
                        break;
                      case QVariant::Invalid:
// intreperet as empty string
#ifdef DEBUGB
                        qDebug() << "found invalid string line " << __LINE__
                                 << "maxsizeignored" << maxsizeignored
                                 << "maxsz" << maxsz;
#endif
                        ms.append("'' )");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::Double:
                        ms.append(
                            std::get<2>(i).toString().toUtf8().constData());
                        ms.append(" )");
                        break;
                      case QVariant::Time:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toTime()
                                      .toString("hh:mm")
                                      .toUtf8()
                                      .constData());
                        ms.append("' )");
                        break;
                      case QVariant::Int:
                        ms.append(
                            std::get<2>(i).toString().toUtf8().constData());
                        ms.append(" )");
                        break;
                    }
                    maxsizeignored++;
                  }
                });
          }

        } else {
          if (std::any_of(in.ignorecolumns.begin(), in.ignorecolumns.end(),
                          [&cl](std::tuple<int, bool, QVariant> i) {
                            return (std::get<1>(i) == true) &&
                                   (cl == std::get<0>(i));
                          })) {
            // ini kolom buat idsistem
            QByteArray tm;
            auto now=QDateTime::currentDateTime ();
            tm=now.toString ("ddMMyyhhmmsszzz").toLocal8Bit ();
            std::string ms;
            ms.append("'");
            ms.append(tm.toHex().constData());
            ms.append("' ,");
            ins.append(ms.c_str());

          } else {
            // ini kolom user supplied default type
            int maxsizeignored = 0;
            int maxsz = static_cast<int>(in.ignorecolumns.size()) - 1;
            std::for_each(
                in.ignorecolumns.begin(), in.ignorecolumns.end(),
                [&cl, &ins, &maxsizeignored,
                 &maxsz](std::tuple<int, bool, QVariant> i) {
                  if (maxsizeignored == maxsz) {
                    return;
                  }
                  if (cl == std::get<0>(i)) {
                    std::string ms;
                    switch (std::get<2>(i).type()) {
                      case QVariant::DateTime:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toDateTime()
                                      .toString("yyyy/MM/dd hh:mm:ss")
                                      .toUtf8()
                                      .constData());
                        ms.append("' ,");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::Time:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toTime()
                                      .toString("hh:mm")
                                      .toUtf8()
                                      .constData());
                        ms.append("' ,");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::String:
                        if ((std::get<2>(i).toByteArray() == "TRUE") ||
                            (std::get<2>(i).toByteArray() == "FALSE")) {
                          ms.append(std::get<2>(i).toByteArray());
                          ms.append(" ,");
                        } else {
                          ms.append("'");
                          ms.append(
                              std::get<2>(i).toString().toUtf8().constData());
                          ms.append("',");
                          ins.append(ms.c_str());
                        }
                        break;
                      case QVariant::Bool:
                        if (std::get<2>(i).toBool()) {
                          ms.append("TRUE");
                        } else {
                          ms.append("FALSE");
                        }
                        ms.append(" ,");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::Date:
                        ms.append("'");
                        ms.append(std::get<2>(i)
                                      .toDate()
                                      .toString("dd/MM/yyyy")
                                      .toUtf8()
                                      .constData());
                        ms.append("' ,");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::ByteArray:
                        if ((std::get<2>(i).toByteArray() == "TRUE") ||
                            (std::get<2>(i).toByteArray() == "FALSE")) {
                          ms.append(std::get<2>(i).toByteArray());
                          ms.append(" ,");
                        } else {
                          ms.append("'");
                          ms.append(std::get<2>(i).toByteArray().constData());
                          ms.append("',");
                          ins.append(ms.c_str());
                        }
                        break;
                      case QVariant::Invalid:
                        // intreperet as empty string
                        ms.append("'' ,");
                        ins.append(ms.c_str());
                        break;
                      case QVariant::Double:
                        ms.append(
                            std::get<2>(i).toString().toUtf8().constData());
                        ms.append(" ,");
                        break;
                      case QVariant::Int:
                        ms.append(
                            std::get<2>(i).toString().toUtf8().constData());
                        ms.append(" ,");
                        break;
                    }
                    maxsizeignored++;
                  }
                });
          }
        }
      }
    }
    ins.append(" on conflict do nothing;");
    std::cout << "ins:" << ins.toUtf8().constData() << "\n";
    utilproduk::sqlmodelwork sql;

    std::pair<bool, QString> rets = sql.crdelalter(in.infologin, ins);
    if (!rets.first) {
      Q_EMIT resultimportexcel(ImportExcelResult::importexcelsqlerror,
                               tr("%1").arg(rets.second));
      return;
    }
  }

  qDebug() << "size coll for all:"
           << in.colcellranges.size() + in.ignorecolumns.size();

  Q_EMIT resultimportexcel(ImportExcelResult::importexcelsuccess,
                           tr("Sukses. Query Import Selesai."));
}

std::tuple<bool, QByteArray, qint64>
ImportExcelWorking::parsecellnamehorizontal(const QString &name) {
  auto syntax = R"(
                            CELL_EXCEL <- COLUMN
                            COLUMN <- VERT_COL NUMBER
                            VERT_COL <- [a-zA-Z]+
                            NUMBER <- [0-9]+
                       )";
  peg::parser parser(syntax);
  QByteArray mname;
  qint64 row;
  parser["NUMBER"] = [&row](const peg::SemanticValues &sv) {
    QString ns(sv.str().c_str());
    row = ns.toInt();
  };
  parser["VERT_COL"] = [&mname](const peg::SemanticValues &sv) {
    switch (sv.choice) {
      case 0:
        mname.append(sv.str().c_str());
        break;
      default:
        break;
    }
  };
  parser.enable_packrat_parsing();
  auto ret = parser.parse(name.toUtf8().constData());
  // make sure user input dont overflow here.
  if (row > std::numeric_limits<int>::max()) {
    if (ret) {
      ret = false;
    }
  }
  return std::make_tuple(ret, mname, row);
}

QVariant ImportExcelWorking::parseexcelvalue(const QString &fname,
                                             const QString &cellname) {
  QXlsx::Document xlsx(fname);
  return xlsx.read(cellname);
}

ImportExcelThread::ImportExcelThread(QObject *parent) : QObject(parent) {
  wk.reset(new ImportExcelWorking);
  wk->moveToThread(&th);
  connect(wk.data(), &ImportExcelWorking::maxbarimportexcel, this,
          &ImportExcelThread::maxbarimportexcel);
  connect(wk.data(), &ImportExcelWorking::valuebarimportexcel, this,
          &ImportExcelThread::valuebarimportexcel);
  connect(wk.data(), &ImportExcelWorking::resultimportexcel, this,
          &ImportExcelThread::resultimportexcel);
  connect(wk.data(), &ImportExcelWorking::setmsgcontent, this,
          &ImportExcelThread::setmsgcontent);
  connect(&th, &QThread::started, wk.data(), &ImportExcelWorking::doimport);
  connect(&th, &QThread::started, this, &ImportExcelThread::startdoing);
}

ImportExcelThread::~ImportExcelThread() {}

void ImportExcelThread::importsetup(const DataImportExcel &inp) {
  wk->importsetup(inp);
}

void ImportExcelThread::doimport() { th.start(); }

void ImportExcelThread::selesai() {
  if (th.isRunning()) {
    th.requestInterruption();
    th.quit();
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << "thread.quit now....";
#endif
  }
}
