#ifndef LOGDBWIDGET_H
#define LOGDBWIDGET_H

#include <QStandardItemModel>
#include <QWidget>

class EventSelectionModel;

namespace Ui {
class LogDbWidget;
}

class LogDbWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogDbWidget(QWidget * parent = nullptr);
    ~LogDbWidget();

private:
    void Initial();
    void InitialConnections();
    void RefreshReadLogModel(const QString & filter = "");
private Q_SLOTS:
    void Slot_Search();

private:
    Ui::LogDbWidget * ui;
    EventSelectionModel * event_model_;
};

class EventSelectionModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit EventSelectionModel(QObject * parent = nullptr);
    int columnCount(const QModelIndex & /*parent*/) const;
public Q_SLOTS:
    void Slot_ItemChanged(QStandardItem * item);

private:
    void setupModel();
};

#endif // LOGDBWIDGET_H
