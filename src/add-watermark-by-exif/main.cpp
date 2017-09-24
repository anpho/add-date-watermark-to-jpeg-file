#include <QGuiApplication>
#include <QCommandLineParser>
#include <QImage>
#include <QFile>
#include "qexifimageheader.h"
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include "main.h"
#include <QFileInfo>


#define BASE_WIDTH_PX  900
#define BASE_HEIGHT_PX  600
#define BASE_FONT_HEIGHT_PX  24

enum DETEMINE_BY {
    DETEMINE_BY_EXIF_DATE,
    DETEMINE_BY_FILE_DATE
};

int main(int argc, char *argv[])
{
    QGuiApplication  a(argc, argv);
    QGuiApplication ::setApplicationName("add-watermark-by-exif");
    QGuiApplication ::setApplicationVersion("0.1");

    const QStringList args =a.arguments();

    //     source is args.at(0), destination is args.at(1)
    QString src(args[1]);
    QString tgt(args[2]);
    QFileInfo qfi (src);
    if (qfi.exists()){
        qDebug()<<src;
        QString exifDate = read_exif_date(src);
        qDebug()<<exifDate;
        qDebug()<<qfi.fileName();
        QString targetFilename = tgt + "\\"+qfi.fileName();
        qDebug()<<targetFilename;
        if (!exifDate.isEmpty()){
            add_watermark_to_image(src,exifDate,targetFilename);
            qDebug()<<endl<<"done";
        }else{
            qDebug()<<"Exif date not found.";
        }
    }
    a.exit();
    return 0;
}

/*
 * Read_Exif_Date from Image file at filepath
 * Return:
 *      "2010-01-01"    if Exif exists
 *      ""              if Exif Fails
 */
QString read_exif_date(QString filepath){
    QExifImageHeader exif;

    if (exif.loadFromJpeg(filepath)){
        // EXIF info exists
        if (exif.contains(QExifImageHeader::DateTime)){
            // has date time
            QExifValue v = exif.value(QExifImageHeader::DateTime);
            QString isoDate = v.toDateTime().date().toString( Qt::ISODate);
            return isoDate;
        }
    }
    return "";
}

/*
 * Add Watermark To Image
 *
 *
 */
bool add_watermark_to_image(QString &filepath, QString watermark_text, QString targetFilePath){
    QPixmap pm;
    if (pm.load(filepath)){
        qDebug()<<filepath<<" Loaded.";
        QImage image=pm.toImage();

        // Detemine fontsize to use.
        int img_width = image.width();
        int img_height = image.height();
        int longside = img_height > img_width ? img_height:img_width;
        int font_size = int(BASE_FONT_HEIGHT_PX) * longside / int(BASE_WIDTH_PX);

        // init draw properties
        QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        QPen pen = painter.pen();
        pen.setColor(QColor().fromRgb(255,204,0));

        QFont font = painter.font();
        font.setFamily("Arial");
        font.setBold(true);
        font.setPixelSize(font_size);

        painter.setPen(pen);
        painter.setFont(font);
        QRect textRect = image.rect();
        textRect.setHeight(textRect.height() * 0.95);
        textRect.setWidth(textRect.width() * 0.95);
        painter.drawText(textRect,Qt::AlignRight | Qt::AlignBottom,watermark_text);
        image.save(targetFilePath,"JPEG",85);
        return true;
    }else{
        qDebug()<<"PixMap create Failed.";
        return false;
    }
}
