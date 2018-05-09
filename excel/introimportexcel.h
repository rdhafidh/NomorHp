#ifndef INTROIMPORTEXCEL_H
#define INTROIMPORTEXCEL_H

#include <QWizardPage>

namespace Ui {
class IntroImportExcel;
}

class IntroImportExcel : public QWizardPage
{
    Q_OBJECT

public:
    explicit IntroImportExcel(QWidget *parent = 0);
    ~IntroImportExcel();

protected:
    int nextId()const override;

private:
    Ui::IntroImportExcel *ui;
};

#endif // INTROIMPORTEXCEL_H
