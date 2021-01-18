#ifndef EXPORTIMAGETHREAD_H
#define EXPORTIMAGETHREAD_H

#include <QThread>
#include <QStringList>

class ExportImageThread : public QThread {
    Q_OBJECT
  public:
    /* output file types */
    enum E_FileType {
        EFT_Dicom,
        EFT_RawPNM,
        EFT_8bitPNM,
        EFT_16bitPNM,
        EFT_NbitPNM,
        EFT_BMP,
        EFT_8bitBMP,
        EFT_24bitBMP,
        EFT_32bitBMP,
        EFT_JPEG,
        EFT_TIFF,
        EFT_PNG,
        EFT_PastelPNM,
    };

    explicit ExportImageThread(QObject *parent = nullptr);

    void run();

    void setJpegQuality(quint32 q);
    void setFileType (E_FileType type);
    void setAbort(bool yes);
    void setImageFiles(const QStringList &images);
    void setDestDir(const QString &dir);

  Q_SIGNALS:
    void resultReady(const QString &msg);

  private:
    bool abort_;
    QStringList image_files_;
    QString dest_dir_;
    quint32 jpeg_quality_;
    E_FileType file_type_;

};

#endif // EXPORTIMAGETHREAD_H
