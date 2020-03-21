import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

Item {
    ListModel {
        id: rosterModel
        ListElement { password: false; name: "Mock hra 1"; owner: "Bizon"; players: 3; maximumPlayers: 8 }
        ListElement { password: true; name: "Mock hra 2"; owner: "Whoever"; players: 1; maximumPlayers: 4 }
        ListElement { password: false; name: "Yellow fever"; owner: "syyyr"; players: 1; maximumPlayers: 2 }
    }

    Button {
        text: "⚙️"
        width: height
        anchors {
            right: parent.right
            top: parent.top
            margins: 6
        }
    }

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width / 3 * 2
        height: parent.height
        spacing: 9

        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: "Karantény a dostihy"
            font.pixelSize: 32
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Text {
                text: "Tvoje jméno:"
            }
            TextField {
                text: "Guest_49A3F131"
            }
            CheckBox {
                text: "Máš kostku?"
            }
        }

        ListView {
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            header: Item {
                width: parent.width
                height: rosterHeaderLayout.height + 1
                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "black"
                    height: 1
                }

                RowLayout {
                    id: rosterHeaderLayout
                    width: parent.width
                    Text {
                        Layout.fillWidth: true
                        text: "Jméno hry"
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                    }
                    Text {
                        Layout.preferredWidth: 96
                        text: "Založil"
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                    }
                    Text {
                        Layout.preferredWidth: 64
                        text: "Hráči"
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                    }
                }
            }

            model: rosterModel
            delegate: Rectangle {
                width: parent.width
                height: rosterItemLayout.height + 7

                color: rosterItemMouse.pressed ? "#ffdddddd" : rosterItemMouse.containsMouse ? "#ffeeeeee" : "#00ffffff"
                Behavior on color { ColorAnimation { duration: 100 } }

                RowLayout {
                    y: 3
                    id: rosterItemLayout
                    width: parent.width
                    Text {
                        Layout.fillWidth: true
                        text: (password ? "🔒 " : "") + name
                    }
                    Text {
                        Layout.preferredWidth: 96
                        text: owner
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        Layout.preferredWidth: 64
                        text: players + "/" + maximumPlayers
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                MouseArea {
                    id: rosterItemMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (password) {
                            passwordDialog.visible = true
                        }
                        else {

                        }
                    }
                }
                Dialog {
                    id: passwordDialog
                    function accept() {
                        visible = false
                    }
                    function reject() {
                        visible = false
                    }
                    ColumnLayout {
                        anchors.fill: parent
                        onVisibleChanged: {
                            if (visible) {
                                passwordTextField.text = ""
                                passwordTextField.forceActiveFocus()
                            }
                        }
                        Text {
                            text: "Zadej heslo ke hře:"
                        }
                        TextField {
                            id: passwordTextField
                            focus: true
                            onAccepted: passwordDialog.accept()
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                Layout.fillWidth: true
                                text: "Zrušit"
                                onClicked: passwordDialog.reject()
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "OK"
                                onClicked: passwordDialog.accept()
                            }
                        }
                    }
                }
            }
        }
    }
}
