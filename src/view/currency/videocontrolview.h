#ifndef VIDEOCONTROLVIEW_H
#define VIDEOCONTROLVIEW_H

#include <QTimer>
#include <QWidget>

namespace Ui {
    class VideoControlView;
}

class VideoControlView : public QWidget {
    Q_OBJECT
  public:
    explicit VideoControlView(QWidget *parent = nullptr);
    virtual ~VideoControlView() override;
    void GetCurrentTimeIn(qint32 &time) const;
    void GetMaxTimeIn(qint32 &time) const;
    void SetCurrentTimeIn(const qint32 &time);
    void StopMovie();
  public Q_SLOTS:
    void SlotTimeChangeIn(const qint32 time, const qint32 total_time);
  Q_SIGNALS:
    void SignalValueChangeOut(const qint32 time);
  protected:
    virtual void enterEvent(QEvent *e) override;
    virtual void leaveEvent(QEvent *e) override;
    virtual void paintEvent(QPaintEvent *event) override;
  private:
    void Initial();
    void SetTimerStop();
    void SetToolVisible(bool type);
  private slots:
    void SlotControlButtonClicked();
    void SlotSpeedTimeOut();
    void SlotValueChangedIn(int value);
    void SlotPlusMinusButtonClicked();
  private:
    Ui::VideoControlView *ui;
    QTimer *play_speed_;
    QTimer *next_speed_;
    QTimer *prev_speed_;
    qint32 time_;
    qint32 total_time_;
};

#endif // FFRVTKCONTROLVIEW_H
