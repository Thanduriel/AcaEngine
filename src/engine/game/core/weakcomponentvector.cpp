#include "weakcomponentvector.hpp"

namespace game {

	WeakComponentVector::WeakComponentVector(WeakComponentVector&& _oth) noexcept
		: m_clear(_oth.m_clear),
		m_entities(std::move(_oth.m_entities)),
		m_components(std::move(_oth.m_components))
	{
	}

	WeakComponentVector& WeakComponentVector::operator=(WeakComponentVector&& _oth) noexcept
	{
		m_clear = _oth.m_clear;
		m_entities = std::move(_oth.m_entities);
		m_components = std::move(_oth.m_components);

		return *this;
	}

}