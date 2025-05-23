#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

// Includes for menus
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QApplication> // Needed for qApp->quit() and QApplication::aboutToQuit()

// NEW: Includes for file operations
#include <QFile>       // For file handling
#include <QTextStream> // For reading and writing text to files

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simple To-Do List");
    setFixedSize(400, 600);

    // --- Menu Bar Setup ---
    QMenuBar *menuBar = this->menuBar();
    fileMenu = menuBar->addMenu("File");
    exitAction = new QAction("Exit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, &MainWindow::quitApplication);
    // --- End Menu Bar Setup ---


    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    taskInput = new QLineEdit(this);
    taskInput->setPlaceholderText("Enter a new task...");
    inputLayout->addWidget(taskInput);
    addButton = new QPushButton("Add Task", this);
    inputLayout->addWidget(addButton);
    mainLayout->addLayout(inputLayout);

    taskList = new QListWidget(this);
    mainLayout->addWidget(taskList);

    completeButton = new QPushButton("Mark Selected as Complete", this);
    mainLayout->addWidget(completeButton);

    quitButton = new QPushButton("Quit", this);
    mainLayout->addWidget(quitButton);

    // --- Styling additions ---
    taskInput->setStyleSheet(
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "}"
    );

    addButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;" // Green
        "   color: white;"
        "   padding: 8px 15px;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
    );

    completeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;" // Red
        "   color: white;"
        "   padding: 8px 15px;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}"
    );

    quitButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #555555;" // Dark gray
        "   color: white;"
        "   padding: 8px 15px;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #333333;"
        "}"
    );

    taskList->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ddd;"
        "   border-radius: 5px;"
        "   background-color: #5a6b78;"
        "   font-size: 14px;"
        "}"
        "QListWidget::item {"
        "   padding: 5px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #a8d9ff;" // Light blue for selected item
        "   color: black;"
        "}"
    );
    // --- End Styling additions ---

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(completeButton, &QPushButton::clicked, this, &MainWindow::markTaskComplete);
    connect(taskInput, &QLineEdit::returnPressed, this, &MainWindow::addTask);
    connect(quitButton, &QPushButton::clicked, this, &MainWindow::quitApplication);

    // NEW: Connect QApplication's aboutToQuit signal to our saveTasksToFile slot
    connect(QApplication::instance(), &QApplication::aboutToQuit, this, &MainWindow::saveTasksToFile);

    // NEW: Load tasks when the application starts
    loadTasksFromFile();
}

MainWindow::~MainWindow()
{
    // Qt's parent-child ownership model handles deletion of child widgets.
}

void MainWindow::addTask()
{
    QString taskText = taskInput->text().trimmed();

    if (!taskText.isEmpty()) {
        todoItems.push_back(taskText.toStdString());
        taskList->addItem(taskText);
        taskInput->clear();
    } else {
        QMessageBox::warning(this, "Input Error", "Task cannot be empty!");
    }
}

void MainWindow::markTaskComplete()
{
    QList<QListWidgetItem*> selectedItems = taskList->selectedItems();

    if (!selectedItems.isEmpty()) {
        for (QListWidgetItem *item : selectedItems) {
            int row = taskList->row(item);
            delete taskList->takeItem(row);

            if (static_cast<size_t>(row) < todoItems.size()) {
                todoItems.erase(todoItems.begin() + row);
            }
        }
    } else {
        QMessageBox::information(this, "Selection", "Please select a task to mark as complete.");
    }
}

void MainWindow::updateTaskListDisplay()
{
    taskList->clear();
    for (const std::string& item : todoItems) {
        taskList->addItem(QString::fromStdString(item));
    }
}

void MainWindow::quitApplication()
{
    QApplication::quit();
}

// NEW: Implementation of loadTasksFromFile()
void MainWindow::loadTasksFromFile()
{
    // Define the file name where tasks will be stored.
    QString fileName = "tasks.txt";
    QFile file(fileName); // Create a QFile object for the file.

    // Try to open the file in read-only mode.
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // If the file doesn't exist or can't be opened, it's fine for the first run.
        // QMessageBox::warning(this, "Error", "Could not open tasks file for reading: " + file.errorString());
        return; // Exit if we can't open the file (e.g., it doesn't exist yet).
    }

    QTextStream in(&file); // Create a QTextStream to read text from the file.

    // Read each line from the file until the end.
    while (!in.atEnd()) {
        QString line = in.readLine(); // Read one line.
        if (!line.isEmpty()) {
            todoItems.push_back(line.toStdString()); // Add to internal vector.
            taskList->addItem(line); // Add to QListWidget display.
        }
    }

    file.close(); // Close the file.
}

// NEW: Implementation of saveTasksToFile()
void MainWindow::saveTasksToFile()
{
    // Define the file name.
    QString fileName = "tasks.txt";
    QFile file(fileName); // Create a QFile object.

    // Try to open the file in write-only mode, truncating it (clearing existing content).
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, "Error", "Could not open tasks file for writing: " + file.errorString());
        return; // Exit if we can't open the file.
    }

    QTextStream out(&file); // Create a QTextStream to write text to the file.

    // Iterate through the internal todoItems vector and write each task to a new line.
    for (const std::string& task : todoItems) {
        out << QString::fromStdString(task) << "\n"; // Write task and a newline.
    }

    file.close(); // Close the file.
}
