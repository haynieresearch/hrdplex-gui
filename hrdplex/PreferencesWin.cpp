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

#include "PreferencesWin.h"
#include "gui.h"
#include "Fonts.h"

#include <QFileDialog>

#include <iostream>

PreferencesWin::PreferencesWin( const std::string& currentPath, QWidget *parent)
    : QDialog(parent), mCurrentPath(currentPath), mPreferences(&Preferences::getInstance())
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	ui.AnimateCheckBox->setVisible(false);
#endif

	connect(ui.okButton, SIGNAL(pressed()), this, SLOT(okPressed()));
	connect(ui.cancelButton, SIGNAL(pressed()), this, SLOT(cancelPressed()));
	connect(ui.hercDirButton, SIGNAL(pressed()), this, SLOT(hercDirPressed()));
	connect(ui.configDirButton, SIGNAL(pressed()), this, SLOT(configDirPressed()));
	connect(ui.logsDirButton, SIGNAL(pressed()), this, SLOT(logsDirPressed()));
	connect(ui.mipsGaugeButton, SIGNAL(toggled(bool)), this, SLOT(mipsToggled()));

	ui.hercDirLineEdit->setText(mPreferences->hercDir().c_str());
	ui.configDirLineEdit->setText(mPreferences->configDir().c_str());
	ui.logsDirLineEdit->setText(mPreferences->logsDir().c_str());

	QStringList& list = Fonts::getInstance().getList();

	ui.fontNameLog->addItems(list);
	ui.fontNameRegs->addItems(list);
	ui.fontNamePsw->addItems(list);
	ui.fontNameCommand->addItems(list);
	ui.fontNameConfiguration->addItems(list);

	int fontIndex = Fonts::getInstance().indexOf(mPreferences->fontName(Preferences::LogFontObject).c_str());
	if (fontIndex == -1) fontIndex = 0;
	ui.fontNameLog->setCurrentIndex(fontIndex);
	ui.fontSizeLog->setValue(mPreferences->fontSize(Preferences::LogFontObject));
	ui.boldLog->setChecked(mPreferences->fontIsBold(Preferences::LogFontObject));
	ui.italicLog->setChecked(mPreferences->fontIsItalic(Preferences::LogFontObject));

	fontIndex = Fonts::getInstance().indexOf(mPreferences->fontName(Preferences::RegsFontObject).c_str());
	if (fontIndex == -1) fontIndex = 0;
	ui.fontNameRegs->setCurrentIndex(fontIndex);
	ui.fontSizeRegs->setValue(mPreferences->fontSize(Preferences::RegsFontObject));
	ui.boldRegs->setChecked(mPreferences->fontIsBold(Preferences::RegsFontObject));
	ui.italicRegs->setChecked(mPreferences->fontIsItalic(Preferences::RegsFontObject));

	fontIndex = Fonts::getInstance().indexOf(mPreferences->fontName(Preferences::PswFontObject).c_str());
	if (fontIndex == -1) fontIndex = 0;
	ui.fontNamePsw->setCurrentIndex(fontIndex);
	ui.fontSizePsw->setValue(mPreferences->fontSize(Preferences::PswFontObject));
	ui.boldPsw->setChecked(mPreferences->fontIsBold(Preferences::PswFontObject));
	ui.italicPsw->setChecked(mPreferences->fontIsItalic(Preferences::PswFontObject));

	fontIndex = Fonts::getInstance().indexOf(mPreferences->fontName(Preferences::CommandFontObject).c_str());
	if (fontIndex == -1) fontIndex = 0;
	ui.fontNameCommand->setCurrentIndex(fontIndex);
	ui.fontSizeCommand->setValue(mPreferences->fontSize(Preferences::CommandFontObject));
	ui.boldCommand->setChecked(mPreferences->fontIsBold(Preferences::CommandFontObject));
	ui.italicCommand->setChecked(mPreferences->fontIsItalic(Preferences::CommandFontObject));

	fontIndex = Fonts::getInstance().indexOf(mPreferences->fontName(Preferences::ConfigurationFontObject).c_str());
	if (fontIndex == -1) fontIndex = 0;
	ui.fontNameConfiguration->setCurrentIndex(fontIndex);
	ui.fontSizeConfiguration->setValue(mPreferences->fontSize(Preferences::ConfigurationFontObject));
	ui.boldConfiguration->setChecked(mPreferences->fontIsBold(Preferences::ConfigurationFontObject));
	ui.italicConfiguration->setChecked(mPreferences->fontIsItalic(Preferences::ConfigurationFontObject));

	ui.logTimestampCheckbox->setChecked(mPreferences->logTimestamp());

	if (mPreferences->mipsAsGauge())
		ui.mipsGaugeButton->setChecked(true);
	else
		ui.mipsLedButton->setChecked(true);

	if (mPreferences->pswMode() == Psw::Docked)
		ui.pswDock->setChecked(true);
	else
		ui.pswDock->setChecked(false);

	ui.splitLogCheckbox->setChecked(mPreferences->splitLog());
	ui.autosaveLogCheckbox->setChecked(mPreferences->autosaveLog());

	if (mPreferences->theme() == Preferences::Classic)
		ui.classicRadioButton->setChecked(true);
	else
		ui.modernRadioButton->setChecked(true);

	if (mPreferences->logFileLines() > 0)
		ui.memLines->setValue( mPreferences->logFileLines() );
	else
		ui.memLines->setValue(5000);

	if (mPreferences->greenLed())
		ui.GreenCheckBox->setChecked(true);
	else
		ui.GreenCheckBox->setChecked(false);

	if (mPreferences->animate())
		ui.AnimateCheckBox->setChecked(true);
	else
		ui.AnimateCheckBox->setChecked(false);

	if (mPreferences->darkBackground())
		ui.ThemeComboBox->setCurrentIndex(1);
	else
		ui.ThemeComboBox->setCurrentIndex(0);
}

