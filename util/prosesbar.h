#ifndef PROSESBAR_H
#define PROSESBAR_H

#include <QDialog>
#include <QString>

namespace Ui {
class prosesbar;
}

class prosesbar : public QDialog
{
    Q_OBJECT
    
public:
    explicit prosesbar(QWidget *parent = 0);
    ~prosesbar();
    void setmsg(const char *msg);
    void setmax(int n);
    void setprogressStep(int n);
    void setEnableCancelTask(bool e);
    
Q_SIGNALS:
    void asktocancelProsesBerlangsung();
    
protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_toolButton_cancel_berlangsung_clicked();
    
private:
    Ui::prosesbar *ui;
};

#endif // PROSESBAR_H
