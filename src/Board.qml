import QtQuick 2.13
import QtQuick.Layouts 1.12

Item {
    id: board

    property real fieldWidth: board.width / (game.board.fieldCount / 4 + 1)
    property real fieldHeight: board.width / (game.board.fieldCount / 4 + 1)

    function fieldPosition(index) {
        if (index <= game.board.fieldCount / 4)
            return Qt.point(
                index * fieldWidth,
                0
            )
        else if (index <= 2 * game.board.fieldCount / 4)
            return Qt.point(
                (game.board.fieldCount / 4) * fieldWidth,
                ((index % (game.board.fieldCount / 4 + 1)) + 1) * fieldHeight
            )
        else if (index <= 3 * game.board.fieldCount / 4)
            return Qt.point(
                ((game.board.fieldCount / 4) - ((index - 1) % (game.board.fieldCount / 4)) - 1) * fieldWidth,
                (game.board.fieldCount / 4 ) * fieldHeight
            )
        else
            return Qt.point(
                0,
                (game.board.fieldCount / 4 - ((index - 1) % (game.board.fieldCount / 4)) - 1) * fieldHeight
            )
    }

    Repeater {
        model: game.board.fields
        delegate: Rectangle {
            clip: true
            color: modelData.color
            x: fieldPosition(index).x
            y: fieldPosition(index).y

            width: fieldWidth
            height: fieldHeight
            border.color: "black"
            border.width: 1
            ColumnLayout {
                anchors.fill: parent
                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    text: modelData.name
                }
                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: modelData.price > 0 ? modelData.price : ""
                }
                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: modelData.fee > 0 ? modelData.fee : ""
                }
            }
        }
    }
}
