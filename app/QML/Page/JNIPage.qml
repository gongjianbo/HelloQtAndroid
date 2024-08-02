import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// JNI测试
Rectangle {
    id: control

    Column {
        anchors.centerIn: parent
        spacing: 20
        Button {
            text: "Restart"
            onClicked: {
                AndroidTool.restartApp()
            }
        }
        Button {
            text: "Home"
            onClicked: {
                AndroidTool.gotoHome()
            }
        }
    }
}
