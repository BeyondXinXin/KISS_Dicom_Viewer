#include "graphicstextmarkdialog.h"
#include "ui_graphicstextmarkdialog.h"

//----------------------------------------------------------------
GraphicsTextMarkDialog::GraphicsTextMarkDialog(QWidget * parent)
  : QDialog(parent)
  , ui(new Ui::GraphicsTextMarkDialog)
{
    ui->setupUi(this);
}

//----------------------------------------------------------------
GraphicsTextMarkDialog::~GraphicsTextMarkDialog()
{
    delete ui;
}

//----------------------------------------------------------------
QString GraphicsTextMarkDialog::getText()
{
    return ui->textEdit->text();
}
