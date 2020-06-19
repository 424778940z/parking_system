#ifndef LPR_WRAPPER_H
#define LPR_WRAPPER_H

#include <QString>
#include <QPixmap>

#include <QDebug>

#include "../include/Pipeline.h"
#include<fstream>
#include<vector>

struct bzh_LPRService_pkg
{
    bool isSucceed;
    QString text;
    float confidence;
    QPixmap image;
};

inline QImage  cvMatToQImage( const cv::Mat &inMat );
inline QPixmap cvMatToQPixmap( const cv::Mat &inMat );
struct bzh_LPRService_pkg lpr_image( const cv::Mat &image);

#endif // LPR_WRAPPER_H
