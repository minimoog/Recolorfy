import QtQuick 1.0
import recolorfy.qmlcomponents 1.0

Item {
    id: wrapper

    //width: 360; height: 640

    Rectangle {
        id: topBar
        z: 1
        height: 80
        color: "#808080"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        Rectangle {
            id: buttonPanZoom
            width: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10

            MouseArea {
                id: panZoomMouseArea
                anchors.fill: parent

                onClicked: {
                    flickable.interactive = !flickable.interactive
                    mouseAreaFlickableImage.enabled = !mouseAreaFlickableImage.enabled
                    zoomin.visible = !zoomin.visible
                    zoomout.visible = !zoomout.visible
                }
            }
        }

        Rectangle {
            id: buttonUndo
            width: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: buttonPanZoom.right
            anchors.leftMargin: 30

            MouseArea {
                id: buttonUndoMouseArea
                anchors.fill: parent

                onClicked: colorgrayImage.undo()
            }
        }
    }

    Rectangle {
        id: zoomin
        width: 40
        height: 40
        color: "#ffffff"
        visible: false
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: topBar.bottom
        anchors.topMargin: 10
        z: 1

        MouseArea {
            id: zoominMouseArea
            anchors.fill: parent

            onPressed: zoominRepeater.start()
            onReleased: zoominRepeater.stop()

            Timer {
                id: zoominRepeater
                interval: 1
                repeat: true
                triggeredOnStart: true

                onTriggered: colorgrayImage.scale += 0.01
            }
        }
    }

    Rectangle {
        id: zoomout
        width: 40
        height: 40
        color: "#ffffff"
        visible: false
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        z: 1

        MouseArea {
            id: zoomoutMouseArea
            anchors.fill: parent

            onPressed: zoomoutRepeater.start()
            onReleased: zoomoutRepeater.stop()

            Timer {
                id: zoomoutRepeater
                interval: 1
                repeat: true
                triggeredOnStart: true

                onTriggered: colorgrayImage.scale -= 0.01
            }
        }
    }

    Flickable {
        id: flickable
        z: 0
        anchors.top: topBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        interactive: false
        clip: true
        contentWidth: colorgrayImage.width * colorgrayImage.scale
        contentHeight: colorgrayImage.height * colorgrayImage.scale

        Item {
            id: imageContainer
            width: Math.max(colorgrayImage.width * colorgrayImage.scale, flickable.width)
            height: Math.max(colorgrayImage.height * colorgrayImage.scale, flickable.height)

            ColorManipulator {
                id: colorgrayImage
                property real prevScale : Math.min(scale, 1)
                anchors.centerIn: parent
                source: 'qml/Recolorfy/images/food_test.jpg'

                scale: flickable.width / width

                onScaleChanged: {
                    if ((width * scale) > flickable.width)
                        flickable.contentX += (flickable.width / 2 + flickable.contentX) * (scale / prevScale - 1);
                    if ((height * scale) > flickable.height)
                        flickable.contentY += (flickable.height / 2 + flickable.contentY) * (scale / prevScale - 1);
                    prevScale = scale
                }

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

//        Image {
//            id: colorgrayImage
//            source: 'images/food_test.jpg'
//            transformOrigin: Item.TopLeft
//        }

    }
}
