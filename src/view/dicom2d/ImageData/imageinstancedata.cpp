#include "imageinstancedata.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmjpeg/djdecode.h"


//---------------------------------------------------------------
void FreeBuffer(void *pBuf) {
    delete pBuf;
}

//---------------------------------------------------------------
bool ImageInstanceData::GetPixmap(const QString &dicomFile, QPixmap &pixmap) {
    ImageInstanceData image(dicomFile);
    return image.GetPixmap(pixmap);
}

//---------------------------------------------------------------
bool ImageInstanceData::Dcm2BmpHelper(
    DicomImage &dcmImage, QPixmap &pixmap, const qint32 frame) {
    qint32 w = static_cast<qint32>(dcmImage.getWidth());
    qint32 h = static_cast<qint32>(dcmImage.getHeight());
    void *pDIB = nullptr;
    qint32 size;
    if(dcmImage.getFrameCount() > 1) {
        quint64 tmp = static_cast<quint64>(frame);
        size = static_cast<qint32>(dcmImage.createWindowsDIB(pDIB, 0, tmp, 32, 0, 1));
    } else {
        size = static_cast<qint32>(dcmImage.createWindowsDIB(pDIB, 0, 0, 32, 0, 1));
    }
    if (size == w * h * 4) {
        QImage image( static_cast<uchar *>(pDIB), w, h,
                      QImage::Format_RGB32, FreeBuffer, pDIB);
        pixmap = QPixmap::fromImage(image);
        return !pixmap.isNull();
    }
    delete pDIB;
    return false;
}

//---------------------------------------------------------------
ImageInstanceData::ImageInstanceData(const QString &file) {
    pixel_x_ = 0;
    pixel_y_ = 0;
    dcm_image_ = nullptr;
    image_file_ = file;
    dcmff_ = new DcmFileFormat;
    if (dcmff_->loadFile(file.toLocal8Bit().data()).good()) {
        InitImage();
    }
}

//---------------------------------------------------------------
ImageInstanceData::ImageInstanceData(DcmFileFormat *dff) {
    dcmff_ = dff;
    pixel_x_ = 0;
    pixel_y_ = 0;
    dcm_image_ = nullptr;
    InitImage();
}

//---------------------------------------------------------------
ImageInstanceData::~ImageInstanceData() {
    delete dcm_image_;
    delete dcmff_;
    DJDecoderRegistration::cleanup();
}

//---------------------------------------------------------------
void ImageInstanceData::SetWindow(const double &center, const double &width) {
    win_center_ = center;
    win_width_ = width;
}

//---------------------------------------------------------------
void ImageInstanceData::GetWindow(double &center, double &width) const {
    center = win_center_;
    width = win_width_;
}

//---------------------------------------------------------------
void ImageInstanceData::SetWindowDelta(const double &dCenter, const double &dWidth) {
    win_center_ += dCenter;
    win_width_ += dWidth;
}

//---------------------------------------------------------------
bool ImageInstanceData::IsNormal() const {
    return dcm_image_ && (dcm_image_->getStatus() == EIS_Normal);
}

//---------------------------------------------------------------
void ImageInstanceData::SetRoiWindow(const QRectF &rect) {
    if (IsNormal()) {
        dcm_image_->setRoiWindow(
            static_cast<unsigned long>(rect.left()),
            static_cast<unsigned long>(rect.top()),
            static_cast<unsigned long>(rect.width()),
            static_cast<unsigned long>(rect.height()));
        dcm_image_->getWindow(win_center_, win_width_);
    }
}

//---------------------------------------------------------------
void ImageInstanceData::SetFullDynamic() {
    if (!IsNormal()) {
        return ;
    }
    dcm_image_->setMinMaxWindow();
    dcm_image_->getWindow(win_center_, win_width_);
}

