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

#include "Launcher.hpp"

int main(int argc, char **argv)
{
	// needs this for capturing discords stdout/stderr
	qputenv("QML_XHR_ALLOW_FILE_READ", "1");

	// force software renderer because some people
	// might not have a gpu
	if (qgetenv("QT_QUICK_BACKEND").isEmpty()) {
        qputenv("QT_QUICK_BACKEND", "software");
    }

	QGuiApplication	app(argc, argv);

	app.setOrganizationName("claration");
	app.setOrganizationDomain("clarati.on");
	app.setApplicationName("VencordLauncher");
	app.setApplicationDisplayName("Vencord Launcher 1.0.0");
	app.setApplicationVersion("1.0.0");
#ifdef Q_OS_WIN32
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.ico"));
#else // Q_OS_WIN32
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.png"));
#endif // Q_OS_WIN32

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
	QString launchArg = parser.value(launchArgOpt);
	QString overrideEntry = parser.value(overrideEntryOpt);

	QStringList launch_mod_argv;

	for (int i = 0; i < argc; ++i)
		launch_mod_argv.append(QString::fromLocal8Bit(argv[i]));

	if (!launch.isEmpty())
	{
		Launcher launcher;
		launcher.launch(launch, launchArg, overrideEntry, launch_mod_argv);
		exit(0); // app.exit() is so broken
	}

#ifdef Q_OS_WIN
	QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

	QQmlApplicationEngine engine;

	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	engine.load(QUrl(QStringLiteral("qrc:/VencordLauncher/Main.qml")));
	return app.exec();
}
