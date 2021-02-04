#ifndef EXPORTIMAGETHREAD_H
#define EXPORTIMAGETHREAD_H

#include <QThread>
#include <QStringList>




class DicomImage;
class ExportImageThread : public QThread {
    Q_OBJECT
  public:
    /* output file types */
    enum E_FileType {
        EFT_Dicom,//
        EFT_RawPNM,//
        EFT_8bitPNM,//
        EFT_16bitPNM,//
        EFT_BMP,//
        EFT_8bitBMP,//
        EFT_24bitBMP,//
        EFT_32bitBMP,//
        EFT_JPEG,//
        EFT_TIFF,//
        EFT_PNG,//
        EFT_PastelPNM,//
    };
    explicit ExportImageThread(QObject *parent = nullptr);
    void run();
    void SetJpegQuality(qint32 q);
    void SetFileType (E_FileType type);
    void SetAbort(bool yes);
    void SetDestDir(const QString &dir);
    void SetMultiplePng(const QStringList &images);
    void SetLeafletPng(const bool &leaflet,
                       const int &frame,
                       const QString &image);

    void SetCurFrame(const int &frame);
    void SetLeaflet(const bool &leaflet);

  Q_SIGNALS:
    void resultReady(const QString &msg);
  private:
    QString GetSuffix(const DicomImage *di);
  private:
    char m_padding[2];
    bool abort_, leaflet_;
    qint32 jpeg_quality_, frame_;
    E_FileType file_type_;
    QString dest_dir_;
    QStringList image_files_;
};

#endif // EXPORTIMAGETHREAD_H
