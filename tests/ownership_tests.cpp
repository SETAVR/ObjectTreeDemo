#include <QtTest>
#include <QDir>
#include <QEvent>
#include <QSignalSpy>
#include "mainwindow.h"

class OwnershipTests : public QObject
{
    Q_OBJECT
private:
    static void flushDeletes()
    {
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    }
    static void capture(MainWindow &window, const QString &name)
    {
        const QString directory = qEnvironmentVariable("DEMO_SCREENSHOT_DIR");
        if (directory.isEmpty())
            return;
        QDir().mkpath(directory);
        window.show();
        QCoreApplication::processEvents();
        const bool saved = window.grab().save(QDir(directory).filePath(name + ".png"));
        QVERIFY(saved);
    }
private slots:
    void initialTree()
    {
        MainWindow window;
        auto *panel = window.findChild<QFrame *>("OwnerPanel");
        auto *movable = window.findChild<QPushButton *>("movable");
        QVERIFY(panel);
        QVERIFY(movable);
        QCOMPARE(panel->parentWidget(), &window);
        QCOMPARE(movable->parentWidget(), panel);
        QVERIFY(window.findChildren<QWidget *>().size() >= 5);
        capture(window, "01_initial");
    }
    void deletingOwnerDeletesChildren()
    {
        MainWindow window;
        QPointer<QFrame> panel = window.findChild<QFrame *>("OwnerPanel");
        QPointer<QPushButton> child = window.findChild<QPushButton *>("movable");
        QPointer<QVBoxLayout> layout = panel->findChild<QVBoxLayout *>();
        QSignalSpy destroyed(child.data(), &QObject::destroyed);
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        QVERIFY(panel); // deleteLater has not destroyed it yet.
        flushDeletes();
        QVERIFY(panel.isNull());
        QVERIFY(child.isNull());
        QVERIFY(layout.isNull());
        QCOMPARE(destroyed.count(), 1);
        capture(window, "02_panel_deleted");
    }
    void movedChildSurvivesOldOwner()
    {
        MainWindow window;
        window.show();
        QPointer<QPushButton> child = window.findChild<QPushButton *>("movable");
        QVERIFY(QMetaObject::invokeMethod(&window, "moveMovableWidget"));
        QCOMPARE(child->parentWidget(), &window);
        QVERIFY(!child->isHidden());
        QVERIFY(window.layout()->indexOf(child.data()) >= 0);
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        flushDeletes();
        QVERIFY(child);
        QCOMPARE(child->parentWidget(), &window);
        capture(window, "03_moved_child_survives");
    }
    void returnToPanelRestoresOwnership()
    {
        MainWindow window;
        QPointer<QPushButton> child = window.findChild<QPushButton *>("movable");
        QVERIFY(QMetaObject::invokeMethod(&window, "moveMovableWidget"));
        QVERIFY(QMetaObject::invokeMethod(&window, "moveMovableWidget"));
        QCOMPARE(child->parentWidget(), window.findChild<QFrame *>("OwnerPanel"));
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        flushDeletes();
        QVERIFY(child.isNull());
    }
    void repeatedActionsAreSafe()
    {
        MainWindow window;
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        QVERIFY(QMetaObject::invokeMethod(&window, "moveMovableWidget"));
        flushDeletes();
        QVERIFY(QMetaObject::invokeMethod(&window, "deletePanel"));
        QVERIFY(QMetaObject::invokeMethod(&window, "moveMovableWidget"));
        QVERIFY(!window.findChild<QPushButton *>("moveButton")->isEnabled());
        QVERIFY(!window.findChild<QPushButton *>("deleteButton")->isEnabled());
    }
    void detachedChildHasSeparateLifetime()
    {
        MainWindow window;
        QVERIFY(QMetaObject::invokeMethod(&window, "demoDetaching"));
        QPointer<QFrame> owner = window.findChild<QFrame *>("TempOwner");
        QVERIFY(owner);
        QPointer<QLabel> attached = owner->findChild<QLabel *>("attachedChild");
        QPointer<QWidget> detached;
        for (auto *widget : QApplication::topLevelWidgets())
            if (widget->objectName() == "detachedChild") detached = widget;
        QVERIFY(attached);
        QVERIFY(detached);
        QVERIFY(!detached->parent());
        QCOMPARE(owner->children().size(), 1);
        flushDeletes();
        QVERIFY(owner.isNull());
        QVERIFY(attached.isNull());
        QVERIFY(detached); // Survived the old owner.
        QCoreApplication::sendPostedEvents(nullptr, QEvent::MetaCall);
        flushDeletes();
        QVERIFY(detached.isNull());
    }
    void shutdownCleansDetachedChild()
    {
        auto *window = new MainWindow;
        QVERIFY(QMetaObject::invokeMethod(window, "demoDetaching"));
        QPointer<QWidget> detached;
        for (auto *widget : QApplication::topLevelWidgets())
            if (widget->objectName() == "detachedChild") detached = widget;
        QVERIFY(detached);
        delete window; // Shutdown before any queued callback can run.
        QVERIFY(detached.isNull());
        QCoreApplication::processEvents();
        flushDeletes();
    }
    void deletingWindowDeletesMovedChild()
    {
        auto *window = new MainWindow;
        QPointer<QPushButton> child = window->findChild<QPushButton *>("movable");
        QVERIFY(QMetaObject::invokeMethod(window, "moveMovableWidget"));
        delete window;
        QVERIFY(child.isNull());
    }
};

QTEST_MAIN(OwnershipTests)
#include "ownership_tests.moc"
