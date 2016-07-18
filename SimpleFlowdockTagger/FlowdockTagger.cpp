#include "FlowdockTagger.h"
#include "FlowHandler.h"

FlowdockTagger::~FlowdockTagger()
{
   for(std::vector<FlowHandler*>::size_type i=0; i<m_apHandlers.size(); i++)
   {
      delete m_apHandlers[i];
   }
}

void FlowdockTagger::AddFlow(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nReplyingFlags /*= RESPONDINGS_ALL*/)
{
   m_apHandlers.push_back(new FlowHandler(strOrg, strFlow, strUsername, strPassword, nReplyingFlags));
}

void FlowdockTagger::HandleMessages()
{
   for(std::vector<FlowHandler*>::size_type i=0; i<m_apHandlers.size(); i++)
   {
      m_apHandlers[i]->HandleMessages();
   }
}
