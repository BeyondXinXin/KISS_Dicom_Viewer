#ifndef DICOMIMAGEVIEW_H
#define DICOMIMAGEVIEW_H

#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include "modalityproperty.h"
#include "ImageData/seriesinstance.h"
#include "view/currency/videocontrolview.h"

class ModalityProperty;
class AbstractPathItem;
class GraphicsTumorItem;
class SeriesInstance;
class DicomImage;




class DicomImageView : public QGraphicsView {
    Q_OBJECT
  public:

    enum CurrentState {
        None,// 无
        AdjustWL,// 床位窗宽
        Zoom,// 缩放
        Pan,// 拖动
        Slicing,// 切片浏览
        Magnifier,// 局部放大
        ROIWindow, // ROI 自动调整灰度
        Drawing,// 画图
    };

    enum DrawingType {
        DrawLine,
        DrawAngle,
        DrawCobbAngle,
        DrawRect,
        DrawEllipse,
        DrawPolygon,
        DrawClosedCurve,
        DrawTextMark,
        DrawArrow,
        DrawCurve,
        RestPan,// 拖动复位
        RemoveAllDraw,// 清空所有注释
    };

    enum ZoomOperation {
        FillViewport,// view 最大化显示
        Zoom100,// 100% 显示
        Zoom200,// 200% 显示
        Zoom400,// 400% 显示
        ZoomIn,// 缩小1.2 倍
        Zoomout,// 放大1.2倍
    };

    enum RoateFlipOperation {
        HFlip,// 水平翻转
        VFlip,// 垂直翻转
        ClearFlip,// 清楚翻转
        RoateCCW,// 逆时针90
        RoateCW,// 顺时针90
        ClearRoate,// 清空翻转
    };

    enum WindowWLWHOperation {
        DefaultWL,
        FullDynamic,
        InverseWl,
    };

    enum FillterOperation {
        FillterNone,
        FillterSharpen1,
        FillterSharpen2,
        FillterSharpen3,
        FillterSmooth1,
        FillterSmooth2,
        FillterSmooth3,
        FillterEdge1,
        FillterEdge2,
        FillterEdge3,
        FillterEmboss1,
        FillterEmboss2,
        FillterEmboss3,
    };


    typedef struct GraphicsAnnoGroup {
        QString pos;
        QList<QGraphicsSimpleTextItem *> items;
        ~GraphicsAnnoGroup() {
            qDeleteAll(items);
        }
    } GraphicsAnnoGroup;

    typedef struct tag_MouseButtonState {
        CurrentState state;
        DrawingType type;
        tag_MouseButtonState() {
            state = None;
            type = DrawLine;
        }
    } MouseButtonState;

  public:
    explicit DicomImageView(ViewType type = VT_XYPlane,
                            SeriesInstance *m_series_ = nullptr,
                            QWidget *parent = nullptr);
    ~DicomImageView();
    // 输入影像
    void SetSeriesInstance(SeriesInstance *m_series_);
    void UpdataSeriesInstance(const bool end = true);
    SeriesInstance *GetSeriesInstance() const;
    // 获取信息
    DicomImage *getHardCopyGrayScaledImage();
    QImage getTextLayerImage(const QSize &size);
    QPixmap getHardCopyPixmap();
    QImage getRenderedImage();
    QString GetImageFile();
    qint32 GetImageNum();
    bool HasSeries();
    // 设置交互
    void SetOperation(const ZoomOperation &operation);// 缩放操作
    void SetOperation(const RoateFlipOperation &operation);// 翻转操作
    void SetOperation(const DrawingType &operation);// 标注操作
    void SetOperation(const CurrentState &operation);// 鼠标状态操作
    void SetOperation(const WindowWLWHOperation &operation);// WL 操作
    void SetOperation(const FillterOperation &operation);// Fillter 操作
    // 设置参数
    void SetBorderHighlight(bool yes);
    void SetShowAnnotations(bool yes);
    void SetShowMeasurements(bool yes);
    void SetAnnoTextFont(const QFont &font);
    //
    void Reset();
    void GotoFrame(const qint32 &i);// 任意张
    void NextFrame();// 上一张
    void PrevFrame();// 下一张
  public Q_SLOTS:
    void Slot_SeriesDelate();
  Q_SIGNALS:
    void Signal_StatusChanged(bool hasImage);
    void Signal_ViewClicked(DicomImageView *view);
    void Singal_viewDoubleclicked(DicomImageView *view);

  protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *e);
    void leaveEvent(QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    //
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
  private:
    void RemoveCurrentDrawingItem();
    void SetMagnifierPixmap(const QPointF &itemPos);
    void UpdateScalors();
    void RefreshPixmap();
    void ResizePixmapItem();
    void RepositionAuxItems();
    void AllocAnnoGroups();
    void UpdateAnnotations();
    void FreeAnnoGroups();
    void PosValueShow(QMouseEvent *event);

    void MousePressHandle(QMouseEvent *event,
                          const CurrentState &state, const DrawingType &type);
    void MouseMoveHandle(QMouseEvent *event, const CurrentState &state);
    void MouseReleaseHandle(QMouseEvent *, const CurrentState &state);
    void UpdataShowAnnotations();
    void UpdataShowMeasurements();
  private:
    QGraphicsScene *m_scene_;
    SeriesInstance *m_series_;
    QList<AbstractPathItem *> item_list_;

    QGraphicsPixmapItem *pixmap_item_;// 中央显示图片 item
    QGraphicsPixmapItem *sub_pixmapItem_;// 局部放大 显示图片 item
    QGraphicsPathItem *x_scalor_item_;// x轴比例尺 item
    QGraphicsPathItem *y_scalor_item_;// y轴比例尺 item

    QList<GraphicsAnnoGroup *> anno_grps_;// 周围描述 group
    QGraphicsSimpleTextItem *pos_value_item_; // 鼠标位置 数值描述
    QGraphicsSimpleTextItem *window_item_; // 窗位窗宽 数值描述
    QGraphicsSimpleTextItem *zoom_item_;// 缩放倍数 数值描述
    QGraphicsSimpleTextItem *mag_factor_item_;// 局部放大 倍数描述

    VideoControlView *video_controlview_;// 播放支持

    QString window_text_pattern_;
    QString pos_text_pattern_;
    QString zoom_text_pattern_;

    MouseButtonState mouse_left_state_;
    MouseButtonState mouse_mid_state_;
    MouseButtonState mouse_right_state_;

    AbstractPathItem *current_path_item_;
    QPoint prev_mouse_pos_;// 上一个鼠标位置
    double factor_;// 放大倍数
    double fix_factor_;// 比例尺
    double mag_factor_;// 放大镜 比例
    ModalityPref m_pref_;
    QFont anno_font_;

    bool show_measurements_;// 记录批注显示状态
    bool show_annotations_;// 记录描述显示状态
    bool manual_zoom_;// 手动缩放开启
    bool manual_pan_;
    bool hflip_;// 水平翻转
    bool vflip_;// 垂直翻转
    qint32 rotate_angle_;// 旋转角度
    ViewType m_vtype_;

    FillterOperation fillter_;// fillter 状态
};

#endif // DICOMIMAGEVIEW_H
