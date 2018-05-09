#include "formtableview.h"
#include "excel/exportwizard.h"
#include "excel/importwizard.h"
#include "ui_formtableview.h"

FormTableView::FormTableView(QWidget *parent, const tableviewcommon &settbl)
    : QWidget(parent), ui(new Ui::FormTableView) {
  ui->setupUi(this);
modejointabel=std::make_pair(false,QLatin1Literal(""));
  qRegisterMetaType<ExportExcelResult>("ExportExcelResult");
  qRegisterMetaType<ImportExcelResult>("ImportExcelResult");
  qRegisterMetaType<DataImportExcel>("DataImportExcel");
  m_model = new SqlModel(this);
  selectmodel = new QItemSelectionModel(m_model);
  connect(selectmodel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
          SLOT(itemselectchanged(QItemSelection, QItemSelection)));
  if (settbl.isset() && settbl.sql().isset()) {
    m_model->setHorizontalHeader(settbl.headertabel());
    m_model->settabeltruetoya(settbl.pairtrueorya());
    limited = settbl.limitpage();
    configdb = settbl.config();
    m_sqlobject = settbl.sql();
    m_model->setAlternateRowColor(settbl.alternatetabelrowcolor());
    if (settbl.isusetextcolorforcolumn()) {
      m_model->setcolortext(settbl.textcolor().first,
                            settbl.textcolor().second);
    }
    set = true;
    ui->tableView->setready(true, settbl.access());
  }
  hdrimport.clear();
  connect(ui->tableView, SIGNAL(emitadd()), SLOT(thisadd()));
  connect(ui->tableView, SIGNAL(emitedit(QString)), SLOT(thisedit(QString)));
  connect(ui->tableView, SIGNAL(emithapus(QString)), SLOT(thisdelete(QString)));
  connect(ui->tableView, SIGNAL(refreshing()), SLOT(thisrefresh()));

  connect(ui->tableView, SIGNAL(activated(QModelIndex)),
          SLOT(tablerowselectmoved(QModelIndex)));
  connect(ui->tableView, SIGNAL(emitexportexcel()), SLOT(doexportexcel()));
  connect(ui->tableView, SIGNAL(emitimportexcel()), SLOT(doimportexcel()));
}

FormTableView::~FormTableView() {
  selectmodel->deleteLater();
  m_model->deleteLater();

  delete ui;
}

void FormTableView::setfilter(const std::pair<int, QString> &data) {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  
  modefilterset = 1;
  modefilter();
  filterdata = data;
  implcountrecord();
  implfetch();
  // TODO here we will add more custom datatype editing to support equal
  // operator =, instead of like one.
}

void FormTableView::setfiltervariant(const std::pair<int, QVariant> &data) {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  
  modefilterset = 2;
  modefilter();
  filterdatavariant = data;
  implcountrecord();
  implfetch();
}

void FormTableView::setfilternone(const std::pair<int, QVariant> &data) {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }

  modefilterset = 3;
  modefilter();
  filterdatavariant = data;
  implcountrecord();
  implfetch();
}

void FormTableView::setfilterunik1(const Param2LikeAndEqual &data)
{
    if (!set) {
      qDebug() << "FATAL internal setting belum diset.";
      return;
    }
    unik2filter1=data;
    modefilterset = 4;
    modefilter();
    implcountrecord();
    implfetch();
}

void FormTableView::setfilterunik2(const Param2VariantAndVariant &data)
{
    if (!set) {
      qDebug() << "FATAL internal setting belum diset.";
      return;
    }
    unik2filter2=data;
    modefilterset =5;
    modefilter();
    implcountrecord();
    implfetch();
}

void FormTableView::settableviewsetting(const tableviewcommon &setting) {
  m_model->setHorizontalHeader(setting.headertabel());
  m_model->settabeltruetoya(setting.pairtrueorya());
  limited = setting.limitpage();
  configdb = setting.config();
  m_sqlobject = setting.sql();
  m_model->setAlternateRowColor(setting.alternatetabelrowcolor());
  set = true;
  ui->tableView->setready(true, setting.access());
  sethowmanyexportcol(setting.headertabel().size());
}

void FormTableView::sethowmanyexportcol(int n) { howmanyexportcol = n; }

QString FormTableView::getcurrentselectionID()
{
  return ui->tableView->getcurrentselectionID();
}

void FormTableView::setimportexcelVisibleTableInfo(
    const QString &nm,
    const std::vector<std::tuple<int, bool, QVariant> > &ignorecolumnscons,
    const QStringList &hd) {
  tblname = nm;
  ignorecolumnsimportexcel = ignorecolumnscons;
  hdrimport = hd;
}

void FormTableView::setNamedKategoriExport(const QString &kategori_export)
{
    name_kategori_export=kategori_export;
}

void FormTableView::appendListMenu(QList<customslottable> *acts)
{
ui->tableView->appendListMenu(acts);
}

void FormTableView::changeEvent(QEvent *e) {
  QWidget::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void FormTableView::closeEvent(QCloseEvent *event) { event->accept(); }

void FormTableView::tablerowselectmoved(const QModelIndex &index) {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }

#ifdef DEBUGB
  qDebug() << "tablerowselectmoved triggered " << __FILE__ << ":" << __LINE__
           << "index.row: " << index.row()
           << " index column: " << index.column()
           << "isvalid:" << index.isValid();
  if (index.isValid()) {  // guards against invalid value index
    qDebug() << m_model->rowidbyvalue(index.row());
  }
#endif
  if (index.isValid()) {
    Q_EMIT onaktifcolumnid(m_model->rowidbyvalue(index.row()).toString());
  }
}

