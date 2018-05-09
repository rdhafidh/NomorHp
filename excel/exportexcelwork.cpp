#include "exportexcelwork.h"
#include <xlsx/xlsxdocument.h>
#include <xlsx/xlsxformat.h>
#include <QDateTime>
#include <QFile>
#include <limits>

ExportExcelWork::ExportExcelWork(QObject *parent)
    : QObject(parent), mp(nullptr) {}

ExportExcelWork::~ExportExcelWork() {}

void ExportExcelWork::setTableAsliName(const QString &tbl,
                                       const dbbase::configdb &mdb) {
  tabel_asliname = tbl;
  db_config = mdb;
}

void ExportExcelWork::exportSetup(std::deque<QSqlRecord> *mrect, int howmanycol,
                                  const QString &fn, const QString &mtitle,
                                  const QStringList &colnm, bool m_use_62,
                                  bool is_mode_csv_or_excel) {
  mp = mrect;
  fname_kategori = fn;
  manycol = howmanycol;
  tbltitle = mtitle;
  hddata = colnm;
  use_62 = m_use_62;
  mode_csv_or_excel = is_mode_csv_or_excel;
}

void ExportExcelWork::doexporting() {
  // fname auto generated
  QString path = QDir::currentPath();
  QDir mypath(path + "/folder_utama");
  if (!mypath.exists()) {
    if (!mypath.mkdir(path + "/folder_utama")) {
      manycol = 0;
      Q_EMIT emitMaxBar(10);
      Q_EMIT emitValueBar(10);
      Q_EMIT result(ExportExcelResult::exportfailnotwritable);
      return;
    }
  }
  if (!mode_csv_or_excel) {
    QDateTime tm = QDateTime::currentDateTime();
    QString fname = tm.toString("ddMMyyhhmmsszzz");
    fname.prepend(QString("export_data_%1").arg (fname_kategori));
    fname.append(".xlsx");
    fname = mypath.path() + "/" + fname;
    QFile inf(fname);
    if (!inf.open(QIODevice::ReadWrite)) {
      if (mp->size() == 0) {
        manycol = 0;
        Q_EMIT emitMaxBar(10);
        Q_EMIT emitValueBar(10);
      } else {
        Q_EMIT emitMaxBar(mp->size());
        Q_EMIT emitValueBar(mp->size());
      }
      Q_EMIT result(ExportExcelResult::exportfailnotwritable);
      return;
    }
    inf.close();
    inf.remove();
    if (mp == nullptr) {
      manycol = 0;
      if (mp->size() == 0) {
        Q_EMIT emitMaxBar(10);
        Q_EMIT emitValueBar(10);
      } else {
        Q_EMIT emitMaxBar(mp->size());
        Q_EMIT emitValueBar(mp->size());
      }
      Q_EMIT result(ExportExcelResult::exportfailnullpointertableviewdatasetup);
      return;
    }

    // detect if mp size == 0
    if (mp->size() == 0) {
      manycol = 0;
      Q_EMIT emitMaxBar(10);
      Q_EMIT emitValueBar(10);
      Q_EMIT result(ExportExcelResult::exportsuccessemptycol);
      return;
    }

#ifdef DEBUGB
// qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
    Q_EMIT emitMaxBar(mp->size() * manycol);

    // find out how many col
    std::deque<std::string> celltitle;
    for (int cl = 0; cl < manycol; cl++) {
      QString cf = whatcolumnlabel(cl + 3);
      celltitle.push_back(cf.toStdString());
      // draw column always at second cell

      openemptyandwrite1(fname, cf.append("2").toUtf8(),
                         hddata.at(cl).toUtf8());
    }
    std::string cellmergetitle;
    cellmergetitle.append("C1:");
    cellmergetitle.append(celltitle.at(celltitle.size() - 1).c_str());
    cellmergetitle.append("1");

    openemptyandwriteheadertable(fname, "C1", cellmergetitle.c_str(),
                                 tbltitle.toUtf8());
    celltitle.clear();
    int prog = 0;
    for (int col = 0; col < manycol; col++) {
      for (int row = 0; row < mp->size(); row++) {
        if (row == 0) {
        }
        bool iskolom_nomor = false;
        QByteArray nomor = "";
        if (mp->at(row).fieldName(col) == "nomor") {
          iskolom_nomor = true;
          //  //qDebug()<<"ditemukan kolom nomor dengan value: "<<mp->at
          //  (row).value(col).toByteArray();
          nomor = mp->at(row).value(col).toByteArray();
        }
        bool is_kolom_cluster = false;
        if (col == 4) {
          is_kolom_cluster = true;
        }
        QString cf = whatcolumnlabel(col + 3);
        QVariant cl = row + 3;
        QString cellname;
        cellname.append(cf);
        cellname.append(cl.toByteArray());

        switch (mp->at(row).value(col).type()) {
          case QVariant::Date:
            openemptyandwrite1(fname, cellname, mp->at(row)
                                                    .value(col)
                                                    .toDate()
                                                    .toString("dd/MM/yyyy")
                                                    .toUtf8());
            break;
          case QVariant::Time:
            openemptyandwrite1(
                fname, cellname,
                mp->at(row).value(col).toTime().toString("h:mm:ss").toUtf8());
            break;
          default:
            if (!iskolom_nomor) {
              openemptyandwrite1(fname, cellname,
                                 mp->at(row).value(col).toByteArray());
            } else {
              if (use_62) {
                nomor = nomor.replace(0, 1, "62");
                openemptyandwrite1(fname, cellname, nomor);
              } else {
                openemptyandwrite1(fname, cellname,
                                   mp->at(row).value(col).toByteArray());
              }
            }
            if (is_kolom_cluster) {
              openemptyandwrite1(fname, cellname,
                                 mp->at(row).value(col).toByteArray());
            }
            break;
        }
        Q_EMIT emitValueBar(prog);
        prog++;
      }
    }
  } else {
    QDateTime tm = QDateTime::currentDateTime();
    QString fname = tm.toString("ddMMyyhhmmsszzz");
     fname.prepend(QString("export_data_%1").arg (fname_kategori));
    fname.append(".csv");
    fname = mypath.path() + "/" + fname;
    QFile inf(fname);
    if (!inf.open(QIODevice::ReadWrite)) {
      if (mp->size() == 0) {
        manycol = 0;
        Q_EMIT emitMaxBar(10);
        Q_EMIT emitValueBar(10);
      } else {
        Q_EMIT emitMaxBar(mp->size());
        Q_EMIT emitValueBar(mp->size());
      }
      Q_EMIT result(ExportExcelResult::exportfailnotwritable);
      return;
    }
    //   inf.close();
    //    inf.remove();
    if (mp == nullptr) {
      manycol = 0;
      if (mp->size() == 0) {
        Q_EMIT emitMaxBar(10);
        Q_EMIT emitValueBar(10);
      } else {
        Q_EMIT emitMaxBar(mp->size());
        Q_EMIT emitValueBar(mp->size());
      }
      Q_EMIT result(ExportExcelResult::exportfailnullpointertableviewdatasetup);
      return;
    }

    // detect if mp size == 0
    if (mp->size() == 0) {
      manycol = 0;
      Q_EMIT emitMaxBar(10);
      Q_EMIT emitValueBar(10);
      Q_EMIT result(ExportExcelResult::exportsuccessemptycol);
      return;
    }

#ifdef DEBUGB
// qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
    Q_EMIT emitMaxBar(mp->size() * manycol);

    for (int k = 0; k < manycol; k++) {
      // draw header
      if (k == (manycol - 1)) {
        QByteArray data = mp->at(0).fieldName(k).toUtf8();
        inf.write(data);
      } else {
        QByteArray data = mp->at(0).fieldName(k).toUtf8() + ";";
        inf.write(data);
      }
    }

    int prog = 0;
    for (int row = 0; row < mp->size(); row++) {
      for (int col = 0; col < manycol; col++) {
        bool iskolom_nomor = false;
        if (col == 0) {
          inf.write("\n");
        }
        QByteArray nomor = "";
        if (mp->at(row).fieldName(col) == "nomor") {
          iskolom_nomor = true;
          nomor = mp->at(row).value(col).toByteArray();
        }
        bool is_kolom_cluster = false;
        if (col == 4) {
          is_kolom_cluster = true;
        }
        switch (mp->at(row).value(col).type()) {
          case QVariant::Date:
            if (col == (manycol - 1)) {
              auto data = mp->at(row)
                              .value(col)
                              .toDate()
                              .toString("dd/MM/yyyy")
                              .toUtf8();
              inf.write(data);
            } else {
              auto data = mp->at(row)
                              .value(col)
                              .toDate()
                              .toString("dd/MM/yyyy")
                              .toUtf8() +
                          ";";
              inf.write(data);
            }
            break;
          case QVariant::Time:
            if (col == (manycol - 1)) {
              auto data =
                  mp->at(row).value(col).toTime().toString("h:mm:ss").toUtf8();
              inf.write(data);
            } else {
              auto data =
                  mp->at(row).value(col).toTime().toString("h:mm:ss").toUtf8() +
                  ";";
              inf.write(data);
            }
            break;
          default:
            if (col == (manycol - 1)) {
              if (!iskolom_nomor) {
                auto data = mp->at(row).value(col).toByteArray();
                inf.write(data);
              } else {
                if (use_62) {
                  nomor = nomor.replace(0, 1, "62");
                  inf.write(nomor);
                } else {
                  QByteArray data = mp->at(row).value(col).toByteArray();
                  if (col == 4) {
                    data = data.simplified();
                  }
                  inf.write(data);
                }
              }

            } else {
              if (!iskolom_nomor) {
                auto data = mp->at(row).value(col).toByteArray() + ";";
                if (col == 4) {
                  data = data.simplified();
                }
                if (col == 5) {
                  data = data.simplified();
                }
                inf.write(data);
              } else {
                if (use_62) {
                  nomor = nomor.replace(0, 1, "62");
                  nomor.append(";");
                  inf.write(nomor);
                } else {
                  auto data = mp->at(row).value(col).toByteArray() + ";";
                  inf.write(data);
                }
              }
            }
            break;
        }
        Q_EMIT emitValueBar(prog);
        prog++;
      }
    }
    inf.close();
  }

  manycol = 0;
  Q_EMIT result(ExportExcelResult::exportsuccess);
}

