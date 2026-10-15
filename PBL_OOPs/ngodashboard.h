
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
    QLineEdit *ngoNameInput;
    QLineEdit *locationInput;
    QLineEdit *quantityInput;

    QComboBox *resourceInput;
    QListWidget *resourceList;

    QLabel *statusLabel;

    QPushButton *addButton;
    QPushButton *refreshButton;

    void addResource();
    void loadResources();
};

#endif // NGODASHBOARD_H
