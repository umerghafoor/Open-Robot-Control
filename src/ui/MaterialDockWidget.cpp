#include "MaterialDockWidget.h"
#include <QStyle>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include <QWindow>

// Title bar that owns drag detection itself instead of relying on event
// propagation into QDockWidget's internal drag state machine. The previous
// approach (ignore() on mouse events + propagate to QDockWidget) was brittle:
// QDockWidget's d->state path is designed for the native title bar, not for
// custom title bar widgets, and crashed at drop time when layout reshuffling
// re-entered the dock's mouse handling with a stale/null state.
//
// We instead detect the drag from the panel itself: on a left press over the
// empty title bar area (buttons absorb their own presses, so they never reach
// here), arm a drag; once the cursor moves past startDragDistance, set the
// dock floating. From there, the user is dragging a top-level window and Qt
// handles redocking via the normal QMainWindow drop overlay.
class DockTitleBar final : public QWidget
{
public:
    explicit DockTitleBar(MaterialDockWidget* dock) : QWidget(dock), m_dock(dock) {}

protected:
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton && m_dock && (m_dock->features() & QDockWidget::DockWidgetMovable)) {
            m_pressPos = e->globalPosition().toPoint();
            m_armed    = true;
            e->accept();
            return;
        }
        e->ignore();
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (!m_armed || !m_dock) { e->ignore(); return; }

        const QPoint delta = e->globalPosition().toPoint() - m_pressPos;
        if (delta.manhattanLength() < QApplication::startDragDistance()) {
            e->accept();
            return;
        }

        // Once past threshold, undock (if needed) and hand off to the window
        // manager via startSystemMove(). After that the WM owns the drag —
        // our mouse events stop firing, which is exactly what we want.
        if (!m_dock->isFloating()) {
            m_dock->setFloating(true);
            const QPoint cursor = e->globalPosition().toPoint();
            m_dock->move(cursor - QPoint(m_dock->width() / 4, height() / 2));
        }

        m_armed = false;
        if (QWindow* w = m_dock->windowHandle())
            w->startSystemMove();
        e->accept();
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        m_armed = false;
        e->accept();
    }

    void mouseDoubleClickEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton && m_dock) {
            m_dock->setFloating(!m_dock->isFloating());
            e->accept();
            return;
        }
        e->ignore();
    }

private:
    MaterialDockWidget* m_dock  = nullptr;
    QPoint              m_pressPos;
    bool                m_armed = false;
};

MaterialDockWidget::MaterialDockWidget(const QString& title, QWidget* parent)
    : QDockWidget(title, parent)
{
    buildTitleBar(title);

    // Update floating dynamic property so QSS [floating="true"] selector works.
    // Style polishing is deferred via QTimer to avoid interfering with Qt's
    // internal dock drag state machine, which can cause a crash if layout/style
    // changes are triggered synchronously during a drag operation.
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
        setProperty("floating", floating);
        QTimer::singleShot(0, this, [this]() {
            style()->unpolish(this);
            style()->polish(this);
        });
        updateFullscreenButton();
    });
}

void MaterialDockWidget::buildTitleBar(const QString& title)
{
    m_titleBar = new DockTitleBar(this);
    m_titleBar->setObjectName("dockTitleBar");
    m_titleBar->setMinimumHeight(32);
    m_titleBar->setMaximumHeight(32);
    m_titleBar->setCursor(Qt::SizeAllCursor);

    auto* layout = new QHBoxLayout(m_titleBar);
    layout->setContentsMargins(12, 0, 8, 0);
    layout->setSpacing(8);

    // Drag grip — decorative only, passes mouse events through to title bar
    auto* grip = new QLabel("\u28FF", m_titleBar);
    grip->setObjectName("dockGripLabel");
    grip->setAttribute(Qt::WA_TransparentForMouseEvents);
    layout->addWidget(grip);

    m_titleLabel = new QLabel(title, m_titleBar);
    m_titleLabel->setObjectName("dockTitleLabel");
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    layout->addWidget(m_titleLabel, 1);

    m_fullscreenButton = new QToolButton(m_titleBar);
    m_fullscreenButton->setObjectName("dockFullscreenButton");
    m_fullscreenButton->setText("\u26F6");  // ⛶ fullscreen icon
    m_fullscreenButton->setToolTip(tr("Full Screen"));
    m_fullscreenButton->setAutoRaise(true);
    connect(m_fullscreenButton, &QToolButton::clicked, this, &MaterialDockWidget::onFullscreenClicked);
    layout->addWidget(m_fullscreenButton);

    m_floatButton = new QToolButton(m_titleBar);
    m_floatButton->setObjectName("dockFloatButton");
    m_floatButton->setText("\u2750");  // ❐ float/restore icon
    m_floatButton->setToolTip(tr("Float / Dock"));
    m_floatButton->setAutoRaise(true);
    connect(m_floatButton, &QToolButton::clicked, this, &MaterialDockWidget::onFloatClicked);
    layout->addWidget(m_floatButton);

    m_closeButton = new QToolButton(m_titleBar);
    m_closeButton->setObjectName("dockCloseButton");
    m_closeButton->setText("\u2715");  // ✕
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setAutoRaise(true);
    connect(m_closeButton, &QToolButton::clicked, this, &MaterialDockWidget::onCloseClicked);
    layout->addWidget(m_closeButton);

    setTitleBarWidget(m_titleBar);
}

void MaterialDockWidget::setCardTitle(const QString& title)
{
    setWindowTitle(title);
    if (m_titleLabel)
        m_titleLabel->setText(title);
}

void MaterialDockWidget::onFullscreenClicked()
{
    if (windowState() & Qt::WindowFullScreen) {
        showNormal();
        // Restore to docked if it wasn't floating before going fullscreen
        if (!m_wasFloating)
            setFloating(false);
    } else {
        m_wasFloating = isFloating();
        setFloating(true);
        showFullScreen();
    }
}

void MaterialDockWidget::onFloatClicked()
{
    setFloating(!isFloating());
}

void MaterialDockWidget::onCloseClicked()
{
    close();
}

void MaterialDockWidget::updateFullscreenButton()
{
    if (!m_fullscreenButton)
        return;

    if (windowState() & Qt::WindowFullScreen) {
        m_fullscreenButton->setText("\u2716");  // ✖ — exit fullscreen
        m_fullscreenButton->setToolTip(tr("Exit Full Screen  (Esc)"));
    } else {
        m_fullscreenButton->setText("\u26F6");  // ⛶ — enter fullscreen
        m_fullscreenButton->setToolTip(tr("Full Screen"));
    }
}

void MaterialDockWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && (windowState() & Qt::WindowFullScreen)) {
        onFullscreenClicked();
        event->accept();
        return;
    }
    QDockWidget::keyPressEvent(event);
}

void MaterialDockWidget::changeEvent(QEvent* event)
{
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
        updateFullscreenButton();
}
