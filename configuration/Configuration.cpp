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

#include "Configuration.h"
#include "SystemConfigLine.h"
#include "DeviceConfigLine.h"
#include "ConfigurationFreeEditor.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>

#include <iostream>
#include <sstream>

#define genEntry(keyword)  {#keyword, ConfigurationEditor::keyword},
#define synonymEntry(keyword1, keyword2)  {#keyword1, #keyword2}, {#keyword2, #keyword1},

struct ConfigTableEntry Configuration::mConfigTable[] = {
		genEntry(ARCHMODE)
		genEntry(ASN_AND_LX_REUSE)
		genEntry(ALRF)
		genEntry(AUTO_MOUNT)
		genEntry(AUTO_SCSI_MOUNT)
		//CODEPAGE
		genEntry(CNSLPORT)
		genEntry(CONKPALV)
		genEntry(CPUMODEL)
		genEntry(CPUPRIO)
		genEntry(CPUSERIAL)
		genEntry(CPUVERID)
		//DEFSYM
		genEntry(DEVPRIO)
		genEntry(DEVTMAX)
		genEntry(DIAG8CMD)
		genEntry(ECPSVM)
		genEntry(ENGINES)
		genEntry(HERCPRIO)
		genEntry(HTTPPORT)
		genEntry(HTTPROOT)
		//genEntry(IGNORE)
		//INCLUDE
		genEntry(IODELAY)
		genEntry(LDMOD)
		genEntry(LEGACYSENSEID)
		genEntry(LOADPARM)
		genEntry(LOGOPT)
		genEntry(LPARNAME)
		genEntry(LPARNUM)
		genEntry(MAINSIZE)
		genEntry(MANUFACTURER)
		genEntry(MAXCPU)
		genEntry(MODEL)
		genEntry(MODPATH)
		genEntry(MOUNTED_TAPE_REINIT)
		genEntry(NUMCPU)
		genEntry(NUMVEC)
		genEntry(OSTAILOR)
		genEntry(PANRATE)
		//PANTITLE --
		genEntry(PGMPRDOS)
		genEntry(PLANT)
		genEntry(SHCMDOPT)
		genEntry(SHRDPORT)
		genEntry(SYSEPOCH)
		genEntry(TIMERINT)
		genEntry(TODDRAG)
		genEntry(TODPRIO)
		//TRACEOPT  --
		genEntry(TZOFFSET)
		genEntry(XPNDSIZE)
		genEntry(YROFFSET)

		{"",NULL}
};

Configuration::Configuration(ConfigFile* configFile, bool newConfig, QWidget *parent)
	: QDialog(parent), mParent(parent), mConfigFile(configFile), mNewConfig(newConfig)
{
	ui.setupUi(this);

	int lpnRow = ui.lparNum->pos().y();
	int lpnCol = ui.lparNum->pos().x();
	delete ui.lparNum;
	ui.lparNum = new HexSpinBox(ui.groupBox_4);
	ui.lparNum->move(lpnCol, lpnRow);
	ui.lparNum->show();

	mDevWgt = new DevicesWidget(configFile, ui.Configuration->widget(5));
	mDevWgt->move(0,50);
	mDevWgt->show();

	mFreeEdit = new ConfigurationFreeEditor(ui.Configuration->widget(6));
	mSyntaxHighlighter = new ConfigHighlight(mFreeEdit);
	mSyntaxHighlighter->setDocument(mFreeEdit->document());
	mFreeEdit->move(10,50);
	mFreeEdit->resize(411, 671);

	initilize();
	populate(ConfigurationEditor::toScreen);
}

Configuration::~Configuration()
{
}

