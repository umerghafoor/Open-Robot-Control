#include <QApplication>
#include <QDockWidget>
#include <memory>
#include <cstdlib>
#include "core/Application.h"
#include "utils/Logger.h"
#include "ui/MaterialDockWidget.h"

// Subclass QApplication to guard against QTBUG-43698.
//
// Qt's restoreState() creates bare QDockWidget placeholder objects for any
// dock widget names in the saved state that don't exist in the current layout
// (e.g. a widget added manually in a previous session). These placeholders are
// not MaterialDockWidget instances, so they have no event() override and crash
// on any mouse event via a null internal drag-state dereference.  Additionally,
// Qt's dock group window machinery can briefly route mouse events through a plain
// QDockWidget before the widget is properly set up.
//
// notify() is the only interception point that fires for ALL events to ALL
// objects before any virtual dispatch — so we block mouse events to any dock
// widget that is not one of our MaterialDockWidget instances here.
class OpenRobotControlApp : public QApplication {
public:
    using QApplication::QApplication;

    bool notify(QObject* receiver, QEvent* event) override {
        const QEvent::Type t = event->type();
        if (t == QEvent::MouseButtonPress   ||
            t == QEvent::MouseButtonRelease ||
            t == QEvent::MouseMove          ||
            t == QEvent::MouseButtonDblClick) {
            if (auto* dw = qobject_cast<QDockWidget*>(receiver)) {
                if (!qobject_cast<MaterialDockWidget*>(dw))
                    return false;   // drop event — prevents null drag-state crash
            }
        }
        return QApplication::notify(receiver, event);
    }
};

int main(int argc, char *argv[])
{
    // QDockWidget drag-and-drop requires mouse grabbing which the native Wayland
    // plugin does not support for non-popup windows. Qt's documented solution is
    // to run under the XCB (XWayland) platform plugin where grabMouse() works.
    // This must be set before QApplication is constructed.
    // See: https://bugreports.qt.io/browse/QTBUG-66008
    if (qgetenv("QT_QPA_PLATFORM").isEmpty())
        qputenv("QT_QPA_PLATFORM", "xcb");

    // Initialize Qt Application
    OpenRobotControlApp app(argc, argv);
    app.setApplicationName("OpenRobotControl");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("OpenRobotControl");

    // Initialize Logger
    Logger::instance().initialize("OpenRobotControl.log");
    Logger::instance().info("Application starting...");

    try {
        // Create and run the main application
        auto robotApp = std::make_unique<Application>(argc, argv);

        if (!robotApp->initialize()) {
            Logger::instance().error("Failed to initialize application");
            return -1;
        }

        robotApp->show();
        Logger::instance().info("Application initialized successfully");

        int result = app.exec();
        
        Logger::instance().info("Application shutting down...");
        return result;
    }
    catch (const std::exception& e) {
        Logger::instance().error(QString("Fatal error: %1").arg(e.what()));
        return -1;
    }
}
