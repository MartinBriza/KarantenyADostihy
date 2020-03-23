import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import monopoly 1.0

Dialog {
    modal: true
    property variant model: null
    function show(h, m) {
        header.text = h
        message.text = m
        visible = true
    }
    ColumnLayout {
        spacing: 16
        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            id: header
            font.pixelSize: 32
            font.bold: true
        }
        Text {
            Layout.maximumWidth: 320
            id: message
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
