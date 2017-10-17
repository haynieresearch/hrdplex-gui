/*
#**********************************************************
#* CATEGORY    APPLICATIONS
#* GROUP       HRDPLEX-GUI
#* AUTHOR      LANCE HAYNIE <LHAYNIE@HAYNIEMAIL.COM>
#* DATE        2017-10-17
#**********************************************************
#* MODIFICATIONS
#* 2017-10-17 - LHAYNIE - INITIAL VERSION
#**********************************************************

#HRDPLEX-GUI - A GUI Front End to the Hercules Mainframe Emulator
#Copyright (C) 2017  Haynie Research & Development

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "FieldTip.h"
#include "gui.h"

#include <QPainter>

const QBrush backgroundBrush(QColor(255, 255, 225));
const QPen outlinePen(Qt::black, 1);
const int textPadding = 6;
int tipHeight = 20;


FieldTip::FieldTip(QWidget *parent, bool top) :
    QWidget(parent), mTop(top),attachedField()
{
    setAttribute(Qt::WA_TranslucentBackground);
    expireTimer.setSingleShot(true);
    connect(&expireTimer, SIGNAL(timeout()), this, SLOT(deleteLater()));
}

FieldTip::~FieldTip()
{
}

void FieldTip::showMessage(const QString& message, int timeout/*ms*/) {
    if (!attachedField) return;

    setWindowTitle(message);
    calculatePosition();
    expireTimer.start(timeout);
    show();
    update();
}

QSize FieldTip::findBestSize(const QFontMetrics& fontMetrics, const QString& message) {
    int width = 100;
    QRect rect = fontMetrics.boundingRect(0, 0, width, 0,
                    Qt::AlignLeft | Qt::TextWordWrap, message);
    int count = 0;
    float ratio = float(rect.width())/float(rect.height());
    while ((ratio < 3.0 || ratio > 4.0) && count < 50) {
        width = ratio < 3.0 ? width+20 : width-20;
        rect = fontMetrics.boundingRect(0, 0, width, 0,
                    Qt::AlignLeft | Qt::TextWordWrap, message);
        ratio = float(rect.width())/float(rect.height());
        count++;
    }
    return rect.size();
}


void FieldTip::calculatePosition() {
    if (!attachedField) return;

    QPoint fieldPos = attachedField->mapToParent(QPoint(0,0));
    QSize size = findBestSize(this->fontMetrics(), windowTitle());
    hOutDebug(3,"size: " << size.width() << "," << size.height());
    size.setWidth(size.width() + (textPadding*2));
    size.setHeight(size.height() + (textPadding*2) + tipHeight);

    if (mTop)
    {
        int x = fieldPos.x() + attachedField->width()/2 - size.width() +15;
        int y = fieldPos.y() + attachedField->height() ;
        move(x, y); resize(size);
        hOutDebug(2,fieldPos.x() << "," << fieldPos.y());
        hOutDebug(2,"size: " << size.width() << "," << size.height());
        hOutDebug(2,x << "," << y);
    }
    else
    {
        int x = fieldPos.x() + attachedField->width()/2 - size.width() + 15;
        int y = fieldPos.y() - size.height();
        move(x, y); resize(size);
        hOutDebug(2,fieldPos.x() << "," << fieldPos.y());
        hOutDebug(2,"size: " << size.width() << "," << size.height());
        hOutDebug(2,x << "," << y);

    }
}

void FieldTip::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    const int roundness = 12;

    painter.setRenderHint(QPainter::Antialiasing);

    int xbase = width()-15;
    int ybase = height() - tipHeight;
    QPoint p1, p2, p3;
    if (mTop)
    {
        p1 = QPoint(width()-tipHeight-15, tipHeight+2); // bottom left
        p2 = QPoint(xbase, 0); // top
        p3 = QPoint(xbase, tipHeight+2); // bottom right
    }
    else
    {
        p1 = QPoint(width()-tipHeight-15, ybase-2); // top right
        p2 = QPoint(xbase, height()); // bottom
        p3 = QPoint(xbase, ybase-2); // top left
    }
    QPolygon triangle;
    triangle << p1 << p2 << p3;

    painter.setBrush(backgroundBrush);
    painter.setPen(outlinePen);
    if (mTop)
        painter.drawRoundedRect(1, 1+tipHeight, width()-2, ybase-2, roundness, roundness);
    else
        painter.drawRoundedRect(1, 1, width()-2, ybase-2, roundness, roundness);
    painter.setPen(Qt::transparent);
    painter.drawPolygon(triangle);
    painter.setPen(outlinePen);
    painter.drawLine(p1, p2);
    painter.drawLine(p2, p3);

    painter.drawText(textPadding, textPadding,
                     width()-textPadding, mTop ? ybase+textPadding + tipHeight : ybase-textPadding ,
                     Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,
                     windowTitle());
}
