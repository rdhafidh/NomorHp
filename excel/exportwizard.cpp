#include "exportwizard.h"
#include "introexportexcel.h"
#include "setupexportexcel.h"

ExportWizard::ExportWizard(QWidget *parent) :
    QWizard(parent)
{
    in1=new IntroExportExcel;
    in2=new SetupExportExcel;
    connect(in2,&SetupExportExcel::requestcleanupthread,
            this,&ExportWizard::requestcleanupthread);
    connect(in2,&SetupExportExcel::exportexcel,this
            ,&ExportWizard::exportexcel);

setPage(introexportexcel,in1);
setPage(setupexportexcel,in2 );
setStartId(introexportexcel);

setWizardStyle(QWizard::ModernStyle);

setButtonText(QWizard::BackButton,tr("Kembali"));
setButtonText(QWizard::NextButton,tr("Selanjutnya"));
setButtonText(QWizard::CancelButton,tr("Batal"));
setButtonText(QWizard::FinishButton,tr("Selesai"));
setOption(QWizard::NoBackButtonOnStartPage);
}

ExportWizard::~ExportWizard()
{
in1->deleteLater();
in2->deleteLater();
}

void ExportWizard::maxbarexportexcel(int n)
{
    in2->maxbarexportexcel(n);
}

void ExportWizard::valuebarexportexcel(int n)
{
    in2->valuebarexportexcel(n);
}

void ExportWizard::resultexportexcel(ExportExcelResult res)
{
    in2->resultexportexcel(res);
}

void ExportWizard::showmsgbar()
{
    in2->showmsg();
}
