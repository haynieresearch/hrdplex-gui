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

#include "gui.h"
#include "MainPanelClassic.h"
#include "Environment.h"
#include "PanelButton.h"
#include "Preferences.h"
#include "MipsLed.h"
#include "MipsGauge.h"

#include <sstream>


MainPanelClassic::MainPanelClassic(QWidget *parent)
: MainPanel(parent)
{
    setupUi(parent);
    setLoadAddress(Preferences::getInstance().iplDevice().c_str());
    doConnect();
}

MainPanelClassic::~MainPanelClassic()
{
    delete mMips;
    delete mYellowLow;
    delete mYellowHigh;
}

void MainPanelClassic::setupUi(QWidget *)
{
    const QColor black(50,50,50);
    const QColor white(255,255,20);
    QBrush blackBrush(black);
    QPalette blackPalette(white, black);
    this->setPalette(blackPalette);
    this->setAutoFillBackground(true);

    QString iconsPath = Environment::getIconsPath().c_str();
    mPowerOnButton = new PanelButton(iconsPath + "/poweronoffu.gif", iconsPath + "/poweronoffd.gif", this);
    mPowerOffButton = new PanelButton(iconsPath + "/poweroffu.gif", iconsPath + "/poweroffd.gif", this);
    mInterruptButton = new PanelButton(iconsPath + "/interruptu.gif", iconsPath + "/interruptd.gif", this);
    mLoadButton = new PanelButton(iconsPath + "/loadu.gif", iconsPath + "/loadd.gif", this);

    mRestartButton = new PanelButton(iconsPath + "/restartu.gif", iconsPath + "/restartd.gif", this);
    mStoreButton = new PanelButton(iconsPath + "/storeu.gif", iconsPath + "/stored.gif", this);
    mStopButton = new PanelButton(iconsPath + "/stopu.gif", iconsPath + "/stopd.gif", this);
    mStartButton = new PanelButton(iconsPath + "/startu.gif", iconsPath + "/startd.gif", this);

    mPowerOnButton->setToolTip("Power On");
    mPowerOffButton->setToolTip("Power Off");
    mInterruptButton->setToolTip("Interrupt");
    mLoadButton->setToolTip("Load (IPL)");
    mRestartButton->setToolTip("Restart");
    mStoreButton->setToolTip("Store");
    mStopButton->setToolTip("Stop all CPUs");
    mStartButton->setToolTip("Start all CPUs");

    QFont font;
    font.setPointSize(6);
    mSys = new QLabel(this);
    mSys->setPixmap(*mYellowLow);
    mSysText = new QLabel("SYS",this);
    mSysText->setFont(font);
    mMan = new QLabel(this);
    mMan->setPixmap(*mYellowLow);
    mManText = new QLabel("MAN",this);
    mManText->setFont(font);
    mWait = new QLabel(this);
    mWait->setPixmap(*mYellowLow);    //path = iconsPath + "/yellowhigh.png";

    mPSW = new QLabel(this);
    QFont pswFont("Mono", 10);
    mPSW->setFont(pswFont);

    mWaitText = new QLabel("WAIT",this);
    mWaitText->setFont(font);
    mLoad = new QLabel(this);
    mLoad->setPixmap(*mYellowLow);
    mLoadText = new QLabel("LOAD",this);
    mLoadText->setFont(font);
    mLcd0 = new QLCDNumber(1,this);;
    mLcd0->setSegmentStyle(QLCDNumber::Flat);
    mLcd0->setFrameStyle(QFrame::NoFrame);
    mLcd0->resize(20,20);
    mLcd0->setMode(QLCDNumber::Hex);
    mLcd1 = new QLCDNumber(1,this);
    mLcd1->setSegmentStyle(QLCDNumber::Flat);
    mLcd1->setFrameStyle(QFrame::NoFrame);
    mLcd1->resize(20,20);
    mLcd1->setMode(QLCDNumber::Hex);
    mLcd2 = new QLCDNumber(1,this);
    mLcd2->setSegmentStyle(QLCDNumber::Flat);
    mLcd2->setFrameStyle(QFrame::NoFrame);
    mLcd2->resize(20,20);
    mLcd2->setMode(QLCDNumber::Hex);
    mLcd3 = new QLCDNumber(1,this);
    mLcd3->setSegmentStyle(QLCDNumber::Flat);
    mLcd3->setFrameStyle(QFrame::NoFrame);
    mLcd3->resize(20,20);
    mLcd3->setMode(QLCDNumber::Hex);

    QString path = Environment::getIconsPath().c_str();
    QPixmap pixMap(path + "/dial1.gif");
    mDial0 = new ClickLabel(this,mLcd0);
    mDial0->setPixmap(pixMap);
    mDial1 = new ClickLabel(this,mLcd1);
    mDial1->setPixmap(pixMap);
    mDial2 = new ClickLabel(this,mLcd2);
    mDial2->setPixmap(pixMap);
    mDial3 = new ClickLabel(this,mLcd3);
    mDial3->setPixmap(pixMap);

    mDial0->setToolTip("Click to select IPL address");
    mDial1->setToolTip("Click to select IPL address");
    mDial2->setToolTip("Click to select IPL address");
    mDial3->setToolTip("Click to select IPL address");

    if (Preferences::getInstance().mipsAsGauge())
    {
        mMips = new MipsGauge(this);
    }
    else
    {
        mMips = new MipsLed(this);
    }
	mMips->setActive(false);
}

