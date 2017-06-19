#ifndef OBJECTDESCRIPTION_H
#define OBJECTDESCRIPTION_H

#include <QDialog>

namespace Ui {
class objectDescription;
}

class objectDescription : public QDialog
{
    Q_OBJECT

public:
    explicit objectDescription(QWidget *parent = 0);
    ~objectDescription();

signals:
    void sendData(const QString &, bool);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::objectDescription *ui;
    bool occuludedVal;
    QString name;
};

#endif // OBJECTDESCRIPTION_H
