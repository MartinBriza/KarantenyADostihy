import QtQuick 2.12

Rectangle {
    property var model

    color: model.color
    border {
        width: 1
        color: "black"
    }

    width: 24
    height: width
    radius: width / 2

    Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
}
