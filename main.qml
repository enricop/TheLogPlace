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
                onTriggered: { dialogdownload.open(); swipeView.currentIndex = 0; tabBar.currentIndex = 0;}
            }
            Action {
                text: "Receive New Syslog Messages"
                onTriggered: { dialogreceive.open(); swipeView.currentIndex = 1; tabBar.currentIndex = 1;}
            }
        }
    }

    header: GroupBox {
        title: "Stato"
        //topPadding: 25
        //padding: 25
        Label {
            id: status
            text: "Clikkare su \'Start\' per iniziare"
        }
    }

    Dialog {
        id: dialogdownload
        title: "SSH Logs Download"

        x: 100
        //width: 300
        //height: 200

        modal: true

        ColumnLayout {
            GroupBox {
                title: "State"
                Label {
                    id: dialogstatusold
                    text: oldlogsbackend.connectionInfo
                }
            }
            TextField {
                id: ipaddressboxold
                placeholderText: "IP Address, es: 172.20.30.48"
                validator: RegExpValidator { regExp: /^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$/ }
            }
            TextField {
                id: usernameboxold
                placeholderText: "SSH Username, es: root"
                validator: RegExpValidator { regExp: /^[a-zA-Z0-9_\-][a-zA-Z0-9_\-]*$/ }
            }
            TextField {
                id: passwordboxold
                placeholderText: "SSH Password, can be empty"
                echoMode: TextInput.Password
            }
            Button {
                id: downloadbutton
                text: "Download Logs"
                enabled: ipaddressboxold.acceptableInput && usernameboxold.acceptableInput && oldlogsbackend.downloadCompleted
                onClicked: oldlogsbackend.sshConnector(ipaddressboxold.text,
                                                     usernameboxold.text,
                                                     passwordboxold.text)
            }
            ProgressBar {
                from: 0
                value: oldlogsbackend.downloadProgress
                to: oldlogsbackend.downloadSize
            }
        }

        //standardButtons: Dialog.Close

        onRejected: console.log("Close clicked")
    }

    Dialog {
        id: dialogreceive
        title: "Enable Rsyslog Remote Send"

        x: 100
        //width: 300
        //height: 200

        modal: true

        ColumnLayout {
            GroupBox {
                title: "State"
                Label {
                    id: dialogstatusnew
                    text: newlogsbackend.connectionInfo
                }
            }
            TextField {
                id: ipaddressboxnew
                placeholderText: "IP Address, es: 172.20.30.48"
                validator: RegExpValidator { regExp: /^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$/ }
            }
            TextField {
                id: usernameboxnew
                placeholderText: "SSH Username, es: root"
                validator: RegExpValidator { regExp: /^[a-zA-Z0-9_\-][a-zA-Z0-9_\-]*$/ }
            }
            TextField {
                id: passwordboxnew
                placeholderText: "SSH Password, can be empty"
                echoMode: TextInput.Password
            }
            Button {
                id: downloadbuttonnew
                text: "Start Receiving Logs"
                enabled: ipaddressboxnew.acceptableInput && usernameboxnew.acceptableInput && oldlogsbackend.downloadCompleted
                onClicked: newlogsbackend.sshConnector(ipaddressboxnew.text,
                                                     usernameboxnew.text,
                                                     passwordboxnew.text)
            }
        }

        //standardButtons: Dialog.Close

        onRejected: console.log("Close clicked")
    }


    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            title: "Syslog Old Messages"

            header: GroupBox {
                title: "Filtri"
                TextField {
                    id: oldsyslogsearchBox
                    placeholderText: "Search..."
                    anchors.right: parent.right
                    //width: window.width / 5 * 3
                }
            }

            LogListModel {
                id: oldsyslogmodel
                list: oldlogs
            }

            ScrollView {
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOn

                ListView {
                    id: oldlistview

                    model: LogFilterProxyModel {
                        id: oldsyslogproxymodel
                        source: oldsyslogmodel

                        filterString: oldsyslogsearchBox.text
                        filterCaseSensitivity: Qt.CaseInsensitive
                    }

                    headerPositioning: ListView.OverlayHeader
                    header: RowLayout {
                        //width: parent.width
                        //height: 50
                        //gradient: clubcolors
                        //border {color: "#9EDDF2"; width: 2}
                        Label {
                            text: "Timestamp        "
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Label {
                            text: "Process                  "
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Label {
                            text: "Message"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }

                    delegate: ItemDelegate {
                        RowLayout {
                            Label {
                                text: timestamp
                            }
                            Label {
                                text: processname
                                font.bold: true
                            }
                            Label {
                                text: message
                            }
                        }

                        font.pixelSize: 12
                        font.family: "DejaVu Sans Mono"

                        height: 17
                        //width: parent.width
                        //anchors.left: parent.left
                        //anchors.leftMargin: 2
                    }
                }
            }
        }

        Page {
            title: "Syslog Receiver"
            width: swipeView.width
            height: swipeView.height

            header: GroupBox {
                title: "Filtri"
                TextField {
                    id: newsyslogsearchBox
                    placeholderText: "Search..."
                    anchors.right: parent.right
                    //width: window.width / 5 * 3
                }
            }

            LogListModel {
                id: newsyslogmodel
                list: newlogs
            }

            ScrollView {
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOn

                ListView {
                    id: newlistview

                    model: LogFilterProxyModel {
                        id: newsyslogproxymodel
                        source: newsyslogmodel

                        filterString: newsyslogsearchBox.text
                        filterCaseSensitivity: Qt.CaseInsensitive
                    }

                    headerPositioning: ListView.OverlayHeader
                    header: RowLayout {
                        //width: parent.width
                        //height: 50
                        //gradient: clubcolors
                        //border {color: "#9EDDF2"; width: 2}
                        Label {
                            text: "Timestamp        "
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Label {
                            text: "Process                  "
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Label {
                            text: "Message"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }

                    delegate: ItemDelegate {
                        RowLayout {
                            Label {
                                text: timestamp
                            }
                            Label {
                                text: processname
                                font.bold: true
                            }
                            Label {
                                text: message
                            }
                        }

                        font.pixelSize: 12
                        font.family: "DejaVu Sans Mono"

                        height: 17
                    }
                }
            }

        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: "Old Syslog Messages"
        }
        TabButton {
            text: "Syslog Remote Receiver"
        }
    }
}
