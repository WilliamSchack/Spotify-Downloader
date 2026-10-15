import QtQuick

CustomTextField {
    id: root

    property int min: -50
    property int max: 50

    // Use this to bind a value rather than to text, that will unbind when validating
    property var boundValue: undefined

    // function(int), must return int. It is assumed this is set correctly
    property var extraValidation: null

    signal valueChanged(int value)

    validator: IntValidator {
        id: validatorId
        bottom: root.min
        top: root.max
    }

    function validateText() {
        var value = parseInt(text)
        if (isNaN(value))
            value = Math.max(min, Math.min(max, 0))

        if (extraValidation != null)
            value = extraValidation(value)

        value = Math.max(min, Math.min(max, value))

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