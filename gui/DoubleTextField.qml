import QtQuick

CustomTextField {
    id: root

    property alias min: validatorId.bottom
    property alias max: validatorId.top
    property alias decimals: validatorId.decimals

    validator: DoubleValidator {
        id: validatorId
        bottom: -50.0
        top: 50.0
        decimals: 1
        notation: DoubleValidator.StandardNotation
    }
}