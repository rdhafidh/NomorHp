#ifndef TENTANGAPLIKASI_H
#define TENTANGAPLIKASI_H

#include <QDialog>

namespace Ui {
class TentangAplikasi;
}

class TentangAplikasi : public QDialog
{
    Q_OBJECT
    
public:
    explicit TentangAplikasi(QWidget *parent = 0);
    ~TentangAplikasi();
    
private slots:
    void on_pushButton_tutup_clicked();
    
private:
    Ui::TentangAplikasi *ui;
};

#endif // TENTANGAPLIKASI_H
