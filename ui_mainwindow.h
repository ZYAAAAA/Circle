/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QStackedWidget *stackedWidget_5;
    QWidget *stackedWidget_5Page1;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_5;
    QSlider *horizontalSlider_ImageIndex;
    QLabel *label_layer;
    QSpacerItem *horizontalSpacer;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *DefineButton;
    QPushButton *CalButton;
    QPushButton *SaveButton;
    QPushButton *ExportButton;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_12;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_3;
    QLabel *Max_;
    QHBoxLayout *horizontalLayout_10;
    QLabel *Min_2;
    QLabel *Min_;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *textEdit;
    QPushButton *Calgray;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_10;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_11;
    QLabel *label_9;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_5;
    QLabel *mlabel_1;
    QLabel *mlabel_2;
    QLabel *mlabel_3;
    QLabel *mlabel_4;
    QLabel *mlabel_5;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *pushButton_2;
    QPushButton *ImportButton;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1367, 1000);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        stackedWidget_5 = new QStackedWidget(centralWidget);
        stackedWidget_5->setObjectName(QStringLiteral("stackedWidget_5"));
        stackedWidget_5->setGeometry(QRect(160, 170, 1051, 691));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget_5->sizePolicy().hasHeightForWidth());
        stackedWidget_5->setSizePolicy(sizePolicy);
        stackedWidget_5->setMinimumSize(QSize(0, 410));
        stackedWidget_5->setStyleSheet(QLatin1String(";font:15px 'Arial';color:#ffffff;\n"
"background-color: rgb(194, 194, 194);"));
        stackedWidget_5Page1 = new QWidget();
        stackedWidget_5Page1->setObjectName(QStringLiteral("stackedWidget_5Page1"));
        horizontalLayoutWidget = new QWidget(stackedWidget_5Page1);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 1051, 587));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(horizontalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setMinimumSize(QSize(1040, 585));
        label_2->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));

        horizontalLayout_3->addWidget(label_2);

        layoutWidget = new QWidget(stackedWidget_5Page1);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 620, 1041, 29));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_8);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSlider_ImageIndex = new QSlider(layoutWidget);
        horizontalSlider_ImageIndex->setObjectName(QStringLiteral("horizontalSlider_ImageIndex"));
        horizontalSlider_ImageIndex->setMinimumSize(QSize(0, 25));
        horizontalSlider_ImageIndex->setStyleSheet(QLatin1String("QSlider::groove:horizontal {\n"
"	border: 0px solid #4A708B;\n"
"	background: #C0C0C0;\n"
"	height: 10px;\n"
"	border-radius: 1px;\n"
"	padding-left:-1px;\n"
"	padding-right:-1px;\n"
"	padding-top:-1px;\n"
"	padding-bottom:-1px;\n"
"}\n"
"\n"
"QSlider::sub-page:horizontal {\n"
"	background: #3B84E0;\n"
"	border: 0px solid #4A708B;\n"
"	border-radius: 2px;\n"
"}\n"
"\n"
"QSlider::add-page:horizontal {\n"
"	background: #eeeeee;\n"
"	border: 0px solid #777;\n"
"	height: 10px;\n"
"	border-radius: 2px;\n"
"}\n"
"\n"
"QSlider::handle:horizontal {\n"
"	background: #3B84E0;\n"
"    width: 10px;\n"
"	height:30px;\n"
"    margin-top: -3px;\n"
"    margin-bottom: -3px;\n"
"    /*border-radius: 5px;*/\n"
"}\n"
"\n"
"QSlider::handle:horizontal:hover{\n"
"	background-color:#4398ff;\n"
"}\n"
"\n"
"QSlider::handle:horizontal:pressed{\n"
"	background-color:#2c65aa;\n"
"}\n"
"\n"
"QSlider::sub-page:horizontal:disabled {\n"
"	background: #00009C;\n"
"	border-color: #999;\n"
"}\n"
"\n"
"QSlider::add-page:horizontal:disabled {\n"
""
                        "	background: #eee;\n"
"	border-color: #999;\n"
"}\n"
"\n"
"QSlider::handle:horizontal:disabled {\n"
"	background: #eee;\n"
"	border: 1px solid #aaa;\n"
"	/*border-radius: 4px;*/\n"
"}\n"
""));
        horizontalSlider_ImageIndex->setOrientation(Qt::Horizontal);

        horizontalLayout_5->addWidget(horizontalSlider_ImageIndex);

        label_layer = new QLabel(layoutWidget);
        label_layer->setObjectName(QStringLiteral("label_layer"));
        label_layer->setMinimumSize(QSize(100, 0));
        label_layer->setBaseSize(QSize(0, 0));
        label_layer->setStyleSheet(QStringLiteral("font:15px 'Arial';color:black;"));
        label_layer->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_layer);


        horizontalLayout_7->addLayout(horizontalLayout_5);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        horizontalLayout_7->setStretch(0, 1);
        horizontalLayout_7->setStretch(1, 10);
        horizontalLayout_7->setStretch(2, 1);
        stackedWidget_5->addWidget(stackedWidget_5Page1);
        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(110, 30, 1151, 125));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton = new QPushButton(layoutWidget1);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        DefineButton = new QPushButton(layoutWidget1);
        DefineButton->setObjectName(QStringLiteral("DefineButton"));

        horizontalLayout->addWidget(DefineButton);

        CalButton = new QPushButton(layoutWidget1);
        CalButton->setObjectName(QStringLiteral("CalButton"));

        horizontalLayout->addWidget(CalButton);

        SaveButton = new QPushButton(layoutWidget1);
        SaveButton->setObjectName(QStringLiteral("SaveButton"));

        horizontalLayout->addWidget(SaveButton);

        ExportButton = new QPushButton(layoutWidget1);
        ExportButton->setObjectName(QStringLiteral("ExportButton"));

        horizontalLayout->addWidget(ExportButton);


        horizontalLayout_4->addLayout(horizontalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(layoutWidget1);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_11->addWidget(label_3);

        Max_ = new QLabel(layoutWidget1);
        Max_->setObjectName(QStringLiteral("Max_"));

        horizontalLayout_11->addWidget(Max_);

        horizontalLayout_11->setStretch(0, 1);
        horizontalLayout_11->setStretch(1, 3);

        horizontalLayout_12->addLayout(horizontalLayout_11);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        Min_2 = new QLabel(layoutWidget1);
        Min_2->setObjectName(QStringLiteral("Min_2"));

        horizontalLayout_10->addWidget(Min_2);

        Min_ = new QLabel(layoutWidget1);
        Min_->setObjectName(QStringLiteral("Min_"));

        horizontalLayout_10->addWidget(Min_);

        horizontalLayout_10->setStretch(0, 1);
        horizontalLayout_10->setStretch(1, 3);

        horizontalLayout_12->addLayout(horizontalLayout_10);

        horizontalLayout_12->setStretch(0, 1);
        horizontalLayout_12->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_12);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        textEdit = new QTextEdit(layoutWidget1);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        QFont font;
        font.setPointSize(25);
        textEdit->setFont(font);
        textEdit->setLayoutDirection(Qt::LeftToRight);
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        horizontalLayout_2->addWidget(textEdit);

        Calgray = new QPushButton(layoutWidget1);
        Calgray->setObjectName(QStringLiteral("Calgray"));

        horizontalLayout_2->addWidget(Calgray);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(2, 1);

        horizontalLayout_4->addLayout(verticalLayout);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_10 = new QLabel(layoutWidget1);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_10);

        label_8 = new QLabel(layoutWidget1);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_8);

        verticalLayout_2->setStretch(0, 1);
        verticalLayout_2->setStretch(1, 2);

        horizontalLayout_8->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_11 = new QLabel(layoutWidget1);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_11);

        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));
        label_9->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_9);

        verticalLayout_3->setStretch(0, 1);
        verticalLayout_3->setStretch(1, 2);

        horizontalLayout_8->addLayout(verticalLayout_3);


        horizontalLayout_4->addLayout(horizontalLayout_8);

        horizontalLayout_4->setStretch(0, 10);
        horizontalLayout_4->setStretch(1, 1);
        horizontalLayout_4->setStretch(2, 5);
        horizontalLayout_4->setStretch(3, 1);
        horizontalLayout_4->setStretch(4, 5);
        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(120, 880, 1121, 111));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_6->setSpacing(20);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        mlabel_1 = new QLabel(layoutWidget2);
        mlabel_1->setObjectName(QStringLiteral("mlabel_1"));
        mlabel_1->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));

        horizontalLayout_6->addWidget(mlabel_1);

        mlabel_2 = new QLabel(layoutWidget2);
        mlabel_2->setObjectName(QStringLiteral("mlabel_2"));
        mlabel_2->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));

        horizontalLayout_6->addWidget(mlabel_2);

        mlabel_3 = new QLabel(layoutWidget2);
        mlabel_3->setObjectName(QStringLiteral("mlabel_3"));
        mlabel_3->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));

        horizontalLayout_6->addWidget(mlabel_3);

        mlabel_4 = new QLabel(layoutWidget2);
        mlabel_4->setObjectName(QStringLiteral("mlabel_4"));
        mlabel_4->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));

        horizontalLayout_6->addWidget(mlabel_4);

        mlabel_5 = new QLabel(layoutWidget2);
        mlabel_5->setObjectName(QStringLiteral("mlabel_5"));
        mlabel_5->setStyleSheet(QStringLiteral("background-color: rgb(247, 247, 247);"));

        horizontalLayout_6->addWidget(mlabel_5);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        horizontalLayout_6->setStretch(0, 1);
        horizontalLayout_6->setStretch(1, 5);
        horizontalLayout_6->setStretch(2, 5);
        horizontalLayout_6->setStretch(3, 5);
        horizontalLayout_6->setStretch(4, 5);
        horizontalLayout_6->setStretch(5, 5);
        horizontalLayout_6->setStretch(6, 1);
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(30, 480, 75, 23));
        ImportButton = new QPushButton(centralWidget);
        ImportButton->setObjectName(QStringLiteral("ImportButton"));
        ImportButton->setGeometry(QRect(30, 290, 77, 23));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(30, 340, 75, 23));
        pushButton_4 = new QPushButton(centralWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(30, 410, 75, 23));
        pushButton_5 = new QPushButton(centralWidget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setGeometry(QRect(30, 740, 75, 23));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        stackedWidget_5->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_2->setText(QString());
        label_layer->setText(QApplication::translate("MainWindow", "9999/9999", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "StartTest", nullptr));
        DefineButton->setText(QApplication::translate("MainWindow", "\347\241\256\345\256\232\346\210\252\351\235\242", nullptr));
        CalButton->setText(QApplication::translate("MainWindow", "\350\256\241\347\256\227\345\215\212\345\276\204", nullptr));
        SaveButton->setText(QApplication::translate("MainWindow", "\345\255\230\345\210\260\351\230\237\345\210\227", nullptr));
        ExportButton->setText(QApplication::translate("MainWindow", "\347\224\237\346\210\220xml", nullptr));
        label->setText(QApplication::translate("MainWindow", "  \347\233\256\346\240\207\345\260\272\345\257\270", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Max: ", nullptr));
        Max_->setText(QString());
        Min_2->setText(QApplication::translate("MainWindow", "Min: ", nullptr));
        Min_->setText(QString());
        textEdit->setPlaceholderText(QString());
        Calgray->setText(QApplication::translate("MainWindow", "\350\256\241\347\256\227\347\201\260\345\272\246\345\233\276", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "X\350\275\264\345\210\206\350\276\250\347\216\207", nullptr));
        label_8->setText(QString());
        label_11->setText(QApplication::translate("MainWindow", "Y\350\275\264\345\210\206\350\276\250\347\216\207", nullptr));
        label_9->setText(QString());
        mlabel_1->setText(QString());
        mlabel_2->setText(QString());
        mlabel_3->setText(QString());
        mlabel_4->setText(QString());
        mlabel_5->setText(QString());
        pushButton_2->setText(QApplication::translate("MainWindow", "Save", nullptr));
        ImportButton->setText(QApplication::translate("MainWindow", "\345\257\274\345\205\245STL", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "RO", nullptr));
        pushButton_4->setText(QApplication::translate("MainWindow", "SL", nullptr));
        pushButton_5->setText(QApplication::translate("MainWindow", "Debug", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
