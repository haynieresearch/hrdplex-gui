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

#ifndef MAINPANELMODERN_H_
#define MAINPANELMODERN_H_

#include "MainPanel.h"
#include "StatusUpdateCollector.h"
#include "Mips.h"

#include <QDialog>
#include "ui_Devices.h"
#include <QToolButton>
#include <QLCDNumber>
#include <QMouseEvent>

class MainPanelModern : public MainPanel
{
  Q_OBJECT

public:
  MainPanelModern(QWidget *parent = 0);
  virtual ~MainPanelModern();
  void standby();
  void setDormant();
  bool notify(const QByteArray& statusLine);
  void switchMips();
  bool isStopped() const { return mStopped; };
  virtual void updateLcd(QLCDNumber * , int ){} // null function in this context
  virtual void animate();

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual QSize sizeHint() const;
    virtual void setupUi(QWidget * parent);
    virtual QAbstractButton * powerOnButton() { return mPowerOnButton; };
    virtual QAbstractButton * powerOffButton() { return mPowerOffButton; };
    virtual QAbstractButton* interruptButton() {return mInterruptButton;};
    virtual QAbstractButton* stopButton() {return mStopButton;};
    virtual QAbstractButton* loadButton() {return mLoadButton;};

    void setButton(QToolButton*& button, QString text, QIcon icon);

 private slots:
    void stopClickedSlot();

private:

	template<class T> void animateOne(int x, int y, T* button, int step);

    QToolButton *mPowerOnButton;
    QToolButton *mPowerOffButton;
    QToolButton *mInterruptButton;
    QToolButton *mStopButton;
    QToolButton *mLoadButton;

	QLabel      *mPSW;

  Mips *mMips;
};

#endif /* MAINPANELMODERN_H_ */
