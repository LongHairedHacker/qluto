#include "waterfall.h"
#include <QPainter>

Waterfall::Waterfall(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    qDebug() << "Foo" << this->height();
    image = QImage((int)this->width(), (int)this->height(), QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0));

    QObject::connect(this, &QQuickItem::widthChanged, this, &Waterfall::sizeChanged);
    QObject::connect(this, &QQuickItem::heightChanged, this, &Waterfall::sizeChanged);

    // Generate displayable colors
    QImage img(500, 1, QImage::Format_ARGB32);
    colors.reserve(img.height());
    QPainter painter;
    painter.begin(&img);
    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setFinalStop(img.width(), 0);

    gradient.setColorAt(0.0, QColor(  2,   3,  36));
    gradient.setColorAt(0.1, QColor( 46,  61, 125));
    gradient.setColorAt(0.2, QColor( 79, 105, 191));
    gradient.setColorAt(0.3, QColor(114, 150, 255));
    gradient.setColorAt(0.4, QColor(106, 141, 245));
    gradient.setColorAt(0.5, QColor(164, 187, 255));
    gradient.setColorAt(0.6, QColor(213, 224, 255));
    gradient.setColorAt(0.7, QColor(255, 255, 255));
    gradient.setColorAt(0.8, QColor(255, 255, 255));
    gradient.setColorAt(0.9, QColor(255, 255, 255));
    gradient.setColorAt(1.0, QColor(255, 255,   0));
    gradient.setSpread(QGradient::PadSpread);
    painter.fillRect(QRect(0, 0, img.width(), 1), QBrush(gradient));
    painter.end();
    for (int i = 0; i < img.width(); i++) {
        QRgb rgb = img.pixel(i, 0);
        colors.append(rgb);
    }
}

void Waterfall::paint(QPainter *painter)
{
    QPen pen(QColorConstants::Blue, 2);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawPie(boundingRect().adjusted(1, 1, -1, -1), 90 * 16, 290 * 16);
    painter->drawImage(QRect(0, 0, width(), height()), image, QRect(0, 0, image.width(), image.height()));
}

void Waterfall::sizeChanged() {
    QImage img = QImage((int)this->width(), (int)this->height(), QImage::Format_ARGB32);
    img.fill(QColor(0, 0, 0));

    if (!image.isNull()) {
        QPainter painter;
        painter.begin(&img);
        painter.drawImage(QRect(0, 0, width(), height()), image, QRect(0, 0, image.width(), image.height()));
        painter.end();
    }

    image = img;

    update();
}

void Waterfall::addSamples(std::vector<uint16_t> samples)
{
    float sensitivity = 1.0;

    // Create new image:
    QImage img((int)this->width(), (int)this->height(), QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&img);

    // Draw 1st pixel row: new values
    for (int x = 0; x < img.width(); x++) {
        unsigned int index = ((double)x)/((double)img.width()) * samples.size();
        unsigned result = (int)(((double)samples[index])/8.19); // TODO has to be std::log?
        if(result >= 500)
            result = 499;
        painter.setPen(colors[result]);
        painter.drawRect(x, 0, 1, 1);
    }

    // Draw old values
    if (!image.isNull()) {
        painter.drawImage(QRect(0, 2, width(), height()), image, QRect(0, 0, image.width(), image.height()));
    }

    painter.end();

    image = img;
    update();
}