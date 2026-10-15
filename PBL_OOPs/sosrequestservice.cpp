
#include "sosrequestservice.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace SOSConnect
{

namespace
{

QString toQString(const std::string& value)
{
    return QString::fromStdString(value);
}

std::string toStdString(const QString& value)
{
    return value.toStdString();
}

DisasterType disasterFromText(const QString& text)
{
    if (text.compare("Flood", Qt::CaseInsensitive) == 0)
        return DisasterType::Flood;
    if (text.compare("Earthquake", Qt::CaseInsensitive) == 0)
        return DisasterType::Earthquake;
    if (text.compare("Fire", Qt::CaseInsensitive) == 0)
        return DisasterType::Fire;
    if (text.compare("Landslide", Qt::CaseInsensitive) == 0)
        return DisasterType::Landslide;
    if (text.compare("Cyclone", Qt::CaseInsensitive) == 0)
        return DisasterType::Cyclone;
    if (text.compare("Medical Emergency", Qt::CaseInsensitive) == 0)
        return DisasterType::MedicalEmergency;

    return DisasterType::Other;
}

QString disasterToText(DisasterType type)
{
    switch (type)
    {
    case DisasterType::Flood: return "Flood";
    case DisasterType::Earthquake: return "Earthquake";
    case DisasterType::Fire: return "Fire";
    case DisasterType::Landslide: return "Landslide";
    case DisasterType::Cyclone: return "Cyclone";
    case DisasterType::MedicalEmergency: return "Medical Emergency";
    case DisasterType::Other: return "Other";
    }

    return "Other";
}

Urgency urgencyFromText(const QString& text)
{
    if (text.compare("Low", Qt::CaseInsensitive) == 0)
        return Urgency::Low;
    if (text.compare("Medium", Qt::CaseInsensitive) == 0)
        return Urgency::Medium;
    if (text.compare("High", Qt::CaseInsensitive) == 0)
        return Urgency::High;

    return Urgency::Critical;
}

QString urgencyToText(Urgency urgency)
{
    switch (urgency)
    {
    case Urgency::Low: return "Low";
    case Urgency::Medium: return "Medium";
    case Urgency::High: return "High";
    case Urgency::Critical: return "Critical";
    }

    return "Medium";
}

RequestStatus statusFromText(const QString& text)
{
    if (text.compare("Accepted", Qt::CaseInsensitive) == 0 ||
        text.compare("Assigned", Qt::CaseInsensitive) == 0)
        return RequestStatus::Assigned;

    if (text.compare("In Progress", Qt::CaseInsensitive) == 0)
        return RequestStatus::InProgress;

    if (text.compare("Resolved", Qt::CaseInsensitive) == 0)
        return RequestStatus::Resolved;

    return RequestStatus::Raised;
}

QString statusToText(RequestStatus status)
{
    switch (status)
    {
    case RequestStatus::Raised: return "Pending";
    case RequestStatus::Assigned: return "Accepted";
    case RequestStatus::InProgress: return "In Progress";
    case RequestStatus::Resolved: return "Resolved";
    }

    return "Pending";
}

QString assistanceToText(
    const std::vector<AssistanceType>& assistance)
{
    QStringList values;

    for (AssistanceType item : assistance)
    {
        switch (item)
        {
        case AssistanceType::Food:
            values << "Food";
            break;
        case AssistanceType::Water:
            values << "Water";
            break;
        case AssistanceType::Medicine:
            values << "Medicine";
            break;
        case AssistanceType::Blankets:
            values << "Blankets";
            break;
        case AssistanceType::Shelter:
            values << "Shelter";
            break;
        case AssistanceType::Rescue:
            values << "Rescue";
            break;
        }
    }

    return values.join(", ");
}

std::vector<AssistanceType> assistanceFromText(
    const QString& text)
{
    std::vector<AssistanceType> result;
    const QString lower = text.toLower();

    if (lower.contains("food"))
        result.push_back(AssistanceType::Food);

    if (lower.contains("water"))
        result.push_back(AssistanceType::Water);

    if (lower.contains("medicine") || lower.contains("medical"))
        result.push_back(AssistanceType::Medicine);

    if (lower.contains("blanket"))
        result.push_back(AssistanceType::Blankets);

    if (lower.contains("shelter"))
        result.push_back(AssistanceType::Shelter);

    if (lower.contains("rescue"))
        result.push_back(AssistanceType::Rescue);

    return result;
}

} // namespace


SOSRequestService::SOSRequestService()
    : m_nextRequestNumber(1)
{
    QString dataDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

    if (!dataDirectory.isEmpty())
        QDir().mkpath(dataDirectory);

    // Keep service storage separate from the existing dashboards.
    m_storageFilePath =
        toStdString(dataDirectory + "/sos_service_requests.txt");

    loadRequests();
}


bool SOSRequestService::loadRequests()
{
    m_sosRequests.clear();
    m_nextRequestNumber = 1;

    QFile file(toQString(m_storageFilePath));

    if (!file.exists())
        return true;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QString record;
    QStringList records;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.trimmed() ==
            "----------------------------------------")
        {
            if (!record.trimmed().isEmpty())
                records << record;

            record.clear();
        }
        else
        {
            record += line + "\n";
        }
    }

    if (!record.trimmed().isEmpty())
        records << record;

    file.close();

    for (const QString& currentRecord : records)
    {
        QString requestId;
        QString victimName;
        QString location;
        QString disaster;
        QString urgency;
        QString help;
        QString description;
        QString createdAt;
        QString status = "Pending";

        const QStringList lines = currentRecord.split('\n');

        for (const QString& rawLine : lines)
        {
            QString line = rawLine.trimmed();

            if (line.startsWith("Request ID:"))
                requestId = line.mid(11).trimmed();
            else if (line.startsWith("Victim:"))
                victimName = line.mid(7).trimmed();
            else if (line.startsWith("Location:"))
                location = line.mid(9).trimmed();
            else if (line.startsWith("Disaster:"))
                disaster = line.mid(9).trimmed();
            else if (line.startsWith("Urgency:"))
                urgency = line.mid(8).trimmed();
            else if (line.startsWith("Help Required:"))
                help = line.mid(14).trimmed();
            else if (line.startsWith("Description:"))
                description = line.mid(12).trimmed();
            else if (line.startsWith("Created At:"))
                createdAt = line.mid(11).trimmed();
            else if (line.startsWith("Status:"))
                status = line.mid(7).trimmed();
        }

        if (requestId.isEmpty())
            continue;

        SOSRequest request(
            toStdString(requestId),
            toStdString(victimName),
            disasterFromText(disaster),
            toStdString(location),
            urgencyFromText(urgency),
            assistanceFromText(help),
            toStdString(description)
            );

        request.setVictimName(toStdString(victimName));
        request.setHelpRequired(toStdString(help));
        request.setCreatedAt(toStdString(createdAt));
        request.setRequestStatus(statusFromText(status));

        m_sosRequests.push_back(request);

        if (requestId.startsWith("SOS-"))
        {
            bool ok = false;
            unsigned int number = requestId.mid(4).toUInt(&ok);

            if (ok && number >= m_nextRequestNumber)
                m_nextRequestNumber = number + 1;
        }
    }

    return true;
}


