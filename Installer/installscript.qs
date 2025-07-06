var psGetShortcutTarget = `
$WshShell = New-Object -ComObject WScript.Shell;
$Shortcut = $WshShell.CreateShortcut('%shortcutpath%');
$Directory = Split-Path -Path $Shortcut.TargetPath -Parent
Write-Output $Directory
`

function Controller()
{
    // Set default install path
    this.setDefaultInstallPath();

    // Remove unused pages
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);

    // Connect the after install logic
    installer.finishButtonClicked.connect(this, this.installationFinished);
}

function getShortcutTarget(shortcutPath) {
    // Setup the script
    var psScript = psGetShortcutTarget.replace("%shortcutpath%", shortcutPath);

    // Start the script and return the output
    return installer.execute("powershell.exe", ["-NoProfile", psScript])[0].trim();
}

Controller.prototype.setDefaultInstallPath = function()
{
    var defaultPath = "@ApplicationsDir@/Spotify Downloader";

    // Get the start menu shortcut path
    var appdataPath = installer.environmentVariable("APPDATA");
    var startMenuPath = appdataPath + "\\Microsoft\\Windows\\Start Menu\\Programs\\";
    var shortcutPath = startMenuPath + installer.value("StartMenuDir") + "\\Spotify Downloader.lnk";

    // If a shortcut exists, use its target directory
    if (installer.fileExists(shortcutPath))
        defaultPath = getShortcutTarget(shortcutPath);

    installer.setValue("TargetDir", defaultPath);
};

Controller.prototype.FinishedPageCallback = function()
{
    var page = gui.pageById(QInstaller.InstallationFinished);

    // Setup auto run checkbox
    var runAppCheckbox = page.RunItCheckBox;
    runAppCheckbox.text = "Run Spotify Downloader";
    runAppCheckbox.checked = true;
    runAppCheckbox.visible = true;

};

Controller.prototype.installationFinished = function()
{
    var page = gui.pageById(QInstaller.InstallationFinished);

    // Open the app if run checkbox is enabled
    if (page.RunItCheckBox.checked)
        installer.executeDetached(installer.value("TargetDir") + "/Spotify Downloader.exe");
};
