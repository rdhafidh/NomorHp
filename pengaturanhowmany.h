#ifndef PENGATURANHOWMANY_H
#define PENGATURANHOWMANY_H

#include <QDialog>

namespace Ui {
class PengaturanHowMany;
}

class PengaturanHowMany : public QDialog
{
    Q_OBJECT
    
public:
    explicit PengaturanHowMany(QWidget *parent = 0);
    ~PengaturanHowMany();
    
private slots:
    void on_pushButton_simpan_clicked();
    
    void on_pushButton_batal_clicked();
    
private:
    Ui::PengaturanHowMany *ui;
};

#endif // PENGATURANHOWMANY_H
