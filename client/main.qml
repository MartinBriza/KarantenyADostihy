import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import monopoly 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Karanteny")

    Roster {
        visible: client.state === ClientData.ROSTER
        anchors.fill: parent
    }
    Lobby {
        visible: client.state === ClientData.LOBBY
        anchors.fill: parent
    }
    Game {
        visible: client.state === ClientData.GAME
        anchors.fill: parent
    }

    ErrorView {
        anchors.centerIn: parent
    }

    Component.onCompleted: console.warn(client.state)
}