void Configuration::initilize()
{

	DoubleDigitSpinBox * hh = new DoubleDigitSpinBox(ui.groupBox_4, true);
	hh->setObjectName(QString::fromUtf8("tzOffsetHH"));
	hh->setGeometry(QRect(ui.tzOffsetHH->geometry()));
	hh->setMinimum(ui.tzOffsetHH->minimum());
	hh->setMaximum(ui.tzOffsetHH->maximum());
	if (ui.tzOffsetHH != NULL) delete ui.tzOffsetHH;
	ui.tzOffsetHH = hh;

	DoubleDigitSpinBox * mm = new DoubleDigitSpinBox(ui.groupBox_4, false);
	mm->setObjectName(QString::fromUtf8("tzOffsetMM"));
	mm->setGeometry(QRect(ui.tzOffsetMM->geometry()));
	mm->setMaximum(ui.tzOffsetMM->maximum());
	if (ui.tzOffsetMM != NULL) delete ui.tzOffsetMM;
	ui.tzOffsetMM = mm;

	mCurrTab = ui.Configuration->currentIndex();
	ui.logopt->setVisible(false); // irrelevant for gui, but needed for syntax checking

	if (mConfigFile->locateLine("MAXCPU") == NULL)
	{
		const ConfigLine * numCpuLine = mConfigFile->locateLine("NUMCPU");
		if (numCpuLine != NULL)
		{
			QString s = " MAXCPU ";
			s += "0";
			SystemConfigLine maxCpuLine(s.toUtf8().data());
			maxCpuLine.replaceParameters(numCpuLine->getToken(1).c_str());
			mConfigFile->addNonDev(&maxCpuLine);
			ui.maxCPUSpin->setValue(ui.numCpuSpin->value());
		}
	}

	connect(ui.pushButton_OK, SIGNAL(clicked()), this , SLOT(okPressed()));
	connect(ui.pushButton_Cancel, SIGNAL(clicked()), this , SLOT(cancelPressed()));
	connect(ui.autoMountBrowseButton, SIGNAL(clicked()), this, SLOT(autoMountBrowsePressed()));
	connect(ui.autoScsiMountCheck, SIGNAL(stateChanged(int)), this, SLOT(autoScsiMountChanged()));
	connect(ui.cpuPrioSlider, SIGNAL(valueChanged(int)), this, SLOT(cpuPrioChanged(int)) );
	connect(ui.devPrioSlider, SIGNAL(valueChanged(int)), this, SLOT(devPrioChanged(int)) );
	connect(ui.hercPrioSlider, SIGNAL(valueChanged(int)), this, SLOT(hercPrioChanged(int)) );
	connect(ui.todPrioSlider, SIGNAL(valueChanged(int)), this, SLOT(todPrioChanged(int)) );
	connect(ui.ecpSvm, SIGNAL(stateChanged(int)), this, SLOT(ecpSvmChanged(int)) );
	connect(ui.httpPortCheckBox, SIGNAL(stateChanged(int)), this, SLOT(httpPortChanged(int)));
	connect(ui.httpRootBrowseButton, SIGNAL(clicked()), this, SLOT(httpRootBrowsePressed()));
	connect(ui.modPathButton, SIGNAL(clicked()), this, SLOT(modPathBrowsePressed()));
	connect(ui.Configuration, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui.maxCPUSpin, SIGNAL(valueChanged(int)), this, SLOT(validateMaxCpu()));
	connect(ui.numCpuSpin, SIGNAL(valueChanged(int)), this, SLOT(validateMaxCpu()));
	connect(ui.model, SIGNAL(textChanged(QString)), this, SLOT(validateModels()));
	connect(ui.capacityModel, SIGNAL(textChanged(QString)), this, SLOT(validateModels()));
	connect(ui.tempCapacityModel, SIGNAL(textChanged(QString)), this, SLOT(validateModels()));
	connect(ui.permCapacityModel, SIGNAL(textChanged(QString)), this, SLOT(validateModels()));
	connect(mFreeEdit, SIGNAL(blockCountChanged(int)), this, SLOT(blockCountChangedSlot()));
}

void Configuration::resizeEvent(QResizeEvent *event)
{
	QDialog::resizeEvent(event);

	// move buttons
	int newY = qMax (530, size().height() - 47);
	int newX = qMax ( 430, size().width() - 102);
	ui.pushButton_OK->move(newX,newY);
	ui.pushButton_Cancel->move(newX-100,newY);

	// resize QTabWidget
	int newHeight = qMax ( size().height() - 86 , 491 );
	int newWidth = qMax ( size().width() - 41, 691 );
	ui.Configuration->resize( newWidth, newHeight );

	// resize plainTextEdit
	newHeight = qMax ( size().height() - 164, 411);
	newWidth = qMax ( size().width() - 61, 671);
	mFreeEdit->resize( newWidth, newHeight);

}

