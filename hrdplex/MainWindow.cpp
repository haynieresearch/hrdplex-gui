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

#include "MainWindow.h"
#include "MainPanelClassic.h"
#include "MainPanelModern.h"
#include "PreferencesWin.h"
#include "Watchdog.h"
#include "Recovery.h"
#include "HelpAbout.h"
#include "DasdInit.h"
#include "DasdLoad.h"
#include "DasdIsup.h"
#include "Dasdls.h"
#include "Dasdcat.h"
#include "Dasdconv.h"
#include "Dasdcopy.h"
#include "TapeMap.h"
#include "TapeSplt.h"
#include "TapeCopy.h"
#include "HetInit.h"
#include "HetGet.h"
#include "HetUpd.h"
#include "HetMap.h"
#include "PrinterDialog.h"
#include "StationeryDialog.h"
#include "StationeryDialog.h"
#include "DecolationDialog.h"
#include "NamedPipe.h"
#include "Environment.h"
#include "Arguments.h"
#include "IplConfig.h"
#include "SystemConfigLine.h"
#include "LogWidget.h"

#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>
#include <QFile>
#include <QTimer>

#include <iostream>
#include "cerrno"
#ifndef hFramework
#include <err.h>
#endif
#include <typeinfo>
#ifdef UNISTD_INC
#include <unistd.h>
#endif

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	mTopDock(NULL),
	mDevicesDock(NULL),
	mConfigFile(NULL),
	mCommandLine(NULL),
	mHerculesExecutor(NULL),
    mPrinterDialog(NULL),
    mHerculesActive(false)
{
	ui.setupUi(this);
	setDarkBackground(Preferences::getInstance().darkBackground());
	QString iconPath(Environment::getIconsPath().c_str());
	this->setWindowIcon(QIcon(iconPath+"/tray.ico"));

	if (Arguments::getInstance().configFileName().length() > 0)
	{
		mConfigFile = new ConfigFile(Arguments::getInstance().configFileName());
	}

	QDir d("");
	mCurrentPath = d.absolutePath();

	// left dock (Devices)
	mDevicesDock = new QDockWidget("Devices",this);
	mDevicesDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mDevicesPane = new DevicesPane(this);
	mDevicesDock->setWidget(mDevicesPane);
	addDockWidget(Qt::LeftDockWidgetArea, mDevicesDock );

	// top dock (MainPanel)
	mTopDock = new QDockWidget("Controls",this);
	mTopDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	if (Preferences::getInstance().theme() == Preferences::Modern)
		mMainPanel = new MainPanelModern(this);
	else
		mMainPanel = new MainPanelClassic(this);
	mTopDock->setWidget(mMainPanel);
	addDockWidget(Qt::TopDockWidgetArea,mTopDock );

	// gregs32 dock
	mGRegisters32Dock = new QDockWidget("32-bit General Regs",this);
	mGRegisters32Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mGRegisters32 = new Regs32(this,Regs32::gCommand);
	mGRegisters32Dock->setWidget(mGRegisters32);
	addDockWidget(Qt::TopDockWidgetArea,mGRegisters32Dock );
	this->tabifyDockWidget(mTopDock,mGRegisters32Dock);
	mGRegisters32Dock->setVisible(false);

	// cregs32 dock
	mCRegisters32Dock = new QDockWidget("32-bit Control Regs",this);
	mCRegisters32Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mCRegisters32 = new Regs32(this,Regs32::cCommand);
	mCRegisters32Dock->setWidget(mCRegisters32);
	addDockWidget(Qt::TopDockWidgetArea,mCRegisters32Dock );
	this->tabifyDockWidget(mTopDock,mCRegisters32Dock);
	mCRegisters32Dock->setVisible(false);

	// fregs32 dock
	mFRegisters32Dock = new QDockWidget("32-bit Floating-Point Regs",this);
	mFRegisters32Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mFRegisters32 = new Regs32(this,Regs32::fCommand);
	mFRegisters32Dock->setWidget(mFRegisters32);
	addDockWidget(Qt::TopDockWidgetArea,mFRegisters32Dock );
	this->tabifyDockWidget(mTopDock,mFRegisters32Dock);
	mFRegisters32Dock->setVisible(false);

	// aregs32 dock
	mARegisters32Dock = new QDockWidget("Access Registers",this);
	mARegisters32Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mARegisters32 = new Regs32(this,Regs32::aCommand);
	mARegisters32Dock->setWidget(mARegisters32);
	addDockWidget(Qt::TopDockWidgetArea,mARegisters32Dock );
	this->tabifyDockWidget(mTopDock,mARegisters32Dock);
	mARegisters32Dock->setVisible(false);

	// gregs64 dock
	mGRegisters64Dock = new QDockWidget("64-bit General Regs",this);
	mGRegisters64Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mGRegisters64 = new Regs64(this,Regs64::gCommand);
	mGRegisters64Dock->setWidget(mGRegisters64);
	addDockWidget(Qt::TopDockWidgetArea,mGRegisters64Dock );
	this->tabifyDockWidget(mTopDock,mGRegisters64Dock);
	mGRegisters64Dock->setVisible(false);

	// cregs64 dock
	mCRegisters64Dock = new QDockWidget("64-bit Control Regs",this);
	mCRegisters64Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mCRegisters64 = new Regs64(this, Regs64::cCommand);
	mCRegisters64Dock->setWidget(mCRegisters64);
	addDockWidget(Qt::TopDockWidgetArea,mCRegisters64Dock );
	this->tabifyDockWidget(mTopDock,mCRegisters64Dock);
	mCRegisters64Dock->setVisible(false);

	// fregs64 dock
	mFRegisters64Dock = new QDockWidget("64-bit Floating Point Regs",this);
	mFRegisters64Dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mFRegisters64 = new Regs64(this, Regs64::fCommand);
	mFRegisters64Dock->setWidget(mFRegisters64);
	addDockWidget(Qt::TopDockWidgetArea,mFRegisters64Dock );
	this->tabifyDockWidget(mTopDock,mFRegisters64Dock);
	mFRegisters64Dock->setVisible(false);

	tabifyDockWidget(mGRegisters32Dock, mTopDock);

	// CommandLine
	mBottomDock = new QDockWidget("Command",this);
	mBottomDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mCommandLine = new CommandLine(mBottomDock);
	mCommandLine->setReadOnly(true);
	mBottomDock->setWidget(mCommandLine);
	addDockWidget(Qt::BottomDockWidgetArea,mBottomDock );

	// psw
	mPswDock = new QDockWidget("PSW", this);
	mPswDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mPsw = new Psw(this->statusBar());
	mPswDock->setWidget(mPsw);
	addDockWidget(Qt::BottomDockWidgetArea,mPswDock );
	this->tabifyDockWidget(mPswDock,mBottomDock);
	if (Preferences::getInstance().pswMode() == Psw::Docked)
		mPswDock->setVisible(true);
	else
		mPswDock->setVisible(false);
	mPsw->setMode(Preferences::getInstance().pswMode());
	mPsw->setDormant();

	if( (Preferences::getInstance().regs(Preferences::ViewGR32)) ) editView32BitGr();
	if( (Preferences::getInstance().regs(Preferences::ViewCR32)) ) editView32BitCr();
	if( (Preferences::getInstance().regs(Preferences::ViewFR32)) ) editView32BitFr();
	if( (Preferences::getInstance().regs(Preferences::ViewAR32)) ) editView32BitAr();
	if( (Preferences::getInstance().regs(Preferences::ViewGR64)) ) editView64BitGr();
	if( (Preferences::getInstance().regs(Preferences::ViewCR64)) ) editView64BitCr();
	if( (Preferences::getInstance().regs(Preferences::ViewFR64)) ) editView64BitFr();
	ui.actionView_PSW->setChecked(Preferences::getInstance().regs(Preferences::ViewPsw));
	editViewPSW();

	QString path = Environment::getIconsPath().c_str();
	QIcon trayIcon(path + "/tray.ico");
	mSystemTrayIcon = new QSystemTrayIcon(trayIcon);

    QMenu *trayIconMenu = new QMenu(this);
    QAction *restoreAction = new QAction(tr("&Hide"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(hideRestore()));
    trayIconMenu->addAction(restoreAction);
    mSystemTrayIcon->setContextMenu(trayIconMenu);

	connect(ui.actionNew, SIGNAL(triggered()), this , SLOT(newConfig()));
	connect(ui.actionOpen_configuration, SIGNAL(triggered()), this , SLOT(openConfig()));
	connect(ui.actionEditConfig, SIGNAL(triggered()), this , SLOT(config()));
	connect(ui.actionSave, SIGNAL(triggered()), this , SLOT(saveConfig()));
	connect(ui.actionSave_As, SIGNAL(triggered()), this , SLOT(saveConfigAs()));
	connect(ui.actionExit, SIGNAL(triggered()), this , SLOT(exitNow()));
	connect(ui.actionViewControls, SIGNAL(triggered()), this, SLOT(editViewControls()));
	connect(ui.actionViewDevices, SIGNAL(triggered()), this, SLOT(editViewDevices()));
	connect(ui.actionViewCommand, SIGNAL(triggered()), this, SLOT(editViewCommand()));
	connect(ui.actionView_32_bit_General_Registers, SIGNAL(triggered()), this, SLOT(editView32BitGr()));
	connect(ui.actionView_32_bit_Control_Registers, SIGNAL(triggered()), this, SLOT(editView32BitCr()));
	connect(ui.actionView_32_bit_Floating_Point_Registers, SIGNAL(triggered()), this, SLOT(editView32BitFr()));
	connect(ui.actionView_32_bit_Access_Registers, SIGNAL(triggered()), this, SLOT(editView32BitAr()));
	connect(ui.actionView_64_bit_General_Registers, SIGNAL(triggered()), this, SLOT(editView64BitGr()));
	connect(ui.actionView_64_bit_Control_Registers, SIGNAL(triggered()), this, SLOT(editView64BitCr()));
	connect(ui.actionView_64_bit_Floating_Point_Registers, SIGNAL(triggered()), this, SLOT(editView64BitFr()));
    connect(ui.actionView_PSW, SIGNAL(triggered()), this, SLOT(editViewPSW()));
    connect(ui.actionCopy, SIGNAL(triggered()), this, SLOT(editCopy()));
	connect(ui.actionDelete_messages, SIGNAL(triggered()), this, SLOT(deleteMessages()));
	connect(ui.actionSave_messages, SIGNAL(triggered()), this, SLOT(saveMessages()));
	connect(ui.actionPreferences, SIGNAL(triggered()), this, SLOT(preferences()));
	connect(ui.actionPower_on, SIGNAL(triggered()), this, SLOT(powerOn()));
	connect(ui.actionPower_off, SIGNAL(triggered()), this, SLOT(powerOff()));
	connect(ui.actionIPL_Load, SIGNAL(triggered()), this, SLOT(loadCommand()));
	connect(ui.actionStart, SIGNAL(triggered()), this, SLOT(start()));
	connect(ui.actionStop, SIGNAL(triggered()), this, SLOT(stop()));
	connect(ui.actionExtInterrupt, SIGNAL(triggered()), this, SLOT(extInterrupt()));
	connect(ui.actionStore_status, SIGNAL(triggered()), this, SLOT(store()));
	connect(ui.actionPSW_Restart, SIGNAL(triggered()), this, SLOT(restart()));
	connect(ui.actionDevice_Interrupt, SIGNAL(triggered()), this, SLOT(devInterrupt()));
	connect(ui.actionDasdinit, SIGNAL(triggered()), this, SLOT(dasdinit()));
	connect(ui.actionDasdload, SIGNAL(triggered()), this, SLOT(dasdload()));
	connect(ui.actionDasdisup, SIGNAL(triggered()), this, SLOT(dasdisup()));
	connect(ui.actionDasdls, SIGNAL(triggered()), this, SLOT(dasdls()));
	connect(ui.actionDasdcat, SIGNAL(triggered()), this, SLOT(dasdcat()));
	connect(ui.actionDasdconv, SIGNAL(triggered()), this, SLOT(dasdconv()));
	connect(ui.actionDasdcopy, SIGNAL(triggered()), this, SLOT(dasdcopy()));
	connect(ui.actionTapemap, SIGNAL(triggered()), this, SLOT(tapemap()));
	connect(ui.actionTapecopy, SIGNAL(triggered()), this, SLOT(tapecopy()));
	connect(ui.actionTapesplit, SIGNAL(triggered()), this, SLOT(tapesplit()));
	connect(ui.actionHetinit, SIGNAL(triggered()), this, SLOT(hetinit()));
	connect(ui.actionHetget, SIGNAL(triggered()), this, SLOT(hetget()));
	connect(ui.actionHetupd, SIGNAL(triggered()), this, SLOT(hetupd()));
	connect(ui.actionHetmap, SIGNAL(triggered()), this, SLOT(hetmap()));
    connect(ui.actionStartPrinter, SIGNAL(triggered()), this, SLOT(printer()));
    connect(ui.actionStationery, SIGNAL(triggered()), this, SLOT(stationery()));
    connect(ui.actionDecolation, SIGNAL(triggered()), this, SLOT(decolation()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));
	connect(mDevicesPane, SIGNAL(restartDevices()), this , SLOT(restartDevices()));
	connect(mCommandLine, SIGNAL(returnPressed()), this , SLOT(newCommand()));
    connect(mCommandLine, SIGNAL(ctrl_c()), this , SLOT(editCopy()));
	connect(mSystemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(systrayClick(QSystemTrayIcon::ActivationReason)));
    connectMainPanel();

	if (Preferences::getInstance().splitLog())
	{
		mLogWindow  = new LogWidget(this);
		fontChanged();
		setCentralWidget(static_cast<LogWidget *>(mLogWindow)->tabWidget());
	}
	else
	{
		mLogWindow  = new PlainLogWidget(this);
		mLogWindow->setReadOnly(true);
		fontChanged();
		setCentralWidget(mLogWindow);
	}

	int recoverRc = NamedPipe::getInstance().recover();
	mRecovery = (recoverRc == 1) ;
	if (mRecovery)
	{
		Recovery * r = new Recovery(this);
		r->exec();
		mRecoveryConfig = r->getHerculesConf().c_str();
		if (mRecoveryConfig == "!")
		{
			exit(0);
		}
		if (mRecoveryConfig == "")
			mRecovery = false;
		else
		{
			powerOn();
			mLogWindow->append("--- successfully attached to Hercules ---");
		}
	}
    mCommandLine->restore();
	setVisible(true);
	if (Preferences::getInstance().animate())
        QTimer::singleShot(100, this, SLOT(preferencesChanged()));
	if (Arguments::getInstance().configFileName().length() > 0)
		powerOn();
}

MainWindow::~MainWindow()
{
	DeviceConfigLine::clear();  // free acquired static data
	if (mConfigFile != NULL)
		delete mConfigFile;
}

void MainWindow::connectMainPanel()
{
	connect(mMainPanel, SIGNAL(powerOnClicked()), this , SLOT(powerOn()));
	connect(mMainPanel, SIGNAL(powerOffClicked()), this , SLOT(powerOff()));
	connect(mMainPanel, SIGNAL(interruptClicked()), this , SLOT(extInterrupt()));
	connect(mMainPanel, SIGNAL(loadClicked()), this , SLOT(load()));
	connect(mMainPanel, SIGNAL(restartClicked()), this , SLOT(restart()));
	connect(mMainPanel, SIGNAL(storeClicked()), this , SLOT(store()));
	connect(mMainPanel, SIGNAL(startClicked()), this , SLOT(start()));
	connect(mMainPanel, SIGNAL(stopClicked()), this , SLOT(stop()));
}

void MainWindow::writeToLog(QByteArray line)
{
	mLogWindow->append(line);
}

void MainWindow::preferencesChanged()
{
	fontChanged();
	mipsChanged();
	pswChanged();
	styleChanged();
	themeChanged();
	mLogWindow->preferencesChanged();
}

void MainWindow::fontChanged()
{
	std::string fontName = Preferences::getInstance().fontName(Preferences::LogFontObject);
    if (fontName.size() == 0 || fontName[0] == '(')
	{
		mLogWindow ->setFont(QFont());
	}
	else
	{
		QFont font(fontName.c_str());
		font.setPointSize(Preferences::getInstance().fontSize(Preferences::LogFontObject));
		font.setBold(Preferences::getInstance().fontIsBold(Preferences::LogFontObject));
		font.setItalic(Preferences::getInstance().fontIsItalic(Preferences::LogFontObject));
		mLogWindow->setFont(font);
	}
	mCRegisters32->setFont();
	mGRegisters32->setFont();
	mFRegisters32->setFont();
	mARegisters32->setFont();
	mCRegisters64->setFont();
	mGRegisters64->setFont();
	mFRegisters64->setFont();

	mPsw->setFont();
	mCommandLine->setFont();
}

void MainWindow::mipsChanged()
{
	mMainPanel->switchMips();
}

void MainWindow::pswChanged()
{
    mPsw->setMode(Preferences::getInstance().pswMode());
    mPswDock->setVisible(Preferences::getInstance().pswMode() == Psw::Docked);
}

void MainWindow::styleChanged()
{
	QByteArray loadAddress =
			QByteArray::number(mMainPanel->getLoadAddress(),16); // preserve
	mMainPanel->close();
	delete mMainPanel;
	if (Preferences::getInstance().theme() == Preferences::Classic)
		mMainPanel = new MainPanelClassic(this);
	else
		mMainPanel = new MainPanelModern(this);
	mMainPanel->setLoadAddress(loadAddress);	// restore ipl address
	if (mHerculesActive) mMainPanel->standby();
	mTopDock->setWidget(mMainPanel);
    connectMainPanel();
}

void MainWindow::themeChanged()
{
	setDarkBackground (Preferences::getInstance().darkBackground());
    if (Preferences::getInstance().animate())
		mMainPanel->animate();
}

void MainWindow::writeToLogFromQueue()
{
	if (!mLogQueue.empty())
	{
		QByteArray& s = mLogQueue.front();
		if (!s.isEmpty())
		{
			mLogWindow->append(s);
		}
		mLogQueue.pop_front();
	}
}

void MainWindow::dispatchStatus()
{
	while (!mStatusQueue.empty())
	{
		QByteArray& statusLine = mStatusQueue.front();
        hOutDebug(3, "writeToDevice:" << statusLine.data() << std::endl);
		if (!statusLine.isEmpty())
		{
			if (statusLine[0] == 'D')
			{
				if (mDevicesRecovery) recoverDevices(statusLine);
				mDevicesPane->notify(statusLine);
			}
			else if (statusLine[0] == 'G')
				mGRegisters32->notify(statusLine);
			else if (statusLine[0] == 'C')
				mCRegisters32->notify(statusLine);
			else if (statusLine[0] == 'F')
				mFRegisters32->notify(statusLine);
			else if (statusLine[0] == 'A')
				mARegisters32->notify(statusLine);
			else if (statusLine[0] == '6')
			{
				if (statusLine[3] == 'G')
					mGRegisters64->notify(statusLine);
				else if (statusLine[3] == 'C')
					mCRegisters64->notify(statusLine);
				else if (statusLine[3] == 'F')
					mFRegisters64->notify(statusLine);
			}
			else
			{
				if (mMainPanel->notify(statusLine))
					mStatusReceived = true;
				mPsw->notify(statusLine);
			}
		}
		mStatusQueue.pop_front();
	}
}

void MainWindow::recoverDevices(QByteArray& statusLine)
{
	hOutDebug(3,"recorver:" <<statusLine.data());
	if (statusLine.startsWith("DEVX=")) return;
	if (statusLine[4] == 'X')
	{
		if ( mDevicesRecoveryCommenced )
		{
			mDevicesRecoveryCommenced = false;
			mDevicesRecovery = false;
		}
		else
		{
			mDevicesRecoveryCommenced = true;
		}
		return;
	}
	else if (statusLine.startsWith("DEV="))
		statusLine = "DEVA=" + statusLine.mid(4);
	else  if (statusLine.startsWith("DEV") && statusLine[4] == '=')
		statusLine = "DEVA=" + statusLine.mid(5);

}

void MainWindow::newCommand()
{
	if (mHerculesExecutor == NULL) return;
	CommandLine *cl = static_cast<CommandLine *>(QObject::sender());
	if (cl == NULL) return;
    hOutDebug(2, cl->text().toStdString());
	if (!mLogWindow->isOSLog() || cl->text().left(1).compare(".") == 0)
		mHerculesExecutor->issueFormattedCommand("%s\n",cl->text().toUtf8().data());
	else
		mHerculesExecutor->issueFormattedCommand(".%s\n",cl->text().toUtf8().data());
    setIpled(cl->text());
}


void MainWindow::newConfig()
{
	mConfigFile = new ConfigFile("/usr/share/hrdplex/template.conf");
	mConfigFile->setNew(true);
	mConfiguration = new Configuration(mConfigFile, true, this);
	mConfiguration->show();
	this->setWindowTitle(("HRDPLEX - " + Arguments::getInstance().titleName() + " - Haynie Research & Development").c_str());
}

void MainWindow::config()
{
	if (mConfigFile == NULL)
		openConfig();
	if (mConfigFile == NULL)
		return;
	mConfiguration = new Configuration(mConfigFile, false, this);
	mConfiguration->show();
}

void MainWindow::openConfig()
{
    hOutDebug(5,"config at:" << Preferences::getInstance().configDir() << ".");
	std::string s = QFileDialog::getOpenFileName(this,
            tr("Open configuration"),
            Preferences::getInstance().configDir().c_str(),
			tr("Hercules configuration files (*.conf *.cnf);;All files(*)")).toUtf8().data();
	if (s.length() == 0)
		return;

	mConfigFile = new ConfigFile(s);
	QFileInfo f(s.c_str());
    Preferences::getInstance().setVolatileConfigDir(f.dir().absolutePath().toStdString().c_str());
	this->setWindowTitle(("HRDPLEX - " + Arguments::getInstance().titleName() + " - Haynie Research & Development").c_str());
}

void MainWindow::saveConfig()
{
	if (mConfigFile == NULL) return;
	if (mConfigFile->isNew())
	{
		saveConfigAs();
	}
	else
	{
		mConfigFile->appendNewLines();
		mConfigFile->write();
	}
}

void MainWindow::saveConfigAs()
{
	if (mConfigFile == NULL) return;

    if (mConfigFile->isNew())
	{
		if (mConfigFile->numberOfDevices() == 0)
		{
			int reply = QMessageBox::warning(this, "New Configuration",
											 "The new configuration does not contain any devices.\n"
											 "In most cases Hercules will not be able to process it\n\n"
											 "Are you sure you want to save this configuration?",
											 QMessageBox::Ok,
											 QMessageBox::Cancel);
			if (reply != QMessageBox::Ok)
			{
				mConfiguration = new Configuration(mConfigFile,false,this);
				mConfiguration->setVisible(true);
				return ;
			}
		}
	}

	std::string s = QFileDialog::getSaveFileName(this,
			"Save configuration",
            Preferences::getInstance().configDir().c_str(),
			tr("Hercules configuration files (*.conf *.cnf);;All files(*)")).toUtf8().data();
	if (s.length() == 0)
		return;
	int pos = s.length()-5;
	if ( (s.substr(pos,5) != ".conf") && (s.substr(pos+1,4) != ".cnf") )
		s += ".conf";
	mConfigFile->appendNewLines();
	mConfigFile->write(s);
	this->setWindowTitle(("HRDPLEX - " + Arguments::getInstance().titleName() + " - Haynie Research & Development").c_str());
}

void MainWindow::exitNow()
{
	powerOff();
	close();
}

void MainWindow::editViewControls()
{
	mTopDock->setVisible(!mTopDock->isVisible());
	ui.actionViewControls->setChecked(mTopDock->isVisible());
}

void MainWindow::editViewDevices()
{
	mDevicesDock->setVisible(!mDevicesDock->isVisible());
	ui.actionViewDevices->setChecked(mDevicesDock->isVisible());
}

void MainWindow::editViewCommand()
{
	mBottomDock->setVisible(!mBottomDock->isVisible());
	ui.actionViewCommand->setChecked(mBottomDock->isVisible());
}

void MainWindow::editView32BitGr()
{
	bool newVal = !mGRegisters32Dock->isVisible();
	mGRegisters32Dock->setVisible(newVal);
	mGRegisters32->setActive(newVal);
	ui.actionView_32_bit_General_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewGR32, newVal);
}

