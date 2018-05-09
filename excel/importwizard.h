#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H
#include <QWizard>
#include "excel/dataimportexcel.h"

class IntroImportExcel;
class SetupImportExcel;
class SetupFeedImportExcel;
class ImportWizard : public QWizard
{
Q_OBJECT
public:
    explicit ImportWizard(QWidget *parent=0);
    ~ImportWizard();
    enum {introimportexcel,setupimportexcel,setupfeedimportexcel};

    void setTableinfos(const DataImportExcel4SetupWizard &inf);

public Q_SLOTS:
    void maxbarimportexcel(int n);
    void valuebarimportexcel(int n);
    void resultimportexcel(ImportExcelResult ret, const QString &msg);
    void showmsgbar();
     void setmsgcontent(const QString &dat);
Q_SIGNALS:

    void importexcelwork(const DataImportExcel & in);
     void requestcleanupthread();


private:
    IntroImportExcel *in1;
    SetupImportExcel *in2;
    SetupFeedImportExcel *in3;

};

#endif // IMPORTWIZARD_H
