TEMPLATE = app
QT       += widgets
QT       += network
QT       += gui
TARGET = hrdplex-gui
INCLUDEPATH +=  \
                 hrdplex \
                 configuration \
                 utilities \
                 printer

!win32{
QMAKE_CC   = @echo [c] $< && $$QMAKE_CC
QMAKE_CXX  = @echo [c++] $< && $$QMAKE_CXX
QMAKE_MOC  = @echo [moc] $< && $$QMAKE_MOC
QMAKE_LINK = @echo [link] $$TARGET && $$QMAKE_LINK
#QMAKE_UIC  = @echo [uic ] $< && $$QMAKE_UIC
QMAKE_CXXFLAGS = -pedantic -Wno-long-long -std=c++11
}
UI_DIR = ui
MOC_DIR = moc
OBJECTS_DIR = obj


# Input
HEADERS += configuration/CardReaderProperties.h \
           configuration/ConfigFile.h \
           configuration/ConfigLine.h \
           configuration/Configuration.h \
           configuration/ConfigurationEditor.h \
           configuration/ConfigurationFreeEditor.h \
           configuration/ConsoleProperties.h \
           configuration/CtcProperties.h \
           configuration/DasdProperties.h \
           configuration/DeviceConfigLine.h \
           configuration/DeviceConfigView.h \
           configuration/DeviceListView.h \
           configuration/DeviceMenuProcessor.h \
           configuration/DevicesPane.h \
           configuration/DevicesRename.h \
           configuration/DevicesWidget.h \
           configuration/DeviceTypes.h \
           configuration/Extended_Ui_CardReaderProperties.h \
           configuration/GenericDeviceProperties.h \
           configuration/NewReaderFile.h \
           configuration/PrinterProperties.h \
           configuration/PunchProperties.h \
           configuration/ReaderFileListView.h \
           configuration/StatusUpdateCollector.h \
           configuration/SysgProperties.h \
           configuration/SystemConfigLine.h \
           configuration/TapeProperties.h \
           configuration/TerminalProperties.h \
           configuration/VisualizedDeviceEntry.h \
           hrdplex/Arguments.h \
           hrdplex/CommandLine.h \
           hrdplex/Environment.h \
           hrdplex/FieldTip.h \
           hrdplex/FontPreferences.h \
           hrdplex/Fonts.h \
           hrdplex/HelpAbout.h \
           hrdplex/HerculesExecutor.h \
           hrdplex/gui.h \
           hrdplex/IplConfig.h \
           hrdplex/LogRunner.h \
           hrdplex/LogWidget.h \
           hrdplex/MainPanel.h \
           hrdplex/MainPanelClassic.h \
           hrdplex/MainPanelModern.h \
           hrdplex/MainWindow.h \
           hrdplex/Mips.h \
           hrdplex/MipsGauge.h \
           hrdplex/MipsLed.h \
           hrdplex/NamedPipe.h \
           hrdplex/PanelButton.h \
           hrdplex/Preferences.h \
           hrdplex/PreferencesWin.h \
           hrdplex/Psw.h \
           hrdplex/Recovery.h \
           hrdplex/Regs32.h \
           hrdplex/Regs64.h \
           hrdplex/Runner.h \
           hrdplex/StatusRunner.h \
           hrdplex/StringTokenizer.h \
           hrdplex/SynchronizedQueue.h \
           hrdplex/Watchdog.h \
           utilities/Dasdcat.h \
           utilities/Dasdconv.h \
           utilities/Dasdcopy.h \
           utilities/DasdInit.h \
           utilities/DasdIsup.h \
           utilities/DasdLoad.h \
           utilities/Dasdls.h \
           utilities/DasdTab.h \
           utilities/GenericUtility.h \
           utilities/HetGet.h \
           utilities/HetInit.h \
           utilities/HetMap.h \
           utilities/HetUpd.h \
           utilities/LineEditWithFocus.h \
           utilities/SystemUtils.h \
           utilities/TapeCopy.h \
           utilities/TapeMap.h \
           utilities/TapeSplt.h \
           utilities/UtilityExecutor.h \
           utilities/UtilityRunner.h \
           printer/PrinterDialog.h \
           printer/StationeryDialog.h \
           printer/Stationery.h \
           printer/DecolationRules.h \
           printer/Trigger.h \
           printer/PagePrinter.h \
           printer/PrintRunner.h \
           printer/PrinterItem.h \
           hrdplex/StringedObject.h \
           printer/PrinterController.h \
           printer/DecolationDialog.h \
           hrdplex/IpValidator.h \
           printer/PrinterInterface.h
