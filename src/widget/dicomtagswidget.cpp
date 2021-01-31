#include "dicomtagswidget.h"

#include <engine/KissEngine>
#include <global/KissGlobal>


KissQTreeWidget::KissQTreeWidget(QWidget *parent): QTreeWidget(parent) {
    this->Initial();
}

KissQTreeWidget::~KissQTreeWidget() {
}

QModelIndexList KissQTreeWidget::getSelectedIndexes() const {
    return this->selection_.indexes();
}

void KissQTreeWidget::Initial() {
    this->setHeaderLabels(
        QStringList() << "Tag ID" << "VR" << "VM"
        << "Length" << "Description" << "value");
    this->setColumnWidth(0, 200);
    this->setColumnWidth(1, 70);
    this->setColumnWidth(2, 100);
    this->setColumnWidth(3, 50);
    this->setColumnWidth(4, 300);
    this->setColumnWidth(5, 300);
    this->setGeometry(0, 0, 1200, 800);
    this->header()->setDefaultAlignment(Qt::AlignCenter);
    //
    this->context_menu_ = new QMenu(this);
    //
    context_menu_->addAction(tr("Open the folder where DCM is located"), this, [ = ]() {
    });
    context_menu_->addAction(tr("Copy current selection"), this, [ = ]() {
    });
    context_menu_->addSeparator();
    context_menu_->addAction(tr("Copy all values"), this, [ = ]() {
    });
}

void KissQTreeWidget::contextMenuEvent(QContextMenuEvent *e) {
    if (indexAt(e->pos()).isValid()) {
        context_menu_->popup(e->globalPos());
    }
}


DicomTagsWidget::DicomTagsWidget(const QString &str, QWidget *parent) :
    QWidget(parent) {
    // 界面初始化
    ui.setupUi(this);
    this->tree_wid_ = new KissQTreeWidget(this);
    ui.layut->insertWidget(0, this->tree_wid_);
    QList<QTreeWidgetItem *> items;
    this->setWindowTitle(str);
    // 读取文件
    DcmFileFormat dfile;
    dfile.loadFile(str.toLocal8Bit().data());
    QTreeWidgetItem *metainfo_item = new QTreeWidgetItem;
    metainfo_item->setText(0, "Dicom-MetaInfo");
    items << metainfo_item;
    MyDcmMetaInfo *meta_info = new MyDcmMetaInfo(*dfile.getMetaInfo());
    GenerateItems(items, *meta_info);
    QTreeWidgetItem *dataset_item = new QTreeWidgetItem;
    dataset_item->setText(0, "Dicom-Data-Set");
    items << dataset_item;
    MyDcmDataset *dataset_info = new MyDcmDataset(*dfile.getDataset());
    GenerateItems(items, *dataset_info);
    this->tree_wid_->addTopLevelItems(items);
}

void DicomTagsWidget::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui.retranslateUi(this);
            break;
        default:
            break;
    }
}


