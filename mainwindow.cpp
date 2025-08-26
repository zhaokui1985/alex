#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{this->setStyleSheet(R"(
    QWidget {
        background-color: #f0f3f7;
        font-family: "Microsoft YaHei";
        font-size: 14px;
    }
    QPushButton {
        background-color: #4caf50;
        color: white;
        border-radius: 8px;
        padding: 6px 12px;
    }
    QPushButton:hover {
        background-color: #45a049;
    }
    QSpinBox {
        background-color: white;
        border: 1px solid #ccc;
        border-radius: 5px;
        padding: 2px;
    }
)");

    ui->setupUi(this);

    //ui->surpluslabel->setText("00:00");
    QMenu *helpMenu = menuBar()->addMenu("set");
    QAction *aboutAct = new QAction("about", this);
    QAction *helpAct = new QAction("manual", this);
    helpMenu->addAction(helpAct);
    connect(helpAct, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Instructions for Use",
                                 "How to Use the Tomato Timer\n"
                                 "1. Enter the working time (in minutes), and enter the break time (in minutes). \n"
                                 "2. Click the Start button to enter the working state. \n"
                                 "3. When it's time to take a break, the software will pop up a window to remind you. \n"
                                 "4. After the break is over, you will be reminded again to get back to work. \n"
                                 "5. If no time is set, the software will remind every 5 minutes. \n"
                                 "6. Clicking the Cancel button can stop the timer at any time. \n"
                                 "Suggestion: Maintain a balance between work and rest to protect your eyesight and enhance efficiency");
    });
    helpMenu->addAction(aboutAct);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("C:/cworkspace/image/tomato.png")); // 程序图标
    trayIcon->show();
   // QIcon *image= new QIcon;
   // image->setIcon(QIcon("C:/cworkspace/image/tomato.png"));
    setWindowIcon(QIcon("C:/cworkspace/image/tomato.png"));
    setWindowIconText("tomato time");
    setWindowTitle("tomato time");
    //statusLabel = new QLabel("status", this);
    ui->statusLabel->setText("status");
   ui-> statusLabel->setAlignment(Qt::AlignCenter);
    ui->statusLabel->setStyleSheet("font-size:16px; color:#2c3e50; font-weight:bold;");
   ui->resttimelabel->hide();
    // 把它加到布局里（如果你用 Qt Designer 放了布局，可以用容器）
   // ui->verticalLayout->addWidget(statusLabel);
    // 点击菜单时弹窗显示版权
    connect(aboutAct, &QAction::triggered, this, [=](){
        QMessageBox::about(this, "sofeware",
                           "Tomato Clock v1.0\nCopyright 2025 Alex Zhao\n For learning and personal use only");
    });
    // 初始化计时器
    workTimer = new QTimer(this);
    restTimer = new QTimer(this);
    minuteReminder = new QTimer(this);
    unsetReminder = new QTimer(this);

    workTimer->setInterval(1000 * 60);   // 1分钟
    restTimer->setInterval(1000 * 60);   // 1分钟
    minuteReminder->setInterval(1000 * 60); // 每分钟提醒
    unsetReminder->setInterval(1000 * 60 * 1); // 5分钟
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCountdown);

    timer->start(1000);  // 1000 毫秒 = 1 秒
     surplusTime=0;
    updateCountdown();  // 初始更新一次

    timerRunning = false;
    unsetReminder->start();
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::startTimer);
    connect(ui->btnCancel, &QPushButton::clicked, this, &MainWindow::cancelTimer);

    connect(workTimer, &QTimer::timeout, this, &MainWindow::updateWorkTimer);
    connect(restTimer, &QTimer::timeout, this, &MainWindow::updateRestTimer);
    connect(minuteReminder, &QTimer::timeout, this, &MainWindow::updateWorkTimer);
    connect(unsetReminder, &QTimer::timeout, this, &MainWindow::checkUnsetReminder);
    //label = new QLabel(this);
    //QVBoxLayout *layout = new QVBoxLayout(this);
    //layout->addWidget(label);

    // 创建定时器，每秒触发一次

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startTimer()
{
    workDuration = ui->spinWork->value();
    restDuration = ui->spinRest->value();
    ui->statusLabel->setText("working");
    ui->statusLabel->setStyleSheet("font-size:16px; color:green; font-weight:bold;");
     surplusTime=60*workDuration;// 修改: 设置工作倒计时
    if(workDuration <= 0) {
        QMessageBox::warning(this, "warnning", "please set the worktime!");

        return;
    }

    workPassed = 0;
    restPassed = 0;
    timerRunning = true;

    workTimer->start();
    minuteReminder->start();
    unsetReminder->stop();

    QMessageBox::information(this, "remind", "work time start!");
   // updateCountdown();
}

