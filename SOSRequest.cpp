#include "SOSRequest.h"

namespace SOSConnect
{

// Constructor
SOSRequest::SOSRequest(
    const std::string& requestId,
    const std::string& victimId,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::vector<AssistanceType>& requiredAssistance,
    const std::string& requestDescription
    )
    : m_requestId(requestId),
    m_victimId(victimId),
    m_disasterType(disasterType),
    m_requestLocation(requestLocation),
    m_requestUrgency(requestUrgency),
    m_requiredAssistance(requiredAssistance),
    m_requestDescription(requestDescription),
    m_requestStatus(RequestStatus::Raised),
    m_assignedVolunteerId(""),
    m_createdAt(""),
    m_updatedAt("")
{
}

// Getters

const std::string& SOSRequest::getRequestId() const
{
    return m_requestId;
}

const std::string& SOSRequest::getVictimId() const
{
    return m_victimId;
}

DisasterType SOSRequest::getDisasterType() const
{
    return m_disasterType;
}

const std::string& SOSRequest::getRequestLocation() const
{
    return m_requestLocation;
}

Urgency SOSRequest::getRequestUrgency() const
{
    return m_requestUrgency;
}

const std::vector<AssistanceType>& SOSRequest::getRequiredAssistance() const
{
    return m_requiredAssistance;
}

const std::string& SOSRequest::getRequestDescription() const
{
    return m_requestDescription;
}

RequestStatus SOSRequest::getRequestStatus() const
{
    return m_requestStatus;
}

const std::string& SOSRequest::getAssignedVolunteerId() const
{
    return m_assignedVolunteerId;
}

const std::string& SOSRequest::getCreatedAt() const
{
    return m_createdAt;
}

const std::string& SOSRequest::getUpdatedAt() const
{
    return m_updatedAt;
}

// Setters

void SOSRequest::setRequestStatus(RequestStatus newStatus)
{
    m_requestStatus = newStatus;
}

void SOSRequest::setAssignedVolunteerId(
    const std::string& volunteerId
    )
{
    m_assignedVolunteerId = volunteerId;
}

void SOSRequest::setUpdatedAt(
    const std::string& updatedAt
    )
{
    m_updatedAt = updatedAt;
}

} // namespace SOSConnect