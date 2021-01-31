#include "seriesinstance.h"
#include "imageinstance.h"

#include <global/KissGlobal>
#include <engine/KissEngine>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"

//----------------------------------------------------------------
SeriesInstance::SeriesInstance(
    const QString &seriesUID, QObject *parent):
    QObject(parent) {
    cur_xy_frame_ = 0;
    cur_xz_frame_ = 0;
    cur_yz_frame_ = 0;
    img_height_ = 0;
    img_width_ = 0;
    vol_slice_ = 0;
    vol_ptr_ = nullptr;
    raw_ptr_ = nullptr;
    raw_slice_ = 0;
    m_pola_ = EPP_Normal;
    series_uid_ = seriesUID;
    m_pattern_ = Single_Frame;
    image_map_.clear();
}

//----------------------------------------------------------------
SeriesInstance::~SeriesInstance() {
    emit Signal_AboutToDelete();
    DelVolBuffer();
    qDeleteAll(image_map_.values());
    image_map_.clear();
}

//----------------------------------------------------------------
void SeriesInstance::DelVolBuffer() {
    delete[] vol_ptr_;
    vol_ptr_ = nullptr;
    vol_slice_ = 0;
    delete[] raw_ptr_;
    raw_ptr_ = nullptr;
    raw_slice_ = 0;
}

//----------------------------------------------------------------
bool SeriesInstance::InsertImage(ImageInstance *image) {
    if (!(image && image->IsNormal()
            && image->GetSeriesUid() == series_uid_)) {
        return false;
    }
    if (image_map_.isEmpty()) {
        image->GetWindow(win_center_, win_width_);
        image->GetWindow(def_center_, def_width_);
        image->GetImageSize(img_width_, img_height_);
        m_pola_ = image->GetPolarity();
        cur_xy_frame_ = 0;
        cur_xz_frame_ = 0;
        cur_yz_frame_ = 0;
    }
    int img_number_ = image->GetTagKeyValue(DCM_InstanceNumber).toInt();
    if (image_map_.contains(img_number_)) {
        return false;
    }
    image_map_.insert(img_number_, image);
    if(1 == image_map_.size()) {
        m_pattern_ = Single_Frame;
    } else if(image_map_.size() > 1) {
        m_pattern_ = Multi_Frame;
    }
    return true;
}

//----------------------------------------------------------------
bool SeriesInstance::RemoveImage(const QString &imgFile) {
    switch (m_pattern_) {
        case Single_Frame:
            return image_map_.remove(0) > 0;
        case Multi_Frame: {
                QMap<int, ImageInstance *> map = image_map_;
                QMap<int, ImageInstance *>::iterator i = map.begin();
                for (; i != map.end(); ++i) {
                    if (i.value() && i.value()->GetImageFile() == imgFile) {
                        return image_map_.remove(i.key()) > 0;
                    }
                }
                break;
            }
        default:
            break;
    }
    return false;
}

//----------------------------------------------------------------
bool SeriesInstance::IsEmpty() const {
    return image_map_.isEmpty();
}

