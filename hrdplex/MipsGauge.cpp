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

#include "MipsGauge.h"
#include "gui.h"

#include <QPaintEvent>
#include <QPainter>

MipsGauge::MipsGauge(QWidget * parent)
	: Mips(parent)
{
	mHwm = 0.0;
	resize(120,120);
	mPenColor = QColor::fromRgb(250,250,250,50);
}

MipsGauge::~MipsGauge()
{
}

void MipsGauge::move(int x, int y)
{
	QWidget::move(x,y);
}

void MipsGauge::display(double value)
{
	mValue = value;
	hOutDebug(5,"mValue=" << mValue);
	if (mValue > mHwm)
		mHwm = mValue;
	static QRect rect(0,0,120,120);
	update(rect);
}

void MipsGauge::setActive(bool active)
{
	hOutDebug(5, "MipsGauge " << (active? "Active" : "NotActive"));
	if (active)
		mPenColor = QColor::fromRgb(250,250,250,255);
	else
		mPenColor = QColor::fromRgb(250,250,250,50);
	this->repaint();
}

void MipsGauge::setToolTip(const QString & tip)
{
	QWidget::setToolTip(tip);
}

void MipsGauge::paintEvent(QPaintEvent *)
{
	hOutDebug(5,"Gauge paint" << std::endl);
	QPainter painter(this);
	painter.setPen(mPenColor);

	painter.drawArc(0,10,120,120,30*16,120*16);
	painter.translate(60,70);

	painter.rotate(30.0);
	for (int angle=30; angle<=150; angle+=10)
	{
		painter.drawLine(-65,0,-60,0);
		painter.rotate(10.0);
	}
	painter.rotate(-160.0);
	mPrevAngle = 30.0;

	double angle = 30.0 ;
	if (mHwm > 0.0)
		angle = 30 + (mValue/mHwm*120);
	hOutDebug(5,"angle="<<angle << " hwm=" << mHwm << std::endl);

	painter.rotate(angle);
	painter.drawLine(-50,0,-20,0);
}

void MipsGauge::deleteLater()
{
	QWidget::deleteLater();
}

bool MipsGauge::isVisible()
{
	return QWidget::isVisible();
}
