#ifndef PWrapper_H
#define PWrapper_H
#include <QtCore>
#include <chrono>
namespace dbbase
{
typedef QMap<QString,QVariant> configdb;
}
//Q_DECLARE_METATYPE(dbbase::configdb)

namespace PWrapperThread
{



class serverdbisreadydata
{
public:
    serverdbisreadydata()
    {}
    ~serverdbisreadydata()
    {}
    void setval(const dbbase::configdb &c
                ,const QString &app
                , const QString & dbfolder)
    {
        config=c;
        m_dbfolder=dbfolder;
        m_appdir=app;
        set=true;
    }

    bool isset()const
    {
        return set;
    }
    dbbase::configdb configdbs()const
    {
        return config;
    }
    QString appdir()const
    {
        return m_appdir;
    }
    QString dbfolder()const
    {
        return m_dbfolder;
    }
private:
    bool set=false;
    dbbase::configdb config;
    QString m_dbfolder="";
    QString m_appdir="";
};

class PWrapper_serverdbisready:public QObject
{
    Q_OBJECT
public:
    PWrapper_serverdbisready(const serverdbisreadydata &config, QObject *parent=0):QObject(parent)
      ,configdata_serverdbisready(config)
    {
    }
    ~PWrapper_serverdbisready(){ 
#ifdef DEBUGB
                qDebug()<<"QThread::currentThreadId(): "<<QThread::currentThreadId()<<__FILE__<<__LINE__;
#endif 
    } 
     
Q_SIGNALS:
    void emitError_serverdbisready(QByteArray err);
    void emitOutput_serverdbisready( QByteArray o); //SEND BY VALUE AND MOVE ITERATOR SHOULD BE USED
    void emitFinish_serverdbisready(int exitCode, QProcess::ExitStatus exitStatus);
    void emitStart_serverdbisready(); 
   // void emitErrorReason_serverdbisready(QProcess::ProcessError error);
 
public Q_SLOTS:
    void init_serverdbisready()
    {
        mp_serverdbisready.reset(new QProcess(this));
    #ifdef DEBUGB
                    qDebug()<<"QThread::currentThreadId();: "<<QThread::currentThreadId()<<__FILE__<<__LINE__;
    #endif
        mp_serverdbisready->setProcessChannelMode(QProcess::MergedChannels);   
        connect(mp_serverdbisready.data(),SIGNAL(readyReadStandardOutput()),this,SLOT(handleoutput_serverdbisready()));
        connect(mp_serverdbisready.data(),SIGNAL(readyReadStandardError()),SLOT(handleerror_serverdbisready()));
        connect(mp_serverdbisready.data(),SIGNAL(started()),this,SIGNAL(emitStart_serverdbisready()));
        connect(mp_serverdbisready.data(),SIGNAL(finished(int,QProcess::ExitStatus))
                ,this,SIGNAL(emitFinish_serverdbisready(int,QProcess::ExitStatus)));
//        connect(mp_serverdbisready.data(),SIGNAL(error(QProcess::ProcessError)),
//                this,SIGNAL(emitErrorReason_serverdbisready(QProcess::ProcessError)));
        
    }
    
    void start_serverdbisready()
    { 
        if (configdata_serverdbisready.isset())
        {
            QDir pyhome(configdata_serverdbisready.appdir());
            if (!pyhome.cdUp()){
                Q_EMIT emitError_serverdbisready("Galat inisialisasi program database backup.");
                return;
            }  
    
#ifdef Q_OS_WIN
        auto prog=QDir::toNativeSeparators(configdata_serverdbisready.appdir()+"/pg_ctl.exe");
#else
        auto prog=QDir::toNativeSeparators(configdata_serverdbisready.appdir()+"/pg_ctl");    
#endif 
            
            /* dbbase::configdb c;
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
//            env.insert("PGPASSWORD",configdata_serverdbisready.configdbs().value("password").toString());
            mp_serverdbisready->setProcessEnvironment(env); 
           
           QStringList args;
           
           args << "status"<<"-D"<<QDir::toNativeSeparators(configdata_serverdbisready.dbfolder());
               
    #ifdef DEBUGB
            qDebug()<<Q_FUNC_INFO<<" cmd: "<<prog<<args; 
    #endif
            mp_serverdbisready->start(prog, args);  
        } 
    } 
    void handleoutput_serverdbisready()
    { 
        Q_EMIT emitOutput_serverdbisready(mp_serverdbisready->readAllStandardOutput());
    }
    
    void handleerror_serverdbisready()
    {
        Q_EMIT emitError_serverdbisready(mp_serverdbisready->readAllStandardError());
    } 

    
private: 
    QScopedPointer<QProcess,QScopedPointerDeleteLater> mp_serverdbisready; 
    serverdbisreadydata configdata_serverdbisready;
};


class serverdbisready:public QObject{
Q_OBJECT
public:
    serverdbisready(const serverdbisreadydata& config, QObject *parent=0):QObject(parent)
    {
        
        qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus"); 
        mp=new PWrapper_serverdbisready(config);
        
        mp->moveToThread(&thread);
        connect(&thread,SIGNAL(started()),mp,SLOT(init_serverdbisready()));
        connect(&thread,SIGNAL(finished()),mp,SLOT(deleteLater())); 
        //from user code to internal engine
        connect(this,SIGNAL(em_serverdbisready()),mp,SLOT(start_serverdbisready())); 
        connect(mp,SIGNAL(emitStart_serverdbisready()),this,SIGNAL(emitStart_serverdbisready()));
        connect(mp,SIGNAL(emitError_serverdbisready(QByteArray)),this,SIGNAL(emitError_serverdbisready(QByteArray)));
        connect(mp,SIGNAL(emitOutput_serverdbisready(QByteArray)),this,SIGNAL(emitOutput_serverdbisready(QByteArray))); 
//        connect(mp,SIGNAL(emitErrorReason_serverdbisready(QProcess::ProcessError))
//                ,this,SIGNAL(emitErrorReason_serverdbisready(QProcess::ProcessError)));
        connect(mp,SIGNAL(emitFinish_serverdbisready(int,QProcess::ExitStatus))
                ,this,SIGNAL(emitFinish_serverdbisready(int,QProcess::ExitStatus)));                                                             
        thread.start(); 
    }
    ~serverdbisready()
    {
selesai();
    } 
    void isready()
    { 
#ifdef DEBUGB
                qDebug()<<"QThread::currentThreadId();: "<<QThread::currentThreadId()<<__FILE__<<__LINE__;
#endif
       Q_EMIT em_serverdbisready();
    }
public Q_SLOTS:
    void selesai()
    {
        if (thread.isRunning()){
        thread.requestInterruption();
        thread.quit();
          }
         
    }

Q_SIGNALS:
     void em_serverdbisready();
     void emitFinish_serverdbisready(int,QProcess::ExitStatus);
     void emitOutput_serverdbisready(QByteArray o);
     void emitStart_serverdbisready();
     void emitError_serverdbisready(QByteArray s);
//     void emitErrorReason_serverdbisready(QProcess::ProcessError e);
     
private:
     PWrapper_serverdbisready *mp;
     QThread thread;
};





}


#endif // PWrapper_H
