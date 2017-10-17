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

#include "ConfigurationEditor.h"
#include "Configuration.h"
#include "gui.h"

#include <QRegExp>

#include <iostream>
#include <sstream>
#include <iomanip>

#define min(a,b) ((a) < (b) ? (a) : (b))  //util
#define max(a,b) ((a) > (b) ? (a) : (b))  //util

#define TextItem(PARM,_parm) \
void ConfigurationEditor::PARM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir) \
{       \
	handleText(configUi->_parm, configLine, dir);\
}

#define SliderItem(PARM,_parm) \
void ConfigurationEditor::PARM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir) \
{ \
	handleSlider(configUi->_parm, configLine, dir);     \
}

#define CheckBoxItem(PARM,parm, CHECKED,  UNCHECKED) \
void ConfigurationEditor::PARM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir) \
{ \
	handleCheckbox(configUi->parm, configLine, dir, #CHECKED, #UNCHECKED); \
}

#define SpinItem(PARM,_parm, default_) \
	void ConfigurationEditor::PARM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir) \
{\
  handleSpin(configUi->_parm, configLine, dir, default_); \
}

void ConfigurationEditor::handleText(QLineEdit * lineEdit, const ConfigLine * configLine, Direction dir)
{
	std::string parm = configLine->getToken(1);
    hOutDebug(4,configLine->getToken(0) << std::endl);
	if (dir == toScreen)
	{
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
		lineEdit->setText(parm.c_str());
	}
	else
	{
		/*TODO: verification routine*/
		if (lineEdit->isModified())
		{
			const_cast<ConfigLine *>(configLine)->replaceParameters(lineEdit->text().toStdString());
		}
	}
}

void ConfigurationEditor::handleSpin(QSpinBox * spinBox, const ConfigLine * configLine, Direction dir, int dflt)
{
	std::string keyword = configLine->getUppercaseToken(0);
	std::string parm = configLine->getToken(1);
    hOutDebug(4,parm);
	if (dir == toScreen)
	{
		std::string parm = configLine->getToken(1);
		if (parm.length() > 0)
		{
			if (parm[0] == '+') parm = parm.substr(1);
			int num = parseNum(parm);
			if (parm.length() > 0 && num == 0 && parm[0] != '0')
			{
				num = dflt;
				configLine->setInError(configLine->getAbsoluteTokenColumn(1));
			}
			spinBox->setValue(num);
		}
		else
		{
			spinBox->setValue(0);
		}
		mSpinMap[keyword] = spinBox->value();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		/*TODO: verification routine*/
		if (mSpinMap[keyword] != spinBox->value())
			const_cast<ConfigLine *>(configLine)->replaceParameters(spinBox->text().toStdString());
	}
}

void ConfigurationEditor::handleSpin(QDoubleSpinBox * doubleSpinBox, const ConfigLine * configLine, Direction dir, int dflt)
{
	std::string parm = configLine->getToken(1);
    hOutDebug(4, parm);
	if (dir == toScreen)
	{
		std::string parm = configLine->getToken(1);
		if (parm.length() > 0)
		{
			if (parm[0] == '+') parm = parm.substr(1);
			int num = parseNum(parm);
			if (parm.length() > 0 && num == 0 && parm[0] != '0')
			{
				num = dflt;
				configLine->setInError(configLine->getAbsoluteTokenColumn(1));
			}
			doubleSpinBox->setValue(num);
		}
		else
		{
			doubleSpinBox->setValue(0);
		}
		mSpinMap[parm] = doubleSpinBox->value();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		/*TODO: verification routine*/
		if (mSpinMap[parm] != doubleSpinBox->value())
			const_cast<ConfigLine *>(configLine)->replaceParameters(doubleSpinBox->text().toStdString());
	}
}

