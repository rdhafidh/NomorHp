#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <asklogin.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle (QStyleFactory::create ("fusion"));
    form::MainMenu w;
//    AskLogin *lg = new AskLogin;
   
//    QObject::connect (lg,SIGNAL(asklogin(QByteArray)),&w,SLOT(recieveloginInfo(QByteArray)));
//  lg->setAttribute(Qt::WA_DeleteOnClose);
   
//    QObject::connect (&w,&form::MainMenu::askquit,[&a](){
//        a.quit ();
//    });
//    QObject::connect (lg,&AskLogin::rejected,[&a](){
//        a.quit ();
//    });
//    lg->show ();
    w.show ();
    return a.exec();
}
