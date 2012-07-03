#ifndef SETTINGS_API_H
#define SETTINGS_API_H

#include <QDialog>

namespace Ui {
class SettingsAPI;
}

class SettingsAPI : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsAPI(const QString & organizationName, const QString & appName, QWidget *parent = 0);
    ~SettingsAPI();
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SettingsAPI *ui;
    QString m_organizationName;
    QString m_appName;
};

#endif // SETTINGS_API_H
