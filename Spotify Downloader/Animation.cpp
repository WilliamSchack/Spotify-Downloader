#include "Animation.h"

QMap<QObject*, QVariantAnimation*> Animation::_currentAnimations;

void Animation::AnimatePosition(QObject* target, QPoint newPos, int duration) {
    if (_currentAnimations.contains(target)) {
        _currentAnimations[target]->deleteLater();
        _currentAnimations.remove(target);
    }

    QPropertyAnimation* anim = new QPropertyAnimation(target, "pos");
    anim->setDuration(duration);
    anim->setEasingCurve(QEasingCurve::Type::OutQuart);
    anim->setStartValue(target->property("pos"));
    anim->setEndValue(newPos);

    QObject::connect(anim, &QPropertyAnimation::finished, [=] {
        _currentAnimations.remove(target);
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);

    _currentAnimations.insert(target, anim);
}

void Animation::AnimateBackgroundColour(QWidget* target, QColor newColour, int duration) {
    if (_currentAnimations.contains(target)) {
        _currentAnimations[target]->deleteLater();
        _currentAnimations.remove(target);
    }

    QVariantAnimation* anim = new QVariantAnimation(target);
    anim->setDuration(duration);
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