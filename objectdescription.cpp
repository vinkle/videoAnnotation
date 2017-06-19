#include "objectdescription.h"
#include "ui_objectdescription.h"

objectDescription::objectDescription(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::objectDescription)
{
    ui->setupUi(this);

    ui->radioNo->setChecked(true);

    occuludedVal = false;
}

objectDescription::~objectDescription()
{
    delete ui;
}

void objectDescription::on_buttonBox_accepted()
{
    name = ui->nameObject->text();
    if(ui->radioYes->isChecked())
    {
        occuludedVal = true;
    }
    else
    {
        occuludedVal = false;
    }
    emit sendData(name, occuludedVal);
    this->close();
}

void objectDescription::on_buttonBox_rejected()
{
    emit sendData("", false);
    this->close();
}
