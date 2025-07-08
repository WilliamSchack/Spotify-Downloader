// Translation from github.com/Strvm/musicxmatch-api
// Only contains things relevant to this project

#ifndef MUSIXMATCHAPI_H
#define MUSIXMATCHAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network/Network.h"

#include <stdio.h>

#include <QRegularExpression>

#include <QMessageAuthenticationCode>

class MusixmatchAPI {
	private:
		const QString BASE_URL = "https://www.musixmatch.com/ws/1.1/";

		static inline QString _latestAppURLCache;
		static inline QByteArray _secretCache;
	private:
		static QString GetLatestAppURL();
		static QByteArray GetSecret();
		static QString GenerateSignature(QString url);
};

#endif