PreferencesWin::~PreferencesWin()
{
}

void PreferencesWin::okPressed()
{
	mPreferences->setHercDir(ui.hercDirLineEdit->text().toStdString());
	mPreferences->setConfigDir(ui.configDirLineEdit->text().toStdString());
	mPreferences->setLogsDir(ui.logsDirLineEdit->text().toStdString());


	mPreferences->setFontName(Preferences::LogFontObject, ui.fontNameLog->currentText().toStdString());
	mPreferences->setFontSize(Preferences::LogFontObject, ui.fontSizeLog->value());
	mPreferences->setBold(Preferences::LogFontObject, ui.boldLog->isChecked());
	mPreferences->setItalic(Preferences::LogFontObject, ui.italicLog->isChecked());

	mPreferences->setFontName(Preferences::RegsFontObject, ui.fontNameRegs->currentText().toStdString());
	mPreferences->setFontSize(Preferences::RegsFontObject, ui.fontSizeRegs->value());
	mPreferences->setBold(Preferences::RegsFontObject, ui.boldRegs->isChecked());
	mPreferences->setItalic(Preferences::RegsFontObject, ui.italicRegs->isChecked());

	mPreferences->setFontName(Preferences::PswFontObject, ui.fontNamePsw->currentText().toStdString());
	mPreferences->setFontSize(Preferences::PswFontObject, ui.fontSizePsw->value());
	mPreferences->setBold(Preferences::PswFontObject, ui.boldPsw->isChecked());
	mPreferences->setItalic(Preferences::PswFontObject, ui.italicPsw->isChecked());

	mPreferences->setFontName(Preferences::CommandFontObject, ui.fontNameCommand->currentText().toStdString());
	mPreferences->setFontSize(Preferences::CommandFontObject, ui.fontSizeCommand->value());
	mPreferences->setBold(Preferences::CommandFontObject, ui.boldCommand->isChecked());
	mPreferences->setItalic(Preferences::CommandFontObject, ui.italicCommand->isChecked());

	mPreferences->setFontName(Preferences::ConfigurationFontObject, ui.fontNameConfiguration->currentText().toStdString());
	mPreferences->setFontSize(Preferences::ConfigurationFontObject, ui.fontSizeConfiguration->value());
	mPreferences->setBold(Preferences::ConfigurationFontObject, ui.boldConfiguration->isChecked());
	mPreferences->setItalic(Preferences::ConfigurationFontObject, ui.italicConfiguration->isChecked());

	mPreferences->setLogTimestamp(ui.logTimestampCheckbox->isChecked());

	mPreferences->setMipsAsGauge(ui.mipsGaugeButton->isChecked());

	mPreferences->setPswMode( ui.pswDock->isChecked() ? Psw::Docked : Psw::StatusBar);

	mPreferences->setSplitLog( ui.splitLogCheckbox->isChecked());

	mPreferences->setAutosaveLog( ui.autosaveLogCheckbox->isChecked());

	mPreferences->setTheme( ui.classicRadioButton->isChecked() ? Preferences::Classic : Preferences::Modern);

	mPreferences->setLogFileLines( ui.memLines->value() );

	mPreferences->setGreenLed(ui.GreenCheckBox->isChecked());
	ui.GreenCheckBox->setEnabled(!ui.mipsGaugeButton->isChecked());

	mPreferences->setAnimate(ui.AnimateCheckBox->isChecked());

	mPreferences->setDarkBackground(ui.ThemeComboBox->currentIndex() == 1);

	mPreferences->write();
	setVisible(false); // do not hide animation
	emit preferencesChanged();
	close();
}

void PreferencesWin::cancelPressed()
{
	close();
}

void PreferencesWin::hercDirPressed()
{
	QString s = QFileDialog::getExistingDirectory(this,
		tr("Find Hercules directory"),
		ui.hercDirLineEdit->text(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (s.length() > 0)
		ui.hercDirLineEdit->setText(s);
}
void PreferencesWin::configDirPressed()
{
	QString s = QFileDialog::getExistingDirectory(this,
		tr("Hercules configurations directory"),
		ui.configDirLineEdit->text(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (s.length() > 0)
		ui.configDirLineEdit->setText(s);
}
void PreferencesWin::logsDirPressed()
{
	QString s = QFileDialog::getExistingDirectory(this,
		tr("Logs directory"),
		ui.logsDirLineEdit->text(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (s.length() > 0)
	  ui.logsDirLineEdit->setText(s);
}

void PreferencesWin::mipsToggled()
{
	ui.GreenCheckBox->setEnabled(!ui.mipsGaugeButton->isChecked());
}
