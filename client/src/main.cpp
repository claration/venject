#include <QCommandLineParser>
#include <QCoreApplication>
#include <QApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

#include "DiscordInstall.hpp"
#include "LaunchInstance.hpp"

int main(int argc, char **argv)
{
	// needs this for capturing discords stdout/stderr
	qputenv("QML_XHR_ALLOW_FILE_READ", "1");

	// force software renderer because some people
	// might not have a gpu
	if (qEnvironmentVariableIsEmpty("QT_QUICK_BACKEND")) {
		qputenv("QT_QUICK_BACKEND", "software");
	}

	QGuiApplication app(argc, argv);
	app.setOrganizationName("VencordLauncher");
	app.setOrganizationDomain("clarati.on");
	app.setApplicationName("VencordLauncher");
	app.setApplicationDisplayName("Vencord Launcher " VERSION);
	app.setApplicationVersion(VERSION);
#ifdef Q_OS_WIN32
	QQuickStyle::setStyle("FluentWinUI3");
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.ico"));
#else  // Q_OS_WIN32
	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.png"));
#endif // Q_OS_WIN32

	QCommandLineParser parser;
	parser.setApplicationDescription("Vencord Launcher");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption launchIdOpt(QStringList{"i", "id"}, "Launch an existing instance", "ID");
	parser.addOption(launchIdOpt);
	parser.process(app);

	QString launchId = parser.value(launchIdOpt);
	QStringList launch_mod_argv;

	for (int i = 0; i < argc; ++i)
		launch_mod_argv.append(QString::fromLocal8Bit(argv[i]));

#ifdef Q_OS_WIN
	QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

	if (!launchId.isEmpty()) {
		LaunchInstance::launchById(launchId, launch_mod_argv);
		return 0;
	}

	QQmlApplicationEngine engine;

	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	engine.load(QUrl(QStringLiteral("qrc:/VencordLauncher/MainWindow.qml")));
	return app.exec();
}
