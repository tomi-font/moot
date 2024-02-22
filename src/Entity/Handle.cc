#include <Entity/Handle.hh>

void EntityHandle::destroy()
{
    m_arch = nullptr;
    assert(!*this);
}
