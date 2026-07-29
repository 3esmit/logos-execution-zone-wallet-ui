import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Logos.Theme
import Logos.Controls

Popup {
    id: root

    property string accountId: ""

    signal initializeConfirmed(string accountId)

    modal: true
    dim: true
    padding: Theme.spacing.large
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min(560, parent ? parent.width - Theme.spacing.large * 2 : 560)

    background: Rectangle {
        color: Theme.palette.backgroundSecondary
        radius: Theme.spacing.radiusXlarge
        border.color: Theme.palette.backgroundElevated
    }

    contentItem: ColumnLayout {
        width: root.availableWidth
        spacing: Theme.spacing.large

        LogosText {
            text: qsTr("Initialize public account")
            font.pixelSize: Theme.typography.titleText
            font.weight: Theme.typography.weightBold
            color: Theme.palette.text
        }

        LogosText {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("This submits an on-chain transaction to initialize this public account. Continue only if you intend to use it for public transfers.")
            font.pixelSize: Theme.typography.secondaryText
            color: Theme.palette.textSecondary
        }

        LogosText {
            Layout.fillWidth: true
            wrapMode: Text.WrapAnywhere
            text: qsTr("Account: %1").arg(root.accountId)
            font.pixelSize: Theme.typography.secondaryText
            color: Theme.palette.textMuted
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacing.medium
            spacing: Theme.spacing.medium

            Item { Layout.fillWidth: true }

            LogosButton {
                text: qsTr("Cancel")
                onClicked: root.close()
            }

            LogosButton {
                text: qsTr("Initialize account")
                enabled: root.accountId.length > 0
                onClicked: {
                    root.initializeConfirmed(root.accountId)
                    root.close()
                }
            }
        }
    }
}
