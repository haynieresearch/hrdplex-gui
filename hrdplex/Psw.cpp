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


#include "Psw.h"
#include "gui.h"
#include "Preferences.h"

#include <QStatusBar>
#include <iostream>

Psw::Psw( QStatusBar * statusBar)
: mHasStatusBar(false), mVisible(false), mCpu(NULL)
{
	mStatusBar = statusBar;
	mMode = Preferences::getInstance().pswMode();
	mFontCourier = new QFont("Mono",10);

	setFont();
	setStatusBar();
	mActive = false;
	setStatusVisible(false,false);
	mLine.reserve(100);
	mLine = QString(100,' ');
	setGeometry(0,0,1200,12);
}

Psw::~Psw()
{
	delete mFontCourier;
}

bool Psw::notify(const QByteArray& statusLine)
{
	if (!mActive || !mVisible)
		return false;
	if (statusLine.startsWith("SYS="))
    {
        if (statusLine[4] == '1')
        {
        	mSys->setText("SYS");
        }
        else
        {
        	mSys->setText("   ");
        }
		return true;
    }
	//STATUS=CPU0000 PSW=00000000 00000000 0000000000000000 M.W..... instcount=0
	//STATUS=CP00 PSW=00000000 00000000 0000000000000000 M....... instcount=0
	//0123456789012345678901234567890123456789012345678901234567890
	if (!statusLine.startsWith("STATUS=") || statusLine.length() < 61)
		return false;
	if (mMode == Psw::Docked)
	{
		mLine.replace(0,46, statusLine.mid(7,46));
		mLine.replace(48,statusLine.length()-56, statusLine.mid(60,statusLine.length()-56));
		setText(mLine);
	}
	else
	{
		mCpu->setText(statusLine.mid(7,51));
		mInstCount->setText(statusLine.mid(60));
		if (statusLine[54] == 'M')
			mMan->setText("MAN");
		else
			mMan->setText("   ");
		if (statusLine[56] == 'W')
			mWait->setText("WAIT");
		else
			mWait->setText("    ");
	}
	return true;
}

void Psw::setFont()
{
	Preferences& pref = Preferences::getInstance();
	QFont font(pref.fontName(Preferences::PswFontObject).c_str(),
			pref.fontSize(Preferences::PswFontObject),
			(pref.fontIsBold(Preferences::PswFontObject) ? QFont::Bold : QFont::Normal),
			pref.fontIsItalic(Preferences::PswFontObject));

    font.setStyleHint(QFont::Courier);
    QLabel::setFont(font);
}

void Psw::setMode(Psw::PswMode mode)
{
	mMode = mode;
	setStatusBar();
    setStatusVisible(mVisible, Preferences::getInstance().theme() == Preferences::Modern);
}

void Psw::setStatusBar()
{
	if (!mHasStatusBar)
    {
		mHasStatusBar = true;

		mCpu = new QLabel("        ", mStatusBar);
		mCpu->setMinimumWidth(400);
		mCpu->setFont(*mFontCourier);
		mCpu->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		mStatusBar->addWidget(mCpu);

		mSys = new QLabel("   ", mStatusBar);
		mSys->setFont(*mFontCourier);
		mSys->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		mStatusBar->addWidget(mSys);

		mWait = new QLabel("    ", mStatusBar);
		mWait->setFont(*mFontCourier);
		mWait->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		mStatusBar->addWidget(mWait);

		mMan = new QLabel("   ", mStatusBar);
		mMan->setFont(*mFontCourier);
		mMan->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		mStatusBar->addWidget(mMan);

		mInstCount = new QLabel("        ", mStatusBar);
		mInstCount->setFont(*mFontCourier);
		mInstCount->setFrameStyle(QFrame::StyledPanel | QFrame::NoFrame);
		mStatusBar->addWidget(mInstCount);
	}
}

void Psw::setStatusVisible(bool visible, bool modern)
{
	mCpu->setVisible((mMode != Docked) && visible);
	mSys->setVisible(visible && modern);
	mWait->setVisible(visible && modern);
	mMan->setVisible(visible && modern);
	mInstCount->setVisible((mMode != Docked) && visible);
	mVisible = visible;
}

void Psw::setVisible(bool visible, bool modern)
{
    QLabel::setVisible(visible);
    setStatusVisible(visible, modern);
}

bool Psw::visible()
{
	return mVisible;
}

void Psw::setDormant()
{
	mActive = false;
}

void Psw::standby()
{
	mActive = true;
}
