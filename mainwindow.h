#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>

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


    void on_CheckInPB_clicked();

    void on_PayPB_clicked();

    void on_AddPB_clicked();



private:
    Ui::MainWindow *ui;
    QSqlDatabase ttdb;
    void UpdateTableview(void);
};
#endif // MAINWINDOW_H
