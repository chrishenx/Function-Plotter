#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExp>

namespace Ui {
  class MainWindow;
}

class QCustomPlot;
class QHBoxLayout;

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_actionExit_triggered();
  void on_actionSave_plot_triggered();
  void on_actionSupr_graph_triggered();
  void on_actionAbout_triggered();
  void on_plotButton_clicked();

  void on_youTubeBtn_clicked();

  void on_andriodBtn_clicked();

private:
  Ui::MainWindow *ui;

  QCustomPlot* functionPlot;
  QHBoxLayout* horizontalLayout;

  static const QRegExp NUMBERS_REGEX;
  static const QString ABOUT_STR;

  bool valid_numbers() const;
  void configure_widgets();
  void show_lineedit_tooltip(const QString& str) const;
};

#endif // MAINWINDOW_H
