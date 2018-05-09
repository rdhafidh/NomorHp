#ifndef SETUPIMPORTEXCEL_H
#define SETUPIMPORTEXCEL_H

#include <QWizardPage>
#include "dataimportexcel.h"

namespace Ui {
class SetupImportExcel;
}

class SetupImportExcel : public QWizardPage
{
    Q_OBJECT

public:
    explicit SetupImportExcel(QWidget *parent = 0);
    ~SetupImportExcel();
    void setTableInfos(const DataImportExcel4SetupWizard& inf);

protected:
    int nextId() const override;
    void initializePage() override;

private:
    Ui::SetupImportExcel *ui;
    DataImportExcel4SetupWizard info;
    bool foundinvalidcol=false;
};

#endif // SETUPIMPORTEXCEL_H
