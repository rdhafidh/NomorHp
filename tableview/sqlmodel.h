#ifndef SQLMODEL_H
#define SQLMODEL_H
#include <QtCore>
#include <QtSql> 
#include <iterator>
#include <memory>
#include <deque>
#include  "settinghandler.h"

class SqlModel:public QAbstractTableModel
{
Q_OBJECT
public:
    SqlModel(QObject*parent=0):QAbstractTableModel(parent)
    {
        m_alternate_row_color.setNamedColor ("blue");
        hiddencol=0;
        m_recList.clear();
        headerdata.clear(); 
        tblbrgtruetoya=std::make_pair(false,0);
    }
    ~SqlModel()
    {
        m_recList.clear();
    }
    std::deque<QSqlRecord> *internalrecord()
     {
         return &m_recList;
     }
    void setAlternateRowColor(const QColor & color)
    {
        m_alternate_row_color=color;
    }
    void setHiddenColoumnFromRight(int col)
    {
        hiddencol=col;
    }
    void settabeltruetoya(const std::pair<bool,int> rowcol)
    {
        tblbrgtruetoya=rowcol;
    }
     QHash<int, QByteArray> roleNames() const{
       QHash<int, QByteArray> localrole; 
       if(m_recList.size()==0) return localrole;
       
       //always refer to index 0
       QSqlRecord rec=m_recList.at(0);
       if(rec.count()== 0) return localrole;
       
#ifdef BUILD_QML
        int baserole=Qt::UserRole+1;
       for(int col=0;col<rec.count();col++){ 
           localrole[baserole+col]=rec.field(col).name().toUtf8();
         }
#endif
       return localrole;
     }
     
#ifdef BUILD_QML
     void setData(const QModelIndex &item, const QVariant &value, int role)
       {
       //only valid for qml collapsedrole to be edited, since other data is done in the re-fetching/set-then-refresh external data source
       if (item.isValid() && role == collapsedrole) {
               QAbstractItemModel::setData(item, value,role);
               emit dataChanged(item, item);
               return true;
           }
       return false;
      }
#endif
     

    void setRecordList(std::deque<QSqlRecord> records)
    {
      //for qml side,it is expected that data source must always append collapsed field with value to true
     //
        Q_EMIT  layoutAboutToBeChanged();
        std::move(records.begin(),records.end(),std::back_inserter(m_recList)); 
        records.clear(); 
        Q_EMIT layoutChanged();

    }
    int rowCount(const QModelIndex & ) const
    {
        return m_recList.size();
    }
     int sizerow()const {return m_recList.size(); }
    int columnCount(const QModelIndex & ) const
    {
        if ((headerdata.size()==0)&&(m_recList.size()>=1)){
            return m_recList.at(0).count();
          }
     return headerdata.size(); 
    }


    QVariant rowidbyvalue(int row)
    { 
//        calling this function require user to call qmodelindex.isvalid first to make sure that it was valid 
            return m_recList.at(row).value(0); 
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
#ifndef BUILD_QML
        int col=index.column();
     
            if (role == Qt::DisplayRole
                    && !tblbrgtruetoya.first)
            {
                        return QVariant::fromValue(m_recList.at(index.row()).value(col)); //todo at thread -> not safe.  because QColor is part of QtGui ?
            } 
            if (role == Qt::DisplayRole
                    && tblbrgtruetoya.first)
            {
                       if (col==tblbrgtruetoya.second)
                       { 
                           if (m_recList.at(index.row()).value(col).toBool()){
                             return  QVariant::fromValue(tr("Ya"));
                           }else{
                              return QVariant::fromValue(tr("Tidak"));
                           }
                       }else{
                            return QVariant::fromValue(m_recList.at(index.row()).value(col)); //todo at thread -> not safe.  because QColor is part of QtGui ?
                       }
            }
             if (role ==Qt::BackgroundRole && !isEven(index.row()) ){
                  return QVariant::fromValue(m_alternate_row_color );
             }
#else
             if(index.isValid() && role>Qt::UserRole &&  role!=collapsedrole){
                 int columnIdx = role - Qt::UserRole - 1;
                             QModelIndex modelIndex = this->index(index.row(), columnIdx);
                             return data(modelIndex, Qt::DisplayRole);
               }  
#endif
               
            return QVariant(); 
    } 
    void setHorizontalHeader(const QStringList & data)
    {
        
        headerdata=data;
       Q_EMIT headerDataChanged(Qt::Horizontal,0,headerdata.size());
#ifdef DEBUGB
        qDebug()<<"headerdata: "<< headerdata<<" "<<__FILE__<<":"<<__LINE__;
#endif
    }
    
    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
          if (role == Qt::DisplayRole)
            {
                if (orientation == Qt::Horizontal)
                {
                    //fallback if data returned is equal more than 1
                    if ((headerdata.size()==0)&&(m_recList.size()>=1)){
                        return QVariant::fromValue(m_recList.at(0).field(section).value());
                      }else{
                     return  QVariant::fromValue(headerdata.at(section));
                      }
                }
                if (orientation == Qt::Vertical){
                    if (!m_recList.empty()){
                        return QVariant::fromValue((qint64)section+1);
                    }
                }
            }
            return QVariant(); 
    }
  
    void clear(){
            Q_EMIT layoutAboutToBeChanged();
            m_recList.clear();
            Q_EMIT layoutChanged();
    } 
    void setcolortext(int idx,const QColor  &mcolor)
    {
        textcoloridx=idx;
        m_textcolor=mcolor;
    }
    QStringList headerdatas()const
    {
        return headerdata;
    }


private:
     bool isEven(int a)const
    {
        return (a %2==0)? true : false;
    }
//     QVariant dimensidata(int r,int c)
//     {
//         //check that row is not exceeding m_recList.size()
//         //check that col is not exceeding qsqlrecord fieldName, if fieldName is empty then exceeding return qvariant invalid
//         if (m_recList.size()>r){
//             return QVariant(QVariant::Invalid);
//         }
//         if (m_recList.at(r).fieldName(c).isEmpty())
//         {
//             return QVariant(QVariant::Invalid);
//         }
//         return m_recList.at(r).value(c);
//     }

    
    std::deque<QSqlRecord> m_recList;
    QColor m_alternate_row_color;
    int hiddencol;
    QStringList headerdata;
    std::pair<bool,int> tblbrgtruetoya;
#ifdef BUILD_QML
    int collapsedrole=0;
#endif

    int textcoloridx=-1;
    bool usetextcolor=false;
    QColor m_textcolor;
};

#endif // SQLMODEL_H

