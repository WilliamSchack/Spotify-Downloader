#ifndef FILEDIALOGUTILS_H
#define FILEDIALOGUTILS_H

#include <QFileDialog>
#include <QWindow>

class FileDialogUtils
{
    public:
        static void SetParent(QFileDialog& dialog, QWindow* parentWindow);
};

#endif