#ifndef DUMPSINGLEDBWORK_H
#define DUMPSINGLEDBWORK_H
#include <QtCore>

namespace dumpsql {

typedef QMap<QString,QVariant> dumpconfig;
class dumpsingledbdata
{
public:
    dumpsingledbdata(){}
    ~dumpsingledbdata(){}
    void setval(const QString &rootdb,const dumpconfig &c,const QString dblama,const QString tosqlfile)
    {
        set=true;
        appname=rootdb;
        configdb=c;
        formerdbname=dblama;
        tosqlfilename=tosqlfile;
    }
    bool isset()const
    {
        return set;
    }
    QString appdir()const
    {
        return appname;
    }
    dumpconfig configdbs()const
    {
        return configdb;
    }
    QString formerdbnames()const
    {
        return formerdbname;
    }
    QString tosqlfilenames()const
    {
        return tosqlfilename;
    }
private:
    bool set=false;
    QString appname="";
    dumpconfig configdb;
    QString formerdbname="";
    QString tosqlfilename="";
};

class PWrapper_dumpsingledb:public QObject
{
    Q_OBJECT
public:
    PWrapper_dumpsingledb(const dumpsingledbdata &config, QObject *parent=0):QObject(parent)
      ,configdata_dumpsingledb(config)
    {
    }
    ~PWrapper_dumpsingledb(){ 
#ifdef DEBUGB
                qDebug()<<"QThread::currentThreadId(): "<<QThread::currentThreadId()<<__FILE__<<__LINE__;
#endif 
    }
    
     Q_ENUM(QProcess::ExitStatus)
     
Q_SIGNALS:
    void emitError_dumpsingledb(QByteArray err);
    void emitOutput_dumpsingledb( QByteArray o); //SEND BY VALUE AND MOVE ITERATOR SHOULD BE USED
    void emitFinish_dumpsingledb(int exitCode, QProcess::ExitStatus exitStatus);
    void emitStart_dumpsingledb(); 
//    void emitErrorReason_dumpsingledb(QProcess::ProcessError error);
 
public Q_SLOTS:
    void init_dumpsingledb()
    {
        mp_dumpsingledb.reset(new QProcess(this));
    #ifdef DEBUGB
                    qDebug()<<"QThread::currentThreadId();: "<<QThread::currentThreadId()<<__FILE__<<__LINE__;
    #endif
        mp_dumpsingledb->setProcessChannelMode(QProcess::MergedChannels);   
        connect(mp_dumpsingledb.data(),SIGNAL(readyReadStandardOutput()),this,SLOT(handleoutput_dumpsingledb()));
        connect(mp_dumpsingledb.data(),SIGNAL(readyReadStandardError()),SLOT(handleerror_dumpsingledb()));
        connect(mp_dumpsingledb.data(),SIGNAL(started()),this,SIGNAL(emitStart_dumpsingledb()));
        connect(mp_dumpsingledb.data(),SIGNAL(finished(int,QProcess::ExitStatus))
                ,this,SIGNAL(emitFinish_dumpsingledb(int,QProcess::ExitStatus)));
//        connect(mp_dumpsingledb.data(),SIGNAL(error(QProcess::ProcessError)),
//                this,SIGNAL(emitErrorReason_dumpsingledb(QProcess::ProcessError)));
        
    }
    
