#include "pretreatmentfiltersplugin.h"

#include <QtWidgets>

#include <math.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>


QStringList pretreatmentFiltersPlugin::GetPretreatments() const {
    return {
        tr("none"),
        tr(""),
        tr("sharpen"),
        tr("smooth"),
        tr("Edge"),
        tr("Emboss")
    };
}

cv::Mat QImage2cvMat(QImage image);
QImage Mat2QImage(const cv::Mat &mat);

QPixmap Sharpening(const QPixmap &pix, QWidget * ) {
    QImage image;
    image = pix.toImage();
    cv::Mat src = QImage2cvMat(image.convertToFormat(QImage::Format_ARGB32));
    cv::Mat dst;
    cv::Mat kern = (cv::Mat_<char>(3, 3) <<
                    0, -1, 0,
                    -1, 5, -1,
                    0, -1, 0);
    filter2D(src, dst, src.depth(), kern);
    image = Mat2QImage(dst);
    return QPixmap::fromImage(image);
}

QPixmap Smooth(const QPixmap &pix, QWidget * ) {
    QImage image;
    image = pix.toImage();
    cv::Mat src = QImage2cvMat(image.convertToFormat(QImage::Format_ARGB32));
    cv::Mat dst;
    GaussianBlur(src, dst, cv::Size(7, 7), 0, 0);
    image = Mat2QImage(dst);
    return QPixmap::fromImage(image);
}

QPixmap Edge(const QPixmap &pix, QWidget * ) {
    QImage image;
    image = pix.toImage();
    cv::Mat src = QImage2cvMat(image.convertToFormat(QImage::Format_ARGB32));
    cv::Mat dst;
    //
    cv::Mat src_gray, detected_edges;
    dst.create(src.size(), src.type());
    dst = cv::Scalar::all(0);
    if (src.channels() != 1) {
        cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);    // 转换灰度图像
    } else {
        src_gray = src;
    }
    blur(src_gray, detected_edges, cv::Size(3, 3));
    cv::Canny(detected_edges, detected_edges, 3, 3 * 3, 7);
    src.copyTo(dst, detected_edges);
    //
    image = Mat2QImage(dst);
    return QPixmap::fromImage(image);
}

QPixmap Emboss(const QPixmap &pix, QWidget * ) {
    QImage image;
    image = pix.toImage();
    cv::Mat src = QImage2cvMat(image.convertToFormat(QImage::Format_ARGB32));
    cv::Mat dst;
    cv::Mat kern = (cv::Mat_<char>(3, 3) <<
                    -2, -1, 0,
                    -1, 1, 1,
                    0, 1, 2);
    filter2D(src, dst, src.depth(), kern);
    image = Mat2QImage(dst);
    return QPixmap::fromImage(image);
}

Pretreatmen pretreatmentFiltersPlugin::PretreatmentFun(const QString &str) {
    Pretreatmen fun;
    if (str == tr("none") || str.isEmpty()) {
        fun = [&](const QPixmap & pix, QWidget *) {
            return pix;
        };
    } else if (str == tr("smooth")) {
        fun = Smooth;
    } else if (str == tr("sharpen")) {
        fun = Sharpening;
    }  else if (str == tr("Edge")) {
        fun = Edge;
    } else if (str == tr("Emboss")) {
        fun = Emboss;
    } else {
        fun = [&](const QPixmap & pix, QWidget *) {
            return pix;
        };
    }
    return fun;
}


cv::Mat QImage2cvMat(QImage image) { // QImage改成Mat
    cv::Mat mat;
    switch (image.format()) {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4,
                          (void *)image.constBits(), static_cast<unsigned long>(
                              image.bytesPerLine()));
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3,
                          (void *)image.constBits(), static_cast<unsigned long>(
                              image.bytesPerLine()));
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
        case QImage::Format_Grayscale8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1,
                          (void *)image.constBits(), static_cast<unsigned long>(
                              image.bytesPerLine()));
            break;
        default:
            qDebug() << "err format";
            break;
    }
    return mat.clone();
}

QImage Mat2QImage(const cv::Mat &mat) {
    if (mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for (int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, static_cast<quint32>(mat.cols) );
            pSrc += static_cast<quint32>(mat.step);
        }
        return image;
    } else if (mat.type() == CV_8UC3) {
        const uchar *pSrc = const_cast<const uchar *>(mat.data);
        QImage image(pSrc, mat.cols, mat.rows,
                     static_cast<qint32>(mat.step), QImage::Format_RGB888);
        return image.rgbSwapped();
    } else if (mat.type() == CV_8UC4) {
        const uchar *pSrc = const_cast<const uchar *>(mat.data);
        QImage image(pSrc, mat.cols, mat.rows,
                     static_cast<qint32>(mat.step), QImage::Format_ARGB32);
        return image.copy();
    } else if (mat.type() == 22) {
        cv::Mat src;
        mat.convertTo(src, CV_8UC3);
        const uchar *pSrc = const_cast<const uchar *>(src.data);
        QImage image(pSrc, src.cols, src.rows,
                     static_cast<qint32>(src.step), QImage::Format_RGB888);
        return image.rgbSwapped();
    } else {
        return QImage();
    }
}
