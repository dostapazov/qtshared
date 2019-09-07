#include <qtopcua_client.hpp>
#include <open62541/client_config_default.h>
#include <open62541/client_subscriptions.h>
#include <qdebug.h>

namespace open62541 {

QOpcUaClient::QOpcUaClient(QObject * parent ):QObject(parent)
{

}

QOpcUaClient::~QOpcUaClient()
{
  clientRelease();
}


void QOpcUaClient::clientCreate()
{
 if(!m_client)
 {
  m_client = UA_Client_new();
  m_config = UA_Client_getConfig(m_client);
  UA_ClientConfig_setDefault(m_config);
  m_config->clientContext = this;
  m_config->securityPolicyUri = UA_STRING_NULL;
  m_config->securityMode  = UA_MESSAGESECURITYMODE_NONE;
  m_config->stateCallback = state_callback;
  m_config->inactivityCallback = Q_NULLPTR;
  m_config->subscriptionInactivityCallback  = subscription_inactivity_callback;
 }
}

void QOpcUaClient::clientRelease()
{
  if(m_client)
    {
     UA_Client_disconnect(m_client);
     UA_Client_delete    (m_client);
     m_client = Q_NULLPTR;
    }
  m_config = Q_NULLPTR;

}



UA_ClientState     QOpcUaClient::currentState () const
{

  auto ret = m_client ? UA_Client_getState(m_client) : UA_CLIENTSTATE_DISCONNECTED;
  return ret;
}

UA_StatusCode     QOpcUaClient::lastStateCode()
{
  return m_last_status;
}

QString     QOpcUaClient::lastStateCodeText()
{
  return statusCodeText(m_last_status);
}

QString           QOpcUaClient::statusCodeText(UA_StatusCode code)
{
  return QString::fromLocal8Bit(UA_StatusCode_name(code));
}



UA_MessageSecurityMode QOpcUaClient::securityMode() const
{
  return m_config ? m_config->securityMode : UA_MESSAGESECURITYMODE_INVALID;
}

bool                   QOpcUaClient::setSecurityMode(UA_MessageSecurityMode smode)
{

  if(m_config)
     {
      m_config->securityMode = smode;
      return true;
     }
  return false;
}

void QOpcUaClient::connect_callback(UA_Client *client, void *userdata,UA_UInt32 requestId, void *response)
{
   Q_UNUSED(client)
   Q_UNUSED(requestId)
   Q_UNUSED(response)

   auto cl_inst = reinterpret_cast<QOpcUaClient *>(userdata);
   if(cl_inst)
      emit cl_inst->sig_connect();
}


void QOpcUaClient::client_state_changed(UA_ClientState state)
{
  Q_UNUSED(state)
  qDebug()<<Q_FUNC_INFO<<" client_state "<<state;
}

void  QOpcUaClient::state_callback                   (UA_Client *client, UA_ClientState clientState)
{
  auto qclient = reinterpret_cast<QOpcUaClient*>( UA_Client_getContext( client));
  if(qclient)
  {
    qclient->client_state_changed(clientState);
    emit qclient->sig_state_changed(clientState);
  }
}

void  QOpcUaClient::subscription_inactivity ( UA_UInt32 subId, void *subContext)
{
  Q_UNUSED(subId)
  Q_UNUSED(subContext)
}

void  QOpcUaClient::subscription_inactivity_callback (UA_Client *client, UA_UInt32 subId, void *subContext)
{
  auto qclient = reinterpret_cast<QOpcUaClient*>( UA_Client_getContext( client));
  if(qclient)
  {
     qclient->subscription_inactivity(subId, subContext);
     emit qclient->sig_subscription_inactivity(subId, subContext);
  }
}


bool     QOpcUaClient::connectToServer  (const QString &end_point, bool async, const QString &user , const QString & passw )
{
   clientCreate();
   if(m_client)
   {
     auto _end_point = end_point.toLocal8Bit();
     if(user.isEmpty())
     {
       m_config->securityMode  = UA_MESSAGESECURITYMODE_NONE;
       m_last_status = async ?
                      UA_Client_connect_async(m_client,_end_point.data(),&QOpcUaClient::connect_callback, this) :
                      UA_Client_connect      (m_client,_end_point.data()) ;

     }
     else
     {
       m_last_status = UA_Client_connect_username(m_client, _end_point.data(), user.toLocal8Bit().data(), passw.toLocal8Bit().data());
     }
   }
  return m_last_status == UA_STATUSCODE_GOOD;
}

void  QOpcUaClient::disconnectFromServer()
{
  if(m_client)
     {
       m_last_status = UA_Client_disconnect(m_client);
       clientRelease();
     }
}

QOpcUaBrowse  QOpcUaClient::browser()
{
  return QOpcUaBrowse(m_client);
}

}
