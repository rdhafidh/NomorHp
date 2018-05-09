#ifndef FORMTABLEVIEW_H
#define FORMTABLEVIEW_H
 
#include <QtCore>
#include <QtSql>
#include <deque>
#include <utility>
#include <tuple>
#include <iostream>
#include "uiutil.h"
#include "sqlmodel.h"
#include "utilproduk.h"
#include "tableaccesslevel.h"
#include "excel/exportexcelwork.h"
#include "excel/exportexcelresult.h"
#include "excel/dataimportexcel.h"
#include "excel/importexcelworking.h"
#include "customslottable.h"

class sqlop
{
public:
    sqlop()
    {}
    ~sqlop(){}
    void setsql(const QString& sql,const QString& sqltablename
                      ,const QVector<std::tuple<int,QString,QString> > &fgrp )
    {
        select=sql;
        selectcount=sqltablename; 
        filtergroup=fgrp; 
        set=true;
    } 

     bool isset()const {return set;}
    QString selectsql()const {return select;}
    QString selectcountsql()const {return selectcount;}
    QVector<std::tuple<int,QString,QString> > filtergrp()const {return filtergroup;} 

private:
    bool set=false;
    QString select;
    QString selectcount;
    QVector<std::tuple<int,QString,QString> > filtergroup; 
};

class tableviewcommon{
public:
    tableviewcommon(){}
    ~tableviewcommon(){}
    /*
     * param1 dbbase::configdb = database config
     * 
     * */
    
    void setsetting(const dbbase::configdb& db,const priv::TableAccessLevel & context
                  ,const sqlop &selectfiltercounttblname_sql
                  ,int l
                  ,const QStringList& h
                  ,const std::pair<bool,int>& pairtrueorya,const QColor&color=QColor(212,255,127))
    {
        if (!selectfiltercounttblname_sql.isset())
        {
#ifdef DEBUGB
            qDebug()<< "selectfiltercounttblname_sql belum diset: "<<__FILE__<<__LINE__;
#endif
            return;
        }
        m_sql=selectfiltercounttblname_sql;
        configdb=db; 
        limit=l;
        headerdata=h;
        tabeltruetoya=pairtrueorya;
        alternaterowcolor=color;
        m_access=context;
        
        set=true;
    }
     bool isset()const {return set;}
    QStringList headertabel()const {return headerdata;}
     int limitpage()const {return limit;} 
    //todo groupfilter should be more than one.
    sqlop sql()const {return m_sql;}
    dbbase::configdb config()const {return configdb;}
    std::pair<bool,int>  pairtrueorya()const {return tabeltruetoya;}
    QColor alternatetabelrowcolor()const { return alternaterowcolor;}
    priv::TableAccessLevel access()const{ return m_access;}
    void settextcolorforcolumn(int idx,const QColor &mcolor )
    {
        textcoloridx=idx;
        m_textcoloridx=mcolor;
        m_usetextcoloridx=true;
    }
    bool isusetextcolorforcolumn()const
    {
        return m_usetextcoloridx;
    }
    std::pair<int,QColor> textcolor()const
    {
        return std::make_pair(textcoloridx,m_textcoloridx);
    }

private:
    bool set=false;
    dbbase::configdb configdb;
    sqlop m_sql; 
    int limit;
    QStringList headerdata;
    QColor alternaterowcolor;
    std::pair<bool,int> tabeltruetoya;
    priv::TableAccessLevel m_access;

    int textcoloridx=-1;
    QColor m_textcoloridx;
    bool m_usetextcoloridx=false;
};

struct Param2LikeAndEqual{
    int index;
    QString like;
    QVariant equal;
};
struct Param2VariantAndVariant{
    int index;
    QVariant filter1;
    QVariant filter2;
};

namespace Ui {
class FormTableView;
}

class FormTableView : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormTableView(QWidget *parent = 0,const tableviewcommon& settbl=tableviewcommon());
    ~FormTableView(); 
    void setfilter(const std::pair<int,QString> &data);
    void setfiltervariant(const std::pair<int,QVariant> &data);
    //the input variant data is dummy variant, to make compatible with some legacy code.
    void setfilternone(const std::pair<int, QVariant> &data);
    //input string like dan equal qvariant param
    void setfilterunik1(const  Param2LikeAndEqual &data);
    //
    void setfilterunik2(const Param2VariantAndVariant &data);
    
    
    void settableviewsetting(const tableviewcommon& setting);
    void sethowmanyexportcol(int n);
    QString getcurrentselectionID();

    /*
     * To make it portable across different sql dbs.. Personally I don't like db views and materlized view..
     * so, we make extend to make possible to use independent select join column and tables using
     * offset limit style..
     * The good side, this would make hard to reverse in dbs side, for noobs dbs admin :)
     * */
    void setjoinmanytables(const std::pair<bool,QString> & b){modejointabel=std::make_pair(b.first,b.second);}

    /*
     * note that hdrimport mightbe different with insertion data.
     * please read correct and use something like sqlbrowser qt example
     * and pay attention to qsqldatabase::record ->fieldname sama type.
     *
     * */
    void setimportexcelVisibleTableInfo(const QString &nm, const std::vector<std::tuple<int, bool, QVariant> > &ignorecolumnscons
                                        , const QStringList &hd);
    
    void setNamedKategoriExport(const QString &kategori_export);

    void appendListMenu(QList<customslottable> *acts);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    
