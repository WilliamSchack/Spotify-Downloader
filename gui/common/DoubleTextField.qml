import QtQuick

CustomTextField {
    id: root

    property real min: -50.0
    property real max: 50.0
    property int decimals: 1

    // Use this to bind a value rather than to text, that will unbind when validating
    property var boundValue: undefined

    // function(real), must return real. It is assumed this is set correctly
    property var extraValidation: null

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

    Binding {
        target: root
        property: "text"
        value: root.boundValue
        when: root.boundValue !== undefined && !root.activeFocus
        restoreMode: Binding.RestoreNone
    }

    onActiveFocusChanged: {
        if (activeFocus)
            return;

        validateText()
    }

    Keys.onReturnPressed: validateText()
    Keys.onEnterPressed:  validateText()
}