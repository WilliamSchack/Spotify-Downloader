#include "Animation.h"

void Animation::CheckForAnimation(QObject* target) {
    if (_currentAnimations.contains(target)) {
        _currentAnimations[target]->stop();
        _currentAnimations[target]->deleteLater();
        _currentAnimations.remove(target);
    }
}

void Animation::AnimatePosition(QObject* target, QPoint newPos, int durationMs) {
    CheckForAnimation(target);
    
    if (durationMs == 0) {
        target->setProperty("pos", newPos);
        return;
    }

    QPropertyAnimation* anim = new QPropertyAnimation(target, "pos");
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(target->property("pos"));
    anim->setEndValue(newPos);

    QObject::connect(anim, &QPropertyAnimation::finished, [=] {
        _currentAnimations.remove(target);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);

    _currentAnimations.insert(target, anim);
}

void Animation::AnimateSize(QObject* target, QSize newSize, int durationMs) {
    CheckForAnimation(target);

    if (durationMs == 0) {
        target->setProperty("size", newSize);
        return;
    }

    QPropertyAnimation* anim = new QPropertyAnimation(target, "size");
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(target->property("size"));
    anim->setEndValue(newSize);

    QObject::connect(anim, &QPropertyAnimation::finished, [=] {
        _currentAnimations.remove(target);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);

    _currentAnimations.insert(target, anim);
}

void Animation::AnimateValue(QWidget* target, int newValue, int durationMs) {
    CheckForAnimation(target);

    if (durationMs == 0) {
        target->setProperty("value", newValue);
        return;
    }

    // Stuttery animations using QPropertyAnimation, using QVariantAnimation instead

    QVariantAnimation* anim = new QVariantAnimation(target);
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(target->property("value"));
    anim->setEndValue(newValue);

    QObject::connect(anim, &QVariantAnimation::valueChanged, [=](QVariant value) {
        target->setProperty("value", value);
        target->update();
    });

    QObject::connect(anim, &QPropertyAnimation::finished, [=] {
        _currentAnimations.remove(target);
        });

    anim->start(QAbstractAnimation::DeleteWhenStopped);

    _currentAnimations.insert(target, anim);
}

void Animation::AnimateStylesheetColour(QWidget* target, const QString& styleSheetKey, QColor newColour, int durationMs, bool addKeyAfterStylesheet) {
    // If already animating this style sheet, cancel the previous animation
    if (_animatingStyleSheet.contains(target) && _animatingStyleSheet[target].contains(styleSheetKey))
        CheckForAnimation(target);

    if (durationMs == 0) {
        QPalette newPalette = target->palette();
        newPalette.setColor(target->backgroundRole(), newColour);
        target->setPalette(newPalette);

        return;
    }

    // Get current colour from the stylesheet
    QString styleSheetKeyFull = QString("%1:").arg(styleSheetKey);
    QString initialStyleSheet = target->styleSheet();
    QColor initialColour = QColor(0, 0, 0, 255);
    if (initialStyleSheet.contains(styleSheetKeyFull)) {
        QRegularExpression regex(QString("%1(.+?);").arg(styleSheetKeyFull));
        QStringList matches = regex.match(initialStyleSheet).capturedTexts();

        // The mighty if mountain
        if (matches.count() >= 2) {
            QString colourString = matches[1].trimmed();

            // Get colour based on hex, rgb, rgba
            if (colourString.startsWith("#")) {
                initialColour.setNamedColor(colourString.trimmed());
            }
            else {
                QRegularExpression rgbRegex("\\((.+?)\\)");
                QStringList rgbMatches = rgbRegex.match(colourString).capturedTexts();

                if (rgbMatches.count() >= 2) {
                    QString rgbString = rgbMatches[1];
                    QStringList colours = rgbString.split(",");

                    initialColour.setRed(colours[0].trimmed().toInt());
                    initialColour.setBlue(colours[1].trimmed().toInt());
                    initialColour.setGreen(colours[2].trimmed().toInt());
                    if (colours.count() >= 4) initialColour.setAlpha(colours[3].trimmed().toInt());
                }

            }
        }
    }

    // Setup the animation
    QVariantAnimation* anim = new QVariantAnimation(target);
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(initialColour);
    anim->setEndValue(newColour);

    QObject::connect(anim, &QVariantAnimation::valueChanged, [=](QVariant value) {
        // Remove the stylesheet key
        QString styleSheet = target->styleSheet();
        if (styleSheet.contains(styleSheetKeyFull)) {
            int startIndex = styleSheet.indexOf(styleSheetKeyFull);
            int endIndex = startIndex + styleSheet.mid(startIndex).indexOf(";") + 1;

            styleSheet = styleSheet.left(startIndex) + styleSheet.mid(endIndex);
        }
        
        // Add the new value
        QColor colour = value.value<QColor>();
        QString rgbaString = QString("rgba(%1, %2, %3, %4)").arg(colour.red()).arg(colour.green()).arg(colour.blue()).arg(colour.alpha());

        QString newValueString = QString("%1 %2;").arg(styleSheetKeyFull).arg(rgbaString);
        if (addKeyAfterStylesheet) styleSheet += newValueString;
        else styleSheet = newValueString + styleSheet;

        target->setStyleSheet(styleSheet);
    });

    QObject::connect(anim, &QVariantAnimation::finished, [=] {
        // Remove the stylesheet key
        QString styleSheet = target->styleSheet();
        if (styleSheet.contains(styleSheetKeyFull)) {
            int startIndex = styleSheet.indexOf(styleSheetKeyFull);
            int endIndex = startIndex + styleSheet.mid(startIndex).indexOf(";") + 1;

            styleSheet = styleSheet.left(startIndex) + styleSheet.mid(endIndex);
        }

        // Add the new value
        QString rgbaString = QString("rgba(%1, %2, %3, %4)").arg(newColour.red()).arg(newColour.green()).arg(newColour.blue()).arg(newColour.alpha());

        QString newValueString = QString("%1 %2;").arg(styleSheetKeyFull).arg(rgbaString);
        if (addKeyAfterStylesheet) styleSheet += newValueString;
        else styleSheet = newValueString + styleSheet;

        target->setStyleSheet(styleSheet);

        _currentAnimations.remove(target);

        _animatingStyleSheet[target].removeAll(styleSheetKey);
        if (_animatingStyleSheet[target].empty())
            _animatingStyleSheet.remove(target);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
    _currentAnimations.insert(target, anim);
    
    // If this object is already animating, add this stylesheet key
    if (_animatingStyleSheet.contains(target)) {
        _animatingStyleSheet[target].append(styleSheetKey);
    }
    // If not already animating, add a new list with this stylesheet key
    else {
        QString keyCopy = QString(styleSheetKey);
        QStringList styleSheetKey({ keyCopy });
        _animatingStyleSheet.insert(target, styleSheetKey);
    }
}