void ConfigurationEditor::handleHexSpin(QSpinBox * spinBox, const ConfigLine * configLine, Direction dir, int dflt)
{
	Configuration::HexSpinBox * hSpinBox = static_cast<Configuration::HexSpinBox *>(spinBox);
	std::string keyword = configLine->getUppercaseToken(0);
	std::string parm = configLine->getToken(1);

	if (dir == toScreen)
	{
		std::string parm = configLine->getToken(1);
		if (parm.length() > 0)
		{
			if (parm[0] == '+') parm = parm.substr(1);
			int num = parseNum(parm, 16);
			if (parm.length() > 0 && num == 0 && parm[0] != '0')
			{
				num = dflt;
				configLine->setInError(configLine->getAbsoluteTokenColumn(1));
			}
			hSpinBox->setValue(num);
		}
		else
		{
			hSpinBox->setValue(0);
		}
		mSpinMap[keyword] = hSpinBox->value();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		/*TODO: verification routine*/
		if (mSpinMap[keyword] != hSpinBox->value())
			const_cast<ConfigLine *>(configLine)->replaceParameters(hSpinBox->text().toStdString());
	}
}

void ConfigurationEditor::handleSlider(QSlider * slider, const ConfigLine * configLine, Direction dir)
{
	std::string keyword = configLine->getToken(0);
	std::string parm = configLine->getToken(1);
	if (dir == toScreen)
	{
		int num = parseNum(parm);
		if (parm[0] == '-' || parm[0] == '+') parm = parm.substr(1);
		if (num == 0 && parm.find_first_not_of("0123456789") != std::string::npos)
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		slider->setValue(num);
		mSpinMap[keyword] = slider->value();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		if(slider->isWindowModified()){};
			/*TODO: verification routine*/
		if (mSpinMap[keyword] != slider->value())
		{
			std::stringstream s;
			s << slider->value();
			std::string parm = s.str();
			const_cast<ConfigLine *>(configLine)->replaceParameters(s.str());
		}
	}
}