void FormTableView::implfetch() {
 qDebug()<<Q_FUNC_INFO<<"mode"<<mode;
  if (mode == 1) {
    utilproduk::sqlmodelwork brg;
    utilproduk::selectdatahelper ret = brg.selectReady(
        configdb, m_sqlobject.selectsql(), qMakePair(0, limited));

    if (!ret.data().first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                "mengetahui penyebabnya tersebut."),
          ret.message());
      qDebug()<<" m_sqlobject.selectsql():"<< m_sqlobject.selectsql();
      return;
    }
    m_model->clear();
    m_model->setRecordList(ret.data().second);
    if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
      qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
    }
    ui->tableView->setModel(m_model);

    ui->tableView->setSelectionModel(selectmodel);
    if (total_pagelist_select > 1) {
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_leftmost->setEnabled(false);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_rightmost->setEnabled(true);
    } else {
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_leftmost->setEnabled(false);
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_rightmost->setEnabled(false);
    }
  }
  if (mode == 2) {
    if (modefilterset == 1) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      qDebug()<<"fetching debug like"<<std::get<2>(m_sqlobject.filtergrp().at(filterdata.first));
      utilproduk::selectdatahelper ret = edit.filterbysinglegroupReady(
          configdb, std::get<2>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second, qMakePair(0, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      if (total_pagelist_filter > 1) {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
      } else {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
      }
    }
    if (modefilterset == 2) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterequalbysinglegroupReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second, qMakePair(0, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      if (total_pagelist_filter > 1) {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
      } else {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
      }
    }

    if (modefilterset == 3) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat internal sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }

      utilproduk::selectdatahelper ret = edit.selectReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          qMakePair(0, limited), false);
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      if (total_pagelist_filter > 1) {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
      } else {
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
      }
    }
  
    if(modefilterset==4){
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupStringAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter1.index)),unik2filter1.like,
            unik2filter1.equal, qMakePair(0, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        
        qDebug() << "ret.data ().second.size ()" << ret.data ().second.size ();
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
        if (total_pagelist_filter > 1) {
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(true);
        } else {
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
        }
    }
  
    if(modefilterset==5){
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupVariantAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter2.index)),unik2filter2.filter1,
            unik2filter2.filter2, qMakePair(0, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        
        qDebug() << "ret.data ().second.size ()" << ret.data ().second.size ();
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
        if (total_pagelist_filter > 1) {
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(true);
        } else {
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
        }
    }
  }
}

void FormTableView::implcountrecord() {
  if (mode == 1) {
    utilproduk::sqlmodelwork countrec;

    utilproduk::selectcounthelper ret;
    if (!modejointabel.first) {
      ret = countrec.countrecordReady(configdb, m_sqlobject.selectcountsql());
    } else {
      // modejointabel
      ret = countrec.countrerordjoinReady(configdb, modejointabel.second);
    }
    if (!ret.validity()) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat ketika transaksi database, klik "
                                "selengkapnya untuk keterangan detailnya."),
          ret.message());
      setEnabled(false);
      return;
    }
    m_rowcountselect = ret.countrecord();
    savedcountselect=m_rowcountselect;
    int a = m_rowcountselect % 2;
    if (a == 0) a += 1;
    qint64 b = m_rowcountselect - a;
    total_pagelist_select = b / limited + a;    

    ui->toolButton_leftmost->setEnabled(false);
    ui->toolButton_left->setEnabled(false);
    if (total_pagelist_select < 2) {
      ui->toolButton_rightmost->setEnabled(false);
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_leftmost->setEnabled(false);
    }
    if (total_pagelist_select > 1) {
      ui->toolButton_rightmost->setEnabled(true);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_leftmost->setEnabled(false);
    }
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
             << total_pagelist_select;
#endif
    arrayselectpage.clear();
    arrayselectpage.resize(total_pagelist_select);
    genvalidpage(arrayselectpage.begin(), arrayselectpage.end(), 0, limited);

    ui->label_infohalaman->setText(labelpage.arg(1).arg(total_pagelist_select).arg (m_rowcountselect));
  }
  if (mode == 2) {
    if (modefilterset == 1) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectcounthelper ret = edit.countfilterrecordReady(
          configdb, std::get<1>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second);

      if (!ret.validity()) {
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Terjadi galat ketika transaksi database, klik selengkapnya "
               "untuk keterangan detailnya."),
            ret.message());
        setEnabled(false);
        return;
      }

      m_rowcountfilter = ret.countrecord();
      savedcountfilter=m_rowcountfilter;
      int a = m_rowcountfilter % 2;
      if (a == 0) a += 1;
      qint64 b = m_rowcountfilter - a;
      total_pagelist_filter = b / limited + a;
      ui->toolButton_left->setEnabled(false);
      //        ui.toolButton_backward->setEnabled(false);
      if (total_pagelist_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
      }
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);