void MainPanelClassic::resizeEvent(QResizeEvent * )
{
    int width = this->size().rwidth();
    mPowerOnButton->move(width-90,15);
    mPowerOffButton->move(width-50,15);
    mInterruptButton->move(width-90,45);
    mLoadButton->move(width-50,45);
    mRestartButton->move(width-510,15);
    mStoreButton->move(width-550,15);
    mStopButton->move(width-510,45);
    mStartButton->move(width-550,45);

    mSys->move(width-200,60);
    mSysText->move(width-200,50);
    mMan->move(width-172,60);
    mManText->move(width-172,50);
    mWait->move(width-144,60);
    mWaitText->move(width-144,50);
    mLoad->move(width-116,60);
    mLoadText->move(width-116,50);

    mLcd0->move(width-250,5);
    mLcd1->move(width-310,5);
    mLcd2->move(width-370,5);
    mLcd3->move(width-430,5);

    mDial0->move(width-270,30);
    mDial1->move(width-330,30);
    mDial2->move(width-390,30);
    mDial3->move(width-450,30);

    mMips->move(width-680,20);

    mPSW->setGeometry(width-550,110,48*12,12);

}

void MainPanelClassic::updateLcd(QLCDNumber * lcd, int inc)
{
    lcd->display(lcd->intValue()+inc);
    if (lcd->intValue() > 15)
        lcd->display(0);
    if (lcd->intValue() < 0)
        lcd->display(15);
    hOutDebug(5,"update " << lcd->intValue());
}


void MainPanelClassic::setDormant()
{
	mMips->setActive(false);
	QString iconsPath = Environment::getIconsPath().c_str();
    ((PanelButton *)mPowerOnButton)->replaceButton(iconsPath + "/poweronoffu.gif", iconsPath + "/poweronoffd.gif");
}

void MainPanelClassic::standby()
{
	mMips->setActive(true);
    QString iconsPath = Environment::getIconsPath().c_str();
    ((PanelButton *)mPowerOnButton)->replaceButton(iconsPath + "/powerononu.gif", iconsPath + "/poweronond.gif");
}

bool MainPanelClassic::notify(const QByteArray& statusLine)
{
    bool ret = true;
    if (statusLine.startsWith("SYS="))
    {
        QChar sysVal = statusLine[4];
        if (sysVal == '1')
            mSys->setPixmap(*mYellowHigh);
        else
            mSys->setPixmap(*mYellowLow);

    }
    else if (statusLine.startsWith("LOAD="))
    {
        QChar sysVal = statusLine[4];
        if (sysVal == '1')
            mLoad->setPixmap(*mYellowHigh);
        else
            mLoad->setPixmap(*mYellowLow);

    }
    else if (statusLine.startsWith("MIPS="))
    {
        double d = atof(statusLine.data()+5);
        if (d > mMipsHWM)
        {
            mMipsHWM = d;
            std::stringstream ss;
            ss << "High Water Mark:" << d;
            mMips->setToolTip(ss.str().c_str());
        }
        mMips->display(d);
    }
    //STATUS=CPU0000 PSW=00000000 00000000 0000000000000000 M.W..... instcount=0
    else if ( (statusLine.startsWith("STATUS=")) && (statusLine.length() > 56) )
    {
      if (statusLine[54] == 'M')
        mMan->setPixmap(*mYellowHigh);
      else
        mMan->setPixmap(*mYellowLow);

      if (statusLine[56] == 'W')
        mWait->setPixmap(*mYellowHigh);
      else
        mWait->setPixmap(*mYellowLow);
    }
    else ret = false;

    return ret;
}

