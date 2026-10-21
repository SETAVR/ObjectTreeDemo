#include "mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setObjectName("MainWindow");
    setWindowTitle(QStringLiteral("Лабораторная 2 — Дерево объектов Qt"));
    resize(660, 540);
    qDebug() << "[CREATE] MainWindow";
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setSpacing(12);
    auto *title = new LogLabel("title", QStringLiteral("Дерево объектов Qt: владелец и потомки"), this);
    m_rootLayout->addWidget(title);
    auto *input = new LogLineEdit(this);
    input->setPlaceholderText(QStringLiteral("Поле ввода — дочерний виджет главного окна"));
    m_rootLayout->addWidget(input);
    m_status = new LogLabel("status", QStringLiteral("Все виджеты созданы с явными родителями."), this);
    m_status->setWordWrap(true);
    m_status->setMinimumHeight(65);
    m_rootLayout->addWidget(m_status);

    m_panel = new LogPanel("OwnerPanel", this);
    m_panelLayout = new QVBoxLayout(m_panel.data());
    m_panelLayout->addWidget(new LogButton("staticButton", QStringLiteral("Постоянный ребёнок панели"), m_panel.data()));
    m_movable = new LogButton("movable", QStringLiteral("Перемещаемый виджет (в панели)"), m_panel.data());
    m_panelLayout->addWidget(m_movable.data());
    m_rootLayout->addWidget(m_panel.data());

    m_moveButton = new LogButton("moveButton", QStringLiteral("Переместить виджет: панель ↔ окно"), this);
    connect(m_moveButton, &QPushButton::clicked, this, &MainWindow::moveMovableWidget);
    m_rootLayout->addWidget(m_moveButton);
    m_deleteButton = new LogButton("deleteButton", QStringLiteral("Удалить панель (deleteLater)"), this);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deletePanel);
    m_rootLayout->addWidget(m_deleteButton);
    auto *detach = new LogButton("detachButton", QStringLiteral("Демо setParent(nullptr)"), this);
    connect(detach, &QPushButton::clicked, this, &MainWindow::demoDetaching);
    m_rootLayout->addWidget(detach);
    auto *tree = new LogButton("treeButton", QStringLiteral("Вывести дерево объектов в консоль"), this);
    connect(tree, &QPushButton::clicked, this, [this] { dumpObjectTree(); });
    m_rootLayout->addWidget(tree);
    m_rootLayout->addStretch();
}

MainWindow::~MainWindow()
{
    // QObject's destructor will delete the children after this body runs.
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setStatus(const QString &text)
{
    m_status->setText(text);
    qDebug().noquote() << "[STATUS]" << text;
}

void MainWindow::moveMovableWidget()
{
    if (!m_movable || !m_panel || m_deletionPending) {
        setStatus(QStringLiteral("Перемещение невозможно: панель удалена или ожидает удаления."));
        return;
    }
    if (m_movable->parentWidget() == m_panel.data()) {
        m_panelLayout->removeWidget(m_movable.data());
        m_movable->setParent(this);
        m_rootLayout->insertWidget(m_rootLayout->indexOf(m_panel.data()) + 1, m_movable.data());
        m_movable->setText(QStringLiteral("Перемещаемый виджет (в главном окне)"));
        setStatus(QStringLiteral("Новый владелец — главное окно. Удаление панели не удалит этот виджет."));
    } else {
        m_rootLayout->removeWidget(m_movable.data());
        m_movable->setParent(m_panel.data());
        m_panelLayout->addWidget(m_movable.data());
        m_movable->setText(QStringLiteral("Перемещаемый виджет (в панели)"));
        setStatus(QStringLiteral("Владелец снова панель. Виджет будет удалён вместе с ней."));
    }
    // setParent hides a widget; adding it to a layout alone is insufficient.
    m_movable->show();
    qDebug() << "[SET_PARENT]" << m_movable->parentWidget()->objectName();
}

void MainWindow::deletePanel()
{
    if (!m_panel || m_deletionPending) {
        setStatus(QStringLiteral("Панель уже удалена или ожидает удаления."));
        return;
    }
    m_deletionPending = true;
    m_moveButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    const bool childInside = m_movable && m_movable->parentWidget() == m_panel.data();
    setStatus(childInside ? QStringLiteral("Панель и оба её дочерних виджета будут удалены автоматически.")
                          : QStringLiteral("Панель будет удалена; перемещённый виджет останется в окне."));
    qDebug() << "[DELETE_LATER] OwnerPanel";
    m_panel->deleteLater();
}

void MainWindow::demoDetaching()
{
    auto *owner = new LogPanel("TempOwner", this);
    owner->hide();
    new LogLabel("attachedChild", QStringLiteral("Остаётся у родителя"), owner);
    QPointer<LogLabel> detached = new LogLabel("detachedChild", QStringLiteral("Отвязанный ребёнок"), owner);
    qDebug() << "[DEMO] Children before detach:" << owner->children().size();
    detached->setParent(nullptr);
    qDebug() << "[DEMO] Children after detach:" << owner->children().size();

    // destroyed is emitted before child destruction. A queued callback runs later.
    // The detached object is the callback context so Qt cancels it if it dies first.
    connect(owner, &QObject::destroyed, detached.data(), [detached] {
        qDebug() << "[DEMO] Detached survived owner:" << !detached.isNull();
        if (detached)
            detached->deleteLater();
    }, Qt::QueuedConnection);
    // Also cover application shutdown before the queued callback is processed.
    connect(this, &QObject::destroyed, detached.data(), [detached] { delete detached.data(); });
    owner->deleteLater();
    setStatus(QStringLiteral("Демо: дочерняя метка удаляется с TempOwner; отвязанная переживает его и удаляется отдельно."));
}
