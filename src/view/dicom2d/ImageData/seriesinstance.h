#ifndef SERIESINSTANCE_H
#define SERIESINSTANCE_H

#include "dcmtk/dcmimgle/diutils.h"
#include <QMap>
#include <QObject>

class DicomImage;
class ImageInstance;
class DcmTagKey;

enum ViewType
{
    VT_XYPlane,
    VT_XZPlane,
    VT_YZPlane,
};

class SeriesInstance : public QObject
{
    Q_OBJECT
public:
    enum SeriesPattern
    {
        Empty_Frame, //
        Single_Frame, // 单帧
        Multi_Frame, // 多帧
    };

public:
    explicit SeriesInstance(const QString & seriesUID,
                            QObject * parent = nullptr);
    ~SeriesInstance();
    bool InsertImage(ImageInstance * image);
    bool RemoveImage(const QString & imgFile);
    bool IsEmpty() const;
    bool HasImage(const QString & file);
    QString GetTagKeyValue(const DcmTagKey & key, const ViewType & type = VT_XYPlane) const;
    QString GetImageFile() const;
    qint32 GetFrameCount(ViewType type = VT_XYPlane) const;
    const short ** GetSeriesVolume(const short **& volume,
                                   ulong & width, ulong & height, ulong & slice);
    const ushort ** GetRawVolume(const ushort **& volume,
                                 ulong & width, ulong & height, ulong & slice);
    ImageInstance * GetCurrImageInstance(ViewType type) const;
    QMap<int, ImageInstance *> GetImageInstanceMap();

    bool GetPixmap(QPixmap & pixmap, ViewType type);
    void NextFrame(ViewType type);
    void PrevFrame(ViewType type);
    void GotoFrame(int index, ViewType type);
    int GetCurIndex(ViewType type);
    void SetWindow(const double & center, const double & width);
    void GetWindow(double & center, double & width) const;
    void SetWindowDelta(const double & dCenter, const double & dWidth);
    void SetRoiWindow(const QRectF & rect);
    void SetDefaultWindow();
    void SetFullDynamic();

    void SetPolarity(EP_Polarity polarity);
    EP_Polarity GetPolarity() const;
    double GetPixelValue(long x, long y, ViewType type) const;
    bool GetPixSpacing(double & spacingX, double & spacingY, ViewType type) const;
    void DelVolBuffer();

Q_SIGNALS:
    void Signal_AboutToDelete();

private:
    SeriesInstance(const SeriesInstance &);
    SeriesInstance & operator=(const SeriesInstance &);

private:
    QString series_uid_;
    int cur_xy_frame_;
    int cur_xz_frame_;
    int cur_yz_frame_;
    ulong img_width_;
    ulong img_height_;
    double win_center_;
    double win_width_;
    double def_center_;
    double def_width_;
    EP_Polarity m_pola_;
    const short ** vol_ptr_;
    ulong vol_slice_;
    const ushort ** raw_ptr_;
    ulong raw_slice_;
    QMap<int, ImageInstance *> image_map_;
    SeriesPattern m_pattern_;
};

#endif // SERIESINSTANCE_H
