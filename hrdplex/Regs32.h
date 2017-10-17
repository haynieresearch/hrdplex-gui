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

#ifndef REGS_H_
#define REGS_H_

#include "StatusUpdateCollector.h"

#include <QDialog>
#include <QLabel>
#include <QCheckBox>

#include <string>

class Regs32 : public QWidget, public StatusUpdateCollector
{
  Q_OBJECT
public:
  Regs32(QWidget * parent, std::string command);
  virtual ~Regs32();

  void setFont();
  virtual void setActive(bool active);
  bool notify(const QByteArray& statusLine);

  static std::string gCommand;
  static std::string fCommand;
  static std::string cCommand;
  static std::string aCommand;

protected:
  QWidget * mParent;
  QLabel mLine1, mLine2, mLine3, mLine4;  // subclass can add more
  std::string mCommand;
};

#endif /* REGS_H_ */