FORMS += configuration/CardReaderProperties.ui \
         configuration/Configuration.ui \
         configuration/ConsoleProperties.ui \
         configuration/CtcProperties.ui \
         configuration/DasdProperties.ui \
         configuration/Devices.ui \
         configuration/DevicesRename.ui \
         configuration/configuration.ui \
         configuration/PrinterProperties.ui \
         configuration/PunchProperties.ui \
         configuration/TapeProperties.ui \
         configuration/TerminalProperties.ui \
         hrdplex/helpabout.ui \
         hrdplex/IplConfig.ui \
         hrdplex/MainWindow.ui \
         hrdplex/PreferencesWin.ui \
         hrdplex/Recovery.ui \
         utilities/Dasdcat.ui \
         utilities/Dasdconv.ui \
         utilities/Dasdcopy.ui \
         utilities/DasdInit.ui \
         utilities/DasdIsup.ui \
         utilities/DasdLoad.ui \
         utilities/Dasdls.ui \
         utilities/HetGet.ui \
         utilities/HetInit.ui \
         utilities/HetMap.ui \
         utilities/HetUpd.ui \
         utilities/TapeCopy.ui \
         utilities/TapeMap.ui \
         utilities/TapeSplt.ui \
         utilities/TapeSpltSubDlg.ui \
         printer/PrinterDialog.ui \
         printer/StationeryDialog.ui \
         printer/DecolationDialog.ui
SOURCES += configuration/CardReaderProperties.cpp \
           configuration/ConfigFile.cpp \
           configuration/ConfigLine.cpp \
           configuration/Configuration.cpp \
           configuration/ConfigurationEditor.cpp \
           configuration/ConfigurationFreeEditor.cpp \
           configuration/ConsoleProperties.cpp \
           configuration/CtcProperties.cpp \
           configuration/DasdProperties.cpp \
           configuration/DeviceConfigLine.cpp \
           configuration/DeviceConfigView.cpp \
           configuration/DeviceListView.cpp \
           configuration/DeviceMenuProcessor.cpp \
           configuration/DevicesPane.cpp \
           configuration/DevicesRename.cpp \
           configuration/DevicesWidget.cpp \
           configuration/DeviceTypes.cpp \
           configuration/Extended_Ui_CardReaderProperties.cpp \
           configuration/GenericDeviceProperties.cpp \
           configuration/NewReaderFile.cpp \
           configuration/PrinterProperties.cpp \
           configuration/PunchProperties.cpp \
           configuration/ReaderFileListView.cpp \
           configuration/StatusUpdateCollector.cpp \
           configuration/SysgProperties.cpp \
           configuration/SystemConfigLine.cpp \
           configuration/TapeProperties.cpp \
           configuration/TerminalProperties.cpp \
           configuration/VisualizedDeviceEntry.cpp \
           hrdplex/Arguments.cpp \
           hrdplex/CommandLine.cpp \
           hrdplex/Environment.cpp \
           hrdplex/FieldTip.cpp \
           hrdplex/FontPreferences.cpp \
           hrdplex/Fonts.cpp \
           hrdplex/HelpAbout.cpp \
           hrdplex/HerculesExecutor.cpp \
           hrdplex/IplConfig.cpp \
           hrdplex/LogRunner.cpp \
           hrdplex/LogWidget.cpp \
           hrdplex/main.cpp \
           hrdplex/MainPanel.cpp \
           hrdplex/MainPanelClassic.cpp \
           hrdplex/MainPanelModern.cpp \
           hrdplex/MainWindow.cpp \
           hrdplex/Mips.cpp \
           hrdplex/MipsGauge.cpp \
           hrdplex/MipsLed.cpp \
           hrdplex/NamedPipe.cpp \
           hrdplex/PanelButton.cpp \
           hrdplex/Preferences.cpp \
           hrdplex/PreferencesWin.cpp \
           hrdplex/Psw.cpp \
           hrdplex/Recovery.cpp \
           hrdplex/Regs32.cpp \
           hrdplex/Regs64.cpp \
           hrdplex/Runner.cpp \
           hrdplex/StatusRunner.cpp \
           hrdplex/SynchronizedQueue.cpp \
           hrdplex/Watchdog.cpp \
           utilities/Dasdcat.cpp \
           utilities/Dasdconv.cpp \
           utilities/Dasdcopy.cpp \
           utilities/DasdInit.cpp \
           utilities/DasdIsup.cpp \
           utilities/DasdLoad.cpp \
           utilities/Dasdls.cpp \
           utilities/DasdTab.cpp \
           utilities/GenericUtility.cpp \
           utilities/HetGet.cpp \
           utilities/HetInit.cpp \
           utilities/HetMap.cpp \
           utilities/HetUpd.cpp \
           utilities/LineEditWithFocus.cpp \
           utilities/SystemUtils.cpp \
           utilities/TapeCopy.cpp \
           utilities/TapeMap.cpp \
           utilities/TapeSplt.cpp \
           utilities/UtilityExecutor.cpp \
           utilities/UtilityRunner.cpp \
           printer/PrinterDialog.cpp \
           printer/DecolationRules.cpp \
           printer/StationeryDialog.cpp \
           printer/Stationery.cpp \
           printer/Trigger.cpp \
           printer/PagePrinter.cpp \
           printer/PrintRunner.cpp \
           printer/PrinterItem.cpp \
           hrdplex/StringedObject.cpp \
           printer/PrinterController.cpp \
           printer/DecolationDialog.cpp \
           hrdplex/IpValidator.cpp \
           printer/PrinterInterface.cpp
RESOURCES += hrdplex/gui.qrc

target.path = /usr/bin
INSTALLS += target
CONFIG += warn_on
CONFIG += debug_and_release
macx:RC_FILE = gui.icns
