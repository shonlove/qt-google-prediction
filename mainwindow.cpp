#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings_api.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include "new_model_dialog.h"
#include <QStatusBar>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    m_pOAuth2 = new OAuth2(this);
    m_API_key = m_pOAuth2->getSimpleAPIKey();

    connect(m_pOAuth2, SIGNAL(loginDone()), this, SLOT(loginDone()));


    m_managerPrediction.setAPIKey(m_API_key);

    connect(&m_managerPrediction, SIGNAL(replyText(QString)), this, SLOT(addReplyText(QString)));
    connect(&m_managerPrediction, SIGNAL(recvModelsList(QVariantList)), this, SLOT(recvModelsList(QVariantList)));
    connect(&m_managerPrediction, SIGNAL(recvModelDescription(QVariant)), this, SLOT(recvModelDescription(QVariant)));
    connect(&m_managerPrediction, SIGNAL(recvPredictionResult(QVariant)), this, SLOT(recvPredictionResult(QVariant)));
    connect(&m_managerPrediction, SIGNAL(recvAddToModelComplete(bool)), this, SLOT(recvAddToModelComplete(bool)));

    connect(ui->lwModelsList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedModelChanged()));

    connect(ui->actionDeleteModel, SIGNAL(triggered()), this, SLOT(deleteModel()));
    connect(ui->actionNewModel, SIGNAL(triggered()), this, SLOT(newModel()));
    connect(ui->actionUpdateModelsList, SIGNAL(triggered()), this, SLOT(updateModelsList()));
    connect(ui->actionUpdateModelInfo, SIGNAL(triggered()), this, SLOT(updateModelInfo()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->predictButton, SIGNAL(clicked()), this, SLOT(predict()));


    connect(ui->lineEditTextToPredict, SIGNAL(returnPressed()), this, SLOT(predict()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(login()));

    connect(&timerTest, SIGNAL(timeout()), this, SLOT(testSendRequest()));

    startLogin(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startLogin(bool bForce)
{
    //Now we allow to start logging in when m_oauth2.isAuthorized().
    //User can log in using another Google account.
    if (bForce) {
        m_pOAuth2->startLogin(true);
        return;
    }

    if(!m_pOAuth2->isAuthorized())
    {
        m_pOAuth2->startLogin(bForce); //this is a parent widget for a login dialog.
    }
    else
    {
        loginDone();
    }
}

void MainWindow::setApiKey()
{
    SettingsAPI formSettingsAPI(m_organizationName, m_appName, this);
    if (formSettingsAPI.exec() == QDialog::Accepted)
    {
        QSettings settings(m_organizationName, m_appName);
        m_API_key = settings.value("API_KEY").toString();
        m_ClientID = settings.value("ClientID").toString();
        m_RedirectURI = settings.value("RedirectURI").toString();

        m_managerPrediction.setAPIKey(m_API_key);
    }
}

void MainWindow::updateModelsList()
{
    statusBar()->showMessage("Getting list of models.", 2000);
    m_managerPrediction.getModelsList();
}

void MainWindow::addReplyText(QString str)
{
    ui->teHTTPReplies->setPlainText(str);
}

void MainWindow::loginDone()
{
    m_managerPrediction.setAccessToken(m_pOAuth2->accessToken());
    statusBar()->showMessage("Getting list of models.", 2000);
    m_managerPrediction.getModelsList();
}

void MainWindow::updateModelInfo()
{
    QList<QListWidgetItem *> sel_items = ui->lwModelsList->selectedItems();
    if (sel_items.isEmpty())
    {
        return;
    }
    m_managerPrediction.getModelStatus(sel_items[0]->text());
}

void MainWindow::login()
{
    startLogin(true);
}

void MainWindow::recvModelsList(const QVariantList &list)
{
    ui->lwModelsList->clear();

    QVariant var;
    QString str;
    for (int i=0; i < list.size(); i++)
    {
        var = list.at(i);
        str = var.toMap()["id"].toString();
        ui->lwModelsList->insertItem(0, new QListWidgetItem(str));
    }
    ui->lwModelDescription->clear();
    ui->lineEditTextToPredict->clear();
}

void MainWindow::recvModelDescription(const QVariant &var)
{
    ui->lwModelDescription->clear();
    QVariant var2 = var.toMap()["modelInfo"];
    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Instances - " + var2.toMap()["numberInstances"].toString()));
    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Accuracy - " + var2.toMap()["classificationAccuracy"].toString()));
    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Labels - " + var2.toMap()["numberLabels"].toString()));
    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Type - " + var2.toMap()["modelType"].toString()));

    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Training - " + var.toMap()["trainingStatus"].toString()));
    ui->lwModelDescription->insertItem(0, new QListWidgetItem("Name - " + var.toMap()["id"].toString()));
}

void MainWindow::recvPredictionResult(const QVariant &var)
{
    ui->lwPredictResult->clear();
    ui->cbAddToModel->clear();
    QVariantList list = var.toMap()["outputMulti"].toList();

    QString str;
    str = var.toMap()["outputLabel"].toString();
    if (str == "")
         str = var.toMap()["outputValue"].toString();
    ui->lPredictResult->setText(str);

    //ui->lwTestResults->insertItem(0, new QListWidgetItem(str));
    QString _str = ui->teTestResults->toPlainText() + str + "\n" ;
    ui->teTestResults->setPlainText(_str);

    QVariant var2;

    for (int i=0; i < list.size(); i++)
    {
        var2 = list.at(i);
        str = var2.toMap()["label"].toString() + ": " + var2.toMap()["score"].toString();
        ui->lwPredictResult->insertItem(0, new QListWidgetItem(str));
        ui->cbAddToModel->addItem(var2.toMap()["label"].toString());
    }
    ui->cbAddToModel->setCurrentIndex( ui->cbAddToModel->findText(ui->lPredictResult->text()) );
}

void MainWindow::recvAddToModelComplete(bool result)
{
    if (result)
    {
        statusBar()->showMessage("Data added to model.", 3000);
    }
    if (!result)
    {
        statusBar()->showMessage("Error while adding data to model.", 3000);
    }
}


void MainWindow::deleteModel()
{
    if (!ui->lwModelsList->currentItem())
        return;
    int res = QMessageBox::warning(this, "Warning",
                                   QString("Are you sure you want to delete model '%1'?").arg(ui->lwModelsList->currentItem()->text()),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(res != QMessageBox::Yes)
    {
        return;
    }
    m_managerPrediction.deleteModel(ui->lwModelsList->currentItem()->text());
}

void MainWindow::newModel()
{
    NewModelDialog dlg(this);
    int res = dlg.exec();
    if(res != QDialog::Accepted)
    {
        return;
    }
    QString name, cloudPath;
    dlg.getNewModelParams(name, cloudPath);
    m_managerPrediction.insertModel(name, cloudPath);
}

void MainWindow::predict()
{
    if (!ui->lwModelsList->currentItem())
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected model to predict."));
        return;
    }
    if (ui->lineEditTextToPredict->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Empty text for prediction."));
        return;
    }

    QString currentModel = ui->lwModelsList->currentItem()->text();
    QStringList texts;
    if(currentModel == "moscow_weather")
    {
        texts = ui->lineEditTextToPredict->text().split(",");
        for(int i = 0; i < texts.count(); ++i)
        {
            texts[i] = texts[i].trimmed();
        }
        if(texts.count() != 2)
        {
            QMessageBox::information(this, tr("Information"),
                                     tr("The string you entered cannot be splitted into two values.") + "\n\n\n" +
                                     tr("For prediction you need to enter the string:") + "\n\n"+
                                     tr("125, Spring") + "\n\n" +
                                     tr("Here 125 - is the day of the year (from 1 to 365)") + "\n" +
                                     tr("Spring - is the season, can be one of those:") +"\n\n" +
                                     tr("Winter, Spring, Summer, Fall."));
            return;
        }
        bool bOk;
        int n = texts[0].toInt(&bOk);
        if(!bOk)
        {
            QMessageBox::information(this, tr("Information"),
                                     tr("You entered a wrong day of the year '%1'.").arg(texts[0]) + "\n\n\n" +
                                     tr("For prediction you need to enter the string:") + "\n\n"+
                                     tr("125, Spring") + "\n\n" +
                                     tr("Here 125 - is the day of the year (from 1 to 365)") + "\n" +
                                     tr("Spring - is the season, can be one of those:") +"\n\n" +
                                     tr("Winter, Spring, Summer, Fall."));
            return;
        }
        if(n < 1 || n > 366)
        {
            QMessageBox::information(this, tr("Information"),
                                     tr("You entered a wrong day of the year '%1'.").arg(n) + "\n\n\n" +
                                     tr("For prediction you need to enter the string:") + "\n\n"+
                                     tr("125, Spring") + "\n\n" +
                                     tr("Here 125 - is the day of the year (from 1 to 365)") + "\n" +
                                     tr("Spring - is the season, can be one of those:") +"\n\n" +
                                     tr("Winter, Spring, Summer, Fall."));
            return;
        }
        QString season  = texts[1].trimmed();
        if(season != "Winter" && season != "Spring" && season != "Summer" && season != "Fall")
        {
            QMessageBox::information(this, tr("Information"),
                                     tr("You entered a wrong season name '%1'.").arg(season) + "\n\n\n" +
                                     tr("For prediction you need to enter the string:") + "\n\n"+
                                     tr("125, Spring") + "\n\n" +
                                     tr("Here 125 - is the day of the year (from 1 to 365)") + "\n" +
                                     tr("Spring - is the season, can be one of those:") +"\n\n" +
                                     tr("Winter, Spring, Summer, Fall."));
            return;
        }
    }
    else if(currentModel == "potential_buyers")
    {
        texts = ui->lineEditTextToPredict->text().split(",");
        for(int i = 0; i < texts.count(); ++i)
        {
            texts[i] = texts[i].trimmed();
        }
        if(texts.count() != 6)
        {
            QMessageBox::information(this, tr("Information"),
                                     tr("The string you entered cannot be splitted into six values.") + "\n\n\n" +
                                     tr("For prediction you need to enter the string:") + "\n\n"+
                                     tr("43, m, flat, 1, married, 6") + "\n\n" +
                                     tr("Here 43 - is the age of a person") + "\n" +
                                     tr("m - sex (m or f)") +"\n" +
                                     tr("flat - type of housing (can be flat or house)") +"\n" +
                                     tr("1 - number of cars in the family") +"\n" +
                                     tr("married - marital status (can be married or single)") +"\n" +
                                     tr("6 - number of children in the family."));
            return;
        }
    }
    else
    {
        //if(currentModel == "types_of_sentence" || currentModel == "lang_identifier" || currentModel == "lang_id_russian"
        //        || currentModel == "even-odd" || currentModel == "less-greater-100")
        //{

        //One string models.
        texts << ui->lineEditTextToPredict->text();
        //}

    }
    statusBar()->showMessage("Trying to predict.", 2000);
    m_managerPrediction.getPredict(ui->lwModelsList->currentItem()->text(), texts);
}

void MainWindow::on_pbAddToModel_clicked()
{
    if (!ui->lwModelsList->currentItem())
    {
        QMessageBox::warning(this, tr("Warning"), tr("No selected model."));
        return;
    }
    if (ui->cbAddToModel->currentText().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Empty Label."));
        return;
    }
    if (ui->lineEditTextToPredict->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Empty prediction string."));
        return;
    }

    QStringList texts;
    texts << ui->lineEditTextToPredict->text();


    m_managerPrediction.addDataToModel(ui->lwModelsList->currentItem()->text(),
                                       ui->cbAddToModel->currentText(), texts);

    statusBar()->showMessage("Trying to add data to model.", 2000);
}

void MainWindow::selectedModelChanged()
{
    ui->cbAddToModel->clear();
    QString currentModel = ui->lwModelsList->currentItem()->text();
    if(currentModel == "types_of_sentence")
    {
        ui->lineEditTextToPredict->setText("How is it going?");
        ui->textEditDescription->setHtml("This model is to predict if the sentence is <b>Interrogative</b>, <b>Declarative</b> or <b>Exclamation</b>. <br>Enter the sentence in English, and click <b>Predict</b> button.");
        QStringList lst;
        lst << "Interrogative" << "Declarative" << "Exclamation";
        ui->cbAddToModel->addItems(lst);
    }
    else if(currentModel == "lang_identifier")
    {
        ui->lineEditTextToPredict->setText(QString::fromUtf8("En efeto, rematado ya su juicio, vino a dar en el más estraño."));
        ui->textEditDescription->setHtml("This model is to predict if the sentence is in <b>English</b>, <b>Spanish</b> or <b>French</b>. <br>Enter the sentence in any of those languages, and click <b>Predict</b> button.");
        QStringList lst;
        lst << "English" << "French" << "Spanish";
        ui->cbAddToModel->addItems(lst);
    }
    else if(currentModel == "lang_id_russian")
    {
        ui->lineEditTextToPredict->setText(QString::fromUtf8("Добрый вечер, дорогие друзья!"));
        ui->textEditDescription->setHtml("This model is to predict if the sentence is in <b>English</b>, <b>Spanish</b>, <b>French</b> or <b>Russian</b>. <br>Enter the sentence in any of those languages, and click <b>Predict</b> button.");
        QStringList lst;
        lst << "English" << "French" << "Spanish" << "Russian";
        ui->cbAddToModel->addItems(lst);
    }
    else if(currentModel == "moscow_weather")
    {
        ui->lineEditTextToPredict->setText("240, Summer");
        ui->textEditDescription->setHtml("This model is to predict the temperature in <b>Moscow</b> in centigrade. <br>Enter the day of year and the season:<br> <b>240</b>, <b>Summer</b>");
    }
    else if(currentModel == "potential_buyers")
    {
        ui->lineEditTextToPredict->setText("43, m, flat, 1, married, 6");
        ui->textEditDescription->setHtml("This model is to predict if the customer will buy a bike. <br>Enter the following string for prediction:<br> <b>age, sex(m or f), house type (flat or house), number of cars, marital status (married or single), number of children</b>.");
        QStringList lst;
        lst << "yes" << "no";
        ui->cbAddToModel->addItems(lst);
    }
    else
    {
        ui->lineEditTextToPredict->setText("");
        ui->textEditDescription->setHtml("");
    }

    ui->lwPredictResult->clear();
    ui->lPredictResult->setText("unknown");


    updateModelInfo();
}

void MainWindow::on_pbTestStart_clicked()
{
    if (timerTest.isActive())
    {
        timerTest.stop();
        return;
    }
    ui->sbTestNow->setValue(ui->sbTestFrom->value());
    ui->pbTestWeather->setMinimum(ui->sbTestFrom->value());
    ui->pbTestWeather->setMaximum(ui->sbTestTo->value());
    ui->pbTestWeather->setValue(ui->sbTestNow->value());
    timerTest.start(250);
}

void MainWindow::testSendRequest()
{
    if (ui->sbTestNow->value() > ui->sbTestTo->value())
    {
        timerTest.stop();
        return;
    }
    if (!ui->lwModelsList->currentItem())
    {
        ui->sbTestNow->setValue(-1);
        timerTest.stop();
        return;
    }
    ui->sbTestNow->setValue(ui->sbTestNow->value() + ui->sbTestStep->value());
    ui->pbTestWeather->setValue(ui->sbTestNow->value());

    QString currentModel = ui->lwModelsList->currentItem()->text();
    QStringList texts;
    texts.append(QString::number(ui->sbTestNow->value()));
    texts.append(ui->cbTestSeason->currentText());
    m_managerPrediction.getPredict(currentModel, texts);
}

void MainWindow::on_pbTestClearResults_clicked()
{
    ui->teTestResults->clear();
}

