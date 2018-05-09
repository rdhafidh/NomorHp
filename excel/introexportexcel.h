#ifndef INTROEXPORTEXCEL_H
#define INTROEXPORTEXCEL_H

#include <QWizardPage>
#include "ui_introexportexcel.h"


class IntroExportExcel : public QWizardPage
{
    Q_OBJECT

public:
    explicit IntroExportExcel(QWidget *parent = 0);
    ~IntroExportExcel();

protected:
    int nextId()const override;
    void initializePage() override;

private:
    Ui::WizardPage *ui;

};

#endif // INTROEXPORTEXCEL_H
