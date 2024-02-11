#include <Component/Types.hh>

static_assert(sizeof(ComponentIndex) * 8 >= std::tuple_size_v<Components>);
