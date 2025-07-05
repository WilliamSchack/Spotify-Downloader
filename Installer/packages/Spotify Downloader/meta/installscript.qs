/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the FOO module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/**
 * Powershell script to check for previous uninstall registry keys and delete them
 * Installer not deleting these automatically so have to do it manually
 * Cannot browse registry through QtIFW so use powershell to do it
*/

var psRemovePreviousUninstallerKeys = `
    Get-ItemProperty    HKLM:\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\*,
                        HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\* |
    Where-Object { $_.DisplayName -like "*Spotify Downloader*" -and $_.Publisher -like "*William Schack*"} |
    ForEach-Object {
        $keyPath = $_.PSPath.replace('Microsoft.PowerShell.Core\', '')
        Remove-Item -Path ($keyPath) -Recurse -Force
    }
`;

var targetDirectoryPage = null;

function Component()
{
    var defaultPath = "@ApplicationsDir@/Spotify Downloader";
    installer.setValue("TargetDir", defaultPath);

    component.loaded.connect(this, this.installerLoaded);
}

Component.prototype.createOperations = function()
{
    // Uninstall previous installation
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", ["purge", "-c"], true);

        // Wait for file uninstall through a while loop, cannot sleep or delay or anything in this script its stupid
        console.log("Wait for uninstall...");
        while(installer.fileExists(dir)){
            // Wait for previous install to uninstall
        }
    }

    // Clear all previous version registry keys, requires elevation
    installer.execute("powershell.exe", ["NoProfile", "-ExecutionPolicy", "bypass"], true)

    // Default operations
    component.createOperations();

    // Add the start menu shortcut
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/Spotify Downloader.exe", "@StartMenuDir@/Spotify Downloader.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/Icon.ico");
    }
}

Component.prototype.installerLoaded = function()
{
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");
    targetDirectoryPage.windowTitle = "Choose Installation Directory";
    targetDirectoryPage.description.setText("Please select where the Spotify Downloader will be installed:");
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);

    var admin = installer.gainAdminRights();
}

Component.prototype.targetChooserClicked = function()
{
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    if(dir == "") return;

    dir += "/Spotify Downloader";
    targetDirectoryPage.targetDirectory.setText(dir);
}

Component.prototype.targetDirectoryChanged = function()
{
    var dir = targetDirectoryPage.targetDirectory.text;
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation detected and will be overwritten.</p>");
    }
    else if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. It will be wiped on uninstallation.</p>");
    }
    else {
        targetDirectoryPage.warning.setText("");
    }
    installer.setValue("TargetDir", dir);
}
