#include "importwizard.h"
#include "introimportexcel.h"
#include "setupfeedimportexcel.h"
#include "setupimportexcel.h"
#include "dataimportexcel.h"

ImportWizard::ImportWizard(QWidget *parent):QWizard(parent)
{
    in1=new IntroImportExcel;
    in2=new SetupImportExcel;
    in3=new SetupFeedImportExcel;
    in1->setStyleSheet("");
    in2->setStyleSheet("");
    in3->setStyleSheet("");
    setPage(introimportexcel,in1 );
    setPage(setupimportexcel,in2 );
    setPage(setupfeedimportexcel,in3 );
    setStartId(introimportexcel);
    setWizardStyle(QWizard::ModernStyle);
    setButtonText(QWizard::BackButton,tr("Kembali"));
    setButtonText(QWizard::NextButton,tr("Selanjutnya"));
    setButtonText(QWizard::CancelButton,tr("Batal"));
    setButtonText(QWizard::FinishButton,tr("Selesai"));
    setOptions(QWizard::NoBackButtonOnStartPage|QWizard::IndependentPages);

    connect(in3,&SetupFeedImportExcel::importexcelwork
            ,this,&ImportWizard::importexcelwork
            );
    connect(in3,&SetupFeedImportExcel::requestcleanupthread
            ,this,&ImportWizard::requestcleanupthread);

}

ImportWizard::~ImportWizard()
{
    in1->deleteLater();
    in2->deleteLater();
    in3->deleteLater();
}

void ImportWizard::setTableinfos(const DataImportExcel4SetupWizard &inf)
{
    in2->setTableInfos(inf);
    in3->setTableInfos(inf);
}

void ImportWizard::maxbarimportexcel(int n)
{
 in3->maxbarimportexcel(n);
}

void ImportWizard::valuebarimportexcel(int n)
{
in3->valuebarimportexcel(n);
}

void ImportWizard::resultimportexcel(ImportExcelResult ret ,const  QString &msg)
{
in3->resultimportexcel(ret,msg);
}

void ImportWizard::showmsgbar()
{
    in3->showmsgbar();
}

void ImportWizard::setmsgcontent(const QString &dat)
{
   in3->setmsgcontent(dat);
}
