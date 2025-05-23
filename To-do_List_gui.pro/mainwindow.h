#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <string>

// Forward declarations to avoid including full headers in the .h file
class QLineEdit;
class QPushButton;
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QMessageBox;

// Includes for menus
class QMenu;
class QAction;
class QMenuBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addTask();
    void markTaskComplete();
    void quitApplication(); // Slot for the Exit action and Quit button
    void saveTasksToFile(); // NEW: Slot to save tasks

private:
    QLineEdit *taskInput;
    QPushButton *addButton;
    QPushButton *completeButton;
    QPushButton *quitButton;
    QListWidget *taskList;

    std::vector<std::string> todoItems;

    void updateTaskListDisplay();
    void loadTasksFromFile(); // NEW: Function to load tasks from file

    // Menu-related members
    QMenu *fileMenu;
    QAction *exitAction;
};

#endif // MAINWINDOW_H
