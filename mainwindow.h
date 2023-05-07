
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QLineEdit;
class QLabel;

class MainWindow : public QMainWindow

{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLineEdit *number;
    QLineEdit *name;
    QLineEdit *version;

    QLabel *error;
};

#endif // MAINWINDOW_H
