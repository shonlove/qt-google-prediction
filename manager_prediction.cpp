#include "manager_prediction.h"
#include <QDebug>
#include <QJson/Parser>
#include <QApplication>


ManagerPrediction::ManagerPrediction(QObject *parent) : QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));


}

void ManagerPrediction::getModelsList()
{
    //QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/list?pp=1&key=%1").arg(m_API_Key);
    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/list?pp=1");
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->get(request);

    qDebug() << "Send URL === " << url;
}

void ManagerPrediction::getModelStatus(QString model)
{
    if (model.isEmpty())
        return;
    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/%1?pp=1").arg(model);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->get(request);

    qDebug() << "Send URL === " << url;
}

void ManagerPrediction::deleteModel(QString model)
{
    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/%1?pp=1").arg(model);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->deleteResource(request);

    qDebug() << "Send URL === " << url;
}

void ManagerPrediction::insertModel(QString model, QString dataPath)
{
    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels?pp=1");
    QString json = QString("{ \"id\": \"%1\", \"storageDataLocation\": \"%2\" }").arg(model).arg(dataPath);

    QNetworkRequest request;
    request.setUrl( QUrl(url) );
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->post(request, json.toLatin1());

    qDebug() << "Send URL === " << url;
    qDebug() << "Send data === " << json.toLatin1();
}

void ManagerPrediction::getPredict(QString model, QStringList texts)
{

    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/%1/predict?pp=1").arg(model);

    //forming csvInstance for column 0-N
    QString strText = "\"" + texts.at(0) + "\"";
    for (int i=1; i < texts.size(); i++)
        strText += ", \"" + texts.at(i) + "\"";
    QString json = QString("{ \"input\": { \"csvInstance\" :[ %1 ] } }").arg(strText);

    QNetworkRequest request;
    request.setUrl( QUrl(url) );
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->post(request, json.toUtf8());

    qDebug() << "getPredict, Send URL === " << url;
    qDebug() << "Send data === " << json.toUtf8();

}

void ManagerPrediction::addDataToModel(QString model, QString label, QStringList texts)
{
    QString url = QString("https://www.googleapis.com/prediction/v1.5/trainedmodels/%1?pp=1").arg(model);

    //forming csvInstance for column 0-N
    QString strText = "\"" + texts.at(0) + "\"";
    for (int i=1; i < texts.size(); i++)
        strText += ", \"" + texts.at(i) + "\"";

    QString json = QString("{ \"label\": \"%1\", \"csvInstance\" :[ %2 ]  }").arg(label).arg(strText);

    QNetworkRequest request;
    request.setUrl( QUrl(url) );
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", (QString("Bearer %1").arg(m_Access_Token)).toLatin1());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pNetworkAccessManager->put(request, json.toUtf8());

    qDebug() << "Send URL === " << url;
    qDebug() << "Send data === " << json.toUtf8();
}

void ManagerPrediction::replyFinished(QNetworkReply *reply)
{
    QApplication::restoreOverrideCursor();
    QString json = reply->readAll();
    qDebug() << "Reply = " << json;
    qDebug() << "URL = " << reply->url();
    QString strUrl = reply->url().toString();

    emit replyText(json);

    QJson::Parser parser;

    bool ok;

    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);
    if(!ok)
    {
        emit errorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }


    //this is models list

    QString queryKind = result.toMap()["kind"].toString();

    qDebug() << "!!!!!!!!!!!" << queryKind;
    if (queryKind == "prediction#output")
    {
        emit recvPredictionResult(result);
        return;
    }
    if (queryKind == "prediction#list")
    {
        QVariantList _varList = result.toMap()["items"].toList();
        emit recvModelsList(_varList);
        return;
    }
    //this is models description
    if (queryKind == "prediction#training")
    {
        QString modelInfo = result.toMap()["trainingStatus"].toString();
        if (modelInfo != "")
            emit recvModelDescription(result);
        if (modelInfo == "")
            emit recvAddToModelComplete(true);

        return;
    }


}
