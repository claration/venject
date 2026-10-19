#include <cstdio>
#include <cstdlib>
#include <getopt.h> // getopt_long
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QSettings>
#include <QCommandLineParser>

#include "launch.hpp"

int main(int argc, char **argv)
{
	// force software renderer because some people
	// might not have a gpu
	if (getenv("QT_QUICK_BACKEND") == nullptr) {
		#ifdef Q_OS_WIN32
		_putenv_s("QT_QUICK_BACKEND", "software");
		#else  // Q_OS_WIN32
		setenv("QT_QUICK_BACKEND", "software", 1);
		#endif // Q_OS_WIN32
	}

	QGuiApplication	app(argc, argv);
	Launcher launcher;

	QCommandLineParser parser;
	parser.setApplicationDescription("Vencord Launcher");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption launchOpt(QStringList{"l", "launch"}, "Launch a Discord executable", "FILE PATH");
	parser.addOption(launchOpt);

	QCommandLineOption launchArgOpt(QStringList{"a", "launch-arg"}, "Override launch arguments", "ARG");
	parser.addOption(launchArgOpt);

	QCommandLineOption overrideEntryOpt(QStringList{"o", "override-entry"}, "Override Vencord patcher", "FILE PATH");
	parser.addOption(overrideEntryOpt);

	parser.process(app);

	QString launch = parser.value(launchOpt);
	QStringList launchArg = parser.values(launchArgOpt);
	QStringList overrideEntry = parser.values(overrideEntryOpt);

	QStringList launch_mod_argv;

	for (int i = 0; i < argc; ++i)
		launch_mod_argv.append(QString::fromLocal8Bit(argv[i]));

	if (!launch.isEmpty())
	{
		launcher.launch(launch, launchArg, launch_mod_argv);
		app.exit();
		#ifndef Q_OS_WIN
		exit(0);
		#endif
	}

	app.setOrganizationName("claration");
	app.setOrganizationDomain("clarati.on");
	app.setApplicationName("VencordLauncher");
	app.setApplicationDisplayName("Vencord Launcher");
	#ifdef Q_OS_WIN32
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.ico"));
	#else // Q_OS_WIN32
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.png"));
	#endif // Q_OS_WIN32

#ifdef Q_OS_WIN
	QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("launcher", &launcher);

	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	engine.load(QUrl(QStringLiteral("qrc:/VencordLauncher/qml/Main.qml")));
	return app.exec();
}
