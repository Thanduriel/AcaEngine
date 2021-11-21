#include "random.hpp"

namespace math {
namespace random {

	thread_local std::default_random_engine g_random(0xa226bf0fu);
}
}