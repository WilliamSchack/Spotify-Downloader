function Controller()
{
    // Set default install path
    var defaultPath = "@ApplicationsDir@/Spotify Downloader";
    installer.setValue("TargetDir", defaultPath);

    // Remove unused pages
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);

    // Connect the after install logic
    installer.finishButtonClicked.connect(this, this.installationFinished);
}

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