    void start_dumpsingledb()
    {
        if (configdata_dumpsingledb.isset())
        {
            QDir pyhome(configdata_dumpsingledb.appdir());
#ifndef Q_OS_WIN32
            if (!pyhome.cdUp()){
               // Q_EMIT emitError_reinstalldb(tr("Galat inisialisasi Envairontment variable program database backup."));
              //  Q_EMIT emitFinish_reinstalldb(-9,QProcess::NormalExit);
                return;
            }  
#endif 
    #ifdef Q_OS_WIN
            auto prog=QDir::toNativeSeparators(configdata_dumpsingledb.appdir()+"/pg_dumpall.exe");
    #else
            auto prog=QDir::toNativeSeparators(configdata_dumpsingledb.appdir()+"/pg_dumpall");
    #endif
            /* dumpconfig c;
             c["dbbackend"]="QPSQL";
             c["hostname"]=SettingHandler::getSetting("database","alamatdb");
             c["dbname"]=SettingHandler::getSetting("database","namadb");
             c["username"]=SettingHandler::getSetting("database","namauser");
             c["password"]=SettingHandler::getSetting("database","password");
             c["port"]=SettingHandler::getSetting("database","port");
             QDateTime tm=QDateTime::currentDateTime();
             c["conname"] = QString("koneksi-%1").arg(tm.toString("dddd_dd_MM_yyyy::HH_mm_ss"));
             */
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("PYTHONHOME", pyhome.path()); 
            env.insert("PGPASSWORD",configdata_dumpsingledb.configdbs().value("password").toString());
            mp_dumpsingledb->setProcessEnvironment(env); 
            //      valid cmd ex:
           /*  SET PGPASSWORD=reno
            *  pg_dumpall -h localhost -p 5432 -U reno -c --if-exists -f b.sql
            *
            * */
           QStringList args;
           args <<"-U"<<configdata_dumpsingledb.configdbs().value("username").toString()
               <<"-h"<<configdata_dumpsingledb.configdbs().value("hostname").toString()
              <<"-p"<<configdata_dumpsingledb.configdbs().value("port").toString()
             <<"-c"
            <<"--if-exists"
              <<"-f"<<QDir::toNativeSeparators(configdata_dumpsingledb.tosqlfilenames());
    #ifdef DEBUGB
            qDebug()<<Q_FUNC_INFO<<" cmd: "<<prog<<args; 
    #endif
            mp_dumpsingledb->start(prog, args);  
        } 
    } 
    void handleoutput_dumpsingledb()
    { 
        Q_EMIT emitOutput_dumpsingledb(mp_dumpsingledb->readAllStandardOutput());
    }
    
    void handleerror_dumpsingledb()
    {
        Q_EMIT emitError_dumpsingledb(mp_dumpsingledb->readAllStandardError());
    } 

    
private: 
    QScopedPointer<QProcess,QScopedPointerDeleteLater> mp_dumpsingledb;
    dumpsingledbdata configdata_dumpsingledb;
};

class dumpsingledb:public QObject{
Q_OBJECT
public:
    dumpsingledb(const dumpsingledbdata& config, QObject *parent=0):QObject(parent)
    {
        
        qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

        mp=new PWrapper_dumpsingledb(config);
        
        mp->moveToThread(&thread);
        connect(&thread,SIGNAL(started()),mp,SLOT(init_dumpsingledb()));
        connect(&thread,SIGNAL(finished()),mp,SLOT(deleteLater())); 
        //from user code to internal engine
        connect(this,SIGNAL(em_dump()),mp,SLOT(start_dumpsingledb()));
        
        connect(mp,SIGNAL(emitStart_dumpsingledb()),this,SIGNAL(emitStart_dumpsingledb()));
        connect(mp,SIGNAL(emitError_dumpsingledb(QByteArray)),this,SIGNAL(emitError_dumpsingledb(QByteArray)));
        connect(mp,SIGNAL(emitOutput_dumpsingledb(QByteArray)),this,SIGNAL(emitOutput_dumpsingledb(QByteArray))); 
//        connect(mp,SIGNAL(emitErrorReason_dumpsingledb(QProcess::ProcessError))
//                ,this,SIGNAL(emitErrorReason_dumpsingledb(QProcess::ProcessError)));
        connect(mp,SIGNAL(emitFinish_dumpsingledb(int,QProcess::ExitStatus))
                ,this,SIGNAL(emitFinish_dumpsingledb(int,QProcess::ExitStatus)));                                                             
 
    }
    ~dumpsingledb()
    {   
        if (thread.isRunning()){
        thread.requestInterruption();
        thread.quit();
       }
    } 
   
    bool isrunning()const
    {
        return thread.isRunning();
    }
public Q_SLOTS:
    void selesai()
    {  
        if (thread.isRunning()){
        thread.requestInterruption();
        thread.quit();
       }
         
    }
    void dump()
    {   if (!thread.isRunning()){
            thread.start();
           }
       Q_EMIT em_dump();
    }

Q_SIGNALS:
     void em_dump();
     void emitFinish_dumpsingledb(int,QProcess::ExitStatus);
     void emitOutput_dumpsingledb(QByteArray o);
     void emitStart_dumpsingledb();
     void emitError_dumpsingledb(QByteArray s);

private:
     PWrapper_dumpsingledb *mp;
     QThread thread;
};

}
#endif // DUMPSINGLEDBWORK_H
