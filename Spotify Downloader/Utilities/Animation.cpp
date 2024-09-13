#include "Animation.h"

QMap<QObject*, QVariantAnimation*> Animation::_currentAnimations;

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

void Animation::AnimateBackgroundColour(QWidget* target, QColor newColour, int durationMs) {
    CheckForAnimation(target);

    if (durationMs == 0) {
        QPalette newPalette = target->palette();
        newPalette.setColor(target->backgroundRole(), newColour);
        target->setPalette(newPalette);

        return;
    }

    QVariantAnimation* anim = new QVariantAnimation(target);
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(target->palette().color(target->backgroundRole()));
    anim->setEndValue(newColour);

    QObject::connect(anim, &QVariantAnimation::valueChanged, [=](QVariant value) {
        // Could not get QPalette working, just ended up setting the style sheet, its a bit manual but it works
        target->setStyleSheet(QString("QWidget#%1 {background-color: %2; border: none;}").arg(target->objectName()).arg(value.value<QColor>().name()));
    });

    QObject::connect(anim, &QVariantAnimation::finished, [=] {
        target->setStyleSheet(QString("QWidget#%1 {background-color: %2; border: none;}").arg(target->objectName()).arg(newColour.name()));
        _currentAnimations.remove(target);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);

    _currentAnimations.insert(target, anim);
}