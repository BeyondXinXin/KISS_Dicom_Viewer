#ifndef IMAGEINSTANCEDATA_H
#define IMAGEINSTANCEDATA_H

#include <QSharedData>
#include <QString>
#include <QPixmap>
#include <QPointer>
#include <QDateTime>
#include <QMetaType>
#include "dcmtk/dcmimgle/diutils.h"
class DcmTagKey;
class DicomImage;
class DcmFileFormat;



class ImageInstanceData: public QSharedData {
  public:
    static bool GetPixmap(const QString &dicomFile, QPixmap &pixmap);
    static bool Dcm2BmpHelper(DicomImage &dcm_image_,
                              QPixmap &pixmap, const qint32 frame = 0);
  public:
    explicit ImageInstanceData(const QString &file);
    explicit ImageInstanceData(DcmFileFormat *dff);
    ~ImageInstanceData();
    void SetWindow(const double &center, const double &width);
    void GetWindow(double &center, double &width) const;
    void SetWindowDelta(const double &dCenter, const double &dWidth);
    void SetRoiWindow(const QRectF &rect);
    void SetFullDynamic();
    void SetDefaultWindow();
    QString GetStudyUid() const;
    QString GetSeriesUid() const;
    QString GetImageUid() const;
    QString GetClassUid() const;
    QString GetImageFile() const;
    void SetPolarity(EP_Polarity polarity);
    EP_Polarity GetPolarity() const;
    bool GetPixmap(QPixmap &pixmap);
    bool GetPixmap(QPixmap &pixmap, const qint32 &frame);
    bool IsNormal() const;
    DicomImage *CreateClippedImage(const QRect &rect, int angle = 0,
                                   bool hflip = false, bool vflip = false,
                                   bool inverted = false);
    QString GetTagKeyValue(const DcmTagKey &key) const;
    QStringList GetTagsKeyValue()const;

    double GetPixelValue(long x, long y) const;
    bool GetPixSpacing(double &spacingX, double &spacingY) const;
    bool GetImageSize(ulong &width, ulong &height) const;
    const short *GetInternalPtr() const;
    const ushort *GetRawData() const;
    const DicomImage *GetDcmImage() const;
    DcmFileFormat *GetFileFormat() const;
    bool SaveFileFormat();
    qint32 GetFrameCount() const;
  private:
    void InitImage();
  private:
    QString study_uid_;
    QString series_uid_;
    QString image_uid_;
    QString class_uid_;

    double pixel_x_;
    double pixel_y_;
    double def_center_;
    double def_width_;
    double win_width_;
    double win_center_;
    DcmFileFormat *dcmff_; // dcm 文件
    DicomImage *dcm_image_; // 图片
    QString image_file_;// 文件名
};


#endif // IMAGEINSTANCEDATA_H