void MainWindow::editView32BitCr()
{
	bool newVal = !mCRegisters32Dock->isVisible();
	mCRegisters32Dock->setVisible(newVal);
	mCRegisters32->setActive(newVal);
	ui.actionView_32_bit_Control_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewCR32, newVal);
}

void MainWindow::editView32BitFr()
{
	bool newVal = !mFRegisters32Dock->isVisible();
	mFRegisters32Dock->setVisible(newVal);
	mFRegisters32->setActive(newVal);
	ui.actionView_32_bit_Floating_Point_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewFR32, newVal);
}

void MainWindow::editView32BitAr()
{
	bool newVal = !mARegisters32Dock->isVisible();
	mARegisters32Dock->setVisible(newVal);
	mARegisters32->setActive(newVal);
	ui.actionView_32_bit_Access_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewAR32, newVal);
}

void MainWindow::editView64BitGr()
{
	bool newVal = !mGRegisters64Dock->isVisible();
	mGRegisters64Dock->setVisible(newVal);
	mGRegisters64->setActive(newVal);
	ui.actionView_64_bit_General_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewGR64, newVal);
}

void MainWindow::editView64BitCr()
{
	bool newVal = !mCRegisters64Dock->isVisible();
	mCRegisters64Dock->setVisible(newVal);
	mCRegisters64->setActive(newVal);
	ui.actionView_64_bit_Control_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewCR64, newVal);
}