void ConfigurationEditor::handleCheckbox(QCheckBox * checkbox, const ConfigLine * configLine, Direction dir, QString checked, QString unchecked)
{
	QString name=configLine->getToken(0).c_str();
	QString parm=configLine->getToken(1).c_str();
    hOutDebug(2,parm.toStdString());
	if (dir == toScreen)
	{
		if (parm=="") parm=unchecked;
		checkbox->setChecked( (parm.toUpper() == checked )  ? true : false);
		mCheckBoxMap[name.toStdString()] = checkbox->isChecked();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		if ( (checkbox->isChecked() != mCheckBoxMap[name.toStdString()]) )
		{
			if (checkbox->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters(checked.toStdString());
			else
				const_cast<ConfigLine *>(configLine)->replaceParameters(unchecked.toStdString());
		}
	}
}


std::map<std::string,bool>ConfigurationEditor::mCheckBoxMap;
std::map<std::string,int>ConfigurationEditor::mSpinMap;

ConfigurationEditor::ConfigurationEditor()
{
    hOutDebug(5,"ConfigurationEditor::ConfigurationEditor");
}

ConfigurationEditor::~ConfigurationEditor()
{
}

void ConfigurationEditor::ARCHMODE(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
	if (dir == toScreen)
	{
        hOutDebug(4,"ARCHMODE");
		std::string token = configLine->getLowercaseToken(1);
		if (token == "s/370")
			configUi->s370Button->setChecked(true);
		else if (token == "esa/390")
			configUi->ESA390Button->setChecked(true);
		else if ((token == "esame") || (token == "z/arch") || (token == "z/arch"))
			configUi->ZArchButton->setChecked(true);
		else
		{
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		}
		int value;
		if (configUi->s370Button->isChecked()) value = 0;
		else if (configUi->ESA390Button->isChecked()) value = 1;
		else value = 2;
		mSpinMap["ARCHMODE"] = value;
		validateMaxTokens(configLine, 1);
	}
	else
	{
		int value;
		if (configUi->s370Button->isChecked()) value = 0;
		else if (configUi->ESA390Button->isChecked()) value = 1;
		else value = 2;
		if (value != mSpinMap["ARCHMODE"])
		{
			std::string parm;
			if (configUi->s370Button->isChecked())
				parm="S/370";
			else if(configUi->ESA390Button->isChecked())
				parm="ESA/390";
			else
				parm="z/Arch";
			const_cast<ConfigLine *>(configLine)->replaceParameters(parm);
		}
	}
}

CheckBoxItem(ASN_AND_LX_REUSE,alrfCheck, ENABLE, DISABLE)
//CheckBoxItem(ALRF,alrfCheck, ENABLE, DISABLE)

void ConfigurationEditor::ALRF(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
	handleCheckbox(configUi->alrfCheck, configLine, dir, "ENABLE", "DISABLE");
}

void ConfigurationEditor::AUTO_MOUNT(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
	// TODO: accept "-", but do not generate one
    hOutDebug(4,"AUTO_MOUNT");
	if (dir == toScreen)
	{
		std::string parm=configLine->getToken(1);
		if (parm=="+" || parm == "-")
			parm=configLine->getToken(1);
		configUi->autoMountDir->setText(parm.c_str());
	}
	else if (configUi->autoMountDir->isModified())
	{
		const std::string val = configUi->autoMountDir->text().toStdString();
		const_cast<ConfigLine *>(configLine)->replaceParameters(val);
	}
}

void ConfigurationEditor::AUTO_SCSI_MOUNT(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"AUTO_SCSI_MOUNT");
	if (dir == toScreen)
	{
		std::string parm = configLine->getLowercaseToken(1);
		int num = parseNum(parm);
		if (parm.length() == 0)
			num = 50;
		if (parm.length() != 0  && parm.compare("yes") == 0)
		{
			configUi->autoScsiMountCheck->setChecked(true);
			configUi->autoScisiSecSpin->setDisabled(false);
			configUi->autoScisiSecSpin->setValue(50);
		}
		else if (parm.length() != 0  && parm.compare("no") == 0)
		{

			configUi->autoScsiMountCheck->setChecked(true);
			configUi->autoScisiSecSpin->setDisabled(false);
			configUi->autoScisiSecSpin->setValue(num);
		}
		else
		{
			configUi->autoScsiMountCheck->setChecked(false);
			configUi->autoScisiSecSpin->setValue(num);
			configUi->autoScisiSecSpin->setDisabled(true);
		}
		mCheckBoxMap["AUTO_SCSI_MOUNT"] = configUi->autoScsiMountCheck->isChecked();
		mSpinMap["AUTO_SCSI_MOUNT"] = configUi->autoScisiSecSpin->value();
	}
	else if (mCheckBoxMap["AUTO_SCSI_MOUNT"] != configUi->autoScsiMountCheck->isChecked() ||
			 mSpinMap["AUTO_SCSI_MOUNT"] != configUi->autoScisiSecSpin->value() )
	{
		std::string parm;
		if (!configUi->autoScsiMountCheck->isChecked())
		{
			parm = "NO";
		}
		else
		{
			int val = configUi->autoScisiSecSpin->value();
			std::stringstream s;
			s << val;
			parm = s.str();
		}
        hOutDebug(3,"parm=" << parm);
		const_cast<ConfigLine *>(configLine)->replaceParameters(parm);
	}
}

SpinItem(CNSLPORT,ConsolePortSpin, 3270)

void ConfigurationEditor::CONKPALV(Ui::ConfigurationClass *, const ConfigLine * configLine, Direction dir)
{
	if (dir == toScreen)
	{
		// just parse (we do not yet have a screen display for this parameter
		QRegExp exp("\\s+\\((\\d*),(\\d*),(\\d*)\\)");

		QString line(configLine->getLine().substr(8).c_str());
		if (exp.indexIn(line) == -1)
			configLine->setInError(8);
		else
			configLine->setNoError();
	}
}

TextItem(CPUMODEL,CpuModel)
SliderItem(CPUPRIO,cpuPrioSlider)
TextItem(CPUSERIAL,cpuSerial)
TextItem(CPUVERID,cpuVerid)
SliderItem(DEVPRIO,devPrioSlider)
SpinItem(DEVTMAX,devTMaxSpin, 0)

