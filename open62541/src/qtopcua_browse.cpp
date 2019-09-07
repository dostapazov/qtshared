#include <qtopcua_client.hpp>


namespace open62541 {

  QOpcUaBrowse::QOpcUaBrowse ( UA_Client * client):m_client(client)
  {
   UA_BrowseRequest_init(&m_request);
   UA_BrowseResponse_init(&m_response);
  }

  QOpcUaBrowse::~QOpcUaBrowse()
  {
    release();
  }

  void QOpcUaBrowse::release()
  {
    UA_BrowseRequest_clear (&m_request);
    UA_BrowseResponse_clear(&m_response);
  }

  QOpcUaBrowse::QOpcUaBrowse     (QOpcUaBrowse && other)
  {
    //*this = other;
  }

  QOpcUaBrowse & QOpcUaBrowse::operator = (QOpcUaBrowse &&other)
  {
   release();
   m_client  = other.m_client;
   UA_BrowseRequest_copy(&other.m_request, &m_request);
   UA_BrowseRequest_clear(&other.m_request);

   UA_BrowseResponse_copy(&other.m_response, &m_response);
   UA_BrowseResponse_clear(&other.m_response);

   return *this;
  }

  const UA_BrowseResult & QOpcUaBrowse::operator [](size_t idx)
  {
    idx = qMin(idx, resultSize());
    return *(m_response.results+idx);
  }

  size_t  QOpcUaBrowse::resultSize()
  {
   return (m_client && m_response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) ?
           m_response.resultsSize : 0;
  }

  size_t  QOpcUaBrowse::beginBrowse(const UA_NodeId & node_id)
  {

    if(m_client)
    {
      m_request.requestedMaxReferencesPerNode = 0;
      if(!m_request.nodesToBrowse)
          m_request.nodesToBrowse = UA_BrowseDescription_new();
      m_request.nodesToBrowseSize = size_t(1);
      m_request.nodesToBrowse->nodeId = node_id ;
      m_request.nodesToBrowse->resultMask = UA_BROWSERESULTMASK_ALL;
      m_response = UA_Client_Service_browse(m_client, m_request);

    }
   return resultSize();
  }

}