void ExportExcelWork::deleteDataById(const QString &id) {
  dbbase::SqlFetch *ch = new dbbase::SqlFetch();
  ch->setConfig(db_config);
  ch->open();
  QString sql =
      QString("delete from %1 where id= %2").arg(tabel_asliname).arg(id);
  // qDebug()<<"generated sql: "<<sql;
  ch->crdelalter(sql);
  if (!db_config["conname"].toString().isEmpty()) {
    QSqlDatabase::removeDatabase(db_config["conname"].toString());
  }
  delete ch;
}

void ExportExcelWork::openemptyandwrite1(const QString &fname,
                                         const QString &cell,
                                         const QByteArray &cnt) {
  QXlsx::Document xlsx(fname);
  QXlsx::Format fmt;
  //  fmt.setBorderStyle( QXlsx::Format::BorderMedium);
  xlsx.write(cell.toUtf8().constData(), cnt.constData(), fmt);
  xlsx.save();
}

void ExportExcelWork::openemptyandwriteheadertable(const QString &fname,
                                                   const QString &cell,
                                                   const QString &mergecell,
                                                   const QByteArray &cnt) {
  QXlsx::Document xlsx(fname);
  QXlsx::Format fmt;
//   fmt.setBorderStyle( QXlsx::Format::BorderMedium);
#ifdef DEBUGB
// qDebug()<<"mergecell"<<mergecell;
// qDebug()<<"cnt"<<cnt;
#endif

  fmt.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
  fmt.setVerticalAlignment(QXlsx::Format::AlignVCenter);
  xlsx.mergeCells(mergecell, fmt);
  xlsx.write(cell.toUtf8().constData(), cnt.constData());
  xlsx.save();
}