void MainPanelClassic::switchMips()
{
    bool updated = false;
    if (Preferences::getInstance().mipsAsGauge())
    {
        MipsLed * oldMips = dynamic_cast<MipsLed *>(mMips);
        if (oldMips != NULL)
        {
            Mips * gauge = new MipsGauge(this);
            mMips = gauge;
            mMips->setToolTip(oldMips->toolTip());
            oldMips->deleteLater();
            updated = true;
        }
    }
    else
    {
        MipsGauge * oldMips = dynamic_cast<MipsGauge *>(mMips);
        if (oldMips != NULL)
        {
            Mips * lcd = new MipsLed(this);
            mMips = lcd;
            mMips->setToolTip(oldMips->toolTip());
            oldMips->deleteLater();
            updated = true;
        }
    }
    if (updated)
    {
		mMips->setActive(true);
        int width = this->size().rwidth();
        mMips->move(width-680,20);
    }

}



QSize MainPanelClassic::sizeHint() const
{
    return QSize(100, 100);
}

bool MainPanelClassic::isStopped() const
{
    return mStopped;
}

template<class T> void MainPanelClassic::animateOne(int x, int y, T* button, int step)
{
	for (int i = 0; i<=x; i+=step)
	{
		button->move(i,y);
		this->repaint();
	}
	button->move(x,y);
}

void MainPanelClassic::animate()
{
	int width = this->size().rwidth();

	mPowerOnButton->move(width,15);
	mPowerOffButton->move(width,15);
	mInterruptButton->move(width,45);
	mLoadButton->move(width,45);
	mRestartButton->move(width,15);
	mStoreButton->move(width,15);
	mStopButton->move(width,45);
	mStartButton->move(width,45);

	mDial0->move(width,30);
	mDial1->move(width,30);
	mDial2->move(width,30);
	mDial3->move(width,30);

	mStoreButton->move(width,15);
	mStopButton->move(width,45);
	mStartButton->move(width,45);

	mLcd0->move(width,5);
	mLcd1->move(width,5);
	mLcd2->move(width,5);
	mLcd3->move(width,5);

	mSys->move(width,60);
	mSysText->move(width,50);
	mMan->move(width,60);
	mManText->move(width,50);
	mWait->move(width,60);
	mWaitText->move(width,50);
	mLoad->move(width,60);
	mLoadText->move(width,50);


	animateOne(width-90,15,mPowerOnButton,5);
	animateOne(width-50,15,mPowerOffButton,5);
	animateOne(width-90,45,mInterruptButton,5);
	animateOne(width-50,45,mLoadButton,5);

	animateOne(width-510,15,mRestartButton,5);
	mStoreButton->move(width-550,15);
	mStopButton->move(width-510,45);
	mStartButton->move(width-550,45);

	animateOne(width-270,30,mDial0,2);
	animateOne(width-330,30,mDial1,15);
	animateOne(width-390,30,mDial2,15);
	animateOne(width-450,30,mDial3,15);

	mSys->move(width-200,60);
	mSysText->move(width-200,50);
	mMan->move(width-172,60);
	mManText->move(width-172,50);
	mWait->move(width-144,60);
	mWaitText->move(width-144,50);
	mLoad->move(width-116,60);
	mLoadText->move(width-116,50);

	mLcd0->move(width-250,5);
	mLcd1->move(width-310,5);
	mLcd2->move(width-370,5);
	mLcd3->move(width-430,5);

	mMips->move(width-680,20);

}
