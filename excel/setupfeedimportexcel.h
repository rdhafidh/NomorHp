#ifndef SETUPFEEDIMPORTEXCEL_H
#define SETUPFEEDIMPORTEXCEL_H

#include <QWizardPage>
#include "dataimportexcel.h"
#include "prosesbar.h"

namespace Ui {
class SetupFeedImportExcel;
}

class SetupFeedImportExcel : public QWizardPage
{
    Q_OBJECT

public:
    explicit SetupFeedImportExcel(QWidget *parent = 0);
    ~SetupFeedImportExcel();
    void setTableInfos(const DataImportExcel4SetupWizard& inf);
Q_SIGNALS:
    void requestcleanupthread();
    void importexcelwork(const DataImportExcel & in);

public Q_SLOTS:
    void maxbarimportexcel(int n);
    void valuebarimportexcel(int n);
    void resultimportexcel(ImportExcelResult ret, const QString &msg);
    void showmsgbar();
    void setmsgcontent(const QString &dat);
protected:
    int nextId()const override;
    void initializePage() override;
    bool isComplete()const override;

private slots:
    void on_toolButton_namafiletoimport_clicked();

    void on_toolButton_doimportexcel_clicked();
private:
    Ui::SetupFeedImportExcel *ui;
    prosesbar *pb;
    bool finishedornot=false;
    QHash<int,QLabel*>mlabels;
    QHash<int,QLineEdit*>medits1;
    QHash<int,QLineEdit*>medits2;
    DataImportExcel di;
};

#endif // SETUPFEEDIMPORTEXCEL_H