#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
      }
      arrayfilterpage.clear();
      arrayfilterpage.resize(total_pagelist_filter);
      genvalidpage(arrayfilterpage.begin(), arrayfilterpage.end(), 0, limited);
      ui->label_infohalaman->setText(
          labelpage.arg(1).arg(total_pagelist_filter).arg (m_rowcountfilter));
    }
    if (modefilterset == 2) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectcounthelper ret = edit.countequalfilterrecordReady(
          configdb,
          std::get<1>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second);

      if (!ret.validity()) {
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Terjadi galat ketika transaksi database, klik selengkapnya "
               "untuk keterangan detailnya."),
            ret.message());
        setEnabled(false);
        return;
      }

      m_rowcountfilter = ret.countrecord();
      savedcountfilter=m_rowcountfilter;
      int a = m_rowcountfilter % 2;
      if (a == 0) a += 1;
      qint64 b = m_rowcountfilter - a;
      total_pagelist_filter = b / limited + a;
      ui->toolButton_left->setEnabled(false);
      //        ui.toolButton_backward->setEnabled(false);
      
      qDebug()<<"m_rowcountfilter:"<<m_rowcountfilter<<"total_pagelist_filter"<< total_pagelist_filter;
      if (total_pagelist_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
      }
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
      }
      arrayfilterpage.clear();
      arrayfilterpage.resize(total_pagelist_filter);
      genvalidpage(arrayfilterpage.begin(), arrayfilterpage.end(), 0, limited);
      ui->label_infohalaman->setText(
          labelpage.arg(1).arg(total_pagelist_filter).arg (m_rowcountfilter));
    }

    if (modefilterset == 3) {
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }

      utilproduk::selectcounthelper ret = edit.countequalfilternonerecordReady(
          configdb,
          std::get<1>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          false);

      if (!ret.validity()) {
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Terjadi galat ketika transaksi database, klik selengkapnya "
               "untuk keterangan detailnya."),
            ret.message());
        setEnabled(false);
        return;
      }

      m_rowcountfilter = ret.countrecord();
      savedcountfilter=m_rowcountfilter;
      int a = m_rowcountfilter % 2;
      if (a == 0) a += 1;
      qint64 b = m_rowcountfilter - a;
      total_pagelist_filter = b / limited + a;
      ui->toolButton_left->setEnabled(false);
      //        ui.toolButton_backward->setEnabled(false);
      if (total_pagelist_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(true);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
      }
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                 << total_pagelist_filter;
#endif
      }
      arrayfilterpage.clear();
      arrayfilterpage.resize(total_pagelist_filter);
      genvalidpage(arrayfilterpage.begin(), arrayfilterpage.end(), 0, limited);
      ui->label_infohalaman->setText(
          labelpage.arg(1).arg(total_pagelist_filter).arg (m_rowcountfilter));
    }
  
    if(modefilterset==4){
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectcounthelper ret = edit.countUnik2LikeStringAndVariantfilterrecordReady (
            configdb,
            std::get<1>(m_sqlobject.filtergrp().at(unik2filter1.index)),
            unik2filter1.like,unik2filter1.equal);
  
        if (!ret.validity()) {
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Terjadi galat ketika transaksi database, klik selengkapnya "
                 "untuk keterangan detailnya."),
              ret.message());
          setEnabled(false);
          return;
        }
  
        m_rowcountfilter = ret.countrecord();
        savedcountfilter=m_rowcountfilter;
        int a = m_rowcountfilter % 2;
        if (a == 0) a += 1;
        qint64 b = m_rowcountfilter - a;
        total_pagelist_filter = b / limited + a;
        ui->toolButton_left->setEnabled(false);
        //        ui.toolButton_backward->setEnabled(false);
        if (total_pagelist_filter > 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                   << total_pagelist_filter;
  #endif
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(true);
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
        }
        if (total_pagelist_filter < 2) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                   << total_pagelist_filter;
  #endif
        }
        arrayfilterpage.clear();
        arrayfilterpage.resize(total_pagelist_filter);
        genvalidpage(arrayfilterpage.begin(), arrayfilterpage.end(), 0, limited);
        ui->label_infohalaman->setText(
            labelpage.arg(1).arg(total_pagelist_filter).arg (savedcountfilter));
        
    }
  
    if(modefilterset==5){
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectcounthelper ret = edit.countUnik2VariantAndVariantfilterrecordReady (
            configdb,
            std::get<1>(m_sqlobject.filtergrp().at(unik2filter2.index)),
            unik2filter2.filter1,unik2filter2.filter2);
  
        if (!ret.validity()) {
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Terjadi galat ketika transaksi database, klik selengkapnya "
                 "untuk keterangan detailnya."),
              ret.message());
          setEnabled(false);
          return;
        }
  
        m_rowcountfilter = ret.countrecord();
        savedcountfilter=m_rowcountfilter;
        int a = m_rowcountfilter % 2;
        if (a == 0) a += 1;
        qint64 b = m_rowcountfilter - a;
        total_pagelist_filter = b / limited + a;
        ui->toolButton_left->setEnabled(false);
        //        ui.toolButton_backward->setEnabled(false);
        if (total_pagelist_filter > 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                   << total_pagelist_filter;
  #endif
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(true);
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
        }
        if (total_pagelist_filter < 2) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__ << " "
                   << total_pagelist_filter;
  #endif
        }
        arrayfilterpage.clear();
        arrayfilterpage.resize(total_pagelist_filter);
        genvalidpage(arrayfilterpage.begin(), arrayfilterpage.end(), 0, limited);
        ui->label_infohalaman->setText(
            labelpage.arg(1).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
  }
}

void FormTableView::thisadd() {
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  Q_EMIT adddata();
}

void FormTableView::thisrefresh() {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  modeselect();
  implcountrecord();
  implfetch();
}

void FormTableView::importexcelwork(const DataImportExcel &in) {
  if (m_model == nullptr) {
    return;
  }
  if (m_model->headerdatas().size() == 0) {
    return;
  }

  connect(m_importexcelthread.data(), SIGNAL(maxbarimportexcel(int)), this,
          SIGNAL(maxbarimportexcel(int)));
  connect(m_importexcelthread.data(), SIGNAL(valuebarimportexcel(int)), this,
          SIGNAL(valuebarimportexcel(int)));
  connect(m_importexcelthread.data(), &ImportExcelThread::resultimportexcel,
          this, &FormTableView::resultimportexcel);

  m_importexcelthread->importsetup(in);
  m_importexcelthread->doimport();
}

void FormTableView::exportexcelwork(
                                    const QString &tbltitle,bool use_62,bool is_mode_csv_or_excel) {
  if (m_model == nullptr) {
    // guard dont do anything
    return;
  }
  if (m_model->headerdatas().size() == 0) {
    return;
  }
  
  this->beforeExportExcelHappen ();
  connect(m_exportexcelthread.data(), SIGNAL(emitMaxBar(int)), this,
          SIGNAL(maxbarexportexcel(int)));
  connect(m_exportexcelthread.data(), SIGNAL(emitValueBar(int)), this,
          SIGNAL(valuebarexportexcel(int)));
  connect(m_exportexcelthread.data(), SIGNAL(result(ExportExcelResult)), this,
          SIGNAL(resultexportexcel(ExportExcelResult)));
  
  m_exportexcelthread->setTableAsliName(m_sqlobject.selectcountsql (),configdb);
  m_exportexcelthread->exportSetup(m_model->internalrecord(), howmanyexportcol,
                                   name_kategori_export, tbltitle, m_model->headerdatas(),use_62,is_mode_csv_or_excel);
  m_exportexcelthread->doexporting();
}

