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

#ifndef COLORMANIPULATOR_H
#define COLORMANIPULATOR_H

#include <QDeclarativeItem>

class ColorManipulator : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit ColorManipulator(QDeclarativeItem *parent = 0);
    QString source() const { return m_source; }
    void setSource(const QString& source);
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    Q_INVOKABLE void click(int x, int y);

signals:
    void sourceChanged();

private:
    void convertImageToGray();

    QString m_source;
    QImage m_image;
    QImage m_grayImage;
    QImage m_alphaLayerImage;
};

#endif // COLORMANIPULATOR_H
