#ifndef SETUPEXPORTEXCEL_H
#define SETUPEXPORTEXCEL_H

#include <QWizardPage>
#include "prosesbar.h"
#include "exportexcelresult.h"
#include <QScopedPointer>

namespace Ui {
class SetupExportExcel;
}


class SetupExportExcel : public QWizardPage
{
    Q_OBJECT

public:
    explicit SetupExportExcel(QWidget *parent = 0);
    ~SetupExportExcel();

Q_SIGNALS:
    void exportexcel(const QString &tbltitle,bool use_62,bool is_mode_csv_or_excel);
    void requestcleanupthread();

public Q_SLOTS:
    void maxbarexportexcel(int n);
    void valuebarexportexcel(int n);
    void resultexportexcel(ExportExcelResult ret);
    void showmsg();

protected:
    int nextId()const override;
    bool isComplete() const override;
    void initializePage() override;

private Q_SLOTS:

    void on_toolButton_doexport_clicked();
    void on_lineEdit_judultabel_textChanged(const QString &arg1);
    
    void on_comboBox_mode_export_is_csv_or_excel_currentIndexChanged(int index);
    
private:
    Ui::SetupExportExcel *ui;
    prosesbar* pb;
    //default csv
    bool mode_csv_or_excel=true;
    bool exportdone=false;
};

#endif // SETUPEXPORTEXCEL_H