void FormTableView::doimportexcel() {
  if (m_model == nullptr) {
    return;
  }
  if (m_model->headerdatas().size() == 0) {
    return;
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  // determine import type from m_sqlobject.selectcountsql()
  // note that we only interested on how many m_model->headerdatas().size()
  // no more than that.
  utilproduk::sqlmodelwork wk1;
  QSqlRecord ret = wk1.typecolumnReady(configdb, m_sqlobject.selectcountsql());

  // haruse ret count()>m_model->headerdatas().size()
  // atau ret count()==m_model->headerdatas().size()s
  if (ret.count() < m_model->headerdatas().size()) {
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug() << "internal setup error tableview doimportexcel 1";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    return;
  }
  if (ret.count() != hdrimport.size()) {
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug() << "internal setup error tableview doimportexcel 2";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug() << "ret.count()" << ret.count() << "hdrimport.size()"
             << hdrimport.size();
    return;
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif

  ImportWizard *ep = new ImportWizard;
  m_importexcelthread.reset(new ImportExcelThread);
  DataImportExcel4SetupWizard wz;

  wz.ignorecolumns = ignorecolumnsimportexcel;
  std::vector<QPair<QString, QVariant::Type> > colnamesandtypes;

  for (int hd = 0; hd < ret.count(); hd++) {
    colnamesandtypes.push_back(
        qMakePair(hdrimport.at(hd), ret.field(hd).type()));
  }

  // signature std::deque<std::tuple<int,bool,QVariant> > ignorecolumns;
  wz.tblsqlname = m_sqlobject.selectcountsql();
  wz.infologin = configdb;
  wz.colnamesandtypes = colnamesandtypes;
  colnamesandtypes.clear();

  if (tblname.isEmpty()) {
    wz.tblname = m_sqlobject.selectcountsql();
    ep->setTableinfos(wz);
  } else {
    wz.tblname = tblname;
    ep->setTableinfos(wz);
  }
  connect(m_importexcelthread.data(), &ImportExcelThread::setmsgcontent, ep,
          &ImportWizard::setmsgcontent);
  connect(m_importexcelthread.data(), &ImportExcelThread::startdoing, ep,
          &ImportWizard::showmsgbar);
  connect(ep, &ImportWizard::requestcleanupthread, m_importexcelthread.data(),
          &ImportExcelThread::selesai);
  connect(ep, &ImportWizard::requestcleanupthread, this,
          &FormTableView::thisrefresh);
  connect(ep, &ImportWizard::importexcelwork, this,
          &FormTableView::importexcelwork);
  connect(this, &FormTableView::maxbarimportexcel, ep,
          &ImportWizard::maxbarimportexcel);
  connect(this, &FormTableView::valuebarimportexcel, ep,
          &ImportWizard::valuebarimportexcel);
  connect(this, &FormTableView::resultimportexcel, ep,
          &ImportWizard::resultimportexcel);

  ep->setAttribute(Qt::WA_DeleteOnClose);
  ep->exec();
}

void FormTableView::doexportexcel() {
  ExportWizard *ep = new ExportWizard;

  m_exportexcelthread.reset(new ExportExcelThread);
  connect(m_exportexcelthread.data(), SIGNAL(startdoing()), ep,
          SLOT(showmsgbar()));
  connect(ep, &ExportWizard::exportexcel, this,
          &FormTableView::exportexcelwork);
  connect(this, &FormTableView::maxbarexportexcel, ep,
          &ExportWizard::maxbarexportexcel);
  connect(this, &FormTableView::valuebarexportexcel, ep,
          &ExportWizard::valuebarexportexcel);
  connect(this, &FormTableView::resultexportexcel, ep,
          &ExportWizard::resultexportexcel);
  connect(ep, SIGNAL(requestcleanupthread()), m_exportexcelthread.data(),
          SLOT(selesai()));
//  connect(ep, &ExportWizard::requestcleanupthread, this,
//          &FormTableView::thisrefresh);

  ep->setAttribute(Qt::WA_DeleteOnClose);
  ep->exec();
}

void FormTableView::itemselectchanged(const QItemSelection &newitem,
                                      const QItemSelection &oldone) {
  Q_UNUSED(oldone)
  if (!newitem.indexes().isEmpty() && newitem.indexes().size() > 1) {
#ifdef DEBUGB
    qDebug() << "!newitem.indexes().isEmpty(), size: "
             << newitem.indexes().size() << " " << __FILE__ << ":" << __LINE__;
    qDebug() << newitem.indexes().at(0).data();

#endif
    QModelIndex data = newitem.indexes().at(0);
    if (data.isValid()) {
      Q_EMIT showimgbyid(data.data().toString());
    }
  } else {
#ifdef DEBUGB
    qDebug() << "empty newitem indexes..." << __FILE__ << ":" << __LINE__;
#endif
  }
}

void FormTableView::thisedit(const QString &id) {
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__;
#endif
  Q_EMIT editdata(id);
}

void FormTableView::thisdelete(const QString &id) {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  Q_EMIT deletedata(id);
}

void FormTableView::on_toolButton_leftmost_clicked() {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  if (mode == 1) {
    if (!ui->toolButton_rightmost->isEnabled()) {
      ui->toolButton_rightmost->setEnabled(true);
    }
    if (backward_select == 1) {
#ifdef DEBUGB
      qDebug() << "\n"
               << __FILE__ << __LINE__ << "forward_select" << forward_select
               << "backward_select" << backward_select;
#endif
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_leftmost->setEnabled(false);
      return;
    }
    limitpre_select = 1;
    utilproduk::sqlmodelwork brg;
    utilproduk::selectdatahelper ret = brg.selectReady(
        configdb, m_sqlobject.selectsql(), qMakePair(0, limited));

    if (!ret.data().first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                "mengetahui penyebabnya tersebut."),
          ret.message());
      return;
    }
    m_model->clear();
    m_model->setRecordList(ret.data().second);
    if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
      qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
    }
    ui->tableView->setModel(m_model);

    ui->tableView->setSelectionModel(selectmodel);
    forward_select = backward_select = 1;
    ui->toolButton_left->setEnabled(false);
    ui->toolButton_right->setEnabled(true);
    ui->toolButton_leftmost->setEnabled(false);
    ui->label_infohalaman->setText(labelpage.arg(1).arg(total_pagelist_select).arg (savedcountselect));
  }

  if (mode == 2) {
    if (modefilterset == 1) {
      if (!ui->toolButton_rightmost->isEnabled()) {
        ui->toolButton_rightmost->setEnabled(true);
      }
      if (backward_filter == 1) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = 1;
      limitpre_filter = 1;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterbysinglegroupReady(
          configdb, std::get<2>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second, qMakePair(0, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

      forward_filter = backward_filter = 1;
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_leftmost->setEnabled(false);
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    if (modefilterset == 2) {
      if (!ui->toolButton_rightmost->isEnabled()) {
        ui->toolButton_rightmost->setEnabled(true);
      }
      if (backward_filter == 1) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = 1;
      limitpre_filter = 1;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterequalbysinglegroupReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second, qMakePair(0, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      forward_filter = backward_filter = 1;
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_leftmost->setEnabled(false);
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }

    if (modefilterset == 3) {
      if (!ui->toolButton_rightmost->isEnabled()) {
        ui->toolButton_rightmost->setEnabled(true);
      }
      if (backward_filter == 1) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = 1;
      limitpre_filter = 1;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.selectReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          qMakePair(0, limited), false);
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      forward_filter = backward_filter = 1;
      ui->toolButton_left->setEnabled(false);
      ui->toolButton_right->setEnabled(true);
      ui->toolButton_leftmost->setEnabled(false);
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
    if(modefilterset==4){
        if (!ui->toolButton_rightmost->isEnabled()) {
          ui->toolButton_rightmost->setEnabled(true);
        }
        if (backward_filter == 1) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                   << "backward_filter" << backward_filter;
  #endif
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_leftmost->setEnabled(false);
          return;
        }
        backward_filter = forward_filter = 1;
        limitpre_filter = 1;
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectdatahelper ret = edit.filterequalby2groupStringAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter1.index)),unik2filter1.like,
            unik2filter1.equal, qMakePair(0, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
        ui->tableView->setSelectionModel(selectmodel);
        forward_filter = backward_filter = 1;
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_leftmost->setEnabled(false);
        ui->label_infohalaman->setText(
            labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
    if(modefilterset==5){
        if (!ui->toolButton_rightmost->isEnabled()) {
          ui->toolButton_rightmost->setEnabled(true);
        }
        if (backward_filter == 1) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                   << "backward_filter" << backward_filter;
  #endif
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_right->setEnabled(true);
          ui->toolButton_leftmost->setEnabled(false);
          return;
        }
        backward_filter = forward_filter = 1;
        limitpre_filter = 1;
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectdatahelper ret = edit.filterequalby2groupVariantAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter2.index)),unik2filter2.filter1,
            unik2filter2.filter2, qMakePair(0, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
        ui->tableView->setSelectionModel(selectmodel);
        forward_filter = backward_filter = 1;
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_right->setEnabled(true);
        ui->toolButton_leftmost->setEnabled(false);
        ui->label_infohalaman->setText(
            labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
  }
}

void FormTableView::on_toolButton_left_clicked() {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  if (mode == 1) {
    if (backward_select < 2) {
      // select();
      ui->toolButton_leftmost->setEnabled(false);
      ui->toolButton_left->setEnabled(false);
      return;
    }
    if (backward_select > 1) {
      ui->toolButton_right->setEnabled(true);
    }
    ui->toolButton_left->setEnabled(true);
    backward_select--;
    forward_select--;
    limitpre_select = limitpre_select - limited;
    // todo handle here if limitpre_select is negative value
    if (limitpre_select < 0) {
      ui->toolButton_left->setEnabled(false);
      return;
    }
    utilproduk::sqlmodelwork brg;
    utilproduk::selectdatahelper ret = brg.selectReady(
        configdb, m_sqlobject.selectsql(),
        qMakePair(arrayselectpage.at(backward_select - 1), limited));

    if (!ret.data().first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                "mengetahui penyebabnya tersebut."),
          ret.message());
      return;
    }
    m_model->clear();
    m_model->setRecordList(ret.data().second);
    if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
      qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
    }
    ui->tableView->setModel(m_model);

    ui->tableView->setSelectionModel(selectmodel);
    ui->toolButton_rightmost->setEnabled(true);
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
             << " forward_filter: " << forward_filter
             << " backward_filter: " << backward_filter;
#endif

    ui->label_infohalaman->setText(
        labelpage.arg(backward_select).arg(total_pagelist_select).arg (savedcountselect));
  }
  if (mode == 2) {
    if (modefilterset == 1) {
      if (backward_filter <= 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      if (backward_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_right->setEnabled(true);
      }
      ui->toolButton_left->setEnabled(true);
      backward_filter--;
      forward_filter--;
      limitpre_filter = limitpre_filter - limited;
      // handle limitpre_filter if negative value <0
      if (limitpre_filter < 0) {
        ui->toolButton_left->setEnabled(false);

        return;
      }

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterbysinglegroupReady(
          configdb, std::get<2>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second,
          qMakePair(arrayfilterpage.at(backward_filter - 1), limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
               << " forward_filter: " << forward_filter
               << " backward_filter: " << backward_filter;
#endif
      ui->toolButton_rightmost->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(backward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    if (modefilterset == 2) {
      if (backward_filter <= 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      if (backward_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_right->setEnabled(true);
      }
      ui->toolButton_left->setEnabled(true);
      backward_filter--;
      forward_filter--;
      limitpre_filter = limitpre_filter - limited;
      // handle limitpre_filter if negative value <0
      if (limitpre_filter < 0) {
        ui->toolButton_left->setEnabled(false);

        return;
      }

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterequalbysinglegroupReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second,
          qMakePair(arrayfilterpage.at(backward_filter - 1), limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
               << " forward_filter: " << forward_filter
               << " backward_filter: " << backward_filter;
#endif
      ui->toolButton_rightmost->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(backward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }

    if (modefilterset == 3) {
      if (backward_filter <= 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_left->setEnabled(false);
        ui->toolButton_leftmost->setEnabled(false);
        return;
      }
      if (backward_filter > 1) {
#ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
#endif
        ui->toolButton_right->setEnabled(true);
      }
      ui->toolButton_left->setEnabled(true);
      backward_filter--;
      forward_filter--;
      limitpre_filter = limitpre_filter - limited;
      // handle limitpre_filter if negative value <0
      if (limitpre_filter < 0) {
        ui->toolButton_left->setEnabled(false);

        return;
      }

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.selectReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          qMakePair(arrayfilterpage.at(backward_filter - 1), limited), false);
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
               << " forward_filter: " << forward_filter
               << " backward_filter: " << backward_filter;
#endif
      ui->toolButton_rightmost->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(backward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
  
    
    if(modefilterset==4){
        if (backward_filter <= 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                   << " forward_filter: " << forward_filter
                   << " backward_filter: " << backward_filter;
  #endif
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          return;
        }
        if (backward_filter > 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                   << " forward_filter: " << forward_filter
                   << " backward_filter: " << backward_filter;
  #endif
          ui->toolButton_right->setEnabled(true);
        }
        ui->toolButton_left->setEnabled(true);
        backward_filter--;
        forward_filter--;
        limitpre_filter = limitpre_filter - limited;
        // handle limitpre_filter if negative value <0
        if (limitpre_filter < 0) {
          ui->toolButton_left->setEnabled(false);
  
          return;
        }
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectdatahelper ret = edit.filterequalby2groupStringAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter1.index)),unik2filter1.like,
            unik2filter1.equal, qMakePair(arrayfilterpage.at(backward_filter - 1), limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
  #endif
        ui->toolButton_rightmost->setEnabled(true);
        ui->label_infohalaman->setText(
            labelpage.arg(backward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
    if(modefilterset==5){
        if (backward_filter <= 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                   << " forward_filter: " << forward_filter
                   << " backward_filter: " << backward_filter;
  #endif
          ui->toolButton_left->setEnabled(false);
          ui->toolButton_leftmost->setEnabled(false);
          return;
        }
        if (backward_filter > 1) {
  #ifdef DEBUGB
          qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                   << " forward_filter: " << forward_filter
                   << " backward_filter: " << backward_filter;
  #endif
          ui->toolButton_right->setEnabled(true);
        }
        ui->toolButton_left->setEnabled(true);
        backward_filter--;
        forward_filter--;
        limitpre_filter = limitpre_filter - limited;
        // handle limitpre_filter if negative value <0
        if (limitpre_filter < 0) {
          ui->toolButton_left->setEnabled(false);
  
          return;
        }
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        
        utilproduk::selectdatahelper ret = edit.filterequalby2groupVariantAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter2.index)),unik2filter2.filter1,
            unik2filter2.filter2, qMakePair(arrayfilterpage.at(backward_filter - 1), limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
                 << " forward_filter: " << forward_filter
                 << " backward_filter: " << backward_filter;
  #endif
        ui->toolButton_rightmost->setEnabled(true);
        ui->label_infohalaman->setText(
            labelpage.arg(backward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
        
    }
    
  
  }
}

void FormTableView::on_toolButton_right_clicked() {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  if (mode == 1) {
    if (forward_select == total_pagelist_select) {
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
               << " forward_select: " << forward_select
               << " backward_select: " << backward_select
               << " pagelist:" << total_pagelist_select;
#endif
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_rightmost->setEnabled(false);
      return;  // we reach forward_select limit
    }
    ui->toolButton_right->setEnabled(true);
    forward_select++;
    backward_select++;
    if (forward_select > 1) {
      ui->toolButton_left->setEnabled(true);
    }
    if (backward_select > 1) {
      ui->toolButton_right->setEnabled(true);
    }
    limitpre_select = limitpre_select + limited;  // move 6, 11, dst ..
    // we know on how many pages to move forward_select and backward_select,
    // when we reach the limit. disable move forward_select button.
    utilproduk::sqlmodelwork brg;
    utilproduk::selectdatahelper ret = brg.selectReady(
        configdb, m_sqlobject.selectsql(),
        qMakePair(arrayselectpage.at((int)forward_select - 1), limited));

    if (!ret.data().first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                "mengetahui penyebabnya tersebut."),
          ret.message());
      return;
    }
    m_model->clear();
    m_model->setRecordList(ret.data().second);
    if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
      qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
    }
    ui->tableView->setModel(m_model);

    ui->tableView->setSelectionModel(selectmodel);
#ifdef DEBUGB
    qDebug() << Q_FUNC_INFO << __FILE__ << __LINE__
             << " forward_select: " << forward_select
             << " backward_select: " << backward_select;
#endif
    ui->toolButton_leftmost->setEnabled(true);

    ui->label_infohalaman->setText(
        labelpage.arg(forward_select).arg(total_pagelist_select).arg (savedcountselect));
  }
  if (mode == 2) {
    if (modefilterset == 1) {
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  //
      }
      if (forward_filter == total_pagelist_filter) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  // we reach forward_filter limit
      }
      ui->toolButton_right->setEnabled(true);
      forward_filter++;
      backward_filter++;
      if (forward_filter > 1) {
        ui->toolButton_left->setEnabled(true);
      }
      if (backward_filter > 1) {
        ui->toolButton_right->setEnabled(true);
      }
      limitpre_filter = limitpre_filter + limited;
      // check if limitpre_filter < 0
      if (limitpre_filter < 0) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__
                 << "limitpre_filter:" << limitpre_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        return;
      }
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterbysinglegroupReady(
          configdb, std::get<2>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second,
          qMakePair(arrayfilterpage.at((int)forward_filter - 1), limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);
      ui->tableView->setSelectionModel(selectmodel);

      ui->toolButton_leftmost->setEnabled(true);
#ifdef DEBUGB
      qDebug() << __FILE__ << __LINE__ << "backward_filter:" << backward_filter
               << "forward_filter:" << forward_filter;

#endif
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    if (modefilterset == 2) {
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  //
      }
      if (forward_filter == total_pagelist_filter) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  // we reach forward_filter limit
      }
      ui->toolButton_right->setEnabled(true);
      forward_filter++;
      backward_filter++;
      if (forward_filter > 1) {
        ui->toolButton_left->setEnabled(true);
      }
      if (backward_filter > 1) {
        ui->toolButton_right->setEnabled(true);
      }
      limitpre_filter = limitpre_filter + limited;
      // check if limitpre_filter < 0
      if (limitpre_filter < 0) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__
                 << "limitpre_filter:" << limitpre_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        return;
      }
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterequalbysinglegroupReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second,
          qMakePair(arrayfilterpage.at((int)forward_filter - 1), limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

      ui->toolButton_leftmost->setEnabled(true);
#ifdef DEBUGB
      qDebug() << __FILE__ << __LINE__ << "backward_filter:" << backward_filter
               << "forward_filter:" << forward_filter;

#endif
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }

    if (modefilterset == 3) {
      if (total_pagelist_filter < 2) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  //
      }
      if (forward_filter == total_pagelist_filter) {
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        return;  // we reach forward_filter limit
      }
      ui->toolButton_right->setEnabled(true);
      forward_filter++;
      backward_filter++;
      if (forward_filter > 1) {
        ui->toolButton_left->setEnabled(true);
      }
      if (backward_filter > 1) {
        ui->toolButton_right->setEnabled(true);
      }
      limitpre_filter = limitpre_filter + limited;
      // check if limitpre_filter < 0
      if (limitpre_filter < 0) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__
                 << "limitpre_filter:" << limitpre_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        return;
      }
      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Galat sistem error index lookup sql filter "
                                  "tidak sama. Mohon kontak customer "
                                  "service aplikasi vendor ini."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.selectReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          qMakePair(arrayfilterpage.at((int)forward_filter - 1), limited),
          false);
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

      ui->toolButton_leftmost->setEnabled(true);
#ifdef DEBUGB
      qDebug() << __FILE__ << __LINE__ << "backward_filter:" << backward_filter
               << "forward_filter:" << forward_filter;

#endif
      ui->label_infohalaman->setText(
          labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
 
    if(modefilterset==4){
        if (total_pagelist_filter < 2) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          return;  //
        }
        if (forward_filter == total_pagelist_filter) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          return;  // we reach forward_filter limit
        }
        ui->toolButton_right->setEnabled(true);
        forward_filter++;
        backward_filter++;
        if (forward_filter > 1) {
          ui->toolButton_left->setEnabled(true);
        }
        if (backward_filter > 1) {
          ui->toolButton_right->setEnabled(true);
        }
        limitpre_filter = limitpre_filter + limited;
        // check if limitpre_filter < 0
        if (limitpre_filter < 0) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__
                   << "limitpre_filter:" << limitpre_filter;
  #endif
          ui->toolButton_right->setEnabled(false);
          return;
        }
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupStringAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter1.index)),unik2filter1.like,
            unik2filter1.equal,
            qMakePair(arrayfilterpage.at((int)forward_filter - 1), limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
        ui->toolButton_leftmost->setEnabled(true);
  #ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << "backward_filter:" << backward_filter
                 << "forward_filter:" << forward_filter;
  
  #endif
        ui->label_infohalaman->setText(
            labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
        
    }
    
    if(modefilterset==5){
        if (total_pagelist_filter < 2) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          return;  //
        }
        if (forward_filter == total_pagelist_filter) {
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_rightmost->setEnabled(false);
          return;  // we reach forward_filter limit
        }
        ui->toolButton_right->setEnabled(true);
        forward_filter++;
        backward_filter++;
        if (forward_filter > 1) {
          ui->toolButton_left->setEnabled(true);
        }
        if (backward_filter > 1) {
          ui->toolButton_right->setEnabled(true);
        }
        limitpre_filter = limitpre_filter + limited;
        // check if limitpre_filter < 0
        if (limitpre_filter < 0) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__
                   << "limitpre_filter:" << limitpre_filter;
  #endif
          ui->toolButton_right->setEnabled(false);
          return;
        }
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupVariantAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter2.index)),unik2filter2.filter1,
            unik2filter2.filter2,
            qMakePair(arrayfilterpage.at((int)forward_filter - 1), limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
        ui->toolButton_leftmost->setEnabled(true);
  #ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << "backward_filter:" << backward_filter
                 << "forward_filter:" << forward_filter;
  
  #endif
        ui->label_infohalaman->setText(
            labelpage.arg(forward_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
  }
}

void FormTableView::on_toolButton_rightmost_clicked() {
  if (!set) {
    qDebug() << "FATAL internal setting belum diset.";
    return;
  }
  if (mode == 1) {
    if (!ui->toolButton_leftmost->isEnabled()) {
      ui->toolButton_leftmost->setEnabled(true);
    }
    if (total_pagelist_select == forward_select) {
#ifdef DEBUGB
      qDebug() << "\n"
               << __FILE__ << __LINE__ << "forward_select" << forward_select
               << "backward_select" << backward_select;
#endif
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_left->setEnabled(true);
      ui->toolButton_rightmost->setEnabled(false);
      return;
    }
    backward_select = forward_select = total_pagelist_select;
    qint64 l = (limited * total_pagelist_select) - limited;
    limitpre_select = l;
    utilproduk::sqlmodelwork brg;
    utilproduk::selectdatahelper ret = brg.selectReady(
        configdb, m_sqlobject.selectsql(), qMakePair(l, limited));

    if (!ret.data().first) {
      error::dialog::emitdialogerror(
          this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                "mengetahui penyebabnya tersebut."),
          ret.message());
      return;
    }
    m_model->clear();
    m_model->setRecordList(ret.data().second);
    if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
      qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
    }
    ui->tableView->setModel(m_model);

    ui->tableView->setSelectionModel(selectmodel);
    // set max
    forward_select = backward_select = total_pagelist_select;
    ui->toolButton_right->setEnabled(false);
    ui->toolButton_left->setEnabled(true);
    ui->toolButton_rightmost->setEnabled(false);
    ui->label_infohalaman->setText(
        labelpage.arg(total_pagelist_select).arg(total_pagelist_select).arg (savedcountselect));
  }
  if (mode == 2) {
    if (modefilterset == 1) {
      if (!ui->toolButton_leftmost->isEnabled()) {
        ui->toolButton_leftmost->setEnabled(true);
      }
      if (total_pagelist_filter == forward_filter) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_left->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = total_pagelist_filter;
      select_id++;
      qint64 l = (limited * total_pagelist_filter) - limited;
      limitpre_filter = l;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdata.first == std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__ << " isi filterdata from user input: ";
        qDebug() << "first: " << filterdata.first
                 << " second: " << filterdata.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterbysinglegroupReady(
          configdb, std::get<2>(m_sqlobject.filtergrp().at(filterdata.first)),
          filterdata.second, qMakePair(l, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);
      forward_filter = backward_filter = total_pagelist_filter;
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_rightmost->setEnabled(false);
      ui->toolButton_left->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(total_pagelist_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    if (modefilterset == 2) {
      if (!ui->toolButton_leftmost->isEnabled()) {
        ui->toolButton_leftmost->setEnabled(true);
      }
      if (total_pagelist_filter == forward_filter) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_left->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = total_pagelist_filter;
      select_id++;
      qint64 l = (limited * total_pagelist_filter) - limited;
      limitpre_filter = l;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama."));
        setEnabled(false);
        return;
      }
      utilproduk::selectdatahelper ret = edit.filterequalbysinglegroupReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          filterdatavariant.second, qMakePair(l, limited));
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

      forward_filter = backward_filter = total_pagelist_filter;
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_rightmost->setEnabled(false);
      ui->toolButton_left->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(total_pagelist_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }

    if (modefilterset == 3) {
      if (!ui->toolButton_leftmost->isEnabled()) {
        ui->toolButton_leftmost->setEnabled(true);
      }
      if (total_pagelist_filter == forward_filter) {
#ifdef DEBUGB
        qDebug() << "\n"
                 << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                 << "backward_filter" << backward_filter;
#endif
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_left->setEnabled(true);
        ui->toolButton_rightmost->setEnabled(false);
        return;
      }
      backward_filter = forward_filter = total_pagelist_filter;
      select_id++;
      qint64 l = (limited * total_pagelist_filter) - limited;
      limitpre_filter = l;

      utilproduk::sqlmodelwork edit;
      bool found = false;
      for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
        if (filterdatavariant.first ==
            std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
          found = true;
        }
      }
      if (!found) {
#ifdef DEBUGB
        qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                   << std::get<2>(m_sqlobject.filtergrp().at(fgp));
        }
        qDebug() << __FILE__ << __LINE__
                 << " isi filterdatavariant from user input: ";
        qDebug() << "first: " << filterdatavariant.first
                 << " second: " << filterdatavariant.second;
#endif
        error::dialog::emitdialogerror(
            this, tr("Galat"),
            tr("Galat sistem error index lookup sql filter tidak sama. Mohon "
               "kontak aplikasi vendor ini "
               "untuk keterangan lebih lanjut."));
        setEnabled(false);
        return;
      }

      utilproduk::selectdatahelper ret = edit.selectReady(
          configdb,
          std::get<2>(m_sqlobject.filtergrp().at(filterdatavariant.first)),
          qMakePair(l, limited), false);
      if (!ret.data().first) {
        error::dialog::emitdialogerror(
            this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                  "mengetahui penyebabnya tersebut."),
            ret.message());
        return;
      }
      m_model->clear();
      m_model->setRecordList(ret.data().second);
      if (!ret.avoiddeepcopy()) {
#ifdef DEBUGB
        qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
#endif
      }
#ifdef DEBUGB
      qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
               << "ret.data.second.size(): " << ret.data().second.size();
#endif
      ui->tableView->setModel(m_model);

      ui->tableView->setSelectionModel(selectmodel);

      forward_filter = backward_filter = total_pagelist_filter;
      ui->toolButton_right->setEnabled(false);
      ui->toolButton_rightmost->setEnabled(false);
      ui->toolButton_left->setEnabled(true);
      ui->label_infohalaman->setText(
          labelpage.arg(total_pagelist_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
  
    if(modefilterset==4){
        if (!ui->toolButton_leftmost->isEnabled()) {
          ui->toolButton_leftmost->setEnabled(true);
        }
        if (total_pagelist_filter == forward_filter) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                   << "backward_filter" << backward_filter;
  #endif
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_left->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(false);
          return;
        }
        backward_filter = forward_filter = total_pagelist_filter;
        select_id++;
        qint64 l = (limited * total_pagelist_filter) - limited;
        limitpre_filter = l;
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupStringAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter1.index)),unik2filter1.like,
            unik2filter1.equal, qMakePair(l, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
        forward_filter = backward_filter = total_pagelist_filter;
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        ui->toolButton_left->setEnabled(true);
        ui->label_infohalaman->setText(
            labelpage.arg(total_pagelist_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
    if(modefilterset==5){
        if (!ui->toolButton_leftmost->isEnabled()) {
          ui->toolButton_leftmost->setEnabled(true);
        }
        if (total_pagelist_filter == forward_filter) {
  #ifdef DEBUGB
          qDebug() << "\n"
                   << __FILE__ << __LINE__ << "forward_filter" << forward_filter
                   << "backward_filter" << backward_filter;
  #endif
          ui->toolButton_right->setEnabled(false);
          ui->toolButton_left->setEnabled(true);
          ui->toolButton_rightmost->setEnabled(false);
          return;
        }
        backward_filter = forward_filter = total_pagelist_filter;
        select_id++;
        qint64 l = (limited * total_pagelist_filter) - limited;
        limitpre_filter = l;
  
        utilproduk::sqlmodelwork edit;
        bool found = false;
        for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
          if (filterdatavariant.first ==
              std::get<0>(m_sqlobject.filtergrp().at(fgp))) {
            found = true;
          }
        }
        if (!found) {
  #ifdef DEBUGB
          qDebug() << __FILE__ << __LINE__ << " isi m_sqlobject: ";
          for (int fgp = 0; fgp < m_sqlobject.filtergrp().size(); fgp++) {
            qDebug() << std::get<0>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<1>(m_sqlobject.filtergrp().at(fgp))
                     << std::get<2>(m_sqlobject.filtergrp().at(fgp));
          }
          qDebug() << __FILE__ << __LINE__
                   << " isi filterdatavariant from user input: ";
          qDebug() << "first: " << filterdatavariant.first
                   << " second: " << filterdatavariant.second;
  #endif
          error::dialog::emitdialogerror(
              this, tr("Galat"),
              tr("Galat sistem error index lookup sql filter tidak sama."));
          setEnabled(false);
          return;
        }
        utilproduk::selectdatahelper ret = edit.filterequalby2groupVariantAndVariantReady (
            configdb,
            std::get<2>(m_sqlobject.filtergrp().at(unik2filter2.index)),unik2filter2.filter1,
            unik2filter2.filter2, qMakePair(l, limited));
        if (!ret.data().first) {
          error::dialog::emitdialogerror(
              this, tr("Galat"), tr("Terjadi galat, klik selengkapnya untuk "
                                    "mengetahui penyebabnya tersebut."),
              ret.message());
          return;
        }
        m_model->clear();
        m_model->setRecordList(ret.data().second);
        if (!ret.avoiddeepcopy()) {
  #ifdef DEBUGB
          qDebug() << "avoiddeepcopy failed " << __FILE__ << ":" << __LINE__;
  #endif
        }
  #ifdef DEBUGB
        qDebug() << Q_FUNC_INFO << ":" << __FILE__ << ":" << __LINE__
                 << "ret.data.second.size(): " << ret.data().second.size();
  #endif
        ui->tableView->setModel(m_model);
  
        ui->tableView->setSelectionModel(selectmodel);
  
        forward_filter = backward_filter = total_pagelist_filter;
        ui->toolButton_right->setEnabled(false);
        ui->toolButton_rightmost->setEnabled(false);
        ui->toolButton_left->setEnabled(true);
        ui->label_infohalaman->setText(
            labelpage.arg(total_pagelist_filter).arg(total_pagelist_filter).arg (savedcountfilter));
    }
    
  }
}

void FormTableView::modeselect() {
  mode = 1;
  forward_select = backward_select = 1;
  forward_filter = backward_filter = 1;
  limitpre_filter = limitpre_select = 0;
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << " " << __LINE__ << " " << __FILE__
           << " mode: " << mode << "total_pagelist_select"
           << total_pagelist_select;
#endif
}

void FormTableView::modefilter() {
  mode = 2;
  forward_select = backward_select = 1;
  forward_filter = backward_filter = 1;
  limitpre_filter = limitpre_select = 0;
#ifdef DEBUGB
  qDebug() << Q_FUNC_INFO << " " << __LINE__ << " " << __FILE__
           << " mode: " << mode
           << "total_pagelist_filter:" << total_pagelist_filter;
#endif
}

void FormTableView::on_tableView_activated(const QModelIndex &index) {
  QModelIndexList idx = ui->tableView->selectionModel()->selectedRows();
#ifdef DEBUGB
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  qDebug() << "index column:" << index.column()
           << "index.data().toString():" << index.data().toString();
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  qDebug() << "idx.size()" << idx.size();
  for (decltype(idx.size()) x = 0; x < idx.size(); x++) {
    qDebug() << "idx.at(x).data().toString()" << idx.at(x).data().toString();
  }
  qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
#else
  Q_UNUSED(index)
#endif
  if (idx.size() >= 1) {
    Q_EMIT onactivatedtableviewid(idx.at(0).data().toString());
  }
}
