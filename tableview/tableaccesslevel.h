#ifndef TABLEACCESSLEVEL_H
#define TABLEACCESSLEVEL_H

#include <set>
namespace priv{
enum class acc { admin=10,direktur,supervisor,pgtetap,pgtraining,undefined};
enum class act { tambah=100,edit,hapus,select,exportexcel,importexcel};

class TableAccessLevel
{
public:
    TableAccessLevel()
    { 
        m_action.clear();
    }
    ~TableAccessLevel()
    {
        m_action.clear();
    }
    
    bool isset()const{return set;}
    void setsetting(const priv::acc &context, const
                   std::set<priv::act> &action )
    
    {
        m_context=context;
        m_action=action;
        set=true;
    }
    priv::acc access()const
    {
        return m_context;
    }
    std::set<priv::act> actions()const
    {
        return m_action;
    }

private:
    bool set=false;
    priv::acc m_context=acc::undefined;
    std::set<priv::act> m_action;
};
}

#endif // TABLEACCESSLEVEL_H

