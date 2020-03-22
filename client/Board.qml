import QtQuick 2.12
import QtQuick.Layouts 1.12

Item {
    id: board

    property real fieldWidth: board.width / (client.board.fieldCount / 4 + 1)
    property real fieldHeight: board.width / (client.board.fieldCount / 4 + 1)

    function fieldPosition(index) {
        if (index <= client.board.fieldCount / 4)
            return Qt.point(
                index * fieldWidth,
                0
            )
        else if (index <= 2 * client.board.fieldCount / 4)
            return Qt.point(
                (client.board.fieldCount / 4) * fieldWidth,
                ((index % (client.board.fieldCount / 4 + 1)) + 1) * fieldHeight
            )
        else if (index <= 3 * client.board.fieldCount / 4)
            return Qt.point(
                ((client.board.fieldCount / 4) - ((index - 1) % (client.board.fieldCount / 4)) - 1) * fieldWidth,
                (client.board.fieldCount / 4 ) * fieldHeight
            )
        else
            return Qt.point(
                0,
                (client.board.fieldCount / 4 - ((index - 1) % (client.board.fieldCount / 4)) - 1) * fieldHeight
            )
    }

    Repeater {
        model: client.board.fields
        delegate: Field {

        }
    }
}