void ConfigurationEditor::DIAG8CMD(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"DIAG8CMD");
	if (dir == toScreen)
	{
		std::string enableDisable = configLine->getLowercaseToken(1);
 		std::string echoNoEcho = configLine->getLowercaseToken(2);

		if (enableDisable.compare("enable") == 0)
			configUi->diag8CmdCheckBox->setChecked(true);
		else if (enableDisable.compare("disable") == 0)
			configUi->diag8CmdCheckBox->setChecked(false);
		else
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));

		if (echoNoEcho.length() > 0 && echoNoEcho.compare("echo") == 0)
			configUi->diag8EchoCheckBox->setChecked(true);
		else if (echoNoEcho.length() > 0 && echoNoEcho.compare("noecho") == 0)
			configUi->diag8EchoCheckBox->setChecked(false);
		else if (echoNoEcho.length() == 0)
			configUi->diag8EchoCheckBox->setChecked(false);
		else configLine->setInError(configLine->getAbsoluteTokenColumn(2));
		validateMaxTokens(configLine, 2);
		mCheckBoxMap["DIAG8CMD"]=configUi->diag8CmdCheckBox->isChecked();
		mCheckBoxMap["DIAG8ECHO"]=configUi->diag8EchoCheckBox->isChecked();
	}
	else if (mCheckBoxMap["DIAG8CMD"] != configUi->diag8CmdCheckBox->isChecked() ||
			 mCheckBoxMap["DIAG8ECHO"]!= configUi->diag8EchoCheckBox->isChecked())
	{
		std::string enable, echo;
		if (configUi->diag8EchoCheckBox->isChecked())
			echo = "ECHO";
		else
			echo = "NOECHO";
		if (configUi->diag8CmdCheckBox->isChecked())
			enable = "ENABLE";
		else
			enable = "DISABLE";

		std::string parm = enable + " " + echo;
		const_cast<ConfigLine *>(configLine)->replaceParameters(parm);
	}
}

TextItem(ENGINES,engines)

void ConfigurationEditor::ECPSVM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"ECPSVM");
	if (dir == toScreen)
	{
		if (configLine->getLowercaseToken(1) == "yes" || configLine->getLowercaseToken(1) == "level")
		{
			configUi->ecpSvmSpin->setEnabled(true);
			configUi->ecpSvm->setChecked(true);
			int level = 20;
			if (configLine->getToken(2) != "")
				level = parseNum(configLine->getToken(2));
			configUi->ecpSvmSpin->setValue(level);
		}
		else if (configLine->getLowercaseToken(1) == "no")
		{
			configUi->ecpSvm->setChecked(false);
			configUi->ecpSvmSpin->setValue(20);
			configUi->ecpSvmSpin->setEnabled(false);
		}
		else configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		mCheckBoxMap["ECPSVM"] = configUi->ecpSvm->isChecked();
		mSpinMap["ECPSVM"] = configUi->ecpSvmSpin->value();
		validateMaxTokens(configLine, 1);
	}
	else if (  mCheckBoxMap["ECPSVM"] != configUi->ecpSvm->isChecked() ||
			   mSpinMap["ECPSVM"] != configUi->ecpSvmSpin->value() )
	{
		std::string parm;
		if (configUi->ecpSvm->isChecked())
		{
			std::stringstream ss;
			ss << configUi->ecpSvmSpin->value();
			parm = "LEVEL " + ss.str();
		}
		else parm = "NO";
		const_cast<ConfigLine *>(configLine)->replaceParameters(parm);
	}
}

SliderItem(HERCPRIO,hercPrioSlider)

