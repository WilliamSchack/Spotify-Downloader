#include "FileDialogUtils.h"

void FileDialogUtils::SetParent(QFileDialog& dialog, QWindow* parentWindow)
{
    // Sets the dialog window parent to the input window
    // Could have used a qml element but that didnt look as good
    // Main reason is just to get the window floating on systems that use window managers rather than DEs but im sure it has other benefits

    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowFlags(Qt::Dialog);

    dialog.winId();
    if (parentWindow != nullptr && dialog.windowHandle())
        dialog.windowHandle()->setTransientParent(parentWindow);
}