QString ExportExcelWork::whatcolumnlabel(int col) {
  QString str;
  int digits = 1;
  int offset = 0;
  col--;
  for (int limit = 26; col >= limit + offset; limit *= 26, digits++) {
    offset += limit;
  }
  for (int c = col - offset; digits; --digits, c /= 26) {
    str.prepend(QChar('A' + (c % 26)));
  }
  return str;
}

ExportExcelThread::ExportExcelThread(QObject *parent) : QObject(parent) {
  wk.reset(new ExportExcelWork);
  wk->moveToThread(&pth);
  connect(wk.data(), SIGNAL(emitMaxBar(int)), this, SIGNAL(emitMaxBar(int)));
  connect(wk.data(), SIGNAL(emitValueBar(int)), this,
          SIGNAL(emitValueBar(int)));
  connect(wk.data(), SIGNAL(result(ExportExcelResult)), this,
          SIGNAL(result(ExportExcelResult)));
  connect(this, SIGNAL(startdoing()), wk.data(), SLOT(doexporting()));
}

ExportExcelThread::~ExportExcelThread() {}

void ExportExcelThread::exportSetup(std::deque<QSqlRecord> *mrect,
                                    int howmanycol, const QString &fn,
                                    const QString &mtitle,
                                    const QStringList &colnm, bool use_62,
                                    bool is_mode_csv_or_excel) {
  wk->exportSetup(mrect, howmanycol, fn, mtitle, colnm, use_62,
                  is_mode_csv_or_excel);
}

void ExportExcelThread::setTableAsliName(const QString &tbl,
                                         const dbbase::configdb &mdb) {
  wk->setTableAsliName(tbl, mdb);
}

void ExportExcelThread::doexporting() { 
    if(!pth.isRunning ()){
        pth.start();   
    }
      this->startdoing ();                      
}

void ExportExcelThread::selesai() {
  if (pth.isRunning()) {
#ifdef DEBUGB
// qDebug() << Q_FUNC_INFO << "called";
#endif
    pth.requestInterruption();
    pth.quit();
  }
}
