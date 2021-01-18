#include "scpsettingview.h"
#include "ui_scpsettingview.h"

#include <global/KissGlobal>
#include <engine/KissEngine>
#include "script/echoscu.h"


//-------------------------------------------------------
ScpSettingView::ScpSettingView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScpSettingView) {
    ui->setupUi(this);
    this->setWindowTitle("Scp 服务配置设置");
    ui->combo_box->clear();
    QStringList ips = Kiss::getLocalIP();
    foreach (auto ip, ips) {
        ui->combo_box->addItem(ip);
    }
    LocalSettings settings;
    ui->edit_aet->setText(settings.statInfo.aetitle);
    ui->spin_part->setValue(settings.statInfo.store_port);
}
//-------------------------------------------------------
ScpSettingView::~ScpSettingView() {
    delete ui;
}
//-------------------------------------------------------
void ScpSettingView::on_pushButton_2_clicked() {
    LocalSettings settings;
    settings.statInfo.aetitle = ui->edit_aet->text();
    settings.statInfo.store_port = static_cast<quint16>(ui->spin_part->value());
    settings.saveConfig();
    qApp->exit();
}
//-------------------------------------------------------
void ScpSettingView::on_pushButton_clicked() {
    LocalSettings settings;
    QString msg;
    if (EchoSCU(settings.statInfo.aetitle, "Echo",
                ui->combo_box->currentText(), settings.statInfo.store_port, msg)) {
        QMessageBox::information(
            this, QString("Echo SCP"), QString("Echo succeeded."));
    } else {
        QMessageBox::critical(this, QString("Echo SCP"), msg);
    }
}
