import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
    modal: true
    GridLayout {
        columns: 2
        Text {
            text: "Host"
        }
        TextField {
            text: client.server
            onAccepted: client.server = text
            onFocusChanged: {
                if (!focus && acceptableInput) {
                    client.server = text
                }
            }
        }

        Text {
            text: "Port"
        }
        TextField{
            text: client.port
            validator: IntValidator {
                bottom: 1
                top: 65535
            }
            onAccepted: client.port = text
            onFocusChanged: {
                if (!focus && acceptableInput) {
                    client.port = text
                }
            }
        }
    }
}
