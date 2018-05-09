#include "pwrapper.h"

PWrapper::PWrapper(QObject *parent):QObject(parent),m_bench(0)
{
    mp = new QProcess;
    
    mp->setProcessChannelMode(QProcess::MergedChannels);   
    connect(mp,SIGNAL(readyRead()),SLOT(handleOutput()));
//    connect(mp,SIGNAL(error(QProcess::ProcessError)),SLOT(handleError(QProcess::ProcessError)));
    connect(mp,SIGNAL(started()),SLOT(onStart()));
    connect(mp,SIGNAL(finished(int,QProcess::ExitStatus)),SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(this,SIGNAL(emitError(QString)),SLOT(onSyncGetError(QString)));
    connect(this,SIGNAL(emitBench(double)),SLOT(saveBench(double)));
}

void PWrapper::start(const QString &prog, const QStringList &arg,const QString & sat)
{ 
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.remove("PYTHONHOME");
        env.insert("PYTHONHOME", sat);
        mp->setProcessEnvironment(env);
#ifdef DEBUGB
       qDebug()<<Q_FUNC_INFO<<" cmd: "<<prog<<arg;
       qDebug()<<"PYTHONHOME: "<<sat;
#endif
       if (!deatach){
             mp->start(prog, arg);
             if(!mp->waitForFinished(5000000)){
                 Q_EMIT emitError(mp->errorString());
             }
         }else{
           //we need helper process here/
         auto ret=  mp->startDetached(prog,arg);
         if (!ret){
            onFinished(-1,QProcess::CrashExit);
           }else{
             onFinished(0,QProcess::NormalExit);
           }


         }
             
             
}

void PWrapper::handleError(QProcess::ProcessError e)
{//system error
    if (e==QProcess::FailedToStart){
        Q_EMIT emitError(tr("error proses %1 gagal dijalankan").arg(mp->program()));
        return;
     }
    if (e==QProcess::Crashed){
        Q_EMIT emitError(tr("error proses %1 mengalami crash ").arg(mp->program()));
        return;
    }
    if (e==QProcess::Timedout){
        Q_EMIT emitError(tr("error proses %1 mengalami waitfor... timeout ").arg(mp->program()));
        return;
    }
    if (e==QProcess::WriteError){
        Q_EMIT emitError(tr("error program %1 gagal menulis proses  ").arg(mp->program()));
        return;
    }
    if (e==QProcess::ReadError){
        Q_EMIT emitError(tr("error program %1 gagal membaca proses, mungkin program belum dijalankan").arg(mp->program()));
        return;
    }
    if (e==QProcess::UnknownError){
        Q_EMIT emitError(tr("error program %1 mengalamai kesalahan yang tidak diketahui").arg(mp->program()));
        return;
    }
    return; 
}

void PWrapper::handleOutput()
{
    m_data= mp->readAllStandardOutput();
    Q_EMIT emitOutput(QLatin1String(m_data)); 
}

void PWrapper::onStart()
{
    Q_EMIT emitStart();
}

void PWrapper::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    //handle parsing eror here
    if (exitCode==0){
        m_error.append("berhasil");
    }
    if (exitCode>=1){
        m_error.append("gagal");
    }
    if (exitCode<=-1){
        m_error.append("gagal");
    } 
    Q_EMIT emitFinish(exitCode,exitStatus);
} 
