#include "registry/Query.hpp"

registry::size_t registry::Query::m_count = 0;
registry::Query::hashmap_t registry::Query::m_tidsToQid{};
