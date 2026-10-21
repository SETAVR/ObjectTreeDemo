#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>
#include "widgets.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void moveMovableWidget();
    void deletePanel();
    void demoDetaching();

private:
    void setStatus(const QString &text);
    QVBoxLayout *m_rootLayout = nullptr; // Owned by this; never deleted separately.
    QPointer<QVBoxLayout> m_panelLayout;
    QPointer<LogPanel> m_panel;
    QPointer<LogButton> m_movable;
    LogLabel *m_status = nullptr;
    LogButton *m_moveButton = nullptr;
    LogButton *m_deleteButton = nullptr;
    bool m_deletionPending = false;
};
#endif