void MainWindow::cancelTimer()
{
    workTimer->stop();
    restTimer->stop();
    minuteReminder->stop();
    unsetReminder->start();
    ui->statusLabel->setText("rest");
    ui->statusLabel->setStyleSheet("font-size:16px; color:#2c3e50; font-weight:bold;");
    timerRunning = false;
    QMessageBox::information(this, "remind", "time was cancer");
     ui->surpluslabel->setText("00:00");
}

void MainWindow::updateWorkTimer()
{
    if (!timerRunning) return;
    workPassed++;

    if(workPassed >= workDuration) {
        workTimer->stop();
        minuteReminder->stop();

       // QMessageBox::information(this, "remind", "worktime is over ,please have a break!");
         trayIcon->showMessage("remind", "worktime is over ,please have a break!", QSystemTrayIcon::Information, 5000);
        ui->statusLabel->setText("break");
        ui->statusLabel->setStyleSheet("font-size:16px; color:blue; font-weight:bold;");
        if(restDuration > 0) {
            restTimer->start();
        }
    } else {
       // QMessageBox::information(this, "remind", "working ,please have a break!");
         trayIcon->showMessage("remind", "working ,please have a break!", QSystemTrayIcon::Information, 5000);
       ui->statusLabel->setText("break");
       ui->statusLabel->setStyleSheet("font-size:16px; color:blue; font-weight:bold;");

    }
    if(restDuration > 0) {
        restTimer->start();
        surplusTime = 60 * restDuration;  }//在worktimerupadate里赋值， 把work和rest衔接上
}

void MainWindow::updateRestTimer()
{ //if (!timerRunning) return;
    restPassed++;
  // surplusTime=60*restDuration;//(固定值，所以每次重置了)
    updateCountdown();
    if(restPassed >= restDuration) {
        restTimer->stop();
        //QMessageBox::information(this, "remind", "break time is over ,you can start to work!");
         trayIcon->showMessage("remind", "break time is over ,you can start to work!!", QSystemTrayIcon::Information, 5000);
        ui->statusLabel->setText("break");
        ui->statusLabel->setStyleSheet("font-size:16px; color:blue; font-weight:bold;");
        unsetReminder->start();
        timerRunning = false;
        }

}

void MainWindow::checkUnsetReminder()
{
    if(!timerRunning) {
       // QMessageBox::information(this, "remind", "you are not set the worktime");
        trayIcon->showMessage("remind", "you are not set the worktime!", QSystemTrayIcon::Information, 5000);

    }
}
void MainWindow::updateCountdown()

  { if (!timerRunning) return;  // 只有计时器运行时才更新

        if (surplusTime >= 0) {
            // 格式化剩余时间为 "分:秒"（例如 02:30）
            int minutes = surplusTime / 60;
            int seconds = surplusTime % 60;
            ui->surpluslabel->setText(QString("%1:%2")
                               .arg(minutes, 2, 10, QChar('0'))  // 2位宽度，不足补0
                               .arg(seconds, 2, 10, QChar('0')));

            surplusTime--;  // 减少剩余时间
        } else {
            // 倒计时结束，停止定时器
            timer->stop();
            ui->surpluslabel->setText("00:00");  // 显示完成提示
            emit countdownFinished();  // 可选：触发完成信号
        }
    }


