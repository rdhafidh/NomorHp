#ifndef TABLEVIEWEDIT_H
#define TABLEVIEWEDIT_H

#include "uiutil.h"
#include "invalidkoneksi.h"
#include "tableaccesslevel.h"
#include <customslottable.h>

class TableViewEdit:public QTableView
{
Q_OBJECT
public:
    TableViewEdit(QWidget *parent=0):QTableView(parent),localacts(Q_NULLPTR)
    {
        m_access=new priv::TableAccessLevel;                              
    }
    ~TableViewEdit()
    {
        delete m_access;
    }
    void setready(bool e,const priv::TableAccessLevel & maccess)
    {
        ready=e;
        if (maccess.isset())
        { 
            m_access->setsetting(maccess.access()
                                 ,maccess.actions());
        }
    }
    void appendListMenu(QList<customslottable> *acts ){
        localacts=acts;
    }
    QString getcurrentselectionID()
    {
      QModelIndexList idx=selectionModel()->selectedRows();

      if (idx.size()<=0
            ){
          return "";
      }
      QModelIndex dat=idx.at(0);
#ifdef DEBUGB
      qDebug()<<Q_FUNC_INFO<<__FILE__<<__LINE__<<" idx size: "<<idx.size()
             <<"idx column: "<<dat.column();
#endif
      if (dat.isValid())
      {
          if (dat.data().isValid())
          {
              return dat.data().toString();
            }
        }
      return "";
    }

    virtual void contextMenuEvent(QContextMenuEvent * event) Q_DECL_OVERRIDE
    { 

        QMenu * menu=new QMenu(this);
        QString s="QMenu::item  \
        {     \
            padding: 2px 20px 2px 20px; \
             background-color:#EFEBE7;    \
        } \
            QMenu::item:selected  \
            { \
                color: black; \
                background-color: #F5F5F5;  \
            } \
            QMenu::separator  \
            { \
                height: 0.5px;  \
                 color: white;  \
                padding-left: 0.1px;  \
                margin-left: 0.1px; \
                background-color:white; \
                margin-right: 0.1px;    \
            }   \
         ";
        menu->setStyleSheet(s);
#ifndef BUILD_UIDESIGNER

        bool set=false;
        if (localacts !=Q_NULLPTR)
          {
            for(int cs=0;cs<localacts->size();cs++)
              {
                if(localacts->at(cs).func !=Q_NULLPTR &&
                   !localacts->at(cs).name.isEmpty() &&
                   localacts->at(cs).wgt !=Q_NULLPTR)
                  {
                      menu->addAction(localacts->at(cs).icon,localacts->at(cs).name
                                      ,localacts->at(cs).wgt,localacts->at(cs).func);
                      set=true;
                  }
              }
          }
        if (m_access->isset())
        {
            auto tmbh_find=m_access->actions();
            auto tmbh=tmbh_find.find(priv::act::tambah);
            if (tmbh !=tmbh_find.end())
            {
                set=true;
                menu->addAction(QIcon("://res/document-new.png"),tr("Tambah"),this,SLOT(tambah()));
            }

            auto edt_find=m_access->actions();
            auto edt=edt_find.find(priv::act::edit);
            if (edt !=edt_find.end())
            {
                 set=true;
                menu->addAction(QIcon("://res/edit-find-replace.png"),tr("Edit"),this,SLOT(editing()));
            }

            auto hps_find=m_access->actions();
            auto hps=hps_find.find(priv::act::hapus);
            if (hps !=hps_find.end())
            {
                 set=true;
                menu->addAction(QIcon("://res/edit-delete.png"),tr("Hapus"),this,SLOT(hapus()));
            }

            auto slt_find=m_access->actions();
            auto slt=slt_find.find(priv::act::select);
            if (slt != slt_find.end())
            {
               set=true;
               menu->addAction(QIcon("://res/view-refresh.png"),tr("Refresh"),this,SLOT(refresh()));
            }
            auto impt_excel=m_access->actions();
            auto imp_excel=impt_excel.find(priv::act::importexcel);
            if (imp_excel !=impt_excel.end())
              {
                set=true;
                menu->addAction(QIcon("://import_excel1.png"),tr("Import Excel"),this,SLOT(importexcel()));
              }
            auto exprt_excel=m_access->actions();
            auto exp_excel=exprt_excel.find(priv::act::exportexcel);
            if (exp_excel!=exprt_excel.end())
              {
                set=true;
                menu->addAction(QIcon("://res/document-revert-rtl.png"),tr("Export Excel"),this,SLOT(exportexcel()));
              }
        }
        if (set)
        {
            menu->exec(event->globalPos());
        }
#else
//BUILD_UIDESIGNER
        menu->addAction(QIcon("://ic_create_new_folder_black_48dp.png"),tr("Tambah"),this,SLOT(tambah()));
         menu->addAction(QIcon("://ic_create_black_48dp.png"),tr("Edit"),this,SLOT(editing()));
          menu->addAction(QIcon("://ic_delete_black_48dp.png"),tr("Hapus"),this,SLOT(hapus()));
         menu->addAction(QIcon("://ic_loop_black_48dp.png"),tr("Refresh"),this,SLOT(refresh()));
           menu->addAction(QIcon("://import_excel1.png"),tr("Import Excel"),this,SLOT(importexcel()));
            menu->addAction(QIcon("://export_excel3.png"),tr("Export Excel"),this,SLOT(exportexcel()));
             menu->exec(event->globalPos());
#endif

        menu->deleteLater();
    }
   

Q_SIGNALS:
    void emitadd();
    void emitedit( const QString & id);
    void emithapus(const QString & id);
    void refreshing(); 
    void emitimportexcel();
    void emitexportexcel();
    
private Q_SLOTS: 
    void importexcel()
    {
      Q_EMIT emitimportexcel();
    }
    void exportexcel()
    {
      Q_EMIT emitexportexcel();
    }

