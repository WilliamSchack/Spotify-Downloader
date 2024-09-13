// Derived from VelazcoJD's logger at https://github.com/VelazcoJD/QtLogging

#ifndef CRASHLOG_H
#define CRASHLOG_H

#include <QApplication>

#include <QDebug>
#include <QFile>
#include <QHash>

#include <QDateTime>
#include <QDir>
#include <QObject>

class Logger {
	public:
		static void init();
		static void clean();
		static void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
		static void Flush();
	private:
		static QFile* _logFile;
		static bool _isInit;
		static QHash<QtMsgType, QString> _contextNames;
};

#endif