void ConfigurationEditor::HTTPPORT(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir) \
{
    hOutDebug(4,"HTTPPORT");
	if (dir == toScreen)
	{
		std::string parm = configLine->getToken(1);
		if ( parm.find_first_not_of("01234567890") != std::string::npos )
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		int num = parseNum(parm);
		if (num !=0)
		{
			configUi->httpPortSpin->setValue(num);
			configUi->httpPortSpin->setEnabled(true);
			configUi->httpPortCheckBox->setChecked(true);
		}
		else
		{
			configUi->httpPortCheckBox->setChecked(false);
			configUi->httpPortSpin->setValue(0);
			configUi->httpPortSpin->setEnabled(false);
		}
		mSpinMap["HTTPPORT"] = configUi->httpPortSpin->value();
		validateMaxTokens(configLine,1);
	}
	else
	{
		if (mSpinMap["HTTPPORT"] != configUi->httpPortSpin->value())
			const_cast<ConfigLine *>(configLine)->replaceParameters(configUi->httpPortSpin->text().toStdString());
	}
}

void ConfigurationEditor::HTTPROOT(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"HTTPROOT");
	if (dir == toScreen)
	{
		std::string parm=configLine->getToken(1);
		if (parm=="+" || parm == "-")
			parm=configLine->getToken(1);
		configUi->httpRootDir->setText(parm.c_str());
		validateMaxTokens(configLine,1);
	}
	else if (configUi->httpRootDir->isModified())
	{
		const std::string val = configUi->httpRootDir->text().toStdString();
		const_cast<ConfigLine *>(configLine)->replaceParameters(val);
	}
}

SpinItem(IODELAY,ioDelaySpin, 0)
TextItem(LDMOD,ldmod)

void ConfigurationEditor::LEGACYSENSEID(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"LEGACYSENSEID");
	if (dir == toScreen)
	{
		std::string parm=configLine->getUppercaseToken(1);
		if ( (parm == "ON") || (parm == "ENABLE") )
			configUi->legacySenseIdCheckbox->setChecked(true);
		else if ( (parm == "OFF") || (parm == "DISABLE") )
			configUi->legacySenseIdCheckbox->setChecked(false);
		else
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		validateMaxTokens(configLine,1);
		mCheckBoxMap["LEGACYSENSEID"] = configUi->legacySenseIdCheckbox->isChecked();
	}
	else
	{
		if (mCheckBoxMap["LEGACYSENSEID"] != configUi->legacySenseIdCheckbox->isChecked())
		{
			if (configUi->legacySenseIdCheckbox->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("ENABLE");
			else
				const_cast<ConfigLine *>(configLine)->replaceParameters("DISABLE");
		}
	}
}

TextItem(LOADPARM,loadParm)
CheckBoxItem(LOGOPT,logopt, TIMESTAMP, NOTIMESTAMP)
TextItem(LPARNAME,lparName)
void ConfigurationEditor::LPARNUM(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
  handleHexSpin(configUi->lparNum, configLine, dir, 0); \
}



SpinItem(MAINSIZE,mainSizeSpin, 64)
TextItem(MANUFACTURER,manufacturer)
SpinItem(MAXCPU, maxCPUSpin, 1)
void ConfigurationEditor::MODEL(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
	if (dir == toScreen)
	{
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
		configUi->model->setText(configLine->getToken(1).c_str());
		if (configLine->getToken(2).length() != 0)
		{
			configUi->capacityModel->setText(configLine->getToken(2).c_str());
			if (configLine->getToken(3).length() != 0)
			{
				configUi->permCapacityModel->setText(configLine->getToken(3).c_str());
				if (configLine->getToken(4).length() != 0)
				{
					configUi->tempCapacityModel->setText(configLine->getToken(4).c_str());
				}
			}
		}
	}
	else
	{
		if (configUi->model->isModified() || configUi->capacityModel->isModified() || configUi->permCapacityModel->isModified() || configUi->tempCapacityModel->isModified())
		{
			QString newParm = configUi->model->text() + " " + configUi->capacityModel->text() + " " + configUi->permCapacityModel->text() + " " + configUi->tempCapacityModel->text();
			const_cast<ConfigLine *>(configLine)->replaceParameters(newParm.toStdString());
		}
	}
}



