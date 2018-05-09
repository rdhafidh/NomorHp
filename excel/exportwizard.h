#ifndef EXPORTWIZARD_H
#define EXPORTWIZARD_H

#include <QWizard>
#include "exportexcelresult.h"

class IntroExportExcel;
class SetupExportExcel;
class ExportWizard : public QWizard
{
    Q_OBJECT

public:
    enum { introexportexcel=0,setupexportexcel};
    explicit ExportWizard(QWidget *parent = 0);
    ~ExportWizard();

Q_SIGNALS:
    void exportexcel(const QString & tbltitle,bool use_62,bool is_mode_csv_or_excel);
   void requestcleanupthread();

public Q_SLOTS:
    void maxbarexportexcel(int n);
    void valuebarexportexcel(int n);
    void resultexportexcel(ExportExcelResult ret);
    void showmsgbar();


protected:

private:
    IntroExportExcel *in1;
    SetupExportExcel*in2;

};

#endif // EXPORTWIZARD_H
