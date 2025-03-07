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
            // 视频渲染
            VideoOutput {
                id: camera_output
                visible: false
                // 保持原始大小方便截图
                // anchors.fill: parent
                // anchors.margins: 1
                source: camera
                filters: [
                    CameraFilter {
                        id: camera_filter
                        source: camera
                        onCaptureFinished: function(url) {
                            camera_frame.source = ""
                            camera_frame.source = url
                        }
                    }
                ]
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
            // 滤镜
            ShaderEffect {
                id: camera_effect
                property var source: ShaderEffectSource {
                    sourceItem: camera_output
                    live: true
                }
                visible: false
                anchors.fill: camera_output
                // 忽略了qt_Opacity组件透明度
                // 部分环境着色器不支持precision highp float;但是需要设置变量精度
                fragmentShader: "
                    varying mediump vec2 qt_TexCoord0;
                    // uniform highp float qt_Opacity;
                    uniform lowp sampler2D source;
                    void main() {
                        highp vec4 color = texture2D(source, qt_TexCoord0);
                        highp float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
                        gl_FragColor = qt_TexCoord0.x > 0.5 ? vec4(gray, 0.0, 0.0, color.a) : color;
                    }"
            }
            // 实际视图
            Item {
                anchors.fill: parent
                anchors.margins: 1
                ShaderEffectSource {
                    id: camera_view
                    // 如果原图宽高比更大，就以Item宽度来拉伸，否则以高度拉伸，使之保持原始比例
                    property bool byHor: (parent.width / parent.height) < (camera_output.width / camera_output.height)
                    width: parseInt(byHor ? parent.width : ((camera_output.width / camera_output.height) * parent.height))
                    height: parseInt(byHor ? (parent.width / (camera_output.width / camera_output.height)) : parent.height)
                    anchors.centerIn: parent
                    sourceItem: camera_effect
                    live: true
                }
            }
        }
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            Column {
                anchors.centerIn: parent
                spacing: 10
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
                    text: "ImageCpture"
                    onClicked: {
                        // imageCapture捕获的图像可能和VideoOutput的方向不一致，而且没法叠加滤镜
                        if (camera) {
                            camera.imageCapture.captureToLocation(AppTool.appDataPath() + "/capture_temp.jpg")
                        }
                    }
                }
                Button {
                    text: "VideoProbe"
                    onClicked: {
                        camera_probe.capture()
                    }
                    CameraProbe {
                        id: camera_probe
                        source: camera
                        onCaptureFinished: function(url) {
                            camera_frame.source = ""
                            camera_frame.source = url
                        }
                    }
                    Text {
                        anchors.left: parent.right
                        anchors.leftMargin: 10
                        text: camera_probe.fps
                    }
                }
                Button {
                    text: "OutputFilter"
                    onClicked: {
                        camera_filter.capture()
                    }
                    Text {
                        anchors.left: parent.right
                        anchors.leftMargin: 10
                        text: camera_filter.fps
                    }
                }
                Button {
                    text: "grabToImage"
                    onClicked: {
                        // grabToImage截图出来的图像尺寸是Item的，所以需要一个原始尺寸的Item
                        camera_effect.grabToImage(function(result) {
                            console.log("cpature", camera_output.width, camera_output.height, result.url)
                            AppTool.imageInfo(result.image)
                            camera_frame.source = ""
                            camera_frame.source = result.url
                        });
                    }
                }
                Rectangle {
                    width: 160
                    height: 120
                    border.color: "black"
                    Image {
                        id: camera_frame
                        anchors.fill: parent
                        anchors.margins: 1
                        fillMode: Image.PreserveAspectFit
                    }
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
            // 调用imageCapture.capture()默认保存到相册
            // 调用imageCapture.captureToLocation()可以设置保存路径，需要先创建对应文件夹
            imageCapture {
                onCaptureFailed: function(requestId, message) {
                    console.log("capture failed", message)
                }
                onImageCaptured: function(requestId, preview) {
                    console.log("image captured", preview)
                    camera_frame.source = ""
                    camera_frame.source = preview
                }
                onImageSaved: function(requestId, path) {
                    console.log("image saved", path)
                }
            }
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
