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
}

void ColorManipulator::setSource(const QString &source)
{
    if (m_source.compare(source)) {
        m_source = source;

        if (!m_image.load(source))
            qDebug() << "Didn't load";

        setImplicitWidth(m_image.width());
        setImplicitHeight(m_image.height());

        convertImageToGray();

        m_alphaLayerImage = QImage(m_image.size(), QImage::Format_ARGB32_Premultiplied);
        m_alphaLayerImage.fill(qRgba(255, 255, 255, 0));

        m_finalImage = QImage(m_grayImage);

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
    QPainter alphaMaskPainter(&m_alphaLayerImage);
    alphaMaskPainter.setBrush(QColor::fromRgbF(1, 1, 1));
    alphaMaskPainter.drawEllipse(QPoint(x / scale(), y / scale()), 50, 50);

    // ### TODO radius = 50; should be inv scaled by scaled picture
    int topLine = y / scale() - 50 - 1;
    int bottomLine = y / scale() + 50 - 1;
    int leftLine = x / scale() - 50 - 1;
    int rightLine = x / scale() + 50 - 1;
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
            int alpha = qAlpha(alphaPixel);

            int r = alpha * qRed(colorPixel) + (255 - alpha) * qRed(greyPixel);
            int g = alpha * qGreen(colorPixel) + (255 - alpha) * qGreen(greyPixel);
            int b = alpha * qBlue(colorPixel) + (255 - alpha) * qBlue(greyPixel);

            *(resultLine + j) = qRgb(r / 255, g / 255, b / 255);
        }
    }

    update();
}
