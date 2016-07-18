#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include <exception>
#include <stdexcept>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#ifdef WIN32
#include <windows.h>//For Sleep
#endif

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nFlowRespondingsFlags /*= RESPONDINGS_ALL*/)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword),
   m_SaysRemaining(40), m_bExit(false), m_nFlowRespondingsFlags(nFlowRespondingsFlags)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);

   //Important such that I don't see my messages as new messages :)
   if( !FlowAPILibrary::instance().SetDefaults(m_pFlowdock, m_strUsername, m_strPassword) )
      throw std::runtime_error("Unable to set defaults username/password");

   if( !FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword) )
      throw std::runtime_error("Failed to get user's list");

   FlowAPILibrary::instance().StartListening(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword);

   m_thread = pthread_self();
   int iRet = pthread_create( &m_thread, NULL, FlowHandler::HandleThread, (void*)this);
}

FlowHandler::~FlowHandler()
{
   m_bExit = true;
   pthread_join( m_thread, NULL);
   FlowAPILibrary::instance().Destroy(&m_pFlowdock);
}

void* FlowHandler::HandleThread(void* ptr)
{
   FlowHandler* pThis = (FlowHandler*)ptr;
   while(!pThis->m_bExit)
   {
      pThis->HandleMessages();
#ifdef _WIN32
      Sleep(100);//1/10 second
#else
      usleep(100*1000);
#endif
   }

   return NULL;
}

void FlowHandler::HandleMessages()
{
   int nThreadID = 0;
   std::string strUserName;
   std::string strMessage = FlowAPILibrary::instance().Listen(m_pFlowdock, strUserName, nThreadID);

   if( strMessage.empty() )
      return;

   if( m_SaysRemaining<= 0 )
      return;

   bool bSaidSomething = false;

   //if message see if anything to say

   if( !bSaidSomething )
   {
      if ( strUserName.find( "Andrew" ) != -1 )
      {
         FlowAPILibrary::instance().Tag( m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, "YOLO" );
      }
   }

   if( bSaidSomething )
      m_SaysRemaining--;

}

