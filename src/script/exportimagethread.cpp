#include "exportimagethread.h"

#include <engine/KissEngine>
#include <global/KissGlobal>

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/dipijpeg.h"
#include "dcmtk/dcmimage/dipipng.h"
#include "dcmtk/dcmimage/dipitiff.h"

//----------------------------------------------------------------
ExportImageThread::ExportImageThread(QObject *parent) :
    QThread(parent),
    abort_(false),
    leaflet_(false),
    jpeg_quality_(90),
    frame_(0),
    file_type_(EFT_Dicom) {
}

//----------------------------------------------------------------
void ExportImageThread::run() {
    DcmRLEDecoderRegistration::registerCodecs(); // 寄存器RLE解压编解码器
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation); // 注册JPEG解压缩编解码器
    foreach (QString file, image_files_) {
        if (abort_) {
            break;
        }
        DcmFileFormat dfile;
        OFCondition cond = dfile.loadFile(file.toLocal8Bit().data());
        if (cond.bad()) {
            emit resultReady(tr("Failed: %1, %2.").arg(file, cond.text()));
            continue;
        }
        QString fileName = file;
        fileName.replace("/", QDir::separator()).replace("\\", QDir::separator());
        fileName = fileName.mid(fileName.lastIndexOf(QDir::separator()) + 1);
        QString filepath = dest_dir_ + QDir::separator() + fileName;
        QDir().mkpath(dest_dir_);
        if (file_type_ == EFT_Dicom) {
            OFCondition l_error = dfile.saveFile(filepath.toLocal8Bit().data());
            if (EC_Normal == l_error) {
                emit resultReady(tr("Exported: %1.").arg(file));
            } else {
                emit resultReady(tr("Failed: %1, %2.").arg(file, l_error.text()));
            }
            continue;
        }
        DcmDataset *dataset = dfile.getDataset();
        E_TransferSyntax xfer = dataset->getOriginalXfer();
        Sint32 frameCount;
        if (dataset->findAndGetSint32(DCM_NumberOfFrames, frameCount).bad()) {
            frameCount = 1;
        }
        ulong compabilityMode = CIF_DecompressCompletePixelData;
        if (frameCount > 1) {
            compabilityMode |= CIF_UsePartialAccessToPixelData;
        }
        DicomImage *di = new DicomImage(&dfile, xfer, compabilityMode);
        if (di->getStatus() != EIS_Normal) {
            emit resultReady(tr("Failed: %1, %2.").arg(file,
                             QString::fromLocal8Bit(DicomImage::getString(di->getStatus()))));
            delete di;
            continue;
        }
        di->setWindow(0);
        /* 将所选帧写入文件 */
        FILE *ofile = nullptr;
        QString suffix = GetSuffix(di);
        int result = 0;
        qint32 generate = this->frame_;
        qint32 generate_end = this->leaflet_ ? this->frame_ + 1 : frameCount;
        if(generate < frameCount) {
            for (; generate < generate_end; generate++) {
                /* 输出到文件 */
                if (fileName.right(4) == QString(".dcm")) {
                    fileName = fileName.mid(0, file.lastIndexOf('.'));
                }
                QString filepath = dest_dir_ +
                                   QDir::separator() +
                                   QString("%1%2%3.%4").arg(
                                       fileName,
                                       frameCount > 1 ? QString::number(generate) : QString(),
                                       this->leaflet_ ? "(" + QString::number(generate) + ")" : "",
                                       suffix);
                ofile = fopen(filepath.toLocal8Bit().data(), "wb");
                if (ofile == nullptr) {
                    emit resultReady(tr("Failed: %1, open dest file failed.").arg(file));
                    continue;
                }
                switch (file_type_) {// 最后创建输出图像文件
                    case EFT_RawPNM:
                        result = di->writeRawPPM(ofile, 8, static_cast<quint32>(generate));
                        break;
                    case EFT_8bitPNM:
                        result = di->writePPM(ofile, 8, static_cast<quint32>(generate));
                        break;
                    case EFT_16bitPNM:
                        result = di->writePPM(ofile, 16, static_cast<quint32>(generate));
                        break;
                    case EFT_BMP:
                        result = di->writeBMP(ofile, 0, static_cast<quint32>(generate));
                        break;
                    case EFT_8bitBMP:
                        result = di->writeBMP(ofile, 8, static_cast<quint32>(generate));
                        break;
                    case EFT_24bitBMP:
                        result = di->writeBMP(ofile, 24, static_cast<quint32>(generate));
                        break;
                    case EFT_32bitBMP:
                        result = di->writeBMP(ofile, 32, static_cast<quint32>(generate));
                        break;
                    case EFT_JPEG: {/* 初始化JPEG插件 */
                            DiJPEGPlugin plugin;
                            plugin.setQuality(static_cast<quint32>(jpeg_quality_));
                            plugin.setSampling(ESS_422);
                            result = di->writePluginFormat(&plugin, ofile, static_cast<quint32>(generate));
                        }
                        break;
                    case EFT_PNG: {/* 初始化PNG插件 */
                            DiPNGPlugin pngPlugin;
                            pngPlugin.setInterlaceType(E_pngInterlaceAdam7);
                            pngPlugin.setMetainfoType(E_pngFileMetainfo);
                            result = di->writePluginFormat(&pngPlugin, ofile, static_cast<quint32>(generate));
                        }
                        break;
                    case EFT_PastelPNM:
                        result = di->writePPM(ofile, MI_PastelColor, static_cast<quint32>(generate));
                        break;
#ifdef WITH_LIBTIFF
                    case EFT_TIFF: {
                            /* initialize TIFF plugin */
                            DiTIFFPlugin tiffPlugin;
                            tiffPlugin.setCompressionType(E_tiffPackBitsCompression);
                            tiffPlugin.setLZWPredictor(E_tiffLZWPredictorDefault);
                            tiffPlugin.setRowsPerStrip(OFstatic_cast(unsigned long, 0));
                            result = di->writePluginFormat(&tiffPlugin, ofile, static_cast<quint32>(frame));
                        }
#endif
                    default:
                        break;
                }
                if (result) {
                    emit resultReady(tr("Exported: %1.").arg(file));
                } else {
                    emit resultReady(tr("Failed: %1, conversion failed.").arg(file));
                }
                fclose(ofile);
            }
        }
        delete di;
    }
    DcmRLEDecoderRegistration::cleanup(); // 注销RLE解压缩编解码器
    DJDecoderRegistration::cleanup(); // 注销JPEG解压缩编解码器
}