void Configuration::populate(ConfigurationEditor::Direction dir)
{
	if (mConfigFile == NULL || mNewConfig)
	{
		ui.fileNameLabel->setText("(new file)");
		ui.fileNameLabel_2->setText("(new file)");
		ui.fileNameLabel_3->setText("(new file)");
		ui.fileNameLabel_4->setText("(new file)");
		ui.fileNameLabel_5->setText("(new file)");
		ui.fileNameLabel_6->setText("(new file)");
		ui.fileNameLabel_7->setText("(new file)");
	}
	else
	{
		ui.fileNameLabel->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_2->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_3->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_4->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_5->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_6->setText(mConfigFile->getFileName().c_str());
		ui.fileNameLabel_7->setText(mConfigFile->getFileName().c_str());
	}
	for (int i=0; mConfigTable[i].keyword.compare("") != 0 ; i++)
	{
        hOutDebug(3,"populate:" << mConfigTable[i].keyword)
		const ConfigLine * configLine = mConfigFile->locateLine(mConfigTable[i].keyword, true, true);
		mConfigTable[i].populator(&ui, configLine, dir);
	}
}

void Configuration::handleHighlight(const ConfigLine& configLine, QTextCursor& cursor, QTextCharFormat& format)
{
	if (configLine.inError())
		format.setForeground(Qt::red);
	else
		format.setForeground(Qt::black);
	cursor.setCharFormat(format);
	mFreeEdit->setTextCursor(cursor);
}


void Configuration::okPressed()
{
    if (ui.Configuration->currentIndex() == 6)
        ui.Configuration->setCurrentIndex(0);
    populate(ConfigurationEditor::fromScreen);
    if (mNewConfig)
    {
        mParent->connect(this,SIGNAL(OKSignal()), mParent, SLOT(saveConfigAs()));
        emit OKSignal();
        disconnect(this,0,0,0);
    }

    deleteLater();
}

void Configuration::cancelPressed()
{
	disconnect(this,0,0,0);
	deleteLater();
}

void Configuration::autoScsiMountChanged()
{
	ui.autoScisiSecSpin->setEnabled(ui.autoScsiMountCheck->isChecked());
}

void Configuration::cpuPrioChanged(int newValue)
{
	ui.cpuPrioNumber->display(newValue);
}

void Configuration::devPrioChanged(int newValue)
{
	ui.devPrioNumber->display(newValue);
}

void Configuration::ecpSvmChanged(int newValue)
{
	if (newValue == Qt::Checked)
	{
		ui.ecpSvmSpin->setEnabled(true);
	}
	else
	{
		ui.ecpSvmSpin->setEnabled(false);
	}
}

void Configuration::httpPortChanged(int newValue)
{
	if (newValue == Qt::Checked)
	{
		ui.httpPortSpin->setEnabled(true);
	}
	else
	{
		ui.httpPortSpin->setValue(0);
		ui.httpPortSpin->setEnabled(false);
	}
}

void Configuration::hercPrioChanged(int newValue)
{
	ui.hercPrioNumber->display(newValue);
}

void Configuration::todPrioChanged(int newValue)
{
	ui.clockPrioNumber->display(newValue);
}

void Configuration::autoMountBrowsePressed()
{
	std::string s = QFileDialog::getExistingDirectory(this,"Browse Auto Mount Directory","").toStdString();
	ui.autoMountDir->setText(s.c_str());

}

void Configuration::httpRootBrowsePressed()
{
	std::string s = QFileDialog::getExistingDirectory(this,"Browse HTTP Root Directory",ui.httpRootDir->text()).toStdString();
	ui.httpRootDir->setText(s.c_str());
}

void Configuration::modPathBrowsePressed()
{
	std::string s = QFileDialog::getExistingDirectory(this,"Browse Modules Path Directory",ui.modPathButton->text()).toStdString();
	ui.modPath->setText(s.c_str());

}

void Configuration::validateMaxCpu()
{
	if (ui.maxCPUSpin->value() < ui.numCpuSpin->value())
	{
		QMessageBox::warning(this, "CPU number mismatch", "Number of CPUs (" +
							 QString::number(ui.numCpuSpin->value()) +
							 QString(") must not be greater than maximum number of CPUs (") +
							 QString::number(ui.maxCPUSpin->value()) +
							 ")",
							 QMessageBox::Ok);
        hOutDebug(3,"num " << ui.numCpuSpin->value() << " max " << ui.maxCPUSpin->value());
		ui.numCpuSpin->setValue(ui.maxCPUSpin->value());
	}
}

