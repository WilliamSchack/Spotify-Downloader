import QtQuick

CustomTextField {
    id: root

    property int min: -50
    property int max: 50

    // function(int), must return int. It is assumed this is set correctly
    property var extraValidation: null

    // Use this instead of onTextEdited since that doesnt validate properly with IntValidator
    signal valueChanged(int value)

    validator: IntValidator {
        id: validatorId
        bottom: root.min
        top: root.max
    }

    function validateText() {
        var value = parseInt(text)
        if (isNaN(value))
            value = 0 < min ? min : 0

        if (extraValidation != null)
            value = extraValidation(value)

        value = Math.max(min, Math.min(max, value))

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