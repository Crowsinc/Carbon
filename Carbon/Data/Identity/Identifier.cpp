#include "Identifier.hpp"

#include "Identifiable.hpp"

namespace cbn
{
    //-------------------------------------------------------------------------------------

    uint64_t Identifier::s_NextAliasID = 1;
    std::unordered_map<std::string, uint64_t> Identifier::s_AliasIDs = {{"", 0}};

    //-------------------------------------------------------------------------------------
    
    void Identifier::update_alias(const String& alias)
    {
        // If the alias has already been registered, then just take its UID.
        // Otherwise, register a new alias to get a new UID
        m_Alias = alias;
        if(s_AliasIDs.count(alias))
        {
            m_AliasID = s_AliasIDs.at(alias);
        }
        else
        {
            m_AliasID = s_NextAliasID++;
            s_AliasIDs.emplace(alias, m_AliasID);
        }
    }
    
    //-------------------------------------------------------------------------------------

    Identifier::Identifier()
        : Identifier("") 
    {}
    
    //-------------------------------------------------------------------------------------

    Identifier::Identifier(const char* alias)
        : Identifier(String{alias})
    {}

    //-------------------------------------------------------------------------------------

    Identifier::Identifier(const String& alias)
    {
        update_alias(alias);
    }

    //-------------------------------------------------------------------------------------

    Identifier::Identifier(const Identifier& copy)
        : m_AliasID(copy.m_AliasID),
        m_Alias(copy.m_Alias)
    {}

    //-------------------------------------------------------------------------------------

    uint64_t Identifier::alias_id() const
    {
        return m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    const String& Identifier::alias() const
    {
        return m_Alias;
    }
    
    //-------------------------------------------------------------------------------------

    void Identifier::operator=(const char* alias)
    {
        update_alias(String{alias});
    }

    //-------------------------------------------------------------------------------------

    void Identifier::operator=(const String& alias)
    {
        update_alias(alias);
    }

    //-------------------------------------------------------------------------------------

    void Identifier::operator=(const Identifier& other)
    {
        m_AliasID = other.m_AliasID;
        m_Alias = other.m_Alias;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator>(const Identifier& other) const
    {
        return m_AliasID > other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator<(const Identifier& other) const
    {
        return m_AliasID < other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator==(const Identifier& other) const
    {
        return m_AliasID == other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator!=(const Identifier& other) const
    {
        return m_AliasID != other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator>=(const Identifier& other) const
    {
        return m_AliasID >= other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------

    bool Identifier::operator<=(const Identifier& other) const
    {
        return m_AliasID <= other.m_AliasID;
    }

    //-------------------------------------------------------------------------------------
}