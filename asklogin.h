#ifndef ASKLOGIN_H
#define ASKLOGIN_H

#include <QDialog>

namespace Ui {
class AskLogin;
}

class AskLogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit AskLogin(QWidget *parent = 0);
    ~AskLogin();
    
Q_SIGNALS:
    void askexit();
    void asklogin(const QByteArray &configjson);
    
private slots:
    void on_checkBox_clicked(bool checked);
    
    void on_pushButton_batal_clicked();
    
    void on_pushButton_login_clicked();
    
private:
    Ui::AskLogin *ui;
};

#endif // ASKLOGIN_H
