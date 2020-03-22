import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
    id: errorDialog
    modal: true
    Connections {
        target: client
        onServerError: {
            errorMessage.text = message
            errorDialog.visible = true
        }
    }

    ColumnLayout {
        id: errorLayout
        spacing: 9
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Error"
            font.bold: true
        }
        Text {
            id: errorMessage
        }
    }
}
