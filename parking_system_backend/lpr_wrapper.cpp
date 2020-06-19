#include "lpr_wrapper.h"


//QT Connections
inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Grayscale8 );//Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5
#endif

        return image;
    }

    default:
        qDebug() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

//opencv stuff
//take photo via usb cam

//LPR Stuff
struct bzh_LPRService_pkg lpr_image( const cv::Mat &image)
{
    pr::PipelinePR prc("model/cascade.xml",
                       "model/HorizonalFinemapping.prototxt", "model/HorizonalFinemapping.caffemodel",
                       "model/Segmentation.prototxt", "model/Segmentation.caffemodel",
                       "model/CharacterRecognization.prototxt", "model/CharacterRecognization.caffemodel",
                       "model/SegmenationFree-Inception.prototxt", "model/SegmenationFree-Inception.caffemodel"
                       );

    std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image, pr::SEGMENTATION_FREE_METHOD);

    //find the most confident result, filter out anything lower than 75%
    float max_conf = 0;
    int max_conf_index = -1;
    for(size_t i=0; i< res.size(); i++)
    {
        if(res[i].confidence >= 0.75 && res[i].confidence > max_conf)
        {
            max_conf = res[i].confidence;
            max_conf_index = i;
        }
    }

    struct bzh_LPRService_pkg pkg;
    if(max_conf_index >= 0)
    {
        //draw rectangle
        cv::Rect region = res[max_conf_index].getPlateRect();
        cv::rectangle(image, cv::Point(region.x, region.y), cv::Point(region.x + region.width, region.y + region.height), cv::Scalar(255, 255, 0), 2);

        //filling the PKG
        pkg.isSucceed = true;
        pkg.text = QString::fromLocal8Bit(res[max_conf_index].getPlateName().c_str());
        pkg.confidence = res[max_conf_index].confidence*100;
        pkg.image = cvMatToQPixmap(image);
    }
    else
    {
        pkg.isSucceed = false;
    }
    return pkg;
}
