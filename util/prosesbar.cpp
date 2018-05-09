#include "prosesbar.h"
#include "ui_prosesbar.h"
#include <QMessageBox>
#include <QPushButton>

prosesbar::prosesbar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prosesbar)
{
    ui->setupUi(this);
    ui->toolButton_cancel_berlangsung->setVisible (false);
}

prosesbar::~prosesbar()
{
    delete ui;
}

void prosesbar::setmsg(const char *msg)
{
    ui->label_msg->setText(msg);
}

void prosesbar::setmax(int n)
{
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(n);
}

void prosesbar::setprogressStep(int n)
{
    ui->progressBar->setValue(n);
}

void prosesbar::setEnableCancelTask(bool e)
{
    ui->toolButton_cancel_berlangsung->setVisible (e);
}

void prosesbar::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void prosesbar::on_toolButton_cancel_berlangsung_clicked()
{
    QMessageBox ask(this);
    ask.setIcon(QMessageBox::Information);

    ask.setInformativeText(tr("Apakah Anda yakin untuk melakukan cancel proses yang sedang berlangsung? "));
    ask.setText(tr("Konfirmasi cancel proses yang berlangsung."));
    QPushButton *ya = ask.addButton(tr("Ya"), QMessageBox::ActionRole);
    QPushButton *tidak =
        ask.addButton(tr("Tidak jadi"), QMessageBox::ActionRole);
    ask.setDefaultButton(tidak);
    ask.exec();
    if (ask.clickedButton() == tidak) {
      return;
    }
    if (ask.clickedButton() == ya) {
      this->asktocancelProsesBerlangsung ();
      return;
    }
}
