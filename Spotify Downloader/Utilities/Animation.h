#ifndef ANIMATION_H
#define ANIMATION_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include <QObject>
#include <QWidget>
#include <QMap>

#include <QRegularExpression>

#include <QVariantAnimation>
#include <QPropertyAnimation>

#include <QPoint>
#include <QColor>

class Animation {
	public:
		static void AnimatePosition(QObject* target, QPoint newPos, int durationMs);
		static void AnimateSize(QObject* target, QSize newSize, int durationMs);
		static void AnimateValue(QWidget* target, int newValue, int durationMs);
		static void AnimateStylesheetColour(QWidget* target, const QString& styleSheetKey, QColor newColour, int durationMs);
	private:
		// Used to handle animations overwriting each other
		static inline QMap<QObject*, QVariantAnimation*> _currentAnimations;
		static inline QMap<QObject*, QStringList> _animatingStyleSheet; // Allowed multiple animations
	private:
		static void CheckForAnimation(QObject* target);
};

#endif