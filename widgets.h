#ifndef WIDGETS_H
#define WIDGETS_H

#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// These wrappers log lifetime without owning any extra resources.
class LogLabel : public QLabel
{
public:
    LogLabel(const QString &name, const QString &text, QWidget *parent)
        : QLabel(text, parent)
    { setObjectName(name); qDebug() << "[CREATE] LogLabel" << objectName(); }
    ~LogLabel() override { qDebug() << "[DESTROY] LogLabel" << objectName(); }
};

class LogButton : public QPushButton
{
public:
    LogButton(const QString &name, const QString &text, QWidget *parent)
        : QPushButton(text, parent)
    { setObjectName(name); qDebug() << "[CREATE] LogButton" << objectName(); }
    ~LogButton() override { qDebug() << "[DESTROY] LogButton" << objectName(); }
};

class LogLineEdit : public QLineEdit
{
public:
    explicit LogLineEdit(QWidget *parent) : QLineEdit(parent)
    { setObjectName("input"); qDebug() << "[CREATE] LogLineEdit" << objectName(); }
    ~LogLineEdit() override { qDebug() << "[DESTROY] LogLineEdit" << objectName(); }
};

class LogPanel : public QFrame
{
public:
    LogPanel(const QString &name, QWidget *parent) : QFrame(parent)
    {
        setObjectName(name);
        setFrameShape(QFrame::StyledPanel);
        qDebug() << "[CREATE] LogPanel" << objectName();
    }
    ~LogPanel() override { qDebug() << "[DESTROY] LogPanel" << objectName(); }
};
#endif
