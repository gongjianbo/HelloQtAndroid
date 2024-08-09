import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// Qt Camera测试
Rectangle {
    id: control

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "black"
            VideoOutput {
                id: video_putput
                anchors.fill: parent
                anchors.margins: 1
                source: camera
                autoOrientation: false
                // 横屏旋转
                // orientation: ActivityManager.rotation === 270 ? 180 : 0
                // 四个方向旋转
                orientation: {
                    switch(ActivityManager.rotation) {
                    case 0: return -90
                    case 180: return 90
                    case 270: return 180
                    default: return 0
                    }
                }
            }
        }
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            Column {
                anchors.centerIn: parent
                spacing: 20
                ComboBox {
                    id: device_box
                    width: 160
                }
                Button {
                    text: "Play"
                    onClicked: {
                        let device_index = device_box.currentIndex
                        if (device_index < 0 || device_index >= deviceList.length)
                            return
                        if (!camera) {
                            camera = camera_comp.createObject(control)
                        }
                        camera.deviceId = deviceList[device_index].deviceId
                        camera.start()
                    }
                }
                Button {
                    text: "Stop"
                    onClicked: {
                        if (camera) {
                            camera.stop()
                            camera.destroy()
                            camera = null
                        }
                    }
                }
                Button {
                    text: "Cpture"
                    onClicked: {
                        video_putput.grabToImage(function(result) {
                            console.log("cpature", result.url)
                            camera_frame.source = result.url
                        });
                    }
                }
                Image {
                    id: camera_frame
                    width: 160
                    height: 120
                }
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            ActivityManager.enableRotationListen()
        } else {
            ActivityManager.disableRotationListen()
        }
    }

    Component {
        id: camera_comp
        Camera {
            // 安卓上无法通过camera.supportedViewfinderResolutions()获取分辨率
        }
    }
    property Camera camera: null
    property var deviceList: []
    property var sizeList: []
    Component.onCompleted: {
        deviceList = QtMultimedia.availableCameras
        let name_list = []
        for (let row in deviceList) {
            let device = deviceList[row]
            name_list.push(device.displayName)
        }
        device_box.model = name_list

        if (name_list.length <= 0)
            return
        device_box.currentIndex = 0
    }
}