void Configuration::validateModels()
{
	if (ui.model->text().isEmpty())
	{
		ui.capacityModel->clear();
		ui.capacityModel->setEnabled(false);
		ui.tempCapacityModel->clear();
		ui.tempCapacityModel->setEnabled(false);
		ui.permCapacityModel->clear();
		ui.permCapacityModel->setEnabled(false);
	}
	else
	{
		ui.capacityModel->setEnabled(true);
		if (ui.capacityModel->text().isEmpty())
		{
			ui.permCapacityModel->clear();
			ui.permCapacityModel->setEnabled(false);
			ui.tempCapacityModel->clear();
			ui.tempCapacityModel->setEnabled(false);
		}
		else
		{
			ui.permCapacityModel->setEnabled(true);
			if (ui.permCapacityModel->text().isEmpty())
			{
				ui.tempCapacityModel->clear();
				ui.tempCapacityModel->setEnabled(false);
			}
			else
			{
				ui.tempCapacityModel->setEnabled(true);
			}
		}
	}
}


void Configuration::tabChanged(int newTab)
{
	if (newTab == 6)
	{
		//populate configtab with data from first 5 tabs
		mConfigFile->appendNewLines();
		populate(ConfigurationEditor::fromScreen);

		//build free editor data
		mFreeEdit->document()->clear();
		QTextCursor cursor = mFreeEdit->textCursor();
		QTextCharFormat format = cursor.charFormat();
		format.setFont(QFont(Preferences::getInstance().fontName(Preferences::ConfigurationFontObject).c_str(),
							Preferences::getInstance().fontSize(Preferences::ConfigurationFontObject)));
		cursor.setCharFormat(format);
		mFreeEdit->setTextCursor(cursor);

		for (int i=0; i<=mConfigFile->getLastSys(); i++)
		{
			SystemConfigLine * configLine = (*mConfigFile)[i];
                        if (!configLine->isNew())
				mFreeEdit->textCursor().insertText(configLine->getLine().c_str());
			configLine->setUpdated(false);
			hOutDebug(5,"sys:" << i << configLine->getLine().c_str());
		}
		for (int i=mConfigFile->getLastSys()+1; i<mConfigFile->size(); i++)
		{
			const DeviceConfigLine * configLine = mConfigFile->getDevice(i);
			mFreeEdit->textCursor().insertText(configLine->getLine().c_str());
			hOutDebug(5,"dev:" << i << configLine->getLine().c_str());
		}

		cursor = mFreeEdit->textCursor();
		cursor.setPosition(0);
		mFreeEdit->setTextCursor(cursor);
	}
	else if (mCurrTab == 6)
	{
		//reconstruct configuration table from editor data
		mConfigFile->updateEditor(*mFreeEdit);
		mDevWgt->update(mConfigFile);

		//populate first 5 tabs with data from table
		populate(ConfigurationEditor::toScreen);
	}
	mCurrTab = newTab;
}

Configuration::DoubleDigitSpinBox::DoubleDigitSpinBox(QWidget * parent, bool appendPlus):
	QSpinBox(parent)
{
	mAppendPlus = appendPlus;
}

int Configuration::DoubleDigitSpinBox::valueFromText(const QString& text) const
{
	return QSpinBox::valueFromText(text);
}

QString Configuration::DoubleDigitSpinBox::textFromValue(int value) const
{
	std::stringstream ss;
	int sig=1;
	int pValue=value;
	if (value < 0)
	{
		sig = -1;
		pValue = -value;
	}
	ss << pValue;

	QString ret = ss.str().c_str();
	if (pValue < 10) ret = "0" + ret;
	if (sig == -1) ret = "-" + ret;
	if (mAppendPlus && sig != -1) ret = "+" + ret;
    hOutDebug(5, "textFromValue:" << value << "='" << ret.toStdString())
	return ret;
}

Configuration::HexSpinBox::HexSpinBox(QWidget * parent):
	QSpinBox(parent)
{
	this->setMaximum(0xffff);
}

int Configuration::HexSpinBox::valueFromText(const QString& text) const
{
	hOutDebug(5,"ValueFromText:" << text.toStdString() << ConfigurationEditor::parseNum(text.toStdString(),16) );
	return ConfigurationEditor::parseNum(text.toStdString(),16);
}

