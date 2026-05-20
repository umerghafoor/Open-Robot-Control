#include "MaterialDockWidget.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QStyle>
#include <QTimer>

// ─────────────────────────────────────────────────────────────────────────────
// MaterialDockWidget
//
// Thin QDockWidget subclass that replaces the native chrome with a styled
// title bar while keeping Qt's built-in dock drag/drop/redock machinery
// completely intact.
//
// The key constraint: setTitleBarWidget() with a non-null widget disables
// Qt's native title bar but preserves the QDockWidget drag state machine —
// Qt still handles all mouse events on the dock itself for floating/docking.
// We must NOT intercept or re-implement drag logic here; doing so fights
// Qt's internal QDockWidgetPrivate::state and causes crashes or broken drops.
//
// All the title bar widget does is render styled chrome (grip, label, buttons).
// Mouse events on it fall through to QDockWidget via the normal event chain.
// ─────────────────────────────────────────────────────────────────────────────

MaterialDockWidget::MaterialDockWidget(const QString& title, QWidget* parent)
    : QDockWidget(title, parent)
{
    buildTitleBar(title);

    // Keep "floating" dynamic property in sync for QSS selectors.
    // Deferred one event-loop turn to avoid re-entering Qt's dock layout
    // machinery synchronously during a drag operation.
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
        setProperty("floating", floating);
        QTimer::singleShot(0, this, [this] {
            style()->unpolish(this);
            style()->polish(this);
        });
        updateFullscreenButton();
    });
}

void MaterialDockWidget::buildTitleBar(const QString& title)
{
    // Plain QWidget — no mouse event overrides. Qt's QDockWidget sees all
    // mouse events on child widgets that don't accept them, and routes them
    // through its own drag state machine correctly.
    auto* bar = new QWidget(this);
    bar->setObjectName("dockTitleBar");
    bar->setFixedHeight(32);
    // SizeAllCursor signals draggability to the user without us doing anything.
    bar->setCursor(Qt::SizeAllCursor);

    auto* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(12, 0, 8, 0);
    layout->setSpacing(8);

    auto* grip = new QLabel("⣿", bar);
    grip->setObjectName("dockGripLabel");
    grip->setAttribute(Qt::WA_TransparentForMouseEvents);
    layout->addWidget(grip);

    m_titleLabel = new QLabel(title, bar);
    m_titleLabel->setObjectName("dockTitleLabel");
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    layout->addWidget(m_titleLabel, 1);

    m_fullscreenButton = new QToolButton(bar);
    m_fullscreenButton->setObjectName("dockFullscreenButton");
    m_fullscreenButton->setText("⛶");
    m_fullscreenButton->setToolTip(tr("Full Screen"));
    m_fullscreenButton->setAutoRaise(true);
    connect(m_fullscreenButton, &QToolButton::clicked,
            this, &MaterialDockWidget::onFullscreenClicked);
    layout->addWidget(m_fullscreenButton);

    m_floatButton = new QToolButton(bar);
    m_floatButton->setObjectName("dockFloatButton");
    m_floatButton->setText("❐");
    m_floatButton->setToolTip(tr("Float / Dock"));
    m_floatButton->setAutoRaise(true);
    connect(m_floatButton, &QToolButton::clicked,
            this, &MaterialDockWidget::onFloatClicked);
    layout->addWidget(m_floatButton);

    m_closeButton = new QToolButton(bar);
    m_closeButton->setObjectName("dockCloseButton");
    m_closeButton->setText("✕");
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setAutoRaise(true);
    connect(m_closeButton, &QToolButton::clicked,
            this, &MaterialDockWidget::onCloseClicked);
    layout->addWidget(m_closeButton);

    m_titleBar = bar;
    setTitleBarWidget(bar);
}

void MaterialDockWidget::setCardTitle(const QString& title)
{
    setWindowTitle(title);
    if (m_titleLabel) m_titleLabel->setText(title);
}

void MaterialDockWidget::onFullscreenClicked()
{
    if (windowState() & Qt::WindowFullScreen) {
        showNormal();
        if (!m_wasFloating) setFloating(false);
    } else {
        m_wasFloating = isFloating();
        setFloating(true);
        showFullScreen();
    }
}

void MaterialDockWidget::onFloatClicked() { setFloating(!isFloating()); }
void MaterialDockWidget::onCloseClicked() { close(); }

void MaterialDockWidget::updateFullscreenButton()
{
    if (!m_fullscreenButton) return;
    const bool fs = windowState() & Qt::WindowFullScreen;
    m_fullscreenButton->setText(fs ? "✖" : "⛶");
    m_fullscreenButton->setToolTip(fs ? tr("Exit Full Screen  (Esc)") : tr("Full Screen"));
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
