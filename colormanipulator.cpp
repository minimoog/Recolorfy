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
#include <QPixmap>
#include <QStyleOptionGraphicsItem>

ColorManipulator::ColorManipulator(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void ColorManipulator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
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

        m_finalImage = QImage(m_image.size(), QImage::Format_ARGB32_Premultiplied);
        m_finalImage.fill(qRgb(255, 255, 255));

        update();
        emit sourceChanged();
    }
}

void ColorManipulator::convertImageToGray()
{
    m_grayImage = QImage(m_image.size(), QImage::Format_RGB32);

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
    // ### TODO unoptimized
    QPainter alphaMaskPainter(&m_alphaLayerImage);
    alphaMaskPainter.setBrush(QColor::fromRgbF(1, 1, 1));
    alphaMaskPainter.drawEllipse(QPoint(x / scale(), y / scale()), 50, 50);

    QImage topLayerImage(m_image.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter topLayerPainter(&topLayerImage);
    topLayerPainter.drawImage(m_image.rect(), m_image);
    topLayerPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    topLayerPainter.drawImage(m_image.rect(), m_alphaLayerImage);

    QPainter finalPainter(&m_finalImage);
    finalPainter.drawImage(m_image.rect(), m_grayImage);
    finalPainter.drawImage(m_image.rect(), topLayerImage);

    update();
}
