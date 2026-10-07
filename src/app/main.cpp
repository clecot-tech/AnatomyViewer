#include <QApplication>
#include <QMainWindow>
#include <QString>

#include "core/version.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    const auto version = anatomy::core::version();

    QMainWindow window;
    window.setWindowTitle(
        QStringLiteral("AnatomyViewer %1")
        .arg(QString::fromUtf8(version.data(), static_cast<qsizetype>(version.size()))));
    window.resize(1024, 768);
    window.show();

    return app.exec();
}