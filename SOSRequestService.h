#ifndef SOSREQUESTSERVICE_H
#define SOSREQUESTSERVICE_H

#include "SOSRequest.h"
#include <cstddef>
#include <string>
#include <vector>

namespace SOSConnect
{

class SOSRequestService
{
private:

    // Stores all SOS requests managed by this service
    std::vector<SOSRequest> m_sosRequests;

    // Generates unique request IDs
    unsigned int m_nextRequestNumber;

public:

    // Constructor
    SOSRequestService();

    // Create and store a new SOS request
    std::string createRequest(
        const std::string& victimId,
        DisasterType disasterType,
        const std::string& requestLocation,
        Urgency requestUrgency,
        const std::vector<AssistanceType>& requiredAssistance,
        const std::string& requestDescription
        );

    // Find a request by its ID
    SOSRequest* findRequestById(
        const std::string& requestId
        );

    // Get read-only access to all requests
    const std::vector<SOSRequest>& getAllRequests() const;

    // Update a request's status
    bool updateRequestStatus(
        const std::string& requestId,
        RequestStatus newStatus
        );

    // Assign a volunteer to a request
    bool assignVolunteer(
        const std::string& requestId,
        const std::string& volunteerId
        );

    // Get the total number of requests
    std::size_t getRequestCount() const;
};

} // namespace SOSConnect

#endif // SOSREQUESTSERVICE_H