QString Configuration::HexSpinBox::textFromValue(int value) const
{
	std::stringstream ss;
	ss << std::hex << value;
	char formatted[5];
	if (value <= 0xffff)
		sprintf(formatted,"%4.4X",(value));
	else
		strcpy(formatted,"0000");
	QString ret(formatted);
    hOutDebug(5,  "textFromValue:" << value << "='" << ret.toStdString())
	return ret;
}

void Configuration::blockCountChangedSlot()
{
	int mLastSys=-1;
	for (QTextBlock block = mFreeEdit->document()->begin();
		 block != mFreeEdit->document()->end();
		 block = block.next())
	{
		mLastSys++;
        std::string str = block.text().toUtf8().data();
		int skipSpace = str.find_first_not_of(" ");
		if (skipSpace < 0 || str[skipSpace] == '#') continue;
		if (str[skipSpace] >= '0' && str[skipSpace] <= '9')
			break;
	}
	hOutDebug(5,"lastSys=" << mLastSys);
}



QValidator::State Configuration::HexSpinBox::validate (QString & input, int &) const
{
	if (input.length() > 4 ||
		input.toStdString().find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
		return QValidator::Invalid;
	else
		return QValidator::Acceptable;
}


ConfigHighlight::ConfigHighlight(QWidget * parent)
	: QSyntaxHighlighter(parent)
{
	mDummyUi.setupUi(&mDummyConfig);
	mDummyConfig.setVisible(false);
	mRed = QColor(255,0,0,255);
	mGreen = QColor(0,96,0,255);
	mBlue = QColor(0,0,255,255);
	mBlack = QColor(64,64,64,255);
	mGray = QColor(128,32,32,255);
}

void ConfigHighlight::highlightBlock(const QString &text)
{
	hOutDebug(3,"Highlight:" << text.toStdString());

	if (text.length() == 0) return;

	if (previousBlockState() == 2)
	{
		setCurrentBlockState(2);
	}
	else
	{
        std::string str = text.toUtf8().data();
		int skipSpace = str.find_first_not_of(" ");
		if (skipSpace >= 0 && str[skipSpace] != '#')
		{
			if (str[skipSpace] >= '0' && str[skipSpace] <= '9')
				setCurrentBlockState(2);
			else
				setCurrentBlockState(1);
		}
	}
	if (currentBlockState() == 2)
	{
		setFormat(0,text.length(),mBlue);
		return;
	}


	int remarkCol = text.indexOf("#");

    ConfigLine configLine(text.toUtf8().data());
	int i=0;
	std::string tokenToSearch = configLine.getUppercaseToken(0);
	while (Configuration::mConfigTable[i].keyword.length() != 0 && Configuration::mConfigTable[i].keyword.compare(tokenToSearch) != 0)
	{
		i++;
	}
	if (Configuration::mConfigTable[i].keyword.length() != 0  && Configuration::mConfigTable[i].keyword.compare(tokenToSearch) == 0)
	{
		Configuration::mConfigTable[i].populator(&mDummyUi, &configLine, ConfigurationEditor::toScreen);
		int startCol = configLine.getAbsoluteTokenColumn(0);
		int endCol = configLine.getAbsoluteTokenColumn(1);
		setFormat(startCol,endCol-startCol+1,mBlue);
		if (configLine.size() > 0)
		{
			int lastToken = configLine.size();
			startCol = configLine.getAbsoluteTokenColumn(1);
			endCol = configLine.getAbsoluteTokenColumn(lastToken-1) + configLine.getAbsoluteTokenEnd(lastToken-1);
			setFormat(startCol,endCol-startCol+1,mGray);
		}
		if (configLine.inError())
		{
			setFormat(configLine.errorColumn(),configLine.getLine().size()-configLine.errorColumn(),mRed);
		}
		if (remarkCol != -1)
		{
			setFormat(remarkCol,configLine.getLine().size()-remarkCol,mGreen);
		}
	}
	else
	{
		if (configLine.isRemark())
		{
			setFormat(0,configLine.getLine().size(),mGreen);
		}
		else
		{
			setFormat(0,configLine.getLine().size(),mRed);
			if (remarkCol != -1)
			{
				setFormat(remarkCol,configLine.getLine().size()-remarkCol,mGreen);
			}
		}
	}
}
