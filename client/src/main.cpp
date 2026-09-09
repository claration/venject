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

#include "launch.hpp"

void print_usage(char *argv)
{
	printf(
		"Usage: %s [OPTIONS]\n\n"
		"  -h, --help       Print usage information\n"
		"  -v, --version    Print version\n"
		"\nOptions:\n\n"
		"  -l, --launch <FILE PATH>              Launch a Discord executable\n"
		"  -a, --launch-arg <ARG>                Override launch arguments\n"
		"  -o, --override_entry <FILE PATH>      Override Vencord patcher\n",
		argv);
}

void parse_arguments(int argc, char *argv[])
{
	static struct option long_options[] = {
		{"help", no_argument, nullptr, 'h'},
		{"version", no_argument, nullptr, 'v'},
		{"launch", required_argument, nullptr, 'l'},
		{"launch-arg", required_argument, nullptr, 'a'},
		{"override_entrypoint", required_argument, nullptr, 'o'},
		{nullptr, 0, nullptr, 0}};

	const char *launch_path = nullptr;
	QStringList launch_args;

	int option;
	int option_index = 0;

	while ((option = getopt_long(argc, argv, "hvl:o:a:", long_options, &option_index)) != -1) {
		switch (option) {
			case 'h':
				print_usage(argv[0]);
				exit(0);
			case 'v':
				printf("%s\n", argv[0]);
				printf("%s (%s)\n", GIT_HASH, GIT_BRANCH);
				printf("Git: %s\n", GIT_URL);
				printf("Compiled on %s at %s\n", __DATE__, __TIME__);
				exit(0);
			case 'l':
				launch_path = optarg;
				break;
			case 'a':
				launch_args.append(QString::fromLocal8Bit(optarg));
				break;
			case 'o':
				printf("Not yet implemented.");
				exit(1);
				break;
			default:
				fprintf(stderr, "Unknown option\n");
				print_usage(argv[0]);
				exit(1);
		}
	}

	if (launch_path) {
		QStringList launch_mod_argv;

		for (int i = 0; i < argc; ++i)
			launch_mod_argv.append(QString::fromLocal8Bit(argv[i]));

		Launcher launcher;
		launcher.launch(
			QString::fromLocal8Bit(launch_path),
			launch_args,
			launch_mod_argv);

		exit(0);
	}
}

int main(int argc, char **argv)
{
	parse_arguments(argc, argv);

	// force software renderer
	if (getenv("QT_QUICK_BACKEND") == nullptr) {
#ifdef _WIN32
		_putenv_s("QT_QUICK_BACKEND", "software");
#else  // _WIN32
		setenv("QT_QUICK_BACKEND", "software", 1);
#endif // _WIN32
	}

	QGuiApplication		  app(argc, argv);
	QQmlApplicationEngine engine;

	Launcher launcher;
	engine.rootContext()->setContextProperty("launcher", &launcher);

	app.setWindowIcon(QIcon(":/VencordLauncher/resources/icon.ico"));

	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreationFailed,
		&app,
		[]() { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	engine.load(QUrl(QStringLiteral("qrc:/VencordLauncher/qml/Main.qml")));
	return app.exec();
}
