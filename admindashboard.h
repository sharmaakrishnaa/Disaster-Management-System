#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include <QListWidget>
#include <QString>

class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;
class QResizeEvent;

// INHERITANCE:
// ResponsiveListWidget inherits QListWidget and adds responsive cards.
class ResponsiveListWidget : public QListWidget
{
public:
    explicit ResponsiveListWidget(QWidget *parent = nullptr);

    void addMultilineCard(const QString &text);

protected:
    // POLYMORPHISM: Overrides the inherited resizeEvent().
    void resizeEvent(QResizeEvent *event) override;

private:
    void resizeCards();
};

// INHERITANCE:
// AdminDashboard inherits QWidget.
class AdminDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);

private:
    // Dashboard lists
    ResponsiveListWidget *requestList;
    ResponsiveListWidget *resourceList;

    // Statistics
    QLabel *requestCountLabel;
    QLabel *resourceCountLabel;
    QLabel *pendingCountLabel;
    QLabel *acceptedCountLabel;
    QLabel *progressCountLabel;
    QLabel *resolvedCountLabel;

    // Status and buttons
    QLabel *statusLabel;
    QPushButton *refreshButton;
    QPushButton *updateStatusButton;
    QPushButton *addResourceButton;

    // Request status
    QComboBox *statusInput;

    // Admin resource form
    QComboBox *adminResourceType;
    QLineEdit *adminResourceQuantity;
    QLineEdit *adminResourceLocation;

    // Dashboard operations
    void loadRequests();
    void loadResources();
    void refreshDashboard();
    void updateRequestStatus();

    // Resource management
    void addAdminResource();
};

#endif // ADMINDASHBOARD_H