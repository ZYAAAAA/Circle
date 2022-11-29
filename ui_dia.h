/********************************************************************************
** Form generated from reading UI file 'dia.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIA_H
#define UI_DIA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dia
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QTextEdit *multiple;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QTextEdit *numlayer;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_5;
    QTextEdit *Radius;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QTextEdit *X_num;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QTextEdit *Y_num;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *OKButton;
    QPushButton *CButton;

    void setupUi(QDialog *Dia)
    {
        if (Dia->objectName().isEmpty())
            Dia->setObjectName(QStringLiteral("Dia"));
        Dia->resize(813, 544);
        layoutWidget = new QWidget(Dia);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(160, 30, 491, 393));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label);

        multiple = new QTextEdit(layoutWidget);
        multiple->setObjectName(QStringLiteral("multiple"));

        horizontalLayout->addWidget(multiple);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_2);

        numlayer = new QTextEdit(layoutWidget);
        numlayer->setObjectName(QStringLiteral("numlayer"));

        horizontalLayout_2->addWidget(numlayer);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_5);

        Radius = new QTextEdit(layoutWidget);
        Radius->setObjectName(QStringLiteral("Radius"));

        horizontalLayout_6->addWidget(Radius);

        horizontalLayout_6->setStretch(0, 1);
        horizontalLayout_6->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_3);

        X_num = new QTextEdit(layoutWidget);
        X_num->setObjectName(QStringLiteral("X_num"));

        horizontalLayout_3->addWidget(X_num);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label_4);

        Y_num = new QTextEdit(layoutWidget);
        Y_num->setObjectName(QStringLiteral("Y_num"));

        horizontalLayout_4->addWidget(Y_num);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_4);


        verticalLayout_2->addLayout(verticalLayout);

        layoutWidget1 = new QWidget(Dia);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(210, 459, 391, 51));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        OKButton = new QPushButton(layoutWidget1);
        OKButton->setObjectName(QStringLiteral("OKButton"));

        horizontalLayout_5->addWidget(OKButton);

        CButton = new QPushButton(layoutWidget1);
        CButton->setObjectName(QStringLiteral("CButton"));

        horizontalLayout_5->addWidget(CButton);


        retranslateUi(Dia);

        QMetaObject::connectSlotsByName(Dia);
    } // setupUi

    void retranslateUi(QDialog *Dia)
    {
        Dia->setWindowTitle(QApplication::translate("Dia", "Dialog", nullptr));
        label->setText(QApplication::translate("Dia", "\346\224\276\345\244\247\345\200\215\347\216\207", nullptr));
        label_2->setText(QApplication::translate("Dia", "\346\257\217\345\261\202\345\261\202\345\216\232", nullptr));
        label_5->setText(QApplication::translate("Dia", "\347\233\256\346\240\207\345\215\212\345\276\204", nullptr));
        label_3->setText(QApplication::translate("Dia", "X\350\275\264\345\210\206\350\276\250\347\216\207", nullptr));
        label_4->setText(QApplication::translate("Dia", "Y\350\275\264\345\210\206\350\276\250\347\216\207", nullptr));
        OKButton->setText(QApplication::translate("Dia", "OK", nullptr));
        CButton->setText(QApplication::translate("Dia", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dia: public Ui_Dia {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIA_H
