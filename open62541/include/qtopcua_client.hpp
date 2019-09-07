#ifndef QT_OPCUA_CLIENT_HPP
#define QT_OPCUA_CLIENT_HPP
#include <open62541/client.h>
#include <qobject.h>

namespace open62541
{

inline QString fromUA_String(const UA_String & src)
{
  return QString::fromLocal8Bit(reinterpret_cast<char*>(src.data), int(src.length));
}

class QOpcUaBrowse
{

  public:
  explicit QOpcUaBrowse     (UA_Client * client);
           QOpcUaBrowse     (QOpcUaBrowse && other);

  QOpcUaBrowse & operator = (QOpcUaBrowse && other);
  ~QOpcUaBrowse();
  size_t beginBrowse(const UA_NodeId &node_id);
  size_t resultSize ();
  const UA_BrowseResult & operator [](size_t idx);
  UA_Client * client(){return m_client;}

private:
   void release();

   UA_BrowseRequest   m_request ;
   UA_BrowseResponse  m_response;
   UA_Client        * m_client  = Q_NULLPTR;
};

class QOpcUaClient : public QObject
{
     Q_OBJECT
     Q_DISABLE_COPY_MOVE(QOpcUaClient)
public:
     explicit QOpcUaClient(QObject * parent = Q_NULLPTR);
             ~QOpcUaClient() override;
     UA_ClientConfig  config() const;
     UA_ClientConfig  config() ;


UA_MessageSecurityMode securityMode   () const;
     bool              setSecurityMode(UA_MessageSecurityMode smode);

     bool              connectToServer     (const QString& end_point, bool async,const QString &user = QString(), const QString & passw = QString()) ;
     void              disconnectFromServer();
     UA_ClientState    currentState() const;
     UA_StatusCode     lastStateCode    ();
     QString           lastStateCodeText();
     static QString    statusCodeText   (UA_StatusCode code);
     QOpcUaBrowse      browser();

signals:
    void  sig_connect   ();
    void  sig_disconnect();
    void  sig_state_changed              (UA_ClientState state);
    void  sig_subscription_inactivity    ( UA_UInt32 subId, void *subContext);
protected :
    virtual void client_state_changed    (UA_ClientState state);
    virtual void subscription_inactivity ( UA_UInt32 subId, void *subContext);

    virtual void clientCreate();
    virtual void clientRelease();

private:
     UA_Client       * m_client      = Q_NULLPTR;
     UA_ClientConfig * m_config      = Q_NULLPTR;
     UA_StatusCode     m_last_status = UA_STATUSCODE_GOOD;
     static  void connect_callback                 (UA_Client *client, void *userdata,UA_UInt32 requestId, void *response);
     static void  state_callback                   (UA_Client *client, UA_ClientState clientState);
     static void  subscription_inactivity_callback (UA_Client *client, UA_UInt32 subId, void *subContext);


};
  
inline UA_ClientConfig  QOpcUaClient::config() const
{
   return  *m_config;
}

inline UA_ClientConfig  QOpcUaClient::config()
{
 return  *m_config;
}

}

#endif
