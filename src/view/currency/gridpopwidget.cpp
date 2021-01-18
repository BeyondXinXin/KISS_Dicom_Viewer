#include "gridpopwidget.h"
#include "ui_gridpopwidget.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#define NORMAL_STYLE "background-color: rgb(128, 128, 128);"
#define HIGHLIGHT_STYLE "background-color: rgb(200, 200, 200);"

//-------------------------------------------------------
GridPopWidget::GridPopWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GridPopWidget) {
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Popup | Qt::FramelessWindowHint);
    setMouseTracking(true);
    wArr << (QList<QWidget *>() << ui->w11 << ui->w12 << ui->w13 << ui->w14 << ui->w15);
    wArr << (QList<QWidget *>() << ui->w21 << ui->w22 << ui->w23 << ui->w24 << ui->w25);
    wArr << (QList<QWidget *>() << ui->w31 << ui->w32 << ui->w33 << ui->w34 << ui->w35);
    wArr << (QList<QWidget *>() << ui->w41 << ui->w42 << ui->w43 << ui->w44 << ui->w45);
}

//-------------------------------------------------------
GridPopWidget::~GridPopWidget() {
    delete ui;
}

//-------------------------------------------------------
void GridPopWidget::mousePressEvent(QMouseEvent *e) {
    QWidget::mousePressEvent(e);
    QPoint p = e->pos();
    int row = 0, col = 0;
    QPoint lim = rect().bottomRight();
    if (p.x() > lim.x() || p.y() > lim.y()) {
        return;
    }
    foreach (QWidget *w, wArr.first()) {
        if (p.x() > w->pos().x()) {
            col++;
        }
    }
    foreach (QList<QWidget *> ws, wArr) {
        if (p.y() > ws.first()->pos().y()) {
            row++;
        }
    }
    emit Signal_ViewLayout(col, row);
    close();
}

//-------------------------------------------------------
void GridPopWidget::mouseMoveEvent(QMouseEvent *e) {
    QWidget::mouseMoveEvent(e);
    QPoint p = e->pos();
    foreach (QList<QWidget *> ws, wArr) {
        foreach (QWidget *w, ws) {
            if (p.x() > w->pos().x() && p.y() > w->pos().y()) {
                w->setStyleSheet(HIGHLIGHT_STYLE);
            } else {
                w->setStyleSheet(NORMAL_STYLE);
            }
        }
    }
}

//-------------------------------------------------------
void GridPopWidget::hideEvent(QHideEvent *e) {
    QWidget::hideEvent(e);
    deleteLater();
}
