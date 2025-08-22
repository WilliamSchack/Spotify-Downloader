// Derived from VelazcoJD's logger at https://github.com/VelazcoJD/QtLogging

#include "Logger.h"

QFile* Logger::_logFile = Q_NULLPTR;
bool Logger::_isInit = false;
QHash<QtMsgType, QString> Logger::_contextNames = {
	{QtMsgType::QtDebugMsg,		" Debug  "},
	{QtMsgType::QtInfoMsg,		"  Info  "},
	{QtMsgType::QtWarningMsg,	"Warning "},
	{QtMsgType::QtCriticalMsg,	"Critical"},
	{QtMsgType::QtFatalMsg,		" Fatal  "}
};

QtMessageHandler originalHandler = nullptr;
QString currentLog = QString();

void Logger::init() {
	if (_isInit) {
		return;
	}

	// Get log path
	QString tempDir = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	if (!QDir(tempDir).exists())
		QDir().mkdir(tempDir);

	QString logPath = QString("%1/Log.log").arg(tempDir);

	// Backup previous log file
	if (QFile::exists(logPath)) {
		QString previousLogPath = QString("%1/PreviousLog.log").arg(tempDir);
		if (QFile::exists(previousLogPath))
			QFile::remove(previousLogPath);

		QFile::rename(logPath, QString("%1/PreviousLog.log").arg(tempDir));
	}

	// Create log file
	_logFile = new QFile;
	_logFile->setFileName(logPath);
	_logFile->open(QIODevice::Append | QIODevice::Text);

	// Redirect logs to messageOutput
	originalHandler = qInstallMessageHandler(Logger::messageOutput);

	// Clear file contents
	_logFile->resize(0);

	Logger::_isInit = true;
}

void Logger::clean() {
	if (_logFile != Q_NULLPTR) {
		_logFile->close();
		delete _logFile;
	}
}

void Logger::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
	if(originalHandler)
		originalHandler(type, context, msg);

	QString log = QObject::tr("%1 | %2 | %3 | %4 | %5 | %6\n").
		arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss")).
		arg(Logger::_contextNames.value(type)).
		arg(context.line).
		arg(QString(context.file).
			section('\\', -1)).			// File name without file path											
		arg(QString(context.function).
			section('(', -2, -2).		// Function name only
			section(' ', -1).
			section(':', -1)).
		arg(msg);

	// Manually flush, can cause issues with multiple threads accessing it at the same time otherwise
	try {
		currentLog.append(log);
	}
	catch (...) {
		// Rare error that the log cannot be added to the cache, I have only seen it once but it can happen, no clue why so handle it here
		return;
	}
}

void Logger::Flush() {
	_logFile->write(currentLog.toLocal8Bit());
	_logFile->flush();

	currentLog.clear();
}