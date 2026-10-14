#include "SOSRequestService.h"

namespace SOSConnect
{

// Constructor
SOSRequestService::SOSRequestService()
    : m_nextRequestNumber(1)
{
}

// Create and store a new SOS request
std::string SOSRequestService::createRequest(
    const std::string& victimId,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::vector<AssistanceType>& requiredAssistance,
    const std::string& requestDescription
    )
{
    // Generate a unique request ID
    std::string newRequestId = "SOS-";

    if (m_nextRequestNumber < 10)
    {
        newRequestId += "000";
    }
    else if (m_nextRequestNumber < 100)
    {
        newRequestId += "00";
    }
    else if (m_nextRequestNumber < 1000)
    {
        newRequestId += "0";
    }

    newRequestId += std::to_string(m_nextRequestNumber);

    // Create the new SOS request
    SOSRequest newRequest(
        newRequestId,
        victimId,
        disasterType,
        requestLocation,
        requestUrgency,
        requiredAssistance,
        requestDescription
        );

    // Store the request
    m_sosRequests.push_back(newRequest);

    // Increment the request counter
    ++m_nextRequestNumber;

    return newRequestId;
}

// Find a request by its ID
SOSRequest* SOSRequestService::findRequestById(
    const std::string& requestId
    )
{
    for (SOSRequest& currentRequest : m_sosRequests)
    {
        if (currentRequest.getRequestId() == requestId)
        {
            return &currentRequest;
        }
    }

    // Return nullptr if the request is not found
    return nullptr;
}

// Get read-only access to all requests
const std::vector<SOSRequest>&
SOSRequestService::getAllRequests() const
{
    return m_sosRequests;
}

// Update a request's status
bool SOSRequestService::updateRequestStatus(
    const std::string& requestId,
    RequestStatus newStatus
    )
{
    SOSRequest* targetRequest = findRequestById(requestId);

    if (targetRequest == nullptr)
    {
        return false;
    }

    targetRequest->setRequestStatus(newStatus);

    return true;
}

// Assign a volunteer to a request
bool SOSRequestService::assignVolunteer(
    const std::string& requestId,
    const std::string& volunteerId
    )
{
    // Reject an empty volunteer ID
    if (volunteerId.empty())
    {
        return false;
    }

    SOSRequest* targetRequest = findRequestById(requestId);

    if (targetRequest == nullptr)
    {
        return false;
    }

    targetRequest->setAssignedVolunteerId(volunteerId);

    // Update the request status
    targetRequest->setRequestStatus(RequestStatus::Assigned);

    return true;
}

// Get the total number of requests
std::size_t SOSRequestService::getRequestCount() const
{
    return m_sosRequests.size();
}

} // namespace SOSConnect