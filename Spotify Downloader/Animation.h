#ifndef ANIMATION_H
#define ANIMATION_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include <QObject>
#include <QWidget>
#include <QMap>

#include <QVariantAnimation>
#include <QPropertyAnimation>

#include <QPoint>
#include <QColor>

class Animation {
	public:
		static void AnimatePosition(QObject* target, QPoint newPos, int durationMs);
		static void AnimateSize(QObject* target, QSize newSize, int durationMs);
		static void AnimateBackgroundColour(QWidget* target, QColor newColour, int durationMs);
	private:
		// Used to handle animations overwriting each other
		static QMap<QObject*, QVariantAnimation*> _currentAnimations;
};

#endif