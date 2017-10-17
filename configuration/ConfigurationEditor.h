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

#ifndef CONFIGURATIONEDITOR_H_
#define CONFIGURATIONEDITOR_H_

#include "ConfigLine.h"
#include "ui_Configuration.h"

#include <QGroupBox>

#include <string>
#include <map>


class ConfigurationEditor
{

public:
  enum Direction { toScreen, fromScreen };

  ConfigurationEditor();
  virtual ~ConfigurationEditor();

	static void handleText(QLineEdit * lineEdit, const ConfigLine * configLine, Direction dir);
	static void handleSlider(QSlider * slider, const ConfigLine * configLine, Direction dir);
	static void handleCheckbox(QCheckBox * checkbox, const ConfigLine * configLine, Direction dir, QString checked, QString unchecked);
	static void handleSpin(QSpinBox * spinbox, const ConfigLine * configLine, Direction dir, int dflt);
	static void handleSpin(QDoubleSpinBox * spinbox, const ConfigLine * configLine, Direction dir, int dflt);
	static void handleHexSpin(QSpinBox * hexSpinbox, const ConfigLine * configLine, Direction dir, int dflt);

  static void ARCHMODE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void ASN_AND_LX_REUSE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void ALRF(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void AUTO_MOUNT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void AUTO_SCSI_MOUNT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CNSLPORT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CONKPALV(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CPUMODEL(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CPUPRIO(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CPUSERIAL(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void CPUVERID(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void DEVPRIO(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void DEVTMAX(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void DIAG8CMD(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
  static void ECPSVM(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void ENGINES(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void HERCPRIO(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void HTTPPORT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void HTTPROOT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void IGNORE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void IODELAY(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LDMOD(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LEGACYSENSEID(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LOADPARM(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LOGOPT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LPARNAME(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void LPARNUM(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MAINSIZE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MANUFACTURER(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MAXCPU(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MODEL(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MODPATH(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void MOUNTED_TAPE_REINIT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void NUMCPU(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void NUMVEC(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void OSTAILOR(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void PANRATE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void PGMPRDOS(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void PLANT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void SHCMDOPT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void SHRDPORT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void SYSEPOCH(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void TIMERINT(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void TODDRAG(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void TODPRIO(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void TZOFFSET(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void XPNDSIZE(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);
	static void YROFFSET(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir);

	static bool validateNum(const std::string& str, int base);
	static int parseNum(const std::string& str, int base=10); //TODO util?
	static std::string normalizeNum(const std::string& str, int width=4, int base=10); //util?

private:

	static void validateMaxTokens(const ConfigLine * configLine, int maxNum);

	static std::map<std::string,bool> mCheckBoxMap;
	static std::map<std::string,int> mSpinMap;
};

#endif /* CONFIGURATIONEDITOR_H_ */
