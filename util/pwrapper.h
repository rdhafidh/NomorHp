#ifndef PWrapper_H
#define PWrapper_H

#include <QtCore>
#include <chrono>
               
class PWrapper:public QObject
{
    Q_OBJECT
public:

    PWrapper(QObject *parent=0);
    ~PWrapper(){

          mp->deleteLater();
    };
     void start(const QString &prog, const QStringList &arg, const QString &sat);
     void startwithpwd(const QString &prog, const QStringList &arg, const QString &sat,const QString &pwd)
     {
         QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
         env.insert("PYTHONHOME", sat); 
         env.insert("PGPASSWORD",pwd);
         mp->setProcessEnvironment(env);
         
 #ifdef DEBUGB
         qDebug()<<Q_FUNC_INFO<<" cmd: "<<prog<<arg;
//         qDebug()<<"ENV: "<<env.toStringList();
 #endif
              mp->start(prog, arg);
              if(!mp->waitForFinished(5000000)){
                  Q_EMIT emitError(mp->errorString());
              } 
         
     }

    QByteArray datas()const {return m_data;}
    QString lastError()const { return m_error; }
     double selama()const {return m_bench;}
     void setdeatach(bool t){deatach=t;}
private Q_SLOTS:
    void handleError(QProcess::ProcessError e);
    void handleOutput();
    void onStart();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onSyncGetError(const QString &err){
        m_error=err;
    }
    void saveBench(double b){
        m_bench=b;
    }

Q_SIGNALS:
    void emitError(const QString & err);
    void emitOutput( QString o); //SEND BY VALUE AND MOVE ITERATOR SHOULD BE USED
    void emitFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void emitStart();
    void emitBench(double a);
private: 
    bool deatach=false;
    QProcess *mp;
    QByteArray m_data;
    QString m_error;
    double m_bench;
};

#endif // PWrapper_H
