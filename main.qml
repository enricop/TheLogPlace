import QtQuick 2.9
import QtQuick.Controls 2.2

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
                    width: parent.width
                    model: 20
                    delegate: ItemDelegate {
                        text: "Item " + (index + 1)
                        width: parent.width
                        onClicked: console.log("clicked:", modelData)
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
