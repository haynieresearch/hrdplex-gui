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

#include "PrinterInterface.h"
#include "Environment.h"
#include "gui.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QPainter> //TODO move to PagePrinter

BasicPrinter::BasicPrinter()
{
}


GenericPagePrinter::GenericPagePrinter(StationeryPtr &stationery, float scaling) :
    mStationery(stationery),  mPen(NULL), mLogo(QPixmap(2200,1200)), mLogoWidth(0), mScaling(scaling)
{
    mDecolation.clear();
    mHeightInPixels = 4000;
    mWidthInPixels = 1200;
    mLeftMargin = 0;
    mLogicalDpiX = mLogicalDpiY = 1200;
}

void GenericPagePrinter::preparePage(bool eject)
{
    int builtinMargin = 250;
    const int penWidth = 20;
    const int alternatingStripes = 20;

    setValues(eject);

    mPen->fillRect(0, 0, mWidthInPixels, mHeightInPixels-mLeftMargin, QColor(0xff, 0xff, 0xff));

    if (eject) mEndJobTrigger.clear();
    if (!mDecolation.isNull())
    {
        for (int i=0; i<mDecolation->mSeperator.mTriggers.count(); i++)
        {
            Trigger t = mDecolation->mSeperator.mTriggers[i];
            mEndJobTrigger.insert(t.line(), t);
        }
    }

    hOutDebug(3, "in pixels: " <<mWidthInPixels << " " << mHeightInPixels);
    hOutDebug(1, "left margin " << mLeftMargin);

    int stripe = (mHeightInPixels-(mMargin+mBottomMargin))/alternatingStripes*mScaling;

    if (mHeightInPixels > 0)
    {
        for (int i=mMargin+stripe; i<=mMargin+(alternatingStripes)*stripe; i+=2*stripe)
        {
            int barsR = mBarsColor[0];
            int barsG = mBarsColor[1];
            int barsB = mBarsColor[2];
            mPen->fillRect(mLeftMargin, i, mWidthInPixels-2*mLeftMargin, stripe, QColor(barsR, barsG, barsB));

            //        if (mStationery->mDecorated)
            //        {
            //            mPen->setPen(QPen(QColor(barsR-0x20, barsG-0x20, barsB-0x20), penWidth));
            //            mPen->drawLine(leftMargin,i,widthInPixels-leftMargin,i);
            //            mPen->drawLine(leftMargin,i+stripe,widthInPixels-leftMargin,i+stripe);
            //        }
        }
    }

    if (mStationery->mDecorated)
    {
        int barsR = mBarsColor[0];
        int barsG = mBarsColor[1];
        int barsB = mBarsColor[2];
        mPen->setPen(QPen(QColor(barsR, barsG, barsB), penWidth*(mScaling==1.0 ? 1.0 : 0.1)));
        mPen->drawRect(mLeftMargin, mMargin, mWidthInPixels-2*mLeftMargin, stripe*(alternatingStripes));//heightInPixels-leftMargin);
    }

    if (mStationery->mTractorFeedHoles)
    {
        QString pixmapStr(Environment::getIconsPath().c_str());
        pixmapStr +="/Hole.png";
        QPixmap pixmap(pixmapStr);
        hOutDebug(5, "hole width " << pixmap.width() << " " << pixmap.logicalDpiX() << " "  << pixmap.physicalDpiX());
        float gap = mLogicalDpiY/2;
        float holeStart = mLogicalDpiY/4 - builtinMargin;
        if (mScaling < 1)
        {
            gap = 40;
            holeStart = 20;
        }

        for (int holePos=holeStart; holePos <= mHeightInPixels; holePos += gap) // image is 200 pixels
        {
            //TODO: automatic scaling?
            if (mScaling < 1.0)
            {
                mPen->drawPixmap(50*mScaling, holePos, 15, 15, pixmap, 0,0,200,200);
            }
            else
            {
                mPen->drawPixmap(160, holePos, pixmap);
                mPen->drawPixmap(mWidthInPixels-160-200, holePos, pixmap);
            }
        }
    }
    //mPen->drawRect(0,0,mWidthInPixels, mHeightInPixels); //debug

    if (mLogoWidth == 0)
    {
        mLogo.fill(QColor(255,255,255,255));
        QPainter * logoPainter = new QPainter(&mLogo);

        const char * logo{"Printed With Hercules Studio"};
        logoPainter->setFont(QFont("Verdana", 110));
        logoPainter->setPen(QColor(mBarsColor[0], mBarsColor[1], mBarsColor[2]));
        logoPainter->drawText(0, logoPainter->fontMetrics().height() , logo);
        mLogoWidth = logoPainter->fontMetrics().width(logo);
        mLogoHeight = logoPainter->fontMetrics().height();
        logoPainter->end();
    }
    mPen->drawPixmap(mWidthInPixels-mLeftMargin-mLogoWidth, mHeightInPixels - mLogoHeight, mLogo);
}
