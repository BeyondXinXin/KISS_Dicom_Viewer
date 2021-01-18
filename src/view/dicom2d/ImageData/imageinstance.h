#ifndef IMAGEINSTANCE
#define IMAGEINSTANCE

#include <QExplicitlySharedDataPointer>
#include <QMetaType>

#include "imageinstancedata.h"

class DicomImage;

class ImageInstance: public QObject {
    Q_OBJECT
  public:
    ImageInstance(const QString &file);
    ImageInstance(DcmFileFormat *dff);
    ImageInstance(const ImageInstance &image);
    ~ ImageInstance();
    void SetWindow(const double &center, const double &width);
    void GetWindow(double &center, double &width) const;
    void SetWindowDelta(const double &dCenter, const double &dWidth);
    void SetRoiWindow(const QRectF &rect);
    void SetDefaultWindow();
    void SetFullDynamic();
    void GetPolarity(EP_Polarity p);

    EP_Polarity GetPolarity() const;
    QString GetStudyUid() const;
    QString GetSeriesUid() const;
    QString GetImageUid() const;
    QString GetClassUid() const;
    QString GetImageFile() const;
    bool GetPixmap(QPixmap &pixmap);
    bool GetPixmap(QPixmap &pixmap, const qint32 &frame);
    bool IsNormal() const;
    DicomImage *CreateClippedImage(
        const QRect &rect, int angle = 0, bool hflip = false,
        bool vflip = false, bool inverted = false);
    QString GetTagKeyValue(const DcmTagKey &key) const;
    uint GetPixelValue(long x, long y) const;
    bool GetPixSpacing(double &spacingX, double &spacingY) const;
    bool GetImageSize(ulong &width, ulong &height) const;
    const short *GetInternalPtr() const;
    const ushort *GetRawData() const;
    const DicomImage *GetDcmImage() const;
    DcmFileFormat *GetFileFormat();
    bool SaveFileFormat();
    qint32 GetFrameCount() const;
    //
    static bool GetPixmap(const QString &file, QPixmap &pixmap);
    static bool Dcm2BmpHelper(DicomImage &image, QPixmap &pixmap,
                              const qint32 frame = 0);
  private:
    QExplicitlySharedDataPointer<ImageInstanceData> d_;
};

Q_DECLARE_TYPEINFO(ImageInstance, Q_MOVABLE_TYPE);

#endif // IMAGEINSTANCE
