
#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include <QListWidget>

class QLabel;
class QPushButton;
class QComboBox;
class QResizeEvent;

// Custom list that supports complete, multiline cards.
class ResponsiveListWidget : public QListWidget
{
public:
    explicit ResponsiveListWidget(QWidget *parent = nullptr);

    void addMultilineCard(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void resizeCards();
};

class AdminDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);

private:
    ResponsiveListWidget *requestList;
    ResponsiveListWidget *resourceList;

    QLabel *requestCountLabel;
    QLabel *resourceCountLabel;

    QLabel *pendingCountLabel;
    QLabel *acceptedCountLabel;
    QLabel *progressCountLabel;
    QLabel *resolvedCountLabel;

    QLabel *statusLabel;

    QPushButton *refreshButton;
    QPushButton *updateStatusButton;

    QComboBox *statusInput;

    void loadRequests();
    void loadResources();
    void refreshDashboard();
    void updateRequestStatus();
};

#endif // ADMINDASHBOARD_H
