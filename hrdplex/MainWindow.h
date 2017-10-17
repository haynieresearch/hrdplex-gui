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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gui.h"
#include "ui_MainWindow.h"
#include "MainPanel.h"
#include "DevicesPane.h"
#include "CommandLine.h"
#include "Psw.h"
#include "Regs32.h"
#include "Regs64.h"
#include "HerculesExecutor.h"
#include "LogRunner.h"
#include "StatusRunner.h"
#include "Preferences.h"
#include "Watchdog.h"

#include "ConfigFile.h"
#include "Configuration.h"
#include <QTextEdit>
#include <QSystemTrayIcon>
#include <QWaitCondition>

class LogRunner;
class HerculesExecutor;
class PlainLogWidget;
class PrinterDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    bool  			issueCommand(const std::string& command);
    ConfigFile      * getConfigurationFile();

private:
    Ui::MainWindowClass ui;
    QDockWidget         * mTopDock;
    QDockWidget         * mDevicesDock;
    QDockWidget         * mBottomDock;
    QDockWidget         * mGRegisters32Dock;
    QDockWidget         * mCRegisters32Dock;
    QDockWidget         * mFRegisters32Dock;
    QDockWidget         * mARegisters32Dock;
    QDockWidget         * mGRegisters64Dock;
    QDockWidget         * mCRegisters64Dock;
    QDockWidget         * mFRegisters64Dock;
    QDockWidget			* mPswDock;

    Configuration       * mConfiguration;
    ConfigFile          * mConfigFile;
    PlainLogWidget     * mLogWindow;
    QSystemTrayIcon     * mSystemTrayIcon;
    MainPanel           * mMainPanel;
    DevicesPane         * mDevicesPane;
    CommandLine         * mCommandLine;
    Psw					* mPsw;
    Regs32              * mGRegisters32;
    Regs32		 		* mCRegisters32;
    Regs32				* mFRegisters32;
    Regs32				* mARegisters32;
    Regs64  			* mGRegisters64;
    Regs64              * mCRegisters64;
    Regs64              * mFRegisters64;
    SynchronizedQueue   mLogQueue;
    SynchronizedQueue   mStatusQueue;
    HerculesExecutor    * mHerculesExecutor;
    LogRunner *         mLogRunner;
    StatusRunner        * mStatusRunner;
    Watchdog            * mWatchdog;
    PrinterDialog       * mPrinterDialog;
    bool                mHerculesActive;
    bool				mMinimizeOnClose;
    bool                mRecovery;
    bool 				mDevicesRecovery;
    bool 				mDevicesRecoveryCommenced;
    bool                mSystrayHintEjected;
    bool				mStatusReceived;
    QString             mRecoveryConfig;
    QString             mCurrentPath;
    QString             mAdHocLoadParm;
    QString             mIplDevno;

    void 				testGui();
    void  				connectMainPanel();
	void				fontChanged();
	void				mipsChanged();
	void				pswChanged();
	void				styleChanged();
	void				themeChanged();
    void                setIpled(QString ipled);
	void				setDarkBackground(bool dark);


public slots:
    void powerOn();
    void powerOff();
    void extInterrupt();
    void devInterrupt();
    void load();
    void restart();
    void store();
    void start();
    void stop();
    void restartDevices();

    // menu-related slots
    void newConfig();
    void config();
    void saveConfig();
    void saveConfigAs();
    void openConfig();
    void exitNow();
    void loadCommand();
    void editViewControls();
    void editViewDevices();
    void editViewCommand();
    void editView32BitGr();
    void editView32BitCr();
    void editView32BitFr();
    void editView32BitAr();
    void editView64BitGr();
    void editView64BitCr();
    void editView64BitFr();
    void editViewPSW();
    void dasdinit();
    void dasdload();
    void dasdisup();
    void dasdls();
    void dasdcat();
    void dasdconv();
    void dasdcopy();
    void tapemap();
    void tapecopy();
	void tapesplit();
	void hetinit();
	void hetget();
	void hetupd();
	void hetmap();
    void printer();
    void printerFinished();
    void stationery();
    void decolation();
    void helpAbout();

    void writeToLogFromQueue();
	void writeToLog(QByteArray line);
	void preferencesChanged();
    void dispatchStatus();
    void newCommand();
    void herculesEndedSlot();
    void editCopy();
    void deleteMessages();
    void saveMessages();
    void preferences();
    void closeEvent(QCloseEvent * event);
    void tryAbort();
    void systrayClick(QSystemTrayIcon::ActivationReason);
    void hideRestore();
    void systrayHint();
    void recoverDevices(QByteArray& statusLine);
	void keyPressEvent(QKeyEvent * event);
};

#endif // MAINWINDOW_H
