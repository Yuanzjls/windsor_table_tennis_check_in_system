#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /********************************/
    /***********Connect database*****/
    ttdb = QSqlDatabase::addDatabase("QSQLITE");
    ttdb.setDatabaseName("D:\\Laboratory\\Qt\\Table_tennis_management\\table_tennis.db");

    if (!ttdb.open())
    {
        qDebug() << "Cannot Open the data base file";
    }
    else
    {
      qDebug() << "Database: connection ok";
    }
    QSqlQuery query(ttdb);
    query.prepare("SELECT \"User_Name\" From \"User_Info\"");
    query.exec();
    int idName = query.record().indexOf("User_Name");
    while (query.next())
    {
       QString name = query.value(idName).toString();

       ui->nameselect->addItem(name); // combobox add item
    }
    /*******************************/
    UpdateTableview();
}

MainWindow::~MainWindow()
{
    ttdb.close();
    delete ui;
}



void MainWindow::on_CheckInPB_clicked()
{
    QSqlQuery query(ttdb);
    QString SelectName = ui->nameselect->currentText();
    QString querystring;

    /******************** check check in histroy, in order to prevent multiple check-ins************/
    querystring = "SELECT * FROM \"Check_In\" WHERE (SELECT \"ID\" From \"User_Info\" Where \"User_Name\" =  \"" + SelectName \
            +  "\") = \"User_ID\" AND \"Date\" = date('now', 'localtime')";
    query.prepare(querystring);
    query.exec();
    if (query.next())
    {
        QMessageBox::information(this, "Check In", "You have checked in today");
        return;
    }
    /******************** check check in histroy, in order to prevent multiple check-ins************/
    /******** get how many times you have left for check in *****************/
    querystring = "SELECT \"Times\" From \"User_Info\" Where \"User_Name\" = \"" + ui->nameselect->currentText() + "\"";
    query.prepare(querystring);
    if (!query.exec())
    {
        QMessageBox::information(this, "Check In", "Get Times Error");
        return;
    }
    if (!query.next())
    {
        return;
    }
    int idName = query.record().indexOf("Times");
    int times = query.value(idName).toInt();
    /******** get how many times you have left for check in *****************/


    if (times > 0)
    {
        querystring = "INSERT INTO \"Check_In\" (\"User_ID\", \"Date\", \"Time_Stamp\") \
                VALUES ((SELECT \"ID\" From \"User_Info\" Where \"User_Name\" = \"" + SelectName + "\")\
                ,  date('now', 'localtime'), CURRENT_TIMESTAMP)";
        query.prepare(querystring);
        if (!query.exec())
        {
            QMessageBox::information(this, "Check In", "Failed, You have checked in today");
            return;
        }
        querystring = "UPDATE \"User_Info\" Set \"Times\" = \"Times\" - 1 Where \"User_Name\" = \"" + SelectName + "\"";
        query.prepare(querystring);
        query.exec();
        UpdateTableview();
        QMessageBox::information(this, "Check In", "Success");
    }
    else
    {
        QMessageBox::information(this, "Check In", "Failed, You haven't paid");
    }
}

void MainWindow::on_PayPB_clicked()
{
    QSqlQuery query(ttdb);
    QString SelectName = ui->nameselect->currentText();
    QString querystring = "UPDATE \"User_Info\" Set \"Times\" = 1 Where \"User_Name\" = \"" + SelectName + "\"";
    query.prepare(querystring);
    if (query.exec())
    {
        QMessageBox::information(this, "Payment", "You paid success");
        querystring = "INSERT INTO \"Payment\" (\"User_ID\", \"Date_Paid\", \"Time_Stamp\") \
           VALUES ((SELECT \"ID\" From \"User_Info\" Where \"User_Name\" = \"" + SelectName + "\") \
                , date('now', 'localtime'), CURRENT_TIMESTAMP)";
        query.prepare(querystring);
        query.exec();
    }
    else
    {
        QMessageBox::information(this, "Payment", "You paid failed");
    }

}

void MainWindow::on_AddPB_clicked()
{
    QSqlQuery query(ttdb);
    QString NameInput = ui->NameLE->text();
    QString querystring = "INSERT INTO \"User_Info\" (\"User_Name\" ,\"Time_Add\")\
            VALUES( \""+ NameInput +"\",	CURRENT_TIMESTAMP)";

    query.prepare(querystring);
    if (!query.exec())
    {
        QMessageBox::information(this, "Add Person", "Failed, This person is already in the database");
        return;
    }
    ui->nameselect->addItem(NameInput);
    QMessageBox::information(this, "Add Person", "Success, You add a person into the database");
}

void MainWindow::UpdateTableview()
{
    QSqlQuery query(ttdb);
    QSqlQueryModel *model = new QSqlQueryModel;

    QString querystring = "SELECT \"User_Info\".\"User_Name\" As \"Name\", time(\"Check_In\".\"Time_Stamp\", 'localtime') As \"Time Enter\" From \"User_Info\" INNER JOIN \"Check_In\" On \"User_Info\".\"ID\" = \"Check_In\".\"User_ID\"";
    query.prepare(querystring);
    query.exec();
    model->setQuery(query);
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0, ui->tableView->width()/2);
    ui->tableView->setColumnWidth(1, ui->tableView->width()/2);
}
