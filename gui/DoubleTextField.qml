import QtQuick

CustomTextField {
    id: root

    property real min: -50.0
    property real max: 50.0
    property int decimals: 1

    // function(real), must return real. It is assumed this is set correctly
    property var extraValidation: null

    // Use this instead of onTextEdited since that doesnt validate properly with IntValidator
    signal valueChanged(int value)

    validator: DoubleValidator {
        id: validatorId
        bottom: root.min
        top: root.max
        decimals: root.decimals
        notation: DoubleValidator.StandardNotation
    }

    function validateText() {
        var value = parseFloat(text)
        if (isNaN(value))
            value = Math.max(min, Math.min(max, 0))

        if (extraValidation != null)
            value = extraValidation(value)

        value = Math.max(min, Math.min(max, value))
        value = value.toFixed(decimals)

        text = value
        valueChanged(value)
    }

    onActiveFocusChanged: {
        if (activeFocus)
            return;

        validateText()
    }

    Keys.onReturnPressed: validateText()
    Keys.onEnterPressed:  validateText()
}