bool SOSRequestService::saveRequests() const
{
    QFile file(toQString(m_storageFilePath));

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Truncate |
            QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);

    for (const SOSRequest& request : m_sosRequests)
    {
        out << "Request ID: "
            << toQString(request.getRequestId()) << "\n";

        out << "Victim: "
            << toQString(request.getVictimName()) << "\n";

        out << "Location: "
            << toQString(request.getRequestLocation()) << "\n";

        out << "Disaster: "
            << disasterToText(request.getDisasterType()) << "\n";

        out << "Urgency: "
            << urgencyToText(request.getRequestUrgency()) << "\n";

        out << "Help Required: "
            << toQString(request.getHelpRequired()) << "\n";

        out << "Description: "
            << toQString(request.getRequestDescription()) << "\n";

        out << "Created At: "
            << toQString(request.getCreatedAt()) << "\n";

        out << "Status: "
            << statusToText(request.getRequestStatus()) << "\n";

        out << "----------------------------------------\n";
    }

    file.close();

    return file.error() == QFileDevice::NoError;
}


std::string SOSRequestService::createRequest(
    const std::string& victimId,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::vector<AssistanceType>& requiredAssistance,
    const std::string& requestDescription)
{
    QString newRequestId;

    do
    {
        newRequestId = QString("SOS-%1")
        .arg(m_nextRequestNumber++, 4, 10, QChar('0'));
    }
    while (findRequestById(toStdString(newRequestId)) != nullptr);

    SOSRequest request(
        toStdString(newRequestId),
        victimId,
        disasterType,
        requestLocation,
        requestUrgency,
        requiredAssistance,
        requestDescription
        );

    request.setVictimName(victimId);
    request.setHelpRequired(
        toStdString(assistanceToText(requiredAssistance)));

    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request.setCreatedAt(toStdString(now));
    request.setUpdatedAt(toStdString(now));

    m_sosRequests.push_back(request);

    if (!saveRequests())
    {
        m_sosRequests.pop_back();
        return "";
    }

    return toStdString(newRequestId);
}


