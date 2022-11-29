﻿#ifndef DIA_H
#define DIA_H

#include <QDialog>

namespace Ui
{
    class Dia;
}

class Dia : public QDialog
{
    Q_OBJECT

public:
    explicit Dia(QWidget* parent = 0);
    ~Dia();
    float f;
    float mlayer;
    float radius;
    int x, y;
private slots:
    void on_OKButton_clicked();

    void on_CButton_clicked();

private:
    Ui::Dia* ui;

};

#endif // DIA_H
