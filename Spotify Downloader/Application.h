#ifndef APPLICATION_H
#define APPLICATION_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include <QApplication>

class Application : public QApplication {
	Q_OBJECT

	public:
		explicit Application(int& argc, char** argv);
	private:
		bool notify(QObject* receiver, QEvent* event);
};

#endif