// New function: accepts the ID generated by Victim Dashboard.
std::string SOSRequestService::createRequestWithId(
    const std::string& requestId,
    const std::string& victimId,
    const std::string& victimName,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::string& helpRequired,
    const std::string& requestDescription,
    const std::string& createdAt)
{
    if (requestId.empty() ||
        findRequestById(requestId) != nullptr)
    {
        return "";
    }

    std::vector<AssistanceType> requiredAssistance =
        assistanceFromText(toQString(helpRequired));

    SOSRequest request(
        requestId,
        victimId,
        disasterType,
        requestLocation,
        requestUrgency,
        requiredAssistance,
        requestDescription
        );

    request.setVictimName(victimName);
    request.setHelpRequired(helpRequired);
    request.setCreatedAt(createdAt);
    request.setRequestStatus(RequestStatus::Raised);
    request.setUpdatedAt(createdAt);

    m_sosRequests.push_back(request);

    if (!saveRequests())
    {
        m_sosRequests.pop_back();
        return "";
    }

    return requestId;
}


SOSRequest* SOSRequestService::findRequestById(
    const std::string& requestId)
{
    for (SOSRequest& request : m_sosRequests)
    {
        if (request.getRequestId() == requestId)
            return &request;
    }

    return nullptr;
}


const std::vector<SOSRequest>&
SOSRequestService::getAllRequests() const
{
    return m_sosRequests;
}


bool SOSRequestService::updateRequestStatus(
    const std::string& requestId,
    RequestStatus newStatus)
{
    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    RequestStatus oldStatus = request->getRequestStatus();
    std::string oldUpdatedAt = request->getUpdatedAt();

    request->setRequestStatus(newStatus);

    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request->setUpdatedAt(toStdString(now));

    if (!saveRequests())
    {
        request->setRequestStatus(oldStatus);
        request->setUpdatedAt(oldUpdatedAt);
        return false;
    }

    return true;
}


bool SOSRequestService::assignVolunteer(
    const std::string& requestId,
    const std::string& volunteerId)
{
    if (volunteerId.empty())
        return false;

    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    std::string oldVolunteer =
        request->getAssignedVolunteerId();

    RequestStatus oldStatus = request->getRequestStatus();
    std::string oldUpdatedAt = request->getUpdatedAt();

    request->setAssignedVolunteerId(volunteerId);
    request->setRequestStatus(RequestStatus::Assigned);

    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request->setUpdatedAt(toStdString(now));

    if (!saveRequests())
    {
        request->setAssignedVolunteerId(oldVolunteer);
        request->setRequestStatus(oldStatus);
        request->setUpdatedAt(oldUpdatedAt);
        return false;
    }

    return true;
}


std::size_t SOSRequestService::getRequestCount() const
{
    return m_sosRequests.size();
}


bool SOSRequestService::updateVictimDetails(
    const std::string& requestId,
    const std::string& victimName,
    const std::string& helpRequired,
    const std::string& createdAt)
{
    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    std::string oldName = request->getVictimName();
    std::string oldHelp = request->getHelpRequired();
    std::string oldCreatedAt = request->getCreatedAt();

    request->setVictimName(victimName);
    request->setHelpRequired(helpRequired);
    request->setCreatedAt(createdAt);

    if (!saveRequests())
    {
        request->setVictimName(oldName);
        request->setHelpRequired(oldHelp);
        request->setCreatedAt(oldCreatedAt);
        return false;
    }

    return true;
}

} // namespace SOSConnect
