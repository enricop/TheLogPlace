import QtQuick 2.9
import QtQuick.Controls 2.2

import TheLogPlace 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 640
    title: qsTr("TheLogPlace")

    header: Rectangle {
        Button {
            text: "start"
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            title: "SyslogClient"
            width: swipeView.width
            height: swipeView.height
            header: Rectangle {
                TextField {
                    id: searchBox

                    placeholderText: "Search..."
                    inputMethodHints: Qt.ImhNoPredictiveText

                    //width: window.width / 5 * 3
                    anchors.right: parent.right
                    //anchors.verticalCenter: parent.verticalCenter
                }
            }

            ScrollView {
                anchors.fill: parent

                ListView {
                    focus: true
                    clip: true
                    header: Row {
                        id: banner
                        //width: parent.width
                        height: 50
                        //gradient: clubcolors
                        //border {color: "#9EDDF2"; width: 2}
                        Text {
                            //anchors.centerIn: parent
                            text: "Process"
                            font.pixelSize: 20
                            width: 150
                        }
                        Text {
                            //anchors.centerIn: parent
                            text: "Message"
                            font.pixelSize: 20
                            width: 40
                        }
                    }
                    width: parent.width
                    model: LogListModel {
                        list: logItemList
                    }
                    delegate: ItemDelegate {
                        text: "Item " + index + " " + processname + " " + message
                        width: parent.width
                        //onClicked: console.log("clicked:", modelData)
                        font.pixelSize: 14
                        anchors.left: parent.left
                        anchors.leftMargin: 2
                    }

                    highlight: Rectangle {
                            width: parent.width
                            color: "lightgray"
                    }
                }
            }
        }

        Page {
            title: "SyslogFile"
            width: swipeView.width
            height: swipeView.height

            ScrollView {
                anchors.fill: parent

                ListView {
                    id: syslogfilelist
                    width: parent.width
                    model: Qt.fontFamilies()
                    delegate: ItemDelegate {
                        text: modelData
                        width: parent.width
                        highlighted: ListView.isCurrentItem
                        onClicked: syslogfilelist.currentIndex = index
                    }
                }
            }
        }

        Page {
            title: "Vimar"
            Text {
                id: name
                text: "Ciao"
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: "SyslogRealtime"
        }
        TabButton {
            text: "OldMessages"
        }
        TabButton {
            text: "VimarLog"
        }
    }
}
