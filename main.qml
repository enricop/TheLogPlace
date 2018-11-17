import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import TheLogPlace 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 640
    title: qsTr("TheLogPlace")

    menuBar: MenuBar {
        Button {
            text: "start"
        }
    }

//    header: Rectangle {
//    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            title: "SyslogClient"
            //width: swipeView.width
            //height: swipeView.height

            header: Rectangle {
                TextField {
                    id: syslogsearchBox

                    placeholderText: "Search..."
                    inputMethodHints: Qt.ImhNoPredictiveText

                    anchors.right: parent.right
                    //width: window.width / 5 * 3
                    //anchors.verticalCenter: parent.verticalCenter
                }
            }

            LogListModel {
                id: syslogmodel
                list: logItemList
            }

            ScrollView {
                anchors.fill: parent

                ListView {
                    focus: true
                    clip: true
                    //width: parent.width

                    model: LogFilterProxyModel {
                        id: syslogproxymodel
                        source: syslogmodel

                        filterString: syslogsearchBox.text
                        filterCaseSensitivity: Qt.CaseInsensitive
                    }

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

                    delegate: ItemDelegate {
                        contentItem: RowLayout {
                            Label {
                                text: timestamp
                            }
                            Label {
                                text: processname
                            }
                            Label {
                                text: message
                            }
                        }

                        //text: "Item " + index + " " + timestamp + " " + processname + " " + message
                        //onClicked: console.log("clicked:", modelData)

                        width: parent.width
                        font.pixelSize: 14

                        //anchors.left: parent.left
                        //anchors.leftMargin: 2
                    }

                    highlight: Rectangle {
                        //width: parent.width
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
