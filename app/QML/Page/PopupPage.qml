import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// 弹框测试
Rectangle {
    id: control

    Column {
        anchors.centerIn: parent
        Button {
            text: "Popup"
            onClicked: {
                my_pop.open()
            }
        }
    }

    // 叠加 Dialog 时，内部 Dialog 需要 parent 可见才能显示
    Dialog {
        id: my_pop
        anchors.centerIn: Overlay.overlay
        width: 200
        height: 200
        modal: true
        padding: 0
        // closePolicy 会影响 ESC 响应以及安卓返回处理
        // 默认 Popup.CloseOnEscape | Popup.CloseOnPressOutside
        closePolicy: Dialog.CloseOnEscape | Popup.CloseOnReleaseOutside
        Rectangle {
            anchors.fill: parent
            color: "red"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    my_pop.close()
                }
            }
        }
    }
}
