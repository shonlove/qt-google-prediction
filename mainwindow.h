#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "manager_prediction.h"
#include "oauth2.h"
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //Show login dialog even if there is gotten already access_token.
    void startLogin(bool bForce);
    
private slots:
    void recvModelsList(const QVariantList& list);
    void recvModelDescription(const QVariant& var);
    void recvPredictionResult(const QVariant& var);
    void recvAddToModelComplete(bool result);

    void setApiKey();
    void login();

    void updateModelsList();
    void updateModelInfo();
    void deleteModel();
    void newModel();

    void addReplyText(QString str);

    void loginDone();
    void selectedModelChanged();
    void predict();

    void on_pbAddToModel_clicked();

    void on_pbTestStart_clicked();

    void testSendRequest();

    void on_pbTestClearResults_clicked();

private:
    OAuth2* m_pOAuth2;

    Ui::MainWindow *ui;
    QString m_organizationName;
    QString m_appName;
    QString m_API_key;
    QString m_ClientID;
    QString m_RedirectURI;
    ManagerPrediction m_managerPrediction;

    class VariantListModel *m_vlmModelsList;

    QTimer timerTest;
};

#endif // MAINWINDOW_H
