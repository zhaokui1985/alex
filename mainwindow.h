#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startTimer();
    void cancelTimer();
    void updateWorkTimer();
    void updateRestTimer();
    void checkUnsetReminder();
    void updateCountdown();
private:
    Ui::MainWindow *ui;
    QTimer *workTimer;
    QTimer *restTimer;
    QTimer *minuteReminder;
    QTimer *unsetReminder;

    int workDuration;   // 工作时间 (分钟)
    int restDuration;   // 休息时间 (分钟)
    int workPassed;     // 已工作时间
    int restPassed;     // 已休息时间
    int surplusTime;
    bool timerRunning;
    QSystemTrayIcon *trayIcon;
    QLabel *statusLabel;   // 显示状态（工作/休息/空闲）
    QIcon image;
    QLabel *label;
    QTimer *timer;
signals:
    void countdownFinished();  //
};

#endif // MAINWINDOW_H
