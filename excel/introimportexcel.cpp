#include "introimportexcel.h"
#include "ui_introimportexcel.h"
#include "importwizard.h"

IntroImportExcel::IntroImportExcel(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::IntroImportExcel)
{
    ui->setupUi(this);
    setPixmap(QWizard::BannerPixmap,QPixmap(":/importdialog/banner_excel.png"));
    setPixmap(QWizard::LogoPixmap,QPixmap(":/importdialog/export_excel_logo.png"));
    setPixmap(QWizard::WatermarkPixmap,QPixmap(":/importdialog/watermark_excel1.png"));
    setTitle(tr("Pemandu Import Excel"));
    setSubTitle(tr("Import Excel"));

}

IntroImportExcel::~IntroImportExcel()
{
    delete ui;
}

int IntroImportExcel::nextId() const
{
    return ImportWizard::setupimportexcel;
}
