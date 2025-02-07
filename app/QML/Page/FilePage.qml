import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// 文件测试
Rectangle {
    id: control

    Column {
        anchors.centerIn: parent
        spacing: 20
        Item {
            width: 10
            height: 40
            Text {
                id: file_label
                anchors.centerIn: parent
                text: "-"
            }
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
        Button {
            text: "Select File"
            onClicked: {
                file_dialog.open()
            }
        }
    }

    FileDialog {
        id: file_dialog
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        onAccepted: {
            file_label.text = String("[%1](%2)").arg(
                        AndroidTool.getFileNameFromUri(file_dialog.fileUrl)).arg(
                        file_dialog.fileUrl)
        }
    }
}
