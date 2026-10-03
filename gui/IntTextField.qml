import QtQuick

CustomTextField {
    id: root

    property alias min: validatorId.bottom
    property alias max: validatorId.top

    validator: IntValidator {
        id: validatorId
        bottom: -50.0
        top: 50.0
    }
}