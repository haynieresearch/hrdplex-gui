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
#include "MainPanelModern.h"
#include "Environment.h"
#include "PanelButton.h"
#include "Preferences.h"
#include "MipsLed.h"
#include "MipsGauge.h"
#include "ConfigurationEditor.h"
#include "Preferences.h"

#include <sstream>


MainPanelModern::MainPanelModern(QWidget *parent)
: MainPanel(parent)
{
	setupUi(parent);
	mLcd0 = new QLCDNumber(1,this); mLcd0->setVisible(false);
	mLcd1 = new QLCDNumber(1,this); mLcd1->setVisible(false);
	mLcd2 = new QLCDNumber(1,this); mLcd2->setVisible(false);
	mLcd3 = new QLCDNumber(1,this); mLcd3->setVisible(false);
	mStopped=true;
	doConnect();
}

MainPanelModern::~MainPanelModern()
{
	delete mMips;
	delete mYellowLow;
	delete mYellowHigh;
}

void MainPanelModern::setupUi(QWidget *)
{
	const QColor black(70,70,70);
    const QColor white(255,255,20);
    QBrush blackBrush(black);
    QPalette blackPalette(white, black);
    this->setPalette(blackPalette);
    this->setAutoFillBackground(true);

    QString iconsPath = Environment::getIconsPath().c_str() ;
    hOutDebug(3,"icons path:'" << iconsPath.toStdString() << "'");

    setButton(mPowerOnButton, QString("Power\nOn"), QIcon(iconsPath + "/modern/Play-icon.png"));
    setButton(mPowerOffButton, QString("Power\nOff"), QIcon(iconsPath + "/modern/Stop-icon.png"));
    setButton(mStopButton, QString("Stop"), QIcon(iconsPath + "/modern/Pause.png"));
    setButton(mInterruptButton, QString("Interrupt"), QIcon(iconsPath + "/modern/interrupt.png"));
	setButton(mLoadButton, QString("IPL\n(Boot)"), QIcon(iconsPath + "/modern/load.png"));

    mRestartButton = NULL;
    mStoreButton = NULL;
    mStartButton = NULL;

    mPowerOnButton->setToolTip("Power On");
    mPowerOffButton->setToolTip("Power Off");
    mInterruptButton->setToolTip("Interrupt");
    mLoadButton->setToolTip("Load (IPL)");
    mStopButton->setToolTip("Stop all CPUs");

    QFont font;
    font.setPointSize(6);

    mPSW = new QLabel(this);
    QFont pswFont("Mono", 10);
    mPSW->setFont(pswFont);

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

void MainPanelModern::setButton(QToolButton*& button, QString text, QIcon icon)
{
    button = new QToolButton(this);
    button->resize(60,70);
    button->setText(text);
    button->setIcon(icon);
    button->setStyleSheet("QToolButton {background: rgb(50,50,50); color: white; }");
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void MainPanelModern::resizeEvent(QResizeEvent * )
{
    int width = this->size().rwidth();
	mPowerOnButton->move(10,20);
	mPowerOffButton->move(70,20);
	mStopButton->move(130,20);
	mInterruptButton->move(190,20);
	mLoadButton->move(250,20);
	mMips->move(320,20);

    mPSW->setGeometry(width-550,110,48*12,12);
}

void MainPanelModern::setDormant()
{
    if (!mStopButton->text().contains("Stop", Qt::CaseSensitive))
    {
    	stopClickedSlot();
    }

	mMips->setActive(false);
}

void MainPanelModern::standby()
{
	mMips->setActive(true);

    const QColor black(50,50,50);
    const QColor white(255,255,255);
    QBrush blackBrush(black);
    QBrush whiteBrush(black);
    QPalette blackPalette(white, black);
    QPalette whitePalette(white, white);
    this->setPalette(whitePalette);
    this->setAutoFillBackground(true);
    this->repaint();
    this->setPalette(blackPalette);
}

bool MainPanelModern::notify(const QByteArray& statusLine)
{
	bool ret = true;
    if (statusLine.startsWith("MIPS="))
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
    //3.07 :STATUS=CPU0000 PSW=00000000 00000000 0000000000000000 M....... instcount=0
    else ret = false;

    return ret;
}

void MainPanelModern::switchMips()
{
	bool updated = false;
	bool visible = mMips->isVisible();
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
		mMips->setActive(visible);
		int width = this->size().rwidth();
		mMips->move(width-680,20);
	}

}

QSize MainPanelModern::sizeHint() const
{
    return QSize(100, 100);
}

void MainPanelModern::stopClickedSlot()
{
	if (!mMips->isVisible())  // actually: is hercules running
		return;
    if (mStopButton->text().contains("Stop", Qt::CaseSensitive))
    {
    	delete mStopButton;
    	setButton(mStopButton, QString("Resume"), QIcon(QString(Environment::getIconsPath().c_str()) + "/modern/Resume.png"));
    	mStopped=true;
    }
    else
    {
    	delete mStopButton;
        setButton(mStopButton, QString("Stop"), QIcon(QString(Environment::getIconsPath().c_str()) + "/modern/Pause.png"));
        mStopped=false;
    }
    resizeEvent(NULL);
    connect(stopButton(), SIGNAL(clicked()), this , SLOT(stopClickedSlot()));
    mStopButton->setVisible(true);
    emit stopClicked();
}

template<class T> void MainPanelModern::animateOne(int x, int y, T* button, int step)
{
	for (int i = this->width(); i>=x; i-=step)
	{
		button->move(i,y);
		this->repaint();
	}
}

void MainPanelModern::animate()
{
	int width = this->width();
	mPowerOnButton->move(width,20);
	mPowerOffButton->move(width,20);
	mStopButton->move(width,20);
	mInterruptButton->move(width,20);
	mLoadButton->move(width,20);
	mMips->move(width,20);

	animateOne(10,20,mPowerOnButton, 5);
	animateOne(70,20,mPowerOffButton, 5);
	animateOne(130,20,mStopButton, 10);
	animateOne(190,20,mInterruptButton, 10);
	animateOne(250,20,mLoadButton, 15);
	animateOne(320,20,mMips,5);
}
