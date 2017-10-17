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

#ifndef MAINPANELCLASSIC_H_
#define MAINPANELCLASSIC_H_

#include "MainPanel.h"
#include "StatusUpdateCollector.h"
#include "Mips.h"

#include <QDialog>
#include "ui_Devices.h"
#include <QToolButton>
#include <QMouseEvent>

class MainPanelClassic : public MainPanel
{
  Q_OBJECT

public:
  MainPanelClassic(QWidget *parent = 0);
  virtual ~MainPanelClassic();
  void standby();
  void setDormant();
  bool notify(const QByteArray& statusLine);
  void switchMips();
  bool isStopped() const;
  virtual void updateLcd(QLCDNumber * lcd, int inc);
  virtual void animate();


protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual QSize sizeHint() const;
    virtual void setupUi(QWidget * parent);

private:
	template<class T> void animateOne(int x, int y, T* button, int step);

    QLabel      *mSys;
    QLabel      *mSysText;
    QLabel      *mMan;
    QLabel      *mManText;
    QLabel      *mWait;
    QLabel      *mWaitText;
    QLabel      *mLoad;
    QLabel      *mLoadText;
    QLabel      *mPSW;

  ClickLabel *mDial0, *mDial1, *mDial2, *mDial3;

  Mips *mMips;

};

#endif /* MAINPANELCLASSIC_H_ */
