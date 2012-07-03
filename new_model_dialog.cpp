#include "new_model_dialog.h"
#include "ui_new_model_dialog.h"
#include <QMessageBox>

NewModelDialog::NewModelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewModelDialog)
{
    ui->setupUi(this);
}

NewModelDialog::~NewModelDialog()
{
    delete ui;
}

void NewModelDialog::accept()
{
    if(ui->lineEditModelName->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Empty Model name."));
        return;
    }
    if(ui->lineEditCloudStoragePath->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Empty Cloud Storage path."));
        return;
    }
    QDialog::accept();
}

void NewModelDialog::getNewModelParams(QString& name, QString& cloudPath)
{
    name = ui->lineEditModelName->text();
    cloudPath = ui->lineEditCloudStoragePath->text();
}

