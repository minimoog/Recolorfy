import QtQuick 1.0
import QtMultimediaKit 1.1
import recolorfy.qmlcomponents 1.0

Rectangle {
    id: wrapper
    color: 'white'

    //width: 360; height: 640

    Rectangle {
        id: topBar
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
                    flickableImageEditor.interactive = !flickableImageEditor.interactive
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

        Rectangle {
            id: buttonShowCamera
            width: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left:  buttonUndo.right
            anchors.leftMargin: 30

            MouseArea {
                id: buttonShowCameraMouseArea
                anchors.fill: parent
            }
        }
    }

    Rectangle {
        id: zoomin
        width: 40
        height: 40
        color: "#000000"
        z: 1
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: topBar.bottom
        anchors.topMargin: 10
        visible: false

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
        color: "#000000"
        z: 1
        visible: false
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10

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
        id: flickableImageEditor
        anchors.top: topBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        interactive: false
        clip: true
        visible: false
        contentWidth: colorgrayImage.width * colorgrayImage.scale
        contentHeight: colorgrayImage.height * colorgrayImage.scale

        Item {
            id: imageContainer
            width: Math.max(colorgrayImage.width * colorgrayImage.scale, flickableImageEditor.width)
            height: Math.max(colorgrayImage.height * colorgrayImage.scale, flickableImageEditor.height)

            ColorManipulator {
                id: colorgrayImage
                property real prevScale : Math.min(scale, 1)
                anchors.centerIn: parent
                //source: 'qml/Recolorfy/images/food_test.jpg'

                scale: flickableImageEditor.width / width

                onScaleChanged: {
                    if ((width * scale) > flickableImageEditor.width)
                        flickableImageEditor.contentX += (flickableImageEditor.width / 2 + flickableImageEditor.contentX) * (scale / prevScale - 1);
                    if ((height * scale) > flickableImageEditor.height)
                        flickableImageEditor.contentY += (flickableImageEditor.height / 2 + flickableImageEditor.contentY) * (scale / prevScale - 1);
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
    }

    Camera {
        id: camera

        visible: true
        focus: visible
        anchors.top: topBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onImageSaved: {
            console.log(path)
            colorgrayImage.source = path
        }

        MouseArea {
            id: mouseAreaCamera

            anchors.fill: parent

            onClicked: {
                camera.captureImage()

                // ### TODO: add to state
                camera.visible = false
                camera.focus = false
                flickableImageEditor.visible = true
            }
        }
    }
}
