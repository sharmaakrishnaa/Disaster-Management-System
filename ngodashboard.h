
#ifndef NGODASHBOARD_H
#define NGODASHBOARD_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QListWidget;
class QLabel;
class QPushButton;

class NGODashboard : public QWidget
{
    Q_OBJECT

public:
    explicit NGODashboard(QWidget *parent = nullptr);

private:
    // Encapsulation: dashboard input fields and controls
    // are kept as private members of this class.
    QLineEdit *ngoNameInput;
    QLineEdit *locationInput;
    QLineEdit *quantityInput;

    QComboBox *resourceInput;
    QListWidget *resourceList;

    QLabel *statusLabel;

    QPushButton *addButton;
    QPushButton *refreshButton;

    // Functions for managing relief resources.
    void addResource();
    void loadResources();
};

#endif // NGODASHBOARD_H
