#include "dia.h"
#include "ui_dia.h"

Dia::Dia(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Dia)
{
    ui->setupUi(this);
    ui->multiple->setText("40");
    ui->X_num->setText("1920");
    ui->Y_num->setText("1080");
    ui->numlayer->setText("10");
    ui->Radius->setText("1.5");
    ui->radioCircle->setChecked(1);
    isCircle = true;
}

Dia::~Dia()
{
    delete ui;
}

void Dia::on_OKButton_clicked()
{
    f = ui->multiple->toPlainText().toFloat();
    x = ui->X_num->toPlainText().toFloat();
    y = ui->Y_num->toPlainText().toFloat();
    mlayer = ui->numlayer->toPlainText().toFloat();
    radius = ui->Radius->toPlainText().toFloat();
    QDialog::accept();
}

void Dia::on_CButton_clicked()
{
    QDialog::reject();
}

void Dia::on_radioCircle_clicked()
{
    isCircle = true;
}

void Dia::on_radioLine_clicked()
{
    isCircle = false;
}
