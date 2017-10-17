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
#include "MainPanel.h"
#include "MainPanelClassic.h"
#include "Preferences.h"
#include "Environment.h"
#include "ConfigurationEditor.h"

#include <QResizeEvent>
#include <QColor>
#include <iostream>

MainPanel::MainPanel(QWidget *parent)
    :QWidget(parent),
    mMipsHWM(0)
{
    QString iconsPath = Environment::getIconsPath().c_str();
    mYellowLow = new QPixmap(iconsPath + "/yellow.png");
    mYellowHigh = new QPixmap(iconsPath + "/yellowhigh.png");
}

MainPanel::~MainPanel()
{
}

void MainPanel::doConnect()
{
    connect(powerOnButton(), SIGNAL(clicked()), this , SLOT(powerOnClickedSlot()));
    connect(powerOffButton(), SIGNAL(clicked()), this , SLOT(powerOffClickedSlot()));
    connect(interruptButton(), SIGNAL(clicked()), this , SLOT(interruptClickedSlot()));
    connect(loadButton(), SIGNAL(clicked()), this , SLOT(loadClickedSlot()));
    connect(stopButton(), SIGNAL(clicked()), this , SLOT(stopClickedSlot()));

    checkedConnect(mRestartButton,restartClickedSlot());
    checkedConnect(mStoreButton,storeClickedSlot());
    checkedConnect(mStartButton,startClickedSlot());
}

void MainPanel::powerOnClickedSlot()
{
    emit powerOnClicked();
}
void MainPanel::powerOffClickedSlot()
{
    emit powerOffClicked();
}

void MainPanel::loadClickedSlot()
{
    emit loadClicked();
}

void MainPanel::restartClickedSlot()
{
    emit restartClicked();
}

void MainPanel::storeClickedSlot()
{
    emit storeClicked();
}

void MainPanel::startClickedSlot()
{
  	mStopped=false;
    emit startClicked();
}

void MainPanel::stopClickedSlot()
{
  	mStopped=true;
    emit stopClicked();
}

void MainPanel::interruptClickedSlot()
{
    emit interruptClicked();
}



ClickLabel::ClickLabel(MainPanel * parent, QObject * lcd)
: QLabel(parent), mPanel(parent), mLcd(lcd)
{
}

void ClickLabel::mousePressEvent(QMouseEvent * event)
{
    mPanel->updateLcd((QLCDNumber*)(mLcd), event->x() > 30 ? 1 : -1);
    Preferences::getInstance().setIplDevice( std::string(QByteArray::number(mPanel->getLoadAddress(),16).data()) );
}

int MainPanel::getLoadAddress()
{
	return mLcd0->intValue() + 16*mLcd1->intValue() +
		(16*16)*mLcd2->intValue() + (16*16*16)*mLcd3->intValue();
}

void MainPanel::setLoadAddress(const char *devNo)
{
	int addr = ConfigurationEditor::parseNum(devNo,16);
	QLCDNumber *nums[4] = {mLcd0, mLcd1, mLcd2, mLcd3};
	for (int i=0; i< 4; i++)
	{
		int dig = addr%16;
		addr /= 16;
		nums[i]->display(dig);
	}
    Preferences::getInstance().setIplDevice( std::string(QByteArray::number(getLoadAddress(),16).data()) );
}
