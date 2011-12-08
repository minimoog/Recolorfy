/* Copyright (c) 2011, Antonie Jovanoski
 *
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact e-mail: Antonie Jovanoski <minimoog77_at_gmail.com>
 */

#include "colormanipulator.h"
#include <QtDebug>
#include <QPainter>

ColorManipulator::ColorManipulator(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void ColorManipulator::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawImage(m_grayImage.rect(), m_finalImage);
    //painter->drawImage(m_alphaLayerImage.rect(), m_alphaLayerImage);
}

void ColorManipulator::setSource(const QString &source)
{
    qDebug() << source;
    QImage loadedImage;

    if (m_source.compare(source)) {
        m_source = source;

        if (!loadedImage.load(source))
            qDebug() << "Didn't load";

        //resize to some reasonable size
        if (loadedImage.width() > 1000) {
            m_image = loadedImage.scaledToWidth(1000, Qt::SmoothTransformation);
        } else {
            m_image = loadedImage;
        }


        setImplicitWidth(m_image.width());
        setImplicitHeight(m_image.height());

        convertImageToGray();

        m_alphaLayerImage = QImage(m_image.size(), QImage::Format_ARGB32_Premultiplied);
        m_alphaLayerImage.fill(qRgba(0, 0, 0, 255));

        m_finalImage = QImage(m_grayImage);

        m_paintedCircles.clear();

        update();
        emit sourceChanged();
    }
}

void ColorManipulator::convertImageToGray()
{
    m_grayImage = QImage(m_image.size(), QImage::Format_ARGB32_Premultiplied);

    // SLOW
    for (int i = 0; i < m_image.width(); ++i)
        for (int j = 0; j < m_image.height(); ++j) {
            QRgb pixel = m_image.pixel(i, j);
            int luma = qGray(pixel);
            m_grayImage.setPixel(i, j, qRgb(luma, luma, luma));
        }
}

void ColorManipulator::click(int x, int y)
{
    // ### TODO
    // Discard duplicate coordinates

    int xcenter = x;
    int ycenter = y;
    int radius = 50 / scale();

    paintCircle(xcenter, ycenter, radius, true);

    //push
    Circle circle;
    circle.x = xcenter;
    circle.y = ycenter;
    circle.radius = radius;

    m_paintedCircles.push_back(circle);
}

void ColorManipulator::undo()
{
    if (m_paintedCircles.isEmpty())
        return;

    Circle circle = m_paintedCircles.takeLast();

    //simple erase
    paintCircle(circle.x, circle.y, circle.radius, false);

    // use bounding box instead calc distances
    // use QRect to determine if boundind boxes are intersected

    QRect erasedCircleRect(QPoint(circle.x - circle.radius, circle.y - circle.radius),
                           QPoint(circle.x + circle.radius, circle.y + circle.radius));

    foreach (const Circle& paintedCircle, m_paintedCircles) {
        QPoint topLeft(paintedCircle.x - paintedCircle.radius,
                       paintedCircle.y - paintedCircle.radius);
        QPoint bottomRight(paintedCircle.x + paintedCircle.radius,
                           paintedCircle.y + paintedCircle.radius);
        QRect paintedCircleRect(topLeft, bottomRight);

        if (erasedCircleRect.intersects(paintedCircleRect)) //repaint
            paintCircle(paintedCircle.x, paintedCircle.y, paintedCircle.radius, true);
    }

}

void ColorManipulator::paintCircle(int x, int y, int radius, bool opaque)
{
    QPainter alphaMaskPainter(&m_alphaLayerImage);

    if (opaque) {
        alphaMaskPainter.setBrush(QColor::fromRgbF(1, 1, 1));
        alphaMaskPainter.setPen(QColor::fromRgbF(1, 1, 1));
    }
    else {
        alphaMaskPainter.setBrush(QColor::fromRgbF(0, 0, 0));
        alphaMaskPainter.setPen(QColor::fromRgbF(0, 0, 0));
    }

    alphaMaskPainter.drawEllipse(QPoint(x, y), radius, radius);

    int topLine = y - radius - 1;
    int bottomLine = y + radius - 1;
    int leftLine = x - radius - 1;
    int rightLine = x + radius - 1;
    int imgHeight = m_image.height() - 1;
    int imgWidth = m_image.width() - 1;

    int firstScanLine = (topLine < 0) ? 0 : topLine;
    int lastScanLine = (bottomLine > imgHeight) ? imgHeight : bottomLine;
    int firstIndex = (leftLine < 0) ? 0 : leftLine;
    int lastIndex = (rightLine > imgWidth) ? imgWidth : rightLine;

    for (int i = firstScanLine; i <= lastScanLine; ++i) {
        const QRgb* grayLine = (const QRgb*)m_grayImage.constScanLine(i);
        const QRgb* colorLine = (const QRgb*)m_image.constScanLine(i);
        const QRgb* alphaLine = (const QRgb*)m_alphaLayerImage.constScanLine(i);
        QRgb* resultLine = (QRgb*)m_finalImage.scanLine(i);

        for (int j = firstIndex; j <= lastIndex; ++j) {
            QRgb colorPixel = *(colorLine + j);
            QRgb greyPixel = *(grayLine + j);
            QRgb alphaPixel = *(alphaLine + j);
            int alpha = qRed(alphaPixel);

            int r = alpha * qRed(colorPixel) + (255 - alpha) * qRed(greyPixel);
            int g = alpha * qGreen(colorPixel) + (255 - alpha) * qGreen(greyPixel);
            int b = alpha * qBlue(colorPixel) + (255 - alpha) * qBlue(greyPixel);

            *(resultLine + j) = qRgb(r / 255, g / 255, b / 255);
        }
    }
    update();
}
