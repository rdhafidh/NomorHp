#include "introexportexcel.h"
#include "exportwizard.h"

IntroExportExcel::IntroExportExcel(QWidget *parent) :
    QWizardPage(parent),ui(new Ui::WizardPage)
{
ui->setupUi(this);
setPixmap(QWizard::BannerPixmap,QPixmap(":/importdialog/banner_excel.png"));
setPixmap(QWizard::LogoPixmap,QPixmap(":/importdialog/export_excel_logo.png"));
setPixmap(QWizard::WatermarkPixmap,QPixmap(":/importdialog/watermark_excel1.png"));
setTitle(tr("Pemandu Export Excel"));
setSubTitle(tr("Export Excel"));

}

IntroExportExcel::~IntroExportExcel()
{
delete ui;
}

int IntroExportExcel::nextId() const
{
    return ExportWizard::setupexportexcel;
}

void IntroExportExcel::initializePage()
{

}