    void tambah()
    {
        if (!ready)
        {
            qDebug()<<"FATAL not ready edittabel";
            return;
        }
        Q_EMIT emitadd();
    }
    void editing()
    {
        if (!ready)
        {
            qDebug()<<"FATAL not ready edittabel";
            return;
        }
        QModelIndexList idx=selectionModel()->selectedRows();
        
        if (idx.size()<=0
              ){
            error::dialog::emitdialogerror(this,tr("Galat"),tr("Baris tabel yang dipilih kosong. Pastikan anda meletakkan mouse pointer pada baris dalam tabel."));
            return;
        }
        QModelIndex dat=idx.at(0);
#ifdef DEBUGB
        qDebug()<<Q_FUNC_INFO<<__FILE__<<__LINE__<<" idx size: "<<idx.size()
               <<"idx column: "<<dat.column();
#endif
        if (dat.isValid())
        {
            if (dat.data().isValid())
            {
#ifdef DEBUGB
        qDebug()<<Q_FUNC_INFO<<__FILE__<<__LINE__<<" idx.at(0).data(): "<<dat.data();
#endif
        Q_EMIT emitedit(dat.data().toString()); 
            }
        }
    }
    void hapus()
    {
        if (!ready)
        {
            qDebug()<<"FATAL not ready edittabel";
            return;
        }
       QModelIndexList idx=selectionModel()->selectedRows();
        if (idx.size()<=0){
            error::dialog::emitdialogerror(this,tr("Galat"),tr("Baris tabel yang dipilih kosong. Pastikan anda meletakkan mouse pointer pada baris dalam tabel."));
            return;
        }
        
        QModelIndex dat=idx.at(0);
#ifdef DEBUGB
        qDebug()<<Q_FUNC_INFO<<__FILE__<<__LINE__<<" idx size: "<<idx.size()
               <<"idx column: "<<dat.column();
#endif
        if (dat.isValid())
        {
            if (dat.data().isValid())
            {
#ifdef DEBUGB
        qDebug()<<Q_FUNC_INFO<<__FILE__<<__LINE__<<" idx.at(0).data(): "<<dat.data();
#endif
        Q_EMIT emithapus(dat.data().toString()); 
            }
        }
    }
    void refresh()
    {
        if (!ready)
        {
            qDebug()<<"FATAL not ready edittabel";
            return;
        }
        Q_EMIT refreshing();
    }
   
private:
    bool ready=false;
    priv::TableAccessLevel *m_access;
    QList<customslottable> *localacts;
};

#endif // TABLEVIEWEDIT_H

