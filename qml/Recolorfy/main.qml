import QtQuick 1.0
import recolorfy.qmlcomponents 1.0

Item {
    id: wrapper

    Rectangle {
        id: topBar
        z: 1
        height: 40
        color: "#808080"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        Rectangle {
            id: buttonPanZoom
            width: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5

            MouseArea {
                id: panZoomMouseArea
                anchors.fill: parent

                onClicked: {
                    flickableImage.interactive = !flickableImage.interactive
                    mouseAreaFlickableImage.enabled = !mouseAreaFlickableImage.enabled
                }
            }
        }
    }

    Flickable {
        id: flickableImage
        z: 0
        anchors.top: topBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        interactive: false
        contentWidth: colorgrayImage.width * colorgrayImage.scale
        contentHeight: colorgrayImage.height * colorgrayImage.scale

        ColorManipulator {
            id: colorgrayImage
            source: 'qml/Recolorfy/images/food_test.jpg'
            transformOrigin: Item.TopLeft
        }

//        Image {
//            id: colorgrayImage
//            source: 'images/food_test.jpg'
//            transformOrigin: Item.TopLeft
//        }

        MouseArea {
            id: mouseAreaFlickableImage
            anchors.fill: parent

            onPressed: clickRepeater.start()
            onReleased: clickRepeater.stop()

            Timer {
                id: clickRepeater
                interval: 1
                repeat: true
                triggeredOnStart: true

                onTriggered: colorgrayImage.click(mouseAreaFlickableImage.mouseX, mouseAreaFlickableImage.mouseY)
            }
        }
    }
}