void MainWindow::editView64BitFr()
{
	bool newVal = !mFRegisters64Dock->isVisible();
	mFRegisters64Dock->setVisible(newVal);
	mFRegisters64->setActive(newVal);
	ui.actionView_64_bit_Floating_Point_Registers->setChecked(newVal);
	Preferences::getInstance().setRegs(Preferences::ViewFR64, newVal);
}

void MainWindow::editViewPSW()
{
	bool newVal = ui.actionView_PSW->isChecked();
	Preferences::getInstance().setRegs(Preferences::ViewPsw, newVal);
	mPsw->setVisible(newVal, Preferences::getInstance().theme() == Preferences::Modern);
	if (Preferences::getInstance().pswMode() == Psw::Docked)
	{
		mPswDock->setVisible(newVal);
	}
}

void MainWindow::editCopy()
{
    hOutDebug(4, "COPY");

    if (mLogWindow->textIsSelected())
    {
        mLogWindow->copySelectedText();
    }
}

void MainWindow::deleteMessages()
{
	if (mHerculesActive)
	{
		QMessageBox msgBox;
		msgBox.setText("Messages belong to to a running Hercules.");
		msgBox.setInformativeText("Are you sure you want to delete history messages?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setIcon(QMessageBox::Warning);
		int ret = msgBox.exec();
		if (ret != QMessageBox::Yes)
			return ;
	}
	mLogWindow->clear();
}


void MainWindow::saveMessages()
{
	mLogWindow->writeToFile(PlainLogWidget::MenuCommand);
	return;
}

void MainWindow::preferences()
{
    PreferencesWin * pw = new PreferencesWin(mCurrentPath.toStdString(), this);
    connect(pw, SIGNAL(preferencesChanged()), this, SLOT(preferencesChanged()));
	pw->exec();
}

ConfigFile * MainWindow::getConfigurationFile()
{
	return mConfigFile;
}


void MainWindow::powerOn()
{
	std::string configName;
	mStatusReceived=false;
	if (!mRecovery)
	{
		if (mConfigFile == NULL) openConfig();
		if (mConfigFile == NULL) return;
		configName = mConfigFile->getFileName();
	}
	else
	{
		configName = mRecoveryConfig.toStdString();
		mConfigFile = new ConfigFile(configName);
	}

	if (configName.empty()) return;
	if (mConfigFile->changed())
	{
		int ret = QMessageBox::warning(this, "Configuration",
											"Configuration file was updated since last save action\n"
											"Do you want to save it now?",
											QMessageBox::Save | QMessageBox::Cancel,
											QMessageBox::NoButton);
		if (ret == QMessageBox::Save)
		{
			saveConfig();
			if (mConfigFile->changed()) // was save done? if not - bail out
				return;
		}
		else return;
	}

	if (mHerculesExecutor != NULL) return;
	ui.menuFile->setEnabled(false);
	int herculesPid = 0;
	if (!mRecovery)
	{
        hOutDebug(5,"not recovery");
		mHerculesExecutor = new HerculesExecutor(*this);
        herculesPid = mHerculesExecutor->run(configName, Preferences::getInstance().hercDir());
		mDevicesRecovery = false;
	}
	else
	{
        hOutDebug(2,"recovery");
		herculesPid = NamedPipe::getInstance().getHerculesPid();
		mHerculesExecutor = new HerculesExecutor(*this, herculesPid);
		mDevicesRecovery = true;
	}

	if (herculesPid < 0)
	{
		QMessageBox::critical(this,"Hercules not started",
			"The Hercules program could not be started or not found.\n"
			"Please set the correct path to the hercules program\n"
			"using the Preferences window",
			QMessageBox::Ok);
		delete mHerculesExecutor;
		mHerculesExecutor = NULL;
		return;
	}

#ifndef hFramework
	NamedPipe::getInstance().generatePid(getpid(), herculesPid);
#endif

	mLogRunner = new LogRunner(mLogQueue, mHerculesExecutor, Preferences::getInstance().logFileLines());
    connect(mLogRunner, SIGNAL(newData()), this , SLOT(writeToLogFromQueue()));
	mLogRunner->start();

	mStatusRunner = new StatusRunner(mStatusQueue, mHerculesExecutor);
    connect(mStatusRunner, SIGNAL(newData()), this , SLOT(dispatchStatus()));
	mStatusRunner->start();

#ifndef hFramework
	mWatchdog = new Watchdog(herculesPid, !mRecovery);
#else
	mWatchdog = new Watchdog(mHerculesExecutor->getQProcess());
#endif
    connect(mWatchdog, SIGNAL(HerculesEnded()), this, SLOT(herculesEndedSlot()));
	mWatchdog->start();

	mHerculesActive = true;
	mMinimizeOnClose=true;
	mCommandLine->setReadOnly(false);

	mMainPanel->standby();
	mPsw->standby();
	this->setWindowTitle(("HRDPLEX - " + Arguments::getInstance().titleName() + " - Haynie Research & Development").c_str());
#ifndef hFramework
	if (mRecovery)
	{
		// refresh devices list
		issueCommand("]DEVLIST=1");
		usleep(500000);
		issueCommand("]NEWDEVLIST=1");

	}
#endif
	mRecovery = false;
	mSystemTrayIcon->show();

	if (ui.actionView_32_bit_General_Registers->isChecked())
		this->mGRegisters32->setActive(true);
	if (ui.actionView_32_bit_Control_Registers->isChecked())
		this->mCRegisters32->setActive(true);
	if (ui.actionView_32_bit_Floating_Point_Registers->isChecked())
		this->mFRegisters32->setActive(true);
	if (ui.actionView_32_bit_Access_Registers->isChecked())
		this->mARegisters32->setActive(true);
	if (ui.actionView_64_bit_General_Registers->isChecked())
		this->mGRegisters64->setActive(true);
	if (ui.actionView_64_bit_Control_Registers->isChecked())
		this->mCRegisters64->setActive(true);
	if (ui.actionView_64_bit_Floating_Point_Registers->isChecked())
		this->mFRegisters64->setActive(true);
}

void MainWindow::powerOff()
{
	if (!mHerculesActive) return ;
	mMinimizeOnClose=false;
	printf("Goodbye!\n");
	mHerculesExecutor->issueCommand("quit\n");
	#ifndef hFramework
	usleep(100000);  // TODO32
	#endif
	mDevicesPane->clear();
//	mMainPanel->setDormant();
	mPsw->setDormant();
    mCommandLine->save();
}

void MainWindow::load()
{
	if (!mHerculesActive) return;
	if (Preferences::getInstance().theme()==Preferences::Modern) // modern theme does not have dials
	{
		loadCommand();	// present menu etc.
		return;
	}
	int addr = mMainPanel->getLoadAddress();
    hOutDebug(3, "going to load " << addr);
	mHerculesExecutor->issueFormattedCommand("IPL %X\n",addr);
	mIplDevno.setNum(addr,16);
	mLogWindow->setIpled(true);
}

void MainWindow::loadCommand()
{
	if (!mHerculesActive) return;
	int lpIndex = -1;
	if (mAdHocLoadParm == "")
	{
		hOutDebug(5, "lastSYS=" << mConfigFile->getLastSys())
		for (int i=0; i<= mConfigFile->getLastSys(); i++)
		{
			if ((*mConfigFile)[i]->getLowercaseToken(0) == "loadparm")
			{
				lpIndex = i;
				mAdHocLoadParm = (*mConfigFile)[i]->getToken(1).c_str();
				break;
			}
			else hOutDebug(5,"skipping " << i << ": " << (*mConfigFile)[i]->getLowercaseToken(0));
		}
		if (lpIndex == -1) mAdHocLoadParm = "......";
	}
	if (mIplDevno.isEmpty())
		mIplDevno = "0000";
	if (Preferences::getInstance().theme() == Preferences::Classic)
		mIplDevno = QString::number(mMainPanel->getLoadAddress(),16);

	IplConfig * iplConfig = new IplConfig(mIplDevno,mAdHocLoadParm, this);

	int i = iplConfig->exec() ;
	if (i == QDialog::Accepted)
	{
		QString loadCommand = "LOADPARM " + mAdHocLoadParm;
		issueCommand(loadCommand.toStdString());
		mMainPanel->setLoadAddress(mIplDevno.toStdString().c_str());
		std::string iplCommand = "IPL " + mIplDevno.toStdString();
		issueCommand(iplCommand);
		mLogWindow->setIpled(true);
	}
}

void MainWindow::extInterrupt()
{
	if (!mHerculesActive) return;
    hOutDebug(3, "going to externally interrupt ");
	mHerculesExecutor->issueCommand("ext\n");
	return;
}

void MainWindow::devInterrupt()
{
	if (!mHerculesActive) return;
	int addr = mMainPanel->getLoadAddress();
    hOutDebug(3, "going to interrupt " << addr);
	mHerculesExecutor->issueFormattedCommand("I %X\n", addr);
}

void MainWindow::restart()
{
	if (mHerculesActive)
		mHerculesExecutor->issueCommand("restart\n");
}

void MainWindow::store()
{
	if (mHerculesActive)
		mHerculesExecutor->issueCommand("store\n");
}

void MainWindow::start()
{
	   if (mHerculesActive)
			mHerculesExecutor->issueCommand("startall\n");
}

void MainWindow::stop()
{
	if (mHerculesActive)
	{
		if (mMainPanel->isStopped())  // this is already done
			mHerculesExecutor->issueCommand("stopall\n");
		else
			mHerculesExecutor->issueCommand("startall\n");
	}
}

void MainWindow::dasdinit()
{
	DasdInit * dasdinit = new DasdInit(this);
	connect(dasdinit, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdinit->exec();
}

void MainWindow::dasdload()
{
	DasdLoad * dasdload = new DasdLoad(this);
	connect(dasdload, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdload->exec();
}

void MainWindow::dasdisup()
{
	DasdIsup * dasdisup = new DasdIsup(this);
	connect(dasdisup, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdisup->exec();
}

void MainWindow::dasdls()
{
	Dasdls * dasdls = new Dasdls(this);
	connect(dasdls, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdls->exec();
}

void MainWindow::dasdcat()
{
	Dasdcat * dasdcat = new Dasdcat(this);
	connect(dasdcat, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdcat->exec();
}

void MainWindow::dasdconv()
{
	Dasdconv * dasdconv = new Dasdconv(this);
	connect(dasdconv, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdconv->exec();
}

void MainWindow::dasdcopy()
{
	Dasdcopy * dasdcopy = new Dasdcopy(this);
	connect(dasdcopy, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	dasdcopy->exec();
}

void MainWindow::tapemap()
{
	TapeMap * tapemap = new TapeMap(this);
	connect(tapemap, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	tapemap->exec();
}

void MainWindow::tapecopy()
{
	TapeCopy * tapecopy = new TapeCopy(this);
	connect(tapecopy, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	tapecopy->exec();
}

void MainWindow::tapesplit()
{
	TapeSplt * tapesplit = new TapeSplt(this);
	connect(tapesplit, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	tapesplit->exec();
}

void MainWindow::hetinit()
{
	HetInit * hetinit = new HetInit(this);
	connect(hetinit, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	hetinit->exec();
}

void MainWindow::hetget()
{
	HetGet * hetget = new HetGet(this);
	connect(hetget, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	hetget->exec();
}

void MainWindow::hetupd()
{
	HetUpd * hetupd = new HetUpd(this);
	connect(hetupd, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	hetupd->exec();
}

void MainWindow::hetmap()
{
	HetMap * hetmap = new HetMap(this);
	connect(hetmap, SIGNAL(output(QByteArray)), this , SLOT(writeToLog(QByteArray)));
	hetmap->exec();
}

void MainWindow::printer()
{
    if (mPrinterDialog == NULL)
        mPrinterDialog = new PrinterDialog(0);
    mPrinterDialog->show();
    connect(mPrinterDialog, SIGNAL(destroyed()), this, SLOT(printerFinished()));
}

void MainWindow::printerFinished()
{
    mPrinterDialog = NULL;
}

void MainWindow::stationery()
{
    StationeryDialog* stationeryDialog = new StationeryDialog(this);
    stationeryDialog->exec();
}

void MainWindow::decolation()
{
    DecolationDialog* dialog = new DecolationDialog(this);
    dialog->exec();
}

void MainWindow::herculesEndedSlot()
{
	std::cerr << "############ ended ##############" << std::endl;
	mLogWindow->setIpled(false);
	mLogWindow->writeToFile(PlainLogWidget::Shutdown);
	mLogRunner->terminate();
	mStatusRunner->terminate();
	mWatchdog->terminate();
	if (mWatchdog != NULL) delete mWatchdog;
	mWatchdog = NULL;
	mHerculesActive = false;
	mCommandLine->setReadOnly(true);
	mHerculesExecutor = NULL;
	mMainPanel->setDormant();
	mPsw->setDormant();
	ui.menuFile->setEnabled(true);
	mSystemTrayIcon->setVisible(false);
	setVisible(true);
	mMinimizeOnClose=false;
	testGui();
}

void MainWindow::helpAbout()
{
	HelpAbout *ha = new HelpAbout(this);
	ha->exec();
}

void MainWindow::systrayHint()
{
	if (!mSystrayHintEjected)
	{
        mSystemTrayIcon->showMessage("Hercules is still active","Click the Hercules icon to restore the main window",QSystemTrayIcon::Information,5000);
		mSystemTrayIcon->setToolTip("Click on this icon to restore the main window");
		mSystrayHintEjected = true;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
	 if (mHerculesActive && mMinimizeOnClose)
	 {
		 hOutDebug(5,"systray");
		 mSystemTrayIcon->setVisible(true);
		 systrayHint();
		 setVisible(false);
         mSystemTrayIcon->contextMenu()->actions().at(0)->setText("&Restore");

         event->ignore();
	 }
	 else
	 {
		 hOutDebug(5,"Close");

		 if (mHerculesActive)
		 {
			 if( QMessageBox::warning(this,"Hercules Is Still Running",
					 "\nPress OK - To Wait Until Hercules Finishes Running\n"
					 "Or\n"
					 "Press Abort - To Force Exit",
					 QMessageBox::Ok, QMessageBox::Abort) != QMessageBox::Abort)
			 {
				 event->ignore();
				 QTimer::singleShot(5000, this, SLOT(tryAbort()));
			 }
			 else hOutDebug(5,"abort");
		 }

         if (mPrinterDialog != NULL)
         {
             if (mPrinterDialog->isConnected())
             {
                 if( QMessageBox::warning(this,"Printer Is Still Connected",
                                          "Press Close - To Close The Printer\n"
                                          "Or\n"
                                          "Press Cancel - To Return",
                                          QMessageBox::Cancel, QMessageBox::Close) == QMessageBox::Cancel)
                 {
                     event->ignore();
                 }
                 else
                 {
                     mPrinterDialog->close();
                     delete mPrinterDialog;
                 }
             }
             else
             {
                 mPrinterDialog->close();
                 delete mPrinterDialog;
             }
         }
	 }
 }

void MainWindow::tryAbort()
{
	hOutDebug(5,"tryAbort");
	setVisible(true);
	close();
}

void MainWindow::systrayClick(QSystemTrayIcon::ActivationReason)
{
    hideRestore();
}

void MainWindow::hideRestore()
{
    if (isVisible())
	{
		setVisible(false);
		systrayHint();
        mSystemTrayIcon->contextMenu()->actions().at(0)->setText("Restore");

	}
	else
	{
		setVisible(true);
        mSystemTrayIcon->contextMenu()->actions().at(0)->setText("Hide");
    }
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    hOutDebug(4,"key:" << event->key() << " " << (event->key() == Qt::Key_Up));
	QMainWindow::keyPressEvent(event);
	if (!mCommandLine->hasFocus())
		mCommandLine->setFocus();
}

void MainWindow::restartDevices()
{
	hOutDebug(3,"MainWindow::restartRecovery");
	mDevicesRecovery = true;
	issueCommand("]DEVLIST=1");
	#ifndef hFramework
	usleep(500000);  // TODO32
	#endif
	issueCommand("]NEWDEVLIST=1");
}

bool MainWindow::issueCommand(const std::string& command)
{
    QString qCommand  = command.c_str();
    if (mHerculesActive)
    {
        mHerculesExecutor->issueFormattedCommand("%s\n", command.c_str());
        setIpled(qCommand);
    }

    hOutDebug(2,"issue command:" << command);
   return true;
}

void MainWindow::testGui()
{
	if ( !mStatusReceived &&
		 (mLogWindow->toPlainText().indexOf("No External GUI support") > 0) )
	{
		QMessageBox::critical(this,"No GUI Support",
			 "The Hercules program that you are running does not support external GUI\n"
			 "Please obtain a module that was built with GUI support",
			 QMessageBox::Ok);
	}
}

void MainWindow::setIpled(QString command)
{
    if (mHerculesActive)
    {
         if (command.toLower().leftJustified(4).startsWith("ipl "))
         {
             mLogWindow->setIpled(true);
         }
         else if (command.toLower().leftJustified(5).startsWith("quit "))
         {
             mLogWindow->setIpled(false);
         }

    }
}

void MainWindow::setDarkBackground(bool dark)
{
	if (dark)
	{
        QFile file(":/icons/darkorange.css");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				 return;
		QByteArray ss = file.readAll();
		setStyleSheet(ss);
	}
	else
		setStyleSheet("");
}
