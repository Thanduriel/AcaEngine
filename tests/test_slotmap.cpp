#include "testutils.hpp"

#include <engine/utils/containers/slotmap2.hpp>

int constructed = 0;
int moveConstructed = 0;
int destroyed = 0;

struct Dummy
{
	Dummy(const std::string& _str) : s(_str) { ++constructed; }
	Dummy(Dummy&& _oth) : s(std::move(_oth.s)) { ++moveConstructed; }
	Dummy(const Dummy& _oth) : s(_oth.s) { ++moveConstructed; }
	~Dummy() { ++destroyed; }

	Dummy& operator=(const Dummy& _oth)
	{
		s = _oth.s; 
		return *this;
	}

	Dummy& operator=(Dummy&& _oth)
	{
		s = std::move(_oth.s);
		return *this;
	}

	std::string s;
};

int main()
{
	utils::WeakSlotMap<int> slotMap(static_cast<Dummy*>(nullptr));
	EXPECT(slotMap.empty(), "Construct an empty slotmap.");

	slotMap.emplace<Dummy>(4, "4444");
	EXPECT(slotMap.size() == 1, "Insert a single element.");
	EXPECT(slotMap.at<Dummy>(4).s == "4444", "Retrieve a single element.");

	constexpr int indicies[] = { 2,5,8,11,3 };
	for (int i = 0; i < 5; ++i)
		slotMap.emplace<Dummy>(indicies[i], std::to_string(i));
	EXPECT(slotMap.size() == 6, "Insert multiple elements with capacity increase.");
	EXPECT(slotMap.at<Dummy>(4).s == "4444", "Retrieve a single element after capacity increase.");
	for (int i = 0; i < 5; ++i)
	{
		EXPECT(slotMap.at<Dummy>(indicies[i]).s == std::to_string(i), "Retrieve multiple elements.");
	}

	slotMap.clear();
	EXPECT(constructed + moveConstructed == destroyed, "All constructed objects have been destroyed.");

	return testsFailed;
}