//---------------------------------------------------------------
void ImageInstanceData::SetDefaultWindow() {
    win_center_ = def_center_;
    win_width_ = def_width_;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetStudyUid() const {
    return study_uid_;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetSeriesUid() const {
    return series_uid_;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetImageUid() const {
    return image_uid_;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetClassUid() const {
    return class_uid_;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetImageFile() const {
    return image_file_;
}

//---------------------------------------------------------------
void ImageInstanceData::SetPolarity(EP_Polarity polarity) {
    if (!IsNormal()) {
        return ;
    }
    dcm_image_->setPolarity(polarity);
}

//---------------------------------------------------------------
EP_Polarity ImageInstanceData::GetPolarity() const {
    return IsNormal() ? dcm_image_->getPolarity() : EPP_Normal;
}

//---------------------------------------------------------------
const DicomImage *ImageInstanceData::GetDcmImage() const {
    return dcm_image_;
}

//---------------------------------------------------------------
DcmFileFormat *ImageInstanceData::GetFileFormat() const {
    return dcmff_;
}

//---------------------------------------------------------------
bool ImageInstanceData::SaveFileFormat() {
    DcmDataset *dset;
    DicomImage *image, *tmp;
    if (!IsNormal()) {
        return false;
    }
    dset = dcmff_->getDataset();
    image = new DicomImage(dcmff_, dset->getOriginalXfer());
    tmp = dcm_image_;
    dcm_image_ = image;
    delete tmp;
    dset->findAndGetFloat64(DCM_WindowWidth, win_width_);
    dset->findAndGetFloat64(DCM_WindowCenter, win_center_);
    def_center_ = win_center_;
    def_width_ = win_width_;
    dcm_image_->setWindow(win_center_, win_width_);
    if (!image_file_.isEmpty()) {
        return EC_Normal == dcmff_->saveFile(image_file_.toLocal8Bit().data());
    }
    return false;
}

//---------------------------------------------------------------
qint32 ImageInstanceData::GetFrameCount() const {
    return this->dcm_image_->getFrameCount();
}

//---------------------------------------------------------------
bool ImageInstanceData::GetPixmap(QPixmap &pixmap) {
    if (!IsNormal()) {
        return  false;
    }
    if (win_width_ < 1) {
        win_width_ = 1;
    }
    dcm_image_->setWindow(win_center_, win_width_);
    return Dcm2BmpHelper(*dcm_image_, pixmap);
}


//---------------------------------------------------------------
bool ImageInstanceData::GetPixmap(QPixmap &pixmap, const qint32 &frame) {
    bool ret = false;
    if (IsNormal()) {
        if (win_width_ < 1) {
            win_width_ = 1;
        }
        dcm_image_->setWindow(win_center_, win_width_);
        ret = Dcm2BmpHelper(*dcm_image_, pixmap, frame);
    }
    return ret;
}

//---------------------------------------------------------------
DicomImage *ImageInstanceData::CreateClippedImage(
    const QRect &rect, int angle, bool hflip, bool vflip, bool inverted) {
    DicomImage *image = dcm_image_;
    if (!image) {
        return image;
    }
    int ret = 1;
    double min, max;
    image->getMinMaxValues(min, max);
    double pvalue = image->getPhotometricInterpretation() ==
                    EPI_Monochrome1 ? max : min;
    DicomImage *newImage =
        image->createClippedImage(
            static_cast<long>( rect.left()),
            static_cast<long>( rect.top()),
            static_cast<unsigned long>( rect.width()),
            static_cast<unsigned long>( rect.height()),
            static_cast<unsigned short>( pvalue));
    if (newImage) {
        if (ret && angle) {
            ret = newImage->rotateImage(angle % 360);
        }
        if (ret && hflip) {
            ret = newImage->flipImage(1, 0);
        }
        if (ret && vflip) {
            ret = newImage->flipImage(0, 1);
        }
        if (ret && inverted) {
            ret = newImage->setPolarity(EPP_Reverse);
        }
        if (!ret) {
            delete newImage;
            newImage = nullptr;
        }
    }
    return newImage;
}

//---------------------------------------------------------------
QString ImageInstanceData::GetTagKeyValue(const DcmTagKey &key) const {
    OFString val;
    if (dcmff_ && dcmff_->getDataset()) {
        dcmff_->getDataset()->findAndGetOFString(key, val);
    }
    return QString::fromLocal8Bit(val.c_str());
}

//----------------------------------------------------------
QStringList ImageInstanceData::GetTagsKeyValue() const {
    return QStringList();
}

//---------------------------------------------------------------
double ImageInstanceData::GetPixelValue(long x, long y) const {
    DicomImage *image = dcm_image_;
    if (image) {
        const DiPixel *pixel = image->getInterData();
        if (pixel && (x < static_cast<long>(image->getWidth())) && (x >= 0)
                && (y < static_cast<long>(image->getHeight())) && (y >= 0)) {
            EP_Representation r = pixel->getRepresentation();
            switch (r) {
                case EPR_Sint8:
                    return *((char *)(pixel->getData()) +
                             (y * image->getWidth() + x));
                case EPR_Uint8:
                    return *((uchar *)(pixel->getData()) +
                             (y * image->getWidth() + x));
                case EPR_Sint16:
                    return *((short *)(pixel->getData()) +
                             (y * image->getWidth() + x));
                case EPR_Uint16:
                    return *((ushort *)(pixel->getData()) +
                             (y * image->getWidth() + x));
                case EPR_Sint32:
                    return *((int *)(pixel->getData()) +
                             (y * image->getWidth() + x));
                case EPR_Uint32:
                    return *((uint *)(pixel->getData()) +
                             (y * image->getWidth() + x));
            }
        }
    }
    return 0;
}

//---------------------------------------------------------------
bool ImageInstanceData::GetPixSpacing(double &spacingX, double &spacingY) const {
    if (IsNormal()) {
        spacingX = pixel_x_;
        spacingY = pixel_y_;
        return true;
    }
    return false;
}

//---------------------------------------------------------------
bool ImageInstanceData::GetImageSize(ulong &width, ulong &height) const {
    if (IsNormal()) {
        width = dcm_image_->getWidth();
        height = dcm_image_->getHeight();
        return true;
    }
    return false;
}

//---------------------------------------------------------------
const short *ImageInstanceData::GetInternalPtr() const {
    return IsNormal() ?
           static_cast<const short *>(dcm_image_->getInterData()->getData()) : nullptr;
}

//---------------------------------------------------------------
const ushort *ImageInstanceData::GetRawData() const {
    if (IsNormal()) {
        const ushort *data = nullptr;
        OFCondition cond = dcmff_->getDataset()->findAndGetUint16Array(DCM_PixelData, data);
        return cond.bad() ? nullptr : data;
    }
    return nullptr;
}

//---------------------------------------------------------------
void ImageInstanceData::InitImage() {
    DJDecoderRegistration::registerCodecs();
    DcmDataset *dset;
    OFCondition result;
    if (dcmff_ && (dset = dcmff_->getDataset())) {
        dcmff_->loadAllDataIntoMemory();
        dset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);
        const char *val = nullptr;
        result = dset->findAndGetString(DCM_StudyInstanceUID, val);
        study_uid_ = QString::fromLocal8Bit(val);
        result = dset->findAndGetString(DCM_SeriesInstanceUID, val);
        series_uid_ = QString::fromLocal8Bit(val);
        result = dset->findAndGetString(DCM_SOPInstanceUID, val);
        image_uid_ = QString::fromLocal8Bit(val);
        result = dset->findAndGetString(DCM_SOPClassUID, val);
        class_uid_ = QString::fromLocal8Bit(val);
        result = dset->findAndGetFloat64(DCM_PixelSpacing, pixel_x_, 0);
        result = dset->findAndGetFloat64(DCM_PixelSpacing, pixel_y_, 1);
        if(pixel_x_ < 0.0001 && pixel_y_ < 0.0001) {
            // PixelSpacing 不存在则使用ImagerPixelSpacing
            result = dset->findAndGetFloat64(DCM_ImagerPixelSpacing, pixel_x_, 0);
            result = dset->findAndGetFloat64(DCM_ImagerPixelSpacing, pixel_y_, 1);
        }
        result = dset->findAndGetFloat64(DCM_WindowWidth, win_width_);
        result = dset->findAndGetFloat64(DCM_WindowCenter, win_center_);
        def_center_ = win_center_;
        def_width_ = win_width_;
        dcm_image_ = new DicomImage(dset, dset->getOriginalXfer());
        if (dcm_image_->getStatus() == EIS_Normal) {
            if (win_width_ < 1) {
                dcm_image_->setRoiWindow(0, 0, dcm_image_->getWidth(), dcm_image_->getHeight());
                dcm_image_->getWindow(win_center_, win_width_);
                def_center_ = win_center_;
                def_width_ = win_width_;
            }
        } else {
            delete dcm_image_;
            dcm_image_ = nullptr;
        }
    }
}
