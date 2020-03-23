import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import monopoly 1.0

Dialog {
    modal: true
    property variant model: null
    function show(header, message) {
        h.text = header
        m.text = message
        visible = true
    }
    ColumnLayout {
        visible: model
        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: "red"
        }
        Text {
            text: "AHOJ"
        }
        Text {
            id: h
        }
        Text {
            id: m
        }
    }
}
