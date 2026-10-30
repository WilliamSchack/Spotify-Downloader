#ifndef ADDTRACKSPOPUP_H
#define ADDTRACKSPOPUP_H

#include "EasyQProperties.h"
#include "FileDialogUtils.h"

#include <functional>
#include <string>

#include <QApplication>
#include <QClipboard>

class AddTracksPopup : public QObject
{
    Q_OBJECT
    SIMPLE_QPROPERTY(bool, visible, Visible, _visible)
    SIMPLE_QPROPERTY(bool, checkingInputs, CheckingInputs, _checkingInputs)
    SIMPLE_QPROPERTY(QString, linkInputText, LinkInputText, _linkInputText)
    SIMPLE_QPROPERTY(QString, folderInputText, FolderInputText, _folderInputText)
    SIMPLE_QPROPERTY(QString, errorMessage, ErrorMessage, _errorMessage)

    public:
        explicit AddTracksPopup(QObject* parent = 0);
    public slots:
        void PasteButtonClicked();
        void FolderButtonClicked();
        void DownloadButtonClicked();
    private:
        bool _visible = true;
        bool _checkingInputs = false;

        QString _linkInputText = "";
        QString _folderInputText = "";
        QString _errorMessage = "";
    signals:
        void DownloadRequested(const std::string& link, const std::string& destinationFolder);
};

#endif