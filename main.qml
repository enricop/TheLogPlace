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
        Menu {
            title: "&Start"
            Action {
                text: "Download Old Syslog Messages"
                onTriggered: dialogdownload.open()
            }
            Action {
                text: "Receive New Syslog Messages"
            }
        }
    }

    header: GroupBox {
        title: "Stato"
        //topPadding: 25
        //padding: 25
        Label {
            id: status
            text: "Nessun log caricato. Clikkare su \'Start\' per iniziare"
        }
    }

    Dialog {
        id: dialogdownload
        title: "Titolo del Dialog"

        x: 100
        y: 100
        //width: 300
        //height: 200

        modal: true

        ColumnLayout {
            GroupBox {
                title: "State"
                Label {
                    id: dialogstatus
                    text: oldlogsbackend.connectionInfo
                }
            }
            TextField {
                id: ipaddressbox
                placeholderText: "IP Address"
                validator: RegExpValidator { regExp: /^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$/ }
            }
            TextField {
                id: usernamebox
                placeholderText: "Username"
                validator: RegExpValidator { regExp: /^[a-zA-Z0-9_\-][a-zA-Z0-9_\-]*$/ }
            }
            TextField {
                id: passwordbox
                placeholderText: "Password"
            }
            Button {
                id: downloadbutton
                text: "Download Logs"
                enabled: ipaddressbox.acceptableInput && usernamebox.acceptableInput
                onClicked: oldlogsbackend.sshConnector(ipaddressbox.text,
                                                     usernamebox.text,
                                                     passwordbox.text)
            }
            ProgressBar {
                from: 0
                value: oldlogsbackend.downloadProgress
                to: oldlogsbackend.downloadSize
            }
        }

        standardButtons: Dialog.Close

        //onAccepted: console.log("Ok clicked")
        onRejected: console.log("Close clicked")
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            title: "SyslogClient"

            header: GroupBox {
                title: "Filtri"
                TextField {
                    id: syslogsearchBox
                    placeholderText: "Search..."
                    anchors.right: parent.right
                    //width: window.width / 5 * 3
                }
            }

            LogListModel {
                id: syslogmodel
                list: oldlogs
            }

            ScrollView {
                anchors.fill: parent

                ListView {
                    //focus: true
                    clip: true
                    //width: parent.width

                    model: LogFilterProxyModel {
                        id: syslogproxymodel
                        source: syslogmodel

                        filterString: syslogsearchBox.text
                        filterCaseSensitivity: Qt.CaseInsensitive
                    }

                    header: RowLayout {
                        id: columnsheader
                        //width: parent.width
                        //height: 50
                        //gradient: clubcolors
                        //border {color: "#9EDDF2"; width: 2}
                        Label {
                            text: "Timestamp"
                        }
                        Label {
                            text: "Process"
                        }
                        Label {
                            //anchors.centerIn: parent
                            text: "Message"
                            //font.pixelSize: 20
                            //width: 40
                        }
                    }

                    delegate: ItemDelegate {
                        RowLayout {
                            Label {
                                text: timestamp
                                //Layout.preferredWidth: 100
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
                        //font.pixelSize: 14

                        //anchors.left: parent.left
                        //anchors.leftMargin: 2
                    }

                    /*
                        highlight: Rectangle {
                            //width: parent.width
                            color: "lightgray"
                    } */
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
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: "OldMessages"
        }
        TabButton {
            text: "SyslogRealtime"
        }
    }
}
