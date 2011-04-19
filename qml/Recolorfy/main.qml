import QtQuick 1.0
import recolorfy.qmlcomponents 1.0

Item {
    id: wrapper

    Flickable {
        id: flickableImage
        anchors.fill: parent
        contentWidth: colorgrayImage.width * colorgrayImage.scale
        contentHeight: colorgrayImage.height * colorgrayImage.scale

        ColorManipulator {
            id: colorgrayImage
            source: 'qml/Recolorfy/images/food_test.jpg'
            transformOrigin: Item.TopLeft
        }

        MouseArea {
            id: mousearea
            anchors.fill: parent

            onClicked: colorgrayImage.click(mouseX, mouseY)
        }
    }
}
