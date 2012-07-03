#ifndef NEW_MODEL_DIALOG_H
#define NEW_MODEL_DIALOG_H

#include <QDialog>

namespace Ui {
    class NewModelDialog;
}

class NewModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewModelDialog(QWidget *parent = 0);
    ~NewModelDialog();

    void getNewModelParams(QString& name, QString& cloudPath);

    void accept();

private:
    Ui::NewModelDialog *ui;
};

#endif // NEW_MODEL_DIALOG_H
