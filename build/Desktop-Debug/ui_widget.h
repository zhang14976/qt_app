/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *label_net;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout_1;
    QLabel *label_net_gateway;
    QLineEdit *lineEdit_net_gateway;
    QLabel *label_net_staticIP;
    QLineEdit *lineEdit_net_staticIP;
    QLabel *label_net_subnetNumber;
    QLineEdit *lineEdit_net_subnetNumber;
    QLabel *label_net_DNS;
    QLineEdit *lineEdit_net_DNS;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_net_autoGateway;
    QPushButton *pushButton_net_apply;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_rpc_port;
    QLabel *label_rpc_service_status;
    QLabel *label_rpc_status;
    QPushButton *pushButton_rpc;
    QLabel *label_rpc;
    QLabel *label_model;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_model_path;
    QLineEdit *lineEdit_model_path_text;
    QPushButton *pushButton_model_choice;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_llamacpp_path;
    QLineEdit *lineEdit_llamacpp_path_text;
    QPushButton *pushButton_llamacpp_choice;
    QPushButton *pushButton_model_start_or_stop;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(800, 600);
        label_net = new QLabel(Widget);
        label_net->setObjectName(QString::fromUtf8("label_net"));
        label_net->setGeometry(QRect(20, 60, 71, 31));
        label_net->setLayoutDirection(Qt::LeftToRight);
        label_net->setFrameShadow(QFrame::Plain);
        verticalLayoutWidget = new QWidget(Widget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(30, 90, 271, 161));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        formLayout_1 = new QFormLayout();
        formLayout_1->setObjectName(QString::fromUtf8("formLayout_1"));
        label_net_gateway = new QLabel(verticalLayoutWidget);
        label_net_gateway->setObjectName(QString::fromUtf8("label_net_gateway"));

        formLayout_1->setWidget(0, QFormLayout::LabelRole, label_net_gateway);

        lineEdit_net_gateway = new QLineEdit(verticalLayoutWidget);
        lineEdit_net_gateway->setObjectName(QString::fromUtf8("lineEdit_net_gateway"));

        formLayout_1->setWidget(0, QFormLayout::FieldRole, lineEdit_net_gateway);

        label_net_staticIP = new QLabel(verticalLayoutWidget);
        label_net_staticIP->setObjectName(QString::fromUtf8("label_net_staticIP"));

        formLayout_1->setWidget(1, QFormLayout::LabelRole, label_net_staticIP);

        lineEdit_net_staticIP = new QLineEdit(verticalLayoutWidget);
        lineEdit_net_staticIP->setObjectName(QString::fromUtf8("lineEdit_net_staticIP"));

        formLayout_1->setWidget(1, QFormLayout::FieldRole, lineEdit_net_staticIP);

        label_net_subnetNumber = new QLabel(verticalLayoutWidget);
        label_net_subnetNumber->setObjectName(QString::fromUtf8("label_net_subnetNumber"));

        formLayout_1->setWidget(2, QFormLayout::LabelRole, label_net_subnetNumber);

        lineEdit_net_subnetNumber = new QLineEdit(verticalLayoutWidget);
        lineEdit_net_subnetNumber->setObjectName(QString::fromUtf8("lineEdit_net_subnetNumber"));

        formLayout_1->setWidget(2, QFormLayout::FieldRole, lineEdit_net_subnetNumber);

        label_net_DNS = new QLabel(verticalLayoutWidget);
        label_net_DNS->setObjectName(QString::fromUtf8("label_net_DNS"));

        formLayout_1->setWidget(3, QFormLayout::LabelRole, label_net_DNS);

        lineEdit_net_DNS = new QLineEdit(verticalLayoutWidget);
        lineEdit_net_DNS->setObjectName(QString::fromUtf8("lineEdit_net_DNS"));

        formLayout_1->setWidget(3, QFormLayout::FieldRole, lineEdit_net_DNS);


        verticalLayout_2->addLayout(formLayout_1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_net_autoGateway = new QPushButton(verticalLayoutWidget);
        pushButton_net_autoGateway->setObjectName(QString::fromUtf8("pushButton_net_autoGateway"));

        horizontalLayout->addWidget(pushButton_net_autoGateway);

        pushButton_net_apply = new QPushButton(verticalLayoutWidget);
        pushButton_net_apply->setObjectName(QString::fromUtf8("pushButton_net_apply"));

        horizontalLayout->addWidget(pushButton_net_apply);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayoutWidget = new QWidget(Widget);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(370, 110, 311, 61));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_rpc_port = new QLabel(horizontalLayoutWidget);
        label_rpc_port->setObjectName(QString::fromUtf8("label_rpc_port"));

        verticalLayout->addWidget(label_rpc_port);

        label_rpc_service_status = new QLabel(horizontalLayoutWidget);
        label_rpc_service_status->setObjectName(QString::fromUtf8("label_rpc_service_status"));

        verticalLayout->addWidget(label_rpc_service_status);


        horizontalLayout_2->addLayout(verticalLayout);

        label_rpc_status = new QLabel(horizontalLayoutWidget);
        label_rpc_status->setObjectName(QString::fromUtf8("label_rpc_status"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_rpc_status->sizePolicy().hasHeightForWidth());
        label_rpc_status->setSizePolicy(sizePolicy);
        label_rpc_status->setTextFormat(Qt::AutoText);
        label_rpc_status->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_rpc_status);

        pushButton_rpc = new QPushButton(horizontalLayoutWidget);
        pushButton_rpc->setObjectName(QString::fromUtf8("pushButton_rpc"));

        horizontalLayout_2->addWidget(pushButton_rpc);

        label_rpc = new QLabel(Widget);
        label_rpc->setObjectName(QString::fromUtf8("label_rpc"));
        label_rpc->setGeometry(QRect(350, 70, 71, 31));
        label_rpc->setLayoutDirection(Qt::LeftToRight);
        label_rpc->setFrameShadow(QFrame::Plain);
        label_model = new QLabel(Widget);
        label_model->setObjectName(QString::fromUtf8("label_model"));
        label_model->setGeometry(QRect(330, 290, 101, 31));
        label_model->setLayoutDirection(Qt::LeftToRight);
        label_model->setFrameShadow(QFrame::Plain);
        horizontalLayoutWidget_2 = new QWidget(Widget);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(350, 330, 341, 41));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_model_path = new QLabel(horizontalLayoutWidget_2);
        label_model_path->setObjectName(QString::fromUtf8("label_model_path"));

        horizontalLayout_3->addWidget(label_model_path);

        lineEdit_model_path_text = new QLineEdit(horizontalLayoutWidget_2);
        lineEdit_model_path_text->setObjectName(QString::fromUtf8("lineEdit_model_path_text"));

        horizontalLayout_3->addWidget(lineEdit_model_path_text);

        pushButton_model_choice = new QPushButton(horizontalLayoutWidget_2);
        pushButton_model_choice->setObjectName(QString::fromUtf8("pushButton_model_choice"));

        horizontalLayout_3->addWidget(pushButton_model_choice);

        horizontalLayoutWidget_3 = new QWidget(Widget);
        horizontalLayoutWidget_3->setObjectName(QString::fromUtf8("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(350, 380, 341, 41));
        horizontalLayout_4 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_llamacpp_path = new QLabel(horizontalLayoutWidget_3);
        label_llamacpp_path->setObjectName(QString::fromUtf8("label_llamacpp_path"));

        horizontalLayout_4->addWidget(label_llamacpp_path);

        lineEdit_llamacpp_path_text = new QLineEdit(horizontalLayoutWidget_3);
        lineEdit_llamacpp_path_text->setObjectName(QString::fromUtf8("lineEdit_llamacpp_path_text"));

        horizontalLayout_4->addWidget(lineEdit_llamacpp_path_text);

        pushButton_llamacpp_choice = new QPushButton(horizontalLayoutWidget_3);
        pushButton_llamacpp_choice->setObjectName(QString::fromUtf8("pushButton_llamacpp_choice"));

        horizontalLayout_4->addWidget(pushButton_llamacpp_choice);

        pushButton_model_start_or_stop = new QPushButton(Widget);
        pushButton_model_start_or_stop->setObjectName(QString::fromUtf8("pushButton_model_start_or_stop"));
        pushButton_model_start_or_stop->setGeometry(QRect(350, 440, 93, 28));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        label_net->setText(QCoreApplication::translate("Widget", "\347\275\221\347\273\234\351\205\215\347\275\256", nullptr));
        label_net_gateway->setText(QCoreApplication::translate("Widget", "\347\275\221\345\205\263\345\234\260\345\235\200", nullptr));
        label_net_staticIP->setText(QCoreApplication::translate("Widget", "\351\235\231\346\200\201IP", nullptr));
        label_net_subnetNumber->setText(QCoreApplication::translate("Widget", "\345\255\220\347\275\221\346\216\251\347\240\201\344\275\215\346\225\260", nullptr));
        label_net_DNS->setText(QCoreApplication::translate("Widget", "DNS\345\234\260\345\235\200", nullptr));
        pushButton_net_autoGateway->setText(QCoreApplication::translate("Widget", "\350\207\252\345\212\250\350\216\267\345\217\226", nullptr));
        pushButton_net_apply->setText(QCoreApplication::translate("Widget", "\345\272\224\347\224\250\351\205\215\347\275\256", nullptr));
        label_rpc_port->setText(QString());
        label_rpc_service_status->setText(QString());
        label_rpc_status->setText(QString());
        pushButton_rpc->setText(QCoreApplication::translate("Widget", "PushButton", nullptr));
        label_rpc->setText(QCoreApplication::translate("Widget", "RPC\346\234\215\345\212\241", nullptr));
        label_model->setText(QCoreApplication::translate("Widget", "\346\250\241\345\236\213\345\220\257\345\212\250\351\205\215\347\275\256", nullptr));
        label_model_path->setText(QCoreApplication::translate("Widget", "\346\250\241\345\236\213\350\267\257\345\276\204", nullptr));
        pushButton_model_choice->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251...", nullptr));
        label_llamacpp_path->setText(QCoreApplication::translate("Widget", "llaamaCPP\350\267\257\345\276\204", nullptr));
        pushButton_llamacpp_choice->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251...", nullptr));
        pushButton_model_start_or_stop->setText(QCoreApplication::translate("Widget", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