Q_SIGNALS:
//    sinyal ini buat implementasi public funtion
    void showimgbyid(const QString &id);
    void adddata();
    void refreshdata();
    void editdata(const QString &id);
    void deletedata(const QString &id);
    void onaktifcolumnid(const QString &id);
    void onactivatedtableviewid(const QString &id);

    //export EXCEL SIGNAL HERE
    void maxbarexportexcel(int n);
    void valuebarexportexcel(int n);
    void resultexportexcel(ExportExcelResult ret);
//import EXCEL SIGNAL HERE
    void maxbarimportexcel(int n);
    void valuebarimportexcel(int n);
    void resultimportexcel(ImportExcelResult ret
                           , const QString &msg);
    
    void beforeExportExcelHappen();


public Q_SLOTS:
    void thisrefresh();
private Q_SLOTS:
    void importexcelwork(const DataImportExcel & in);
    void exportexcelwork(const QString &tbltitle, bool use_62, bool is_mode_csv_or_excel);
    void doimportexcel();
    void doexportexcel();

    void itemselectchanged(const QItemSelection& newitem,const QItemSelection& oldone);
    void tablerowselectmoved(const QModelIndex &index);
    //    private testing implementation purpose 
    
    void implfetch();  
    void implcountrecord();
    
//    actual function usage
    void thisadd();
    void thisedit(const QString &id);
    void thisdelete(const QString &id);
    
    void on_toolButton_leftmost_clicked();
    
    void on_toolButton_left_clicked();
    
    void on_toolButton_right_clicked();
    
    void on_toolButton_rightmost_clicked();
    

    void on_tableView_activated(const QModelIndex &index);

private:
    void modeselect();
    void modefilter();
    template<class ForwardIterator, class T>
        void genvalidpage(ForwardIterator first, ForwardIterator last, T value,T step)
        {
            while(first != last) {
                        *first++ = value;
                        value +=step;
            }
        }
     
    Ui::FormTableView *ui;
    SqlModel * m_model;
    dbbase::configdb configdb;
    bool set=false;
//    note total_pagelist_select asal mulanya didapat dari selectCount function
//    kalau total_pagelist_filter asal mulanya didapat dari prepareSelectFilter ::ret::second::size 
    qint64 total_pagelist_select=0,total_pagelist_filter=0;
    qint64 delete_id,select_id,add_id,m_rowcountselect,m_rowcountfilter
    ,countrecord_id,forward_select=1,backward_select=1,forward_filter=1,backward_filter=1;
    qint64 limitpre_filtermost=0,limitpre_selectmost=0;
    QString labelpage=tr("%1 dari %2 halaman dengan total data ditemukan %3");
    sqlop m_sqlobject;
    std::pair<int,QString> filterdata;
    std::pair<int,QVariant> filterdatavariant;
    //modefilterset 1=filter by string that produce like statement
    //modefilterset 2=filter by variant that produce equal statement
    //modefilterset 3= filter by none data that produce controlled sql statement with no user supplied data param
    QString tblname;
    int modefilterset=1;
    int limitpre_select=0,limited=10,limitpre_filter=0;
    //default mode is select ,yakni mode 1
    int mode=1;
    std::deque<qint64> arrayfilterpage,arrayselectpage;
    QStringList headertabel;
    QItemSelectionModel *selectmodel;
    QScopedPointer<ExportExcelThread> m_exportexcelthread;
    QScopedPointer<ImportExcelThread> m_importexcelthread;

    int howmanyexportcol=0;
    qint64 savedcountselect=0;
    qint64 savedcountfilter=0;
    
    Param2LikeAndEqual unik2filter1;
    Param2VariantAndVariant unik2filter2;
    
    //importexcel this behaivor is that:
    //bool jika true maka column itu buat idsistem jika false
    //maka kolumn tadi diBUAT USER kode default input type
    std::vector<std::tuple<int,bool,QVariant> > ignorecolumnsimportexcel;
    std::pair<bool,QString> modejointabel;
    QStringList hdrimport;
    QString name_kategori_export;
};

#endif // FORMTABLEVIEW_H
