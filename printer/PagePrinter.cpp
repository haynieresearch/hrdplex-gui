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

#include "PagePrinter.h"
#include "Stationery.h"
#include "PrintRunner.h"
#include "Environment.h"
#include "gui.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QPainter>
#include <QPdfWriter>
#include <QFile>
#include <QFileInfo>
#include <QDir>

PagePrinter::PagePrinter(StationeryPtr &stationery, QPainter &painter) :
    GenericPagePrinter(stationery, 0.2)
{
    setBarsColor();
    mPainter = &painter;
    mPen = mPainter;

    mPaperHeight = mStationery->mSize1.toFloat() *25.4;
    mPaperWidth = mStationery->mSize2.toFloat() * 25.4;
    mLinesPerPage = mPaperHeight * mLinesPerInch;
}

void PagePrinter::setValues(bool)
{
}

void PagePrinter::setBarsColor()
{
    mStationery->getBarsColor(mBarsColor);
}

bool PagePrinter::print(QByteArray)
{
    preparePage(true);
    return true;
}

void PagePrinter::close()
{

}

PdfPrinter::PdfPrinter(const QString & dir, StationeryPtr &stationery, DecolationRulesPtr &decolation, int linesPerInch, int fontSize) :
    GenericPagePrinter(stationery, 1.0),  mPdf(NULL), mDir(dir), mFontSize(fontSize)
{
    mDecolation = decolation;
    float paperHeight = mStationery->mSize1.toFloat();
    mLinesPerInch = linesPerInch;
    mLinesPerPage = paperHeight * mLinesPerInch;
    mCurPage = 0;
    mCurLine = 0;
    mLastPages = decolation->mSeperator.mPages;
    mLastPagesCountdown = 0;

    setBarsColor();
}

void PdfPrinter::setBarsColor()
{
    mStationery->getBarsColor(mBarsColor);
}

void PdfPrinter::setValues(bool eject)
{
    if (mPdf == NULL) return;
    mLeftMargin = mPdf->logicalDpiX()/2; // 1/2 inch for margin
    mPixelsPerLine = (double)mPdf->logicalDpiY()/(double)mLinesPerInch;
    mMargin = mStationery->mTopMargin / 16.0 * mPdf->logicalDpiY(); //16 is inch factor (5/16 is kept as 5)
    mBottomMargin = 0.375L * mPdf->logicalDpiY();
    mPaperHeight = mStationery->mSize1.toFloat();
    mPaperWidth = mStationery->mSize2.toFloat();
    if (mStationery->mPaperSize.compare("Custom")==0 && !mStationery->mPortrait)
    {
        mPaperHeight = mStationery->mSize2.toFloat();
        mPaperWidth = mStationery->mSize1.toFloat();
    }

    mLinesPerPage = mPaperHeight * mLinesPerInch;

    if (eject) mPdf->newPage();
    mPdf->setPageSizeMM(QSize(mPaperWidth*25.4L, mPaperHeight*25.4L));
    mWidthInPixels = mPdf->width();
    mHeightInPixels = mPdf->height();
    mLogicalDpiX = mPdf->logicalDpiX();
    mLogicalDpiY = mPdf->logicalDpiY();

    if (!mPen) mPen = new QPainter(mPdf);
}


