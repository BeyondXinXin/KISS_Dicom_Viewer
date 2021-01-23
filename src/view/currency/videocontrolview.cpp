#include "videocontrolview.h"
#include "ui_videocontrolview.h"

#include <engine/KissEngine>

VideoControlView::VideoControlView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoControlView) {
    ui->setupUi(this);
    this->Initial();
}

VideoControlView::~VideoControlView() {
    delete ui;
}

void VideoControlView::GetCurrentTimeIn(qint32 &time) const {
    time = ui->progress_slider->value();
}

void VideoControlView::GetMaxTimeIn(qint32 &time) const {
    time = ui->progress_slider->maximum();
}

void VideoControlView::SetCurrentTimeIn(const qint32 &time) {
    time_ = time;
    ui->progress_slider->setValue(time);
}

void VideoControlView::StopMovie() {
    play_speed_->start();
    ui->play_button->click();
}

void VideoControlView::SlotTimeChangeIn(const qint32 time, const qint32 total_time) {
    time_ = time;
    total_time_ = total_time;
    ui->progress_slider->setRange(0, total_time_ - 1);
    ui->progress_slider->setValue(time_);
    if (time_ <= 0 || time >= total_time_) {
        this->SetTimerStop();
        ui->play_button->setProperty("pause", "true");
        ui->play_button->style()->unpolish(ui->play_button);
        ui->play_button->style()->polish(ui->play_button);
        ui->play_button->update();
    }
    if(0 == total_time) {
        this->StopMovie();
        this->setVisible(0);
    } else {
        this->setVisible(1);
    }
}

void VideoControlView::enterEvent(QEvent *e) {
    this->SetToolVisible(true);
    QWidget::enterEvent(e);
}

void VideoControlView::leaveEvent(QEvent *e) {
    this->SetToolVisible(false);
    QWidget::leaveEvent(e);
}

void VideoControlView::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

void VideoControlView::Initial() {
    time_ = 0;
    total_time_ = 0;
    this->SetToolVisible(false);
    play_speed_ = new QTimer(this);
    next_speed_ = new QTimer(this);
    prev_speed_ = new QTimer(this);
    this->SlotValueChangedIn(10);
    ui->play_button->setProperty("pause", "true");
    //
    connect(ui->next_button, &QPushButton::clicked,
            this, &VideoControlView::SlotControlButtonClicked);
    connect(ui->next_fast_button, &QPushButton::clicked,
            this, &VideoControlView::SlotControlButtonClicked);
    connect(ui->prev_button, &QPushButton::clicked,
            this, &VideoControlView::SlotControlButtonClicked);
    connect(ui->prev_fast_button, &QPushButton::clicked,
            this, &VideoControlView::SlotControlButtonClicked);
    connect(ui->play_button, &QPushButton::clicked,
            this, &VideoControlView::SlotControlButtonClicked);
    connect(ui->progress_slider, &QSlider::valueChanged,
            this, &VideoControlView::SignalValueChangeOut);
    connect(play_speed_, &QTimer::timeout,
            this, &VideoControlView::SlotSpeedTimeOut);
    connect(next_speed_, &QTimer::timeout,
            this, &VideoControlView::SlotSpeedTimeOut);
    connect(prev_speed_, &QTimer::timeout,
            this, &VideoControlView::SlotSpeedTimeOut);
    connect(ui->frame_box, SIGNAL(valueChanged(int)),
            this, SLOT(SlotValueChangedIn(int)));
    connect(ui->plus_button, &QPushButton::clicked,
            this, &VideoControlView::SlotPlusMinusButtonClicked);
    connect(ui->minus_button, &QPushButton::clicked,
            this, &VideoControlView::SlotPlusMinusButtonClicked);
}

void VideoControlView::SetToolVisible(bool type) {
    ui->wid->setVisible(type);
}

void VideoControlView::SetTimerStop() {
    play_speed_->stop();
    next_speed_->stop();
    prev_speed_->stop();
}

void VideoControlView::SlotControlButtonClicked() {
    if (total_time_ <= 0) {
        return;
    }
    bool polish = false;
    if (QObject::sender() == ui->next_button) {
        ui->progress_slider->setValue(total_time_);
    } else if (QObject::sender() == ui->next_fast_button) {
        ui->play_button->setProperty("pause", "false");
        polish = true;
        this->SetTimerStop();
        next_speed_->start();
    } else if (QObject::sender() == ui->prev_button) {
        ui->progress_slider->setValue(0);
    } else if (QObject::sender() == ui->prev_fast_button) {
        ui->play_button->setProperty("pause", "false");
        polish = true;
        this->SetTimerStop();
        prev_speed_->start();
    } else if (QObject::sender() == ui->play_button) {
        if (play_speed_->isActive() ||
                next_speed_->isActive() ||
                prev_speed_->isActive()) {
            ui->play_button->setProperty("pause", "true");
            polish = true;
            this->SetTimerStop();
        } else {
            if (time_ <= 0 || time_ >= total_time_) {
                ui->progress_slider->setValue(0);
            }
            ui->play_button->setProperty("pause", "false");
            polish = true;
            play_speed_->start();
        }
    }
    if(polish) {
        ui->play_button->style()->unpolish(ui->play_button);
        ui->play_button->style()->polish(ui->play_button);
        ui->play_button->update();
    }
}

void VideoControlView::SlotSpeedTimeOut() {
    if (QObject::sender() == play_speed_) {
        ui->progress_slider->setValue(time_ + 1);
        if(time_ >= total_time_ - 1) {
            ui->progress_slider->setValue(0);
        }
    } else if (QObject::sender() == next_speed_) {
        ui->progress_slider->setValue(time_ + 1);
        if(time_ >= total_time_ - 1) {
            this->StopMovie();
        }
    } else if (QObject::sender() == prev_speed_) {
        ui->progress_slider->setValue(time_ - 1);
        if(time_ <= 0) {
            this->StopMovie();
        }
    }
}

void VideoControlView::SlotValueChangedIn(int value) {
    play_speed_->setInterval(1000 / value);
    next_speed_->setInterval(400 / value);
    prev_speed_->setInterval(400 / value);
}

void VideoControlView::SlotPlusMinusButtonClicked() {
    if (QObject::sender() == ui->plus_button) {
        ui->frame_box->setValue(ui->frame_box->value() + 1);
    } else if (QObject::sender() == ui->minus_button) {
        ui->frame_box->setValue(ui->frame_box->value() - 1);
    }
}
