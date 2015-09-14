#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math_expressions/math_expression_parser.h"
#include "math_expressions/math_expression_evaluator.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QVector>
#include <QToolTip>
#include <QColor>
#include <QMessageBox>

#include <algorithm>
#include <limits>

#include <qcustomplot/qcustomplot.h>

const QRegExp MainWindow::NUMBERS_REGEX("[+,\\-]?[0-9]+(\\.[0-9]+)?");

const QString MainWindow::ABOUT_STR =
    "ChrisHenx::Plotter 1.0"\
    "\n\nCreated by Christian González on april 2015"\
    "\n\nIPN Student";

static const int COLORS_COUNT = 6;

static const QColor COLORS[COLORS_COUNT] = {
    QColor("blue"), QColor("red"), QColor("green"),
    QColor("magenta"), QColor("black"), QColor("yellow")
};

typedef QVector<double> Vector;

using namespace std;
using namespace math_expression;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setWindowState(Qt::WindowMaximized);

    functionPlot = new QCustomPlot();
    horizontalLayout = new QHBoxLayout(ui->tabWidget->widget(1));
    horizontalLayout->addWidget(functionPlot);
    ui->tabWidget->widget(1)->setLayout(horizontalLayout);

    QFont toolTipFont = QToolTip::font();
    toolTipFont.setWeight(20);
    QToolTip::setFont(toolTipFont);

    configure_widgets();
}

MainWindow::~MainWindow()
{
    delete functionPlot;
    delete horizontalLayout;
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionSave_plot_triggered()
{
    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(
                this, "Save", "", "Plot (*.png);;Data (*.csv)",
                &selectedFilter);
    QString toolTipMessage;
    if (!filename.isEmpty()) {
        if (selectedFilter == "Plot (*.png)") {
            if (functionPlot->savePng(filename)) {
                toolTipMessage = "Image saves correctly!";
            } else {
                toolTipMessage = "Error creating image!";
            }
        } else {
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream textStream(&file);
                const int rows = ui->tableWidget->rowCount();
                for (int i = 0; i < rows; i++) {
                    auto item1 = ui->tableWidget->item(i, 0);
                    auto item2 = ui->tableWidget->item(i, 1);
                    textStream << item1->text() << ','
                               << item2->text() << '\n';
                }
                file.flush();
                file.close();
                toolTipMessage = "cvs file create correctly!";
            } else {
                toolTipMessage = "Error creating csv file!";
            }
        }
    }
    show_lineedit_tooltip(toolTipMessage);
}

void MainWindow::show_lineedit_tooltip(const QString& str) const
{
    QToolTip::showText(ui->functionLineEdit->pos(), str,
                       ui->functionLineEdit, QRect(), 8000);
}