//----------------------------------------------------------------
void ExportImageThread::SetJpegQuality(qint32 q) {
    jpeg_quality_ = q;
}

//----------------------------------------------------------------
void ExportImageThread::SetFileType(ExportImageThread::E_FileType type) {
    file_type_ = type;
}

//----------------------------------------------------------------
void ExportImageThread::SetAbort(bool yes) {
    abort_ = yes;
}

//----------------------------------------------------------------
void ExportImageThread::SetMultiplePng(const QStringList &images) {
    image_files_ = images;
}

//----------------------------------------------------------------
void ExportImageThread::SetLeafletPng(
    const bool &leaflet, const int &frame,  const QString &image) {
    this->frame_ = frame;
    this->leaflet_ = leaflet;
    image_files_.clear();
    image_files_ << image;
}

//----------------------------------------------------------------
void ExportImageThread::SetDestDir(const QString &dir) {
    dest_dir_ = dir;
}

//----------------------------------------------------------------
QString ExportImageThread::GetSuffix(const DicomImage *di) {
    QString ext;
    switch (file_type_) {// 确定默认文件扩展名
        case EFT_BMP:
        case EFT_8bitBMP:
        case EFT_24bitBMP:
        case EFT_32bitBMP:
            ext = "bmp";
            break;
        case EFT_JPEG:
            ext = "jpg";
            break;
        case EFT_TIFF:
            ext = "tif";
            break;
        case EFT_PNG:
            ext = "png";
            break;
        default:
            if (di->isMonochrome()) {
                ext = "pgm";
            } else {
                ext = "ppm";
            }
            break;
    }
    return ext;
}

