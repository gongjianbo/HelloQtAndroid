import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// 文件测试
Rectangle {
    id: control

    Column {
        anchors.centerIn: parent
        spacing: 20
        Text {
            id: file_label
            text: "-"
        }
        Button {
            text: "Read"
            onClicked: {
                file_label.text = FileTool.readTest()
            }
        }
        Button {
            text: "Write"
            onClicked: {
                file_label.text = FileTool.writeTest()
            }
        }
        Button {
            text: "Share File"
            onClicked: {
                FileTool.shareFileTest()
            }
        }
        Button {
            text: "Share Files"
            onClicked: {
                FileTool.shareFilesTest()
            }
        }
    }
}
