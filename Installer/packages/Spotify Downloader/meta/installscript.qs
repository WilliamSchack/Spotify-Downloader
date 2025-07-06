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
    // Get admin perms
    installer.gainAdminRights();

    // Setup the target directory page
    this.setupTargetDirectoryPage();
}

Component.prototype.createOperations = function()
{
    // Uninstall previous installation
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", ["purge", "-c"]);

        // Wait for file uninstall through a while loop, cannot sleep or delay or anything in this script its stupid
        console.log("Wait for uninstall...");
        while(installer.fileExists(dir)){
            // Wait for previous install to uninstall
        }
    }

    // Clear all previous version registry keys, requires elevation
    installer.execute("powershell.exe", ["-NoProfile", "-ExecutionPolicy", "bypass", psRemovePreviousUninstallerKeys])

    // Default operations
    component.createOperations();

    // Add the start menu shortcut
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/Spotify Downloader.exe", "@StartMenuDir@/Spotify Downloader.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/Icon.ico");
    }
};

Component.prototype.setupTargetDirectoryPage = function()
{
    // Disable the default TargetDirectory page with a custom one
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);
    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");

    // Update text
    targetDirectoryPage.windowTitle = "Installation Folder";
    targetDirectoryPage.description.setText("Please select where the Spotify Downloader will be installed:");

    // Update and connect to directory path
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));

    // Connect to browse button
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);
};

Component.prototype.targetDirectoryChanged = function()
{
    var page = gui.currentPageWidget();

    // Get the inputted directory and set the TargetDir
    var dir = targetDirectoryPage.targetDirectory.text;
    installer.setValue("TargetDir", dir);

    // No value inputted
    if (dir == "") {
        targetDirectoryPage.warning.setText("The installation path cannot be empty, please specify a valid directory.");
        page.complete = false;
        return;
    }

    // Previous install found
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("Existing installation detected and will be overwritten.");
        return;
    }

    // Directory set to an occupied location
    if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("Installing in existing directory. It will be wiped on uninstallation.");
        return;
    }

    page.complete = true;
    targetDirectoryPage.warning.setText("");
};

Component.prototype.targetChooserClicked = function()
{
    // Popup a directory choose input
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    if(dir == "") return;

    // Add the Spotify Downloader folder to the directory
    dir += "/Spotify Downloader";
    targetDirectoryPage.targetDirectory.setText(dir);
};
