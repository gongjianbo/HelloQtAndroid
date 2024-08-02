import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

import "./Page"

ApplicationWindow {
    id: main_window
    width: 720
    height: 1280
    visible: true
    title: qsTr("Hello Qt Android")

    // 屏蔽 close 关闭，accepted = false 则不处理该操作
    onClosing: function(event) {
        console.log("main_window close")
        event.accepted = false
    }
    MouseArea {
        anchors.fill: parent
        onPressed: {
            console.log("main_window onPressed")
        }
        onReleased: {
            console.log("main_window onReleased")
        }
        onClicked: {
            console.log("main_window onClicked")
        }
    }

    header: Rectangle {
        color: "gray"
        width: main_window.width
        height: 40
        Row {
            anchors.fill: parent
            leftPadding: 20
            spacing: 20
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Hello Qt Android"
            }
            ComboBox {
                id: page_box
                width: 200
                height: 40
                model: [
                    "Popup"
                    ,"JNI"
                    ,"File"
                    ,"USB"
                ]
            }
        }
    }

    StackLayout {
        id: page_stack
        anchors.fill: parent
        currentIndex: page_box.currentIndex
        PopupPage {}
        JNIPage {}
        FilePage {}
        USBPage {}
    }
}
