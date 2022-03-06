#include "random.hpp"

namespace math {
namespace random {

	thread_local DefaultRandomEngine g_random(0xa226BF0Fu);
}
}