CheckBoxItem(MOUNTED_TAPE_REINIT,mountedTapeReinitCheckbox,ALLOW,DISALLOW)

void ConfigurationEditor::MODPATH(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"MODPATH");
	if (dir == toScreen)
	{
		std::string parm=configLine->getToken(1);
		configUi->modPath->setText(parm.c_str());
		validateMaxTokens(configLine,1);
	}
	else if (configUi->modPath->isModified())
	{
		const std::string val = configUi->modPath->text().toStdString();
		const_cast<ConfigLine *>(configLine)->replaceParameters(val);
	}
}

SpinItem(NUMCPU,numCpuSpin, 1)
SpinItem(NUMVEC,numVecSpin, 0)

void ConfigurationEditor::OSTAILOR(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"OSTAILOR");
	if (dir == toScreen)
	{
		std::string parm = configLine->getUppercaseToken(1);
		int initialValue = 0;
		if (parm == "QUIET")
		{
			configUi->quiet->setChecked(true);
			initialValue = 1;
		}
		else if (parm == "VM")
		{
			configUi->vm->setChecked(true);
			initialValue = 2;
		}
		else if (parm == "VSE")
		{
			configUi->vse->setChecked(true);
			initialValue = 3;
		}
		else if (parm == "LINUX")
		{
			configUi->linuxb->setChecked(true);
			initialValue = 4;
		}
		else if (parm == "OS/390")
		{
			configUi->os390->setChecked(true);
			initialValue = 5;

		}
		else if (parm == "Z/OS")
		{
			configUi->zOS->setChecked(true);
			initialValue = 6;
		}
		else if (parm == "")
		{
			configUi->none->setChecked(true);
			initialValue = 0;
		}
		else
		{
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		}
		validateMaxTokens(configLine,1);
		mSpinMap["OSTAILOR"] = initialValue;
	}
	else
	{
		int value;
		if (configUi->quiet->isChecked()) value=1;
		else if (configUi->vm->isChecked()) value=2;
		else if (configUi->vse->isChecked()) value=3;
		else if (configUi->linuxb->isChecked()) value=4;
		else if (configUi->os390->isChecked()) value=5;
		else if (configUi->zOS->isChecked()) value=6;
		else value=0;
		if (value != mSpinMap["OSTAILOR"])
		{
			if (configUi->quiet->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("QUIET");
			else if (configUi->vm->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("VM");
			else if (configUi->vse->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("VSE");
			else if (configUi->linuxb->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("LINUX");
			else if (configUi->os390->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("OS/390");
			else if (configUi->zOS->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("z/OS");
			else if (configUi->none->isChecked())
				const_cast<ConfigLine *>(configLine)->replaceParameters("NONE");
		}
	}
}

void ConfigurationEditor::PANRATE(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
    hOutDebug(4,"PANRATE");
	if (dir == toScreen)
	{
		std::string panRateSpin = configLine->getLowercaseToken(1);
		if (panRateSpin.length() == 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		if (panRateSpin[0] == '+') panRateSpin = panRateSpin.substr(1);
		int num = parseNum(panRateSpin);
		if (panRateSpin.length() > 0 && num == 0)
		{
			if (panRateSpin.compare("fast") == 0) num=50;
			else if (panRateSpin.compare("slow") == 0) num=500;
			else
			{
				num = 500;
				configLine->setInError(configLine->getAbsoluteTokenColumn(1));
			}
		}
		configUi->panRateSpin->setValue(num);
		mSpinMap["PANRATE"] = configUi->panRateSpin->value();
		if (configLine->getToken(2).length() != 0)
			configLine->setInError(configLine->getAbsoluteTokenColumn(2));
	}
	else
	{
		/*TODO: verification routine*/
		if (mSpinMap["PANRATE"] != configUi->panRateSpin->value())
			const_cast<ConfigLine *>(configLine)->replaceParameters(configUi->panRateSpin->text().toStdString());
	}
}

CheckBoxItem(PGMPRDOS,pgmPrdOs,LICENSED,RESTRICTED)
TextItem(PLANT,plant)
CheckBoxItem(SHCMDOPT,shCmdOpt,DISABLE, NODIAG8)
SpinItem(SHRDPORT,shrdPortSpin, 3990)
SpinItem(SYSEPOCH,sysepochSpin, 1900)
SpinItem(TIMERINT,timerIntSpin, 50)
SpinItem(TODDRAG,todDragSpin, 0)
SliderItem(TODPRIO,todPrioSlider)

void ConfigurationEditor::TZOFFSET(Ui::ConfigurationClass * configUi, const ConfigLine * configLine, Direction dir)
{
	hOutDebug(5,"TZOFFSET:");
	if (dir == toScreen)
	{
		std::string parm = configLine->getToken(1);
		if (parm.length() < 4)
		{
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
			return;
		}
        hOutDebug(5,"parm=" << parm);
		int hh = 0, mm = 0;
		int sig = 1;
		if (parm[0] == '+') parm = parm.substr(1);
		if (parm[0] == '-')
		{
			parm = parm.substr(1);
			sig = -1;
		}
		if (parm.length() == 3)
		{
			hh = sig*(parm[0]-'0');
			mm = parseNum(parm.substr(1));
		}
		else if (parm.length() == 4)
		{
			hh = sig*parseNum(parm.substr(0,2));
			mm = parseNum(parm.substr(2,2));
		}
		else
		{
			configLine->setInError(configLine->getAbsoluteTokenColumn(1));
		}
		configUi->tzOffsetHH->setValue(hh);
		configUi->tzOffsetMM->setValue(mm);
        hOutDebug(5,"HH:" << hh << "MM:" << mm);
		mSpinMap["TZOFFSET"] = parseNum(parm);
		validateMaxTokens(configLine,1);
	}
	else
	{
		int iParm = configUi->tzOffsetHH->value()*100 + configUi->tzOffsetMM->value();
		if (mSpinMap["TZOFFSET"] != iParm)
		{
			QString parm = configUi->tzOffsetHH->text() + configUi->tzOffsetMM->text();
            hOutDebug(5, "parm:" << parm.toUtf8().data());
			const_cast<ConfigLine *>(configLine)->replaceParameters(parm.toStdString());
		}
	}
}



SpinItem(XPNDSIZE,xpndSizeSpin, 0)
SpinItem(YROFFSET,yrOffsetSpin, 0)

/////////////////////////////////////////////////////////////////////////////////////////////////

bool ConfigurationEditor::validateNum(const std::string& str, int base)
{
	if (base == 16)
	{
		QString qstr = str.c_str();
		if (qstr.toLower().toStdString().find_first_not_of("0123456789abcdef") != std::string::npos)
			return false;
		else
			return true;
	}
	return true;
}
int ConfigurationEditor::parseNum(const std::string& str, int base)
{
	int ret=0;
	int sign = 1;
	const char *c = str.c_str();
	static std::string digits = "0123456789abcdef";
	if (*c == '-')
	{
		sign = -1;
		c++;
	}
	for ( ; *c != '\0' ; c++)
	{
		char pc = *c;
		if (base==10 && !isdigit(pc)) break;
		if ( (pc <= 'Z') && (pc >= 'A') ) pc+=('a'-'A');
		ret *= base;
		//ret += (*c-'0');
		size_t i = digits.find_first_of(pc) ;
		ret += i;
	}
	return ret*sign;
}

std::string ConfigurationEditor::normalizeNum(const std::string& str, int width, int base)
{
	int devno = parseNum(str,base);
	std::stringstream newLine;
	newLine << std::hex << std::setw(width) << std::setfill('0') << std::uppercase
		<< devno ;
	return newLine.str();
}

void ConfigurationEditor::validateMaxTokens(const ConfigLine * configLine, int maxNum)
{
	if (configLine->size() > maxNum+1) // size is 1-based maxNum is 0-based
		configLine->setInError(configLine->getAbsoluteTokenColumn(maxNum+1));
}
