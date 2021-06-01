#include "logdbwidget.h"
#include "ui_logdbwidget.h"

#include <dao/KissDb>
#include <engine/KissEngine>
#include <global/KissGlobal>

//----------------------------------------------------------------
LogDbWidget::LogDbWidget(QWidget * parent)
  : QWidget(parent)
  , ui(new Ui::LogDbWidget)
{
    ui->setupUi(this);
    this->Initial();
}

//----------------------------------------------------------------
LogDbWidget::~LogDbWidget()
{
    delete ui;
}

//----------------------------------------------------------------
void LogDbWidget::Initial()
{
    ui->fromDateTimeEdit->setDate(QDate::currentDate());
    ui->fromDateTimeEdit->setTime(QTime(0, 0));
    ui->toDateTimeEdit->setDate(QDate::currentDate());
    ui->toDateTimeEdit->setTime(QTime(23, 59, 59, 999));
    //
    this->event_model_ = new EventSelectionModel(this);
    ui->logTableView->setSortingEnabled(true);
    ui->logTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    event_model_->setHeaderData(0, Qt::Horizontal, tr("Event Type"), Qt::DisplayRole);
    ui->eventSelectionView->setModel(event_model_);
    //
    this->InitialConnections();
    this->RefreshReadLogModel();
}

//----------------------------------------------------------------
void LogDbWidget::InitialConnections()
{
    connect(ui->searchButton, &QPushButton::clicked,
            this, &LogDbWidget::Slot_Search);
}

//----------------------------------------------------------------
void LogDbWidget::RefreshReadLogModel(const QString & filter)
{
    QSqlTableModel * log_model = new QSqlTableModel(
      nullptr, QSqlDatabase::database(DB_CONNECTION_NAME));
    log_model->setTable(LogDao::kTableName);
    log_model->setHeaderData(0, Qt::Horizontal, tr("Log Time"));
    log_model->setHeaderData(1, Qt::Horizontal, tr("User Name"));
    log_model->setHeaderData(2, Qt::Horizontal, tr("Event Type"));
    log_model->setHeaderData(3, Qt::Horizontal, tr("Event Content"));
    log_model->select();
    log_model->setFilter(filter);
    ui->logTableView->setModel(log_model);
}

//----------------------------------------------------------------
void LogDbWidget::Slot_Search()
{
    QString filter;
    if (ui->fromCheckBox->isChecked()) {
        filter = QString("LogTime>\'%1\'")
                   .arg(ui->fromDateTimeEdit->dateTime().toString(NORMAL_DATETIME_FORMAT));
    }
    if (ui->toCheckBox->isChecked()) {
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("LogTime<\'%1\'")
                        .arg(ui->toDateTimeEdit->dateTime().toString(NORMAL_DATETIME_FORMAT)));
    }
    if (!ui->userNameEdit->text().isEmpty()) {
        if (!filter.isEmpty()) {
            filter.append(" and ");
        }
        filter.append(QString("UserName LIKE \'%%1%\'").arg(ui->userNameEdit->text()));
    }
    if (event_model_->item(0)->checkState() == Qt::PartiallyChecked) {
        QString eventFilter;
        for (int i = 1; i < event_model_->rowCount(); ++i) {
            if (event_model_->item(i)->checkState() == Qt::Checked) {
                if (!eventFilter.isEmpty()) {
                    eventFilter.append(" or ");
                }
                eventFilter.append(QString("EventType=\'%1\'")
                                     .arg(LogDao::string_tables_[i]));
            }
        }
        if (!eventFilter.isEmpty()) {
            if (!filter.isEmpty()) {
                filter.append(" and ");
            }
            filter.append(QString("(%1)").arg(eventFilter));
        }
    } else if (event_model_->item(0)->checkState() == Qt::Unchecked) {
        filter.append(" and EventType=\'NoType\'");
    }
    this->RefreshReadLogModel(filter);
}

//----------------------------------------------------------------
EventSelectionModel::EventSelectionModel(QObject * parent)
  : QStandardItemModel(parent)
{
    setupModel();
    connect(this, &EventSelectionModel::itemChanged,
            this, &EventSelectionModel::Slot_ItemChanged);
    item(0)->setCheckState(Qt::Checked);
}

//----------------------------------------------------------------
int EventSelectionModel::columnCount(const QModelIndex &) const
{
    return 1;
}

//----------------------------------------------------------------
void EventSelectionModel::setupModel()
{
    int count = LogDao::string_tables_.size();
    for (int i = 0; i < count; ++i) {
        QStandardItem * item = new QStandardItem(
          LogDao::string_tables_[i]);
        item->setCheckable(true);
        appendRow(item);
    }
}

//----------------------------------------------------------------
void EventSelectionModel::Slot_ItemChanged(QStandardItem * item)
{
    if (this->item(0) == item) {
        Qt::CheckState state = item->checkState();
        if (state != Qt::PartiallyChecked) {
            for (int i = 1; i < rowCount(); ++i) {
                this->item(i)->setCheckState(state);
            }
        }
    } else {
        bool all = true;
        bool none = true;
        for (int i = 1; i < rowCount(); ++i) {
            if (this->item(i)->checkState() == Qt::Checked) {
                none = false;
            } else {
                all = false;
            }
        }
        if (none) {
            this->item(0)->setCheckState(Qt::Unchecked);
        } else if (all) {
            this->item(0)->setCheckState(Qt::Checked);
        } else {
            this->item(0)->setCheckState(Qt::PartiallyChecked);
        }
    }
}