//----------------------------------------------------------------
bool SeriesInstance::HasImage(const QString &file) {
    foreach (ImageInstance *image, image_map_.values()) {
        if (image->GetImageFile() == file) {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------
QString SeriesInstance::GetTagKeyValue(
    const DcmTagKey &key, const ViewType &type) const {
    QString value;
    if (key == DCM_NumberOfFrames) {
        switch (type) {
            case VT_XYPlane: {
                    value = QString::number(this->GetFrameCount());
                    break;
                }
            case VT_XZPlane:
                value = QString::number(img_height_);
                break;
            case VT_YZPlane:
                value =  QString::number(img_width_);
                break;
        }
    } else if(key == DCM_InstanceNumber) {
        switch (type) {
            case VT_XYPlane:
                value = QString::number(this->cur_xy_frame_ + 1);
                break;
            case VT_XZPlane:
                value = QString::number(this->cur_xz_frame_ + 1);
                break;
            case VT_YZPlane:
                value = QString::number(this->cur_yz_frame_ + 1);
                break;
        }
    } else  {
        ImageInstance *inst = GetCurrImageInstance(VT_XYPlane);
        if (inst) {
            value = inst->GetTagKeyValue(key);
            if (key == DCM_PatientBirthDate || key == DCM_StudyDate ||
                    key == DCM_SeriesDate || key == DCM_InstanceCreationDate ||
                    key == DCM_AcquisitionDate || key == DCM_ContentDate) {
                QDate d = QDate::fromString(value, DICOM_DATE_FORMAT);
                value = d.isValid() ? d.toString(NORMAL_DATE_FORMAT) : value;
            } else if (key == DCM_StudyTime || key == DCM_SeriesTime ||
                       key == DCM_InstanceCreationTime || key == DCM_AcquisitionTime ||
                       key == DCM_ContentTime) {
                QTime t = QTime::fromString(value.left(6), DICOM_TIME_FORMAT);
                value = t.isValid() ? t.toString(NORMAL_TIME_FORMAT) : value;
            } else if (key == DCM_AcquisitionDateTime) {
                QDateTime t = QDateTime::fromString(value.left(14), DICOM_DATETIME_FORMAT);
                value = t.isValid() ? t.toString(NORMAL_DATETIME_FORMAT) : value;
            } else if (key == DCM_KVP) {
                value = QString::number(value.toDouble());
            }
        }
    }
    return value;
}

//----------------------------------------------------------------
QString SeriesInstance::GetImageFile() const {
    return this->image_map_[this->image_map_.firstKey()]->GetImageFile();
}

//----------------------------------------------------------------
qint32 SeriesInstance::GetFrameCount(ViewType type) const {
    qint32 conun = 0;
    switch (type) {
        case VT_XYPlane: {
                switch (m_pattern_) {
                    case Single_Frame: {
                            conun = image_map_.first()->GetFrameCount();
                            break;
                        }
                    case Multi_Frame: {
                            conun = image_map_.size();
                            break;
                        }
                    default: {
                            break;
                        }
                }
                break;
            }
        case VT_XZPlane:
            conun = static_cast<qint32>(img_height_);
            break;
        case VT_YZPlane:
            conun = static_cast<qint32>(img_width_);
            break;
    }
    return conun;
}

//----------------------------------------------------------------
const short **SeriesInstance::GetSeriesVolume(
    const short ** &volume, ulong &width, ulong &height, ulong &slice) {
    volume = nullptr;
    if (image_map_.isEmpty()) {
        return volume;
    }
    if ((!vol_ptr_) || image_map_.values().size() != vol_slice_) {
        delete[] vol_ptr_;
        vol_slice_ = image_map_.values().size();
        vol_ptr_ = new const short*[vol_slice_];
        ImageInstance *image;
        for (ulong i = 0; i < vol_slice_; ++i) {
            image = image_map_.values().at(i);
            vol_ptr_[i] = image->GetInternalPtr();
        }
    }
    volume = vol_ptr_;
    width = img_width_;
    height = img_height_;
    slice = vol_slice_;
    return volume;
}

//----------------------------------------------------------------
const ushort **SeriesInstance::GetRawVolume(
    const ushort ** &volume, ulong &width, ulong &height, ulong &slice) {
    volume = nullptr;
    if (image_map_.isEmpty()) {
        return volume;
    }
    if ((!raw_ptr_) || image_map_.values().size() != raw_slice_) {
        delete[] raw_ptr_;
        raw_slice_ = image_map_.values().size();
        raw_ptr_ = new const ushort*[raw_slice_];
        ImageInstance *image;
        for (ulong i = 0; i < raw_slice_; ++i) {
            image = image_map_.values().at(i);
            raw_ptr_[i] = image->GetRawData();
        }
    }
    volume = raw_ptr_;
    width = img_width_;
    height = img_height_;
    slice = raw_slice_;
    return volume;
}

//----------------------------------------------------------------
ImageInstance *SeriesInstance::GetCurrImageInstance(ViewType type) const {
    if ((!image_map_.isEmpty()) && VT_XYPlane == type) {
        switch (m_pattern_) {
            case Single_Frame: {
                    return image_map_.values().at(0);
                }
            case Multi_Frame: {
                    return image_map_.values().at(cur_xy_frame_);
                }
            default: {
                    break;
                }
        }
    }
    return nullptr;
}

//----------------------------------------------------------------
QMap<int, ImageInstance *> SeriesInstance::GetImageInstanceMap() {
    return this->image_map_;
}

//----------------------------------------------------------------
bool SeriesInstance::GetPixmap(QPixmap &pixmap, ViewType type) {
    if (image_map_.isEmpty()) {
        return false;
    }
    ImageInstance *image;
    const short **volume;
    ulong w, h, s, rh;
    switch (type) {
        case VT_XYPlane:
            switch (m_pattern_) {
                case Single_Frame: {
                        image = image_map_.values().at(0);
                        break;
                    }
                case Multi_Frame: {
                        image = image_map_.values().at(cur_xy_frame_);
                        break;
                    }
                default: {
                        return false;
                    }
            }
            if (win_width_ < 1) {
                win_width_ = 1;
            }
            image->SetWindow(win_center_, win_width_);
            image->GetPolarity(m_pola_);
            return image->GetPixmap(pixmap, cur_xy_frame_);
        case VT_XZPlane:
            if (GetSeriesVolume(volume, w, h, s)) {
                double center = win_center_;
                double width = win_width_;
                double factor = 255 / width;
                double lower = center - width / 2;
                QImage srcImage(w, s, QImage::Format_Indexed8);
                QVector<QRgb> grayTable;
                for(int i = 0; i < 256; i++) {
                    grayTable.push_back(qRgb(i, i, i));
                }
                srcImage.setColorTable(grayTable);
                for (int i = 0; i < s; i++) {
                    const short *ptr = volume[i];
                    int idx = cur_xz_frame_ * h;
                    for (int j = 0; j < w; j++) {
                        short val = ptr[j * w + cur_xz_frame_];
                        if (val > lower + width) {
                            srcImage.setPixel(j, i, 255);
                        } else if (val > lower) {
                            qint32 value = (val - lower) * factor;
                            srcImage.setPixel(j, i, value);
                        } else {
                            srcImage.setPixel(j, i, 0);
                        }
                    }
                }
                pixmap = QPixmap::fromImage(srcImage);
                return true;
            }
            break;
        case VT_YZPlane:
            if (GetSeriesVolume(volume, w, h, s)) {
                double center = win_center_;
                double width = win_width_;
                double factor = 255 / width;
                double lower = center - width / 2;
                QImage srcImage(w, s, QImage::Format_Indexed8);
                QVector<QRgb> grayTable;
                for(int i = 0; i < 256; i++) {
                    grayTable.push_back(qRgb(i, i, i));
                }
                srcImage.setColorTable(grayTable);
                for (int i = 0; i < s; i++) {
                    const short *ptr = volume[i];
                    int idx = cur_yz_frame_ * h;
                    for (int j = 0; j < w; j++) {
                        short val = ptr[idx + j];
                        if (val > lower + width) {
                            srcImage.setPixel(j, i, 255);
                        } else if (val > lower) {
                            qint32 value = (val - lower) * factor;
                            srcImage.setPixel(j, i, value);
                        } else {
                            srcImage.setPixel(j, i, 0);
                        }
                    }
                }
                pixmap = QPixmap::fromImage(srcImage);
                return true;
            }
            break;
    }
    return false;
}

//----------------------------------------------------------------
void SeriesInstance::NextFrame(ViewType type) {
    switch (type) {
        case VT_XYPlane:
            cur_xy_frame_++;
            if (cur_xy_frame_ >= this->GetFrameCount()) {
                cur_xy_frame_ = 0;
            }
            break;
        case VT_XZPlane:
            cur_xz_frame_++;
            if (cur_xz_frame_ >= img_height_) {
                cur_xz_frame_ = 0;
            }
            break;
        case VT_YZPlane:
            cur_yz_frame_++;
            if (cur_yz_frame_ >= img_width_) {
                cur_yz_frame_ = 0;
            }
            break;
    }
}

//----------------------------------------------------------------
void SeriesInstance::PrevFrame(ViewType type) {
    if (image_map_.isEmpty()) {
        return;
    }
    switch (type) {
        case VT_XYPlane:
            cur_xy_frame_--;
            if (cur_xy_frame_ < 0) {
                cur_xy_frame_ = this->GetFrameCount() - 1;
            }
            break;
        case VT_XZPlane:
            cur_xz_frame_--;
            if (cur_xz_frame_ < 0) {
                cur_xz_frame_ = img_height_ - 1;
            }
            break;
        case VT_YZPlane:
            cur_yz_frame_--;
            if (cur_yz_frame_ < 0) {
                cur_yz_frame_ = img_width_ - 1;
            }
            break;
    }
}

//----------------------------------------------------------------
void SeriesInstance::GotoFrame(int index, ViewType type) {
    switch (type) {
        case VT_XYPlane:
            if (index < 0) {
                cur_xy_frame_ = 0;
            } else if (index >= this->GetFrameCount()) {
                cur_xy_frame_ = this->GetFrameCount() - 1;
            } else {
                cur_xy_frame_ = index;
            }
            break;
        case VT_XZPlane:
            if (index < 0) {
                cur_xz_frame_ = 0;
            } else if (index >= img_height_) {
                cur_xz_frame_ = img_height_ - 1;
            } else {
                cur_xz_frame_ = index;
            }
            break;
        case VT_YZPlane:
            if (index < 0) {
                cur_yz_frame_ = 0;
            } else if (index >= img_width_) {
                cur_yz_frame_ = img_width_ - 1;
            } else {
                cur_yz_frame_ = index;
            }
            break;
    }
}

//----------------------------------------------------------------
int SeriesInstance::GetCurIndex(ViewType type) {
    switch (type) {
        case VT_XYPlane:
            return cur_xy_frame_;
        case VT_XZPlane:
            return cur_xz_frame_;
        case VT_YZPlane:
            return cur_yz_frame_;
    }
    return 0;
}

//----------------------------------------------------------------
void SeriesInstance::SetWindow(const double &center, const double &width) {
    win_center_ = center;
    win_width_ = width;
}

//----------------------------------------------------------------
void SeriesInstance::GetWindow(double &center, double &width) const {
    center = win_center_;
    width = win_width_;
}

//----------------------------------------------------------------
void SeriesInstance::SetWindowDelta(const double &dCenter, const double &dWidth) {
    win_center_ += dCenter;
    win_width_ += dWidth;
}

//----------------------------------------------------------------
void SeriesInstance::SetRoiWindow(const QRectF &rect) {
    if (image_map_.isEmpty()) {
        return;
    }
    ImageInstance *image;
    switch (m_pattern_) {
        case Single_Frame: {
                image = image_map_.values().at(0);
                break;
            }
        case Multi_Frame: {
                image = image_map_.values().at(cur_xy_frame_);
                break;
            }
        default: {
                return;
            }
    }
    image->SetRoiWindow(rect);
    image->GetWindow(win_center_, win_width_);
}

//----------------------------------------------------------------
void SeriesInstance::SetDefaultWindow() {
    win_center_ = def_center_;
    win_width_ = def_width_;
}

//----------------------------------------------------------------
void SeriesInstance::SetFullDynamic() {
    if (image_map_.isEmpty()) {
        return;
    }
    ImageInstance *image;
    switch (m_pattern_) {
        case Single_Frame: {
                image = image_map_.values().at(0);
                break;
            }
        case Multi_Frame: {
                image = image_map_.values().at(cur_xy_frame_);
                break;
            }
        default: {
                return;
            }
    }
    image->SetFullDynamic();
    image->GetWindow(win_center_, win_width_);
}

//----------------------------------------------------------------
void SeriesInstance::SetPolarity(EP_Polarity polarity) {
    if (image_map_.isEmpty()) {
        return;
    }
    ImageInstance *image;
    switch (m_pattern_) {
        case Single_Frame: {
                image = image_map_.values().at(0);
                break;
            }
        case Multi_Frame: {
                image = image_map_.values().at(cur_xy_frame_);
                break;
            }
        default: {
                return;
            }
    }
    image->GetPolarity(polarity);
    m_pola_ = image->GetPolarity();
}

//----------------------------------------------------------------
EP_Polarity SeriesInstance::GetPolarity() const {
    return m_pola_;
}

//----------------------------------------------------------------
bool SeriesInstance::GetPixSpacing(
    double &spacingX, double &spacingY, ViewType type) const {
    double sx, sy, sz;
    if (image_map_.isEmpty()) {
        return false;
    }
    if (!image_map_.first()->GetPixSpacing(sx, sy)) {
        return false;
    }
    sz = image_map_.first()->GetTagKeyValue(DCM_SliceThickness).toDouble();
    switch (type) {
        case VT_XYPlane:
            spacingX = sx;
            spacingY = sy;
            break;
        case VT_XZPlane:
            if (sz <= 0) {
                return false;
            }
            spacingX = sx;
            spacingY = sz;
            break;
        case VT_YZPlane:
            if (sz <= 0) {
                return false;
            }
            spacingX = sy;
            spacingY = sz;
            break;
    }
    return true;
}

//----------------------------------------------------------------
double SeriesInstance::GetPixelValue(long x, long y, ViewType type) const {
    if (!image_map_.isEmpty()) {
        switch (type) {
            case VT_XYPlane:
                ImageInstance *image;
                switch (m_pattern_) {
                    case Single_Frame: {
                            image = image_map_.values().at(0);
                            break;
                        }
                    case Multi_Frame: {
                            image = image_map_.values().at(cur_xy_frame_);
                            break;
                        }
                    default: {
                            return 0;
                        }
                }
                return image->GetPixelValue(x, y);
            case VT_XZPlane:
                if (y >= 0 && y < image_map_.values().size()) {
                    return image_map_.values().at(y)->GetPixelValue(x, cur_xz_frame_);
                }
                break;
            case VT_YZPlane:
                if (y >= 0 && y < image_map_.values().size()) {
                    return image_map_.values().at(y)->GetPixelValue(cur_yz_frame_, x);
                }
                break;
        }
    }
    return 0;
}
