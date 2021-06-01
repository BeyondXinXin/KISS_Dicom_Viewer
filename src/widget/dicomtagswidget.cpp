#include "dicomtagswidget.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

KissQTreeWidget::KissQTreeWidget(QWidget * parent)
  : QTreeWidget(parent)
{
    this->Initial();
}

KissQTreeWidget::~KissQTreeWidget()
{
}

void KissQTreeWidget::Initial()
{
    QStringList header_list = { "Tag ID", "VR", "VM", "Length", "Description", "value" };
    QList<int> headerwidth_list = { 200, 70, 100, 50, 300, 300 };
    this->setHeaderLabels(header_list);
    qint32 i = 0;
    foreach (auto var, headerwidth_list) {
        this->setColumnWidth(i, var);
        i++;
    }
    this->setGeometry(0, 0, 1200, 800);
    this->header()->setDefaultAlignment(Qt::AlignCenter);
    // 右键菜单
    this->context_menu_ = new QMenu(this);
    context_menu_->addAction(tr("Open the folder where DCM is located"),
                             this, &KissQTreeWidget::SgnOpenFolder);
    context_menu_->addSeparator();
    context_menu_->addAction(tr("Copy current selection"), this, [=]() {
        QTreeWidgetItem * item = this->selectedItems().first();
        QClipboard * clip = QApplication::clipboard();
        QStringList str;
        for (qint32 i = 0; i < item->columnCount(); i++) {
            str << item->text(i);
        }
        clip->setText(str.join("      "));
    });
    context_menu_->addAction(tr("Copy all values"), this, [=]() {
        QList<int> right_justified = { 12, 4, 4, 4, 50, 50 };
        QClipboard * clip = QApplication::clipboard();
        QStringList list_str;
        QTreeWidgetItemIterator it(this);
        while (*it) {
            QStringList tmp_list_str;
            for (qint32 i = 0; i < (*it)->columnCount(); i++) {
                QString str = (*it)->text(i);
                auto var = *it;
                while (var->parent()) {
                    var->parent()->setHidden(false);
                    var = var->parent();
                    str = "   " + str;
                }
                tmp_list_str << str.rightJustified(right_justified.at(i));
            }
            list_str << tmp_list_str.join("      ");
            ++it;
        }
        clip->setText(list_str.join("\n"));
    });
}

void KissQTreeWidget::contextMenuEvent(QContextMenuEvent * e)
{
    if (indexAt(e->pos()).isValid()) {
        context_menu_->popup(e->globalPos());
    }
}

DicomTagsWidget::DicomTagsWidget(const QString & str, QWidget * parent)
  : QWidget(parent)
{
    // 界面初始化
    ui.setupUi(this);
    this->tree_wid_ = new KissQTreeWidget(this);
    ui.layut->insertWidget(0, this->tree_wid_);
    QList<QTreeWidgetItem *> items;
    this->setWindowTitle(str);
    connect(ui.filter, &QLineEdit::textChanged,
            this, &DicomTagsWidget::SlotFilterChanged);
    connect(this->tree_wid_, &KissQTreeWidget::SgnOpenFolder,
            this, &DicomTagsWidget::SLot_OpenFolder);
    // 读取文件
    DcmFileFormat dfile;
    dfile.loadFile(str.toLocal8Bit().data());
    QTreeWidgetItem * metainfo_item = new QTreeWidgetItem;
    metainfo_item->setText(0, "Dicom-MetaInfo");
    items << metainfo_item;
    MyDcmMetaInfo * meta_info = new MyDcmMetaInfo(*dfile.getMetaInfo());
    GenerateItems(items, *meta_info);
    QTreeWidgetItem * dataset_item = new QTreeWidgetItem;
    dataset_item->setText(0, "Dicom-Data-Set");
    items << dataset_item;
    MyDcmDataset * dataset_info = new MyDcmDataset(*dfile.getDataset());
    GenerateItems(items, *dataset_info);
    this->tree_wid_->addTopLevelItems(items);
}

void DicomTagsWidget::changeEvent(QEvent * e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void DicomTagsWidget::SlotFilterChanged()
{
    QTreeWidgetItemIterator it(tree_wid_);
    QString str = ui.filter->text();
    if (str.isEmpty()) {
        while (*it) {
            (*it)->setHidden(false);
            ++it;
        }
        return;
    }
    while (*it) {
        (*it)->setHidden(true);
        ++it;
    }
    QList<QTreeWidgetItem *> items;
    for (qint32 i = 0; i < tree_wid_->columnCount(); i++) {
        items << tree_wid_->findItems(str, Qt::MatchContains | Qt::MatchRecursive, i);
    }
    foreach (auto var, items) {
        var->setHidden(false);
        while (var->parent()) {
            var->parent()->setHidden(false);
            var = var->parent();
        }
    }
}

void DicomTagsWidget::SLot_OpenFolder()
{
    QString str = this->windowTitle();
    QFileInfo file(str);
    QDesktopServices::openUrl(
      QUrl("file:///" + file.absolutePath(), QUrl::TolerantMode));
}
