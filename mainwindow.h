#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "demux.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private:
    void createActions();
//! [2]

//! [3]
    QAction *openAct;
    Ui::MainWindow *ui;
    Demux* demux;

};
#endif // MAINWINDOW_H
