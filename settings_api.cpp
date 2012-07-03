#include "settings_api.h"
#include "ui_settings_api.h"
#include <QSettings>

SettingsAPI::SettingsAPI(const QString &organizationName, const QString &appName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsAPI)
{
    ui->setupUi(this);
    m_organizationName = organizationName;
    m_appName = appName;
    QSettings settings(m_organizationName, m_appName);
    ui->leAPIKey->setText(settings.value("API_KEY").toString());
    ui->leClientID->setText(settings.value("ClientID").toString());
    ui->leRedirectURI->setText(settings.value("RedirectURI").toString());
}

SettingsAPI::~SettingsAPI()
{
    delete ui;
}

void SettingsAPI::on_buttonBox_accepted()
{
    QSettings settings(m_organizationName, m_appName);
    settings.setValue("API_KEY", ui->leAPIKey->text());
    settings.setValue("ClientID", ui->leClientID->text());
    settings.setValue("RedirectURI", ui->leRedirectURI->text());

    QDialog::accept();
}

void SettingsAPI::on_buttonBox_rejected()
{
    QDialog::reject();
}
