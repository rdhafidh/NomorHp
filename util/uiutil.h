#ifndef UIUTIL_H
#define UIUTIL_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore> 

namespace error
{
struct dialog{
    ~dialog(){}
    dialog(){}
static void emitdialoginfo(QWidget *parent,const QString &title,const QString &msg){
    QMessageBox::information(parent,title,msg,QMessageBox::Ok); 
}
static void emitdialogwarning(QWidget *parent,const QString & title,const QString & msg){
    QMessageBox::warning(parent,title,msg,QMessageBox::Ok);
    
}
static void emitdialogerror(QWidget *parent,const QString & text,const QString &infos,const  QString &det=QString(""))
{
    QMessageBox b(parent);
    if (det.isEmpty()){ 
        b.setText(text);
        b.setInformativeText(infos);
        b.setStandardButtons(QMessageBox::Ok);
        b.setIcon(QMessageBox::Critical);
        b.exec();
    }else {
        b.setText(text);
        b.setInformativeText(infos);
        b.setDetailedText(det);
        b.setIcon(QMessageBox::Critical);  
        b.setStandardButtons(QMessageBox::Ok);
        b.exec();
    }
}
};

} 
namespace filereader
{
class readwrite{
public:
    readwrite(){}
    ~readwrite(){}
    static bool forcewritefile(const QString filename,const QString &data)
    {
        QFileInfo infofile(filename);
        QSaveFile qfile(filename);
        if (!qfile.open(QIODevice::Text|QIODevice::ReadWrite|QIODevice::Truncate))
        {      
#ifdef DEBUGB
        qDebug()<<"open: "<<qfile.isOpen()<<" ";
#endif
        return false;
        }
        QTextStream out(&qfile);
        out <<data;
#ifdef DEBUGB
        qDebug()<<Q_FUNC_INFO<<__FILE__<<" "<<__LINE__;
#endif
        return qfile.commit();
    }
    static QByteArray readfile(const QByteArray &arr ){
        QFile fl(arr.data());
        if (!fl.exists() || !fl.open(QIODevice::ReadOnly | QIODevice::Text)){
         return QByteArray("");   
        }
        QByteArray data;
        while(!fl.atEnd()){
            auto b= fl.readLine();
            data.append(b);
        }
        fl.close();
        return data; 
    }
};
}

#endif // UIUTIL_H