bool PdfPrinter::print(QByteArray line)
{
    // if reached maximum number of lines or eject found - advance to next page
    if ((mCurLine >= mLinesPerPage) || (line[0] == '\f') || mCurPage == 0)
    {
        mCurPage++;
        mCurLine = 0;
        if (mLastPagesCountdown > 0)
        {
            if (--mLastPagesCountdown == 0)
            {
                mCurPage = mCurLine = 0;
                close();
            }
        }
        if (line[0] == (char)12)
            return true; // not a real line
    }

    mCurLine++;

    if (mCurPage == 1)
    {
        if (mCurLine == 1)
        {
            mAccountingFields.clear();

            for (int i=0; i<mDecolation->mAccounting.mFields.count(); i++)
            {
                AccountingField& f = mDecolation->mAccounting.mFields[i];
                mAccountingFields.insert(f.mLine, f);
            }
        }

        auto fi = mAccountingFields.find(mCurLine);
        for (; fi != mAccountingFields.end(); fi++)
        {
            if (mCurLine == fi->mLine)
            {
                QString fld = line.mid(fi.value().mColumn-1, fi.value().mLength);
                mVariables.insert(fi.value().mName, fld);
                hOutDebug(3, "FLD:" << fi.value().mName.toStdString() << "=" << fld.toStdString());
            }
        }
        mFirstPage.push_back(line);
        return true;
    }

    if (mCurPage == 2 && mCurLine == 1)
    {
        mCurPage = 1;
        mCurLine = 1;
        QString decodedFileName = decodeFilename(); //TODO handle file tye (.pdf etc)
        QString newFileName = mDir + "/" + decodedFileName;
        QFileInfo fileInfo(newFileName);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists())
        {
            if (!dir.mkpath(dir.absolutePath()))
            {
                mDir = "/tmp/";
                newFileName = mDir + decodedFileName;
                fileInfo.setFile(newFileName);
                dir = fileInfo.absoluteDir();
                dir.mkpath(dir.absolutePath());
            }
        }
        mPen = NULL;
        mPdf = new QPdfWriter(newFileName); //TODO handle invalid filename (pdf is still created, but is not working)
        mVariables.clear();

        mPdf->setTitle(decodedFileName);
        mPdf->setCreator("Haynie Research & Development");
        while(mFirstPage.count() > 0)
        {
            QByteArray l = mFirstPage.front();
            mFirstPage.pop_front();
            doPrint(l);
            mCurLine++;
        }
        mCurPage = 2;
        mCurLine = 1;
    }
    return doPrint(line);
}

bool PdfPrinter::doPrint(QByteArray &line)
{
    if (mCurLine == 1)
    {
        preparePage(mCurPage > 1);

        mPen->setPen(QPen(QColor(0x80, 0x80, 0x80), 1));
#ifdef Q_OS_DARWIN
        mPen->setFont(QFont("Courier",mFontSize));
#else
        mPen->setFont(QFont("FreeMono",mFontSize));
#endif
    }
    if (mPdf != NULL)
        mPen->drawText(mPdf->logicalDpiX()/2,mMargin+mCurLine*mPixelsPerLine, line);

    hOutDebug(3, "line " << mCurLine << " page " << mCurPage << " " << line.data());

    if (mLastPagesCountdown == 0)
    {
        if (endOfJob(line))
        {
            mLastPagesCountdown = mLastPages;
        }
    }
    return true;
}

void PdfPrinter::close()
{
    if (mPen != NULL && mPen->isActive()) mPen->end();
    delete mPdf;
    mPdf = NULL;
}

bool PdfPrinter::endOfJob(QByteArray &line)
{
    auto it = mEndJobTrigger.find(mCurLine);
    while(it != mEndJobTrigger.end() && it->line() == mCurLine)
    {
        if (QString(line.mid(it->column()-1, it->text().length())).compare(it->text(), Qt::CaseInsensitive) == 0)
        {
            mEndJobTrigger.erase(it);
            if (mEndJobTrigger.count() == 0) break;
            it = mEndJobTrigger.find(mCurLine);
        }
        else return false;
    }
    if (mEndJobTrigger.count() != 0) return false;
    return true;
}

QString PdfPrinter::decodeFilename()
{
    QString ret = mDecolation->mFileNameTemplate.toLower();
    for (auto it = mVariables.begin();it != mVariables.end(); it++)
    {
        QString str = "$" + it.key().toLower() + "$";
        ret.replace(str, it.value().trimmed());
    }
    hOutDebug(3,"filename:" << ret.toStdString() << " " << mVariables.count());
    return ret;
}