void MainWindow::on_actionSupr_graph_triggered()
{
    auto selected_graphs = functionPlot->selectedGraphs();
    for (auto graph : selected_graphs) {
        functionPlot->removeGraph(graph);
    }
    functionPlot->replot();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setText(ABOUT_STR);
    msgBox.setWindowTitle("About ChrisHenx::Ploter");
    msgBox.setIconPixmap(
                QPixmap(":icons/app_icon").scaled(
                    150, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setFont(QFont("Georgia", 10));
    msgBox.exec();
    return;
}

bool MainWindow::valid_numbers() const
{
    bool fromIsNumber = NUMBERS_REGEX.exactMatch(ui->fromLineEdit->text());
    bool toIsNumber = NUMBERS_REGEX.exactMatch(ui->toLineEdit->text());
    bool samplesIsNumber = NUMBERS_REGEX.exactMatch(ui->samplesLineEdit->text());
    return fromIsNumber && toIsNumber && samplesIsNumber;
}

void MainWindow::configure_widgets()
{
    functionPlot->xAxis->setLabel("x");
    functionPlot->yAxis->setLabel("f(x)");
    functionPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                                  QCP::iSelectAxes | QCP::iSelectLegend |
                                  QCP::iSelectPlottables);
    functionPlot->addAction(ui->actionSupr_graph);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"X", "f(X)"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_plotButton_clicked()
{
    if (!valid_numbers()) {
        QMessageBox msgBox(this);
        msgBox.setText(
                    "Some of the following fields has an invalid number format:"
                    "\n\t - 'from\n\t - 'to'\n\t - 'samples/step size'");
        msgBox.setWindowTitle("Error");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setFont(QFont("Georgia", 10));
        msgBox.exec();
        return;
    }

    qDebug() << "Numeros validos";

    auto expression = ui->functionLineEdit->text().toStdString();
    Parser parser(expression);
    Tokens tokens = parser.lexical_analysis();
    if (parser.error() == Parser::NON) {
        parser.sintax_analysis(tokens);
        if (parser.error() == Parser::NON) {
            qDebug() << "No hay error en la expresion";
            Instructions instructions = parser.generate_algorithm(tokens);
            Evaluator evaluator(instructions, tokens);
            evaluator.set_variable_value('e', 2.71828182846);
            evaluator.set_variable_value('p', 3.14159265359);
            const double a = ui->fromLineEdit->text().toDouble();
            const double b = ui->toLineEdit->text().toDouble();
            const double lower_bound = min(a, b);
            const double upper_bound = max(a, b);
            double step = ui->samplesLineEdit->text().toDouble();
            const int option = ui->comboBox->currentIndex();

            if (option == 0) {
                step = (upper_bound - lower_bound) / step;
            } else {
                if (step >= (upper_bound - lower_bound) / 2) {
                    step = (upper_bound - lower_bound) * 0.05;
                    show_lineedit_tooltip(
                                "Steps set automatically "
                                "to 5 percent of the range");
                }
            }

            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);

            int data_lenght = (upper_bound - lower_bound) / step;
            double x = lower_bound;
            Vector x_data(data_lenght);
            Vector y_data(data_lenght);
            double y_min = numeric_limits<double>::max();
            double y_max = numeric_limits<double>::lowest();

            qDebug() << "Computando funcion";
            for (int i = 0; i < data_lenght; i++) {
                x_data[i] = x;
                if (!evaluator.expression_is_constant()) {
                    evaluator.set_variable_value('x', x);
                }
                y_data[i] = evaluator.evaluate();

                ui->tableWidget->insertRow(i);
                auto item1 = new QTableWidgetItem(QString::number(x));
                auto item2 = new QTableWidgetItem(QString::number(y_data[i]));
                item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
                item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
                item1->setTextAlignment(Qt::AlignCenter);
                item2->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(i, 0, item1);
                ui->tableWidget->setItem(i, 1, item2);

                if (y_data[i] < y_min) y_min = y_data[i];
                if (y_data[i] > y_max) y_max = y_data[i];
                x += step;
            }
            qDebug() << "Funcion computada";
            functionPlot->addGraph();
            int last_graph_index = functionPlot->graphCount() - 1;
            QPen pen(COLORS[last_graph_index % COLORS_COUNT]);
            pen.setWidth(3);
            functionPlot->graph(last_graph_index)->setPen(pen);
            functionPlot->graph(last_graph_index)->setData(x_data, y_data);
            functionPlot->xAxis->setRange(lower_bound, upper_bound);
            functionPlot->yAxis->setRange(y_min, y_max);
            functionPlot->replot();
        } else { // Sintax error
            show_lineedit_tooltip("Bad expression sintax");
        }
    } else { // Lexical error
        show_lineedit_tooltip("Bad expression format");
    }
}

void MainWindow::on_youTubeBtn_clicked()
{
    QUrl url("https://www.youtube.com/channel/UCMuuMrfDz0Mh9fQOcbBlffQ");
    QDesktopServices::openUrl(url);
}

void MainWindow::on_andriodBtn_clicked()
{
    QUrl url("https://play.google.com/store/apps/details?id="
             "me.simultaneous_equations_solver.app");
    QDesktopServices::openUrl(url);
}
