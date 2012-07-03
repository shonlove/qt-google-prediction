#ifndef MANAGER_PREDICTION_H
#define MANAGER_PREDICTION_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

class ManagerPrediction : public QObject
{
    Q_OBJECT
public:
    explicit ManagerPrediction(QObject *parent = 0);

    void setAPIKey(QString str)
    {m_API_Key = str;};

    void setAccessToken(QString str)
    {m_Access_Token = str;};

    void getModelsList();
    void getModelStatus(QString model);
    void deleteModel(QString model);
    void insertModel(QString model, QString dataPath);
    void getPredict(QString model, QStringList texts);
    void addDataToModel(QString model, QString label, QStringList texts);

signals:
    void replyText(const QString& text);
    void errorOccured(const QString& error);
    void recvModelsList(const QVariantList&);
    void recvModelDescription(const QVariant&);
    void recvPredictionResult(const QVariant&);
    void recvAddToModelComplete(bool result);

private slots:
    void replyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_pNetworkAccessManager;
    QString m_API_Key;
    QString m_Access_Token;
};

#endif // MANAGER_PREDICTION_H

