#include "testutils.hpp"

#include <engine/utils/containers/weakSlotMap.hpp>
#include <unordered_set>

int constructed = 0;
int moveConstructed = 0;
int destroyed = 0;

struct Dummy
{
	Dummy(const std::string& _str) : s(_str) { ++constructed; }
	Dummy(Dummy&& _oth) noexcept : s(std::move(_oth.s)) { ++moveConstructed; }
	Dummy(const Dummy& _oth) : s(_oth.s) { ++moveConstructed; }
	~Dummy() { ++destroyed; }

	Dummy& operator=(const Dummy& _oth)
	{
		s = _oth.s; 
		return *this;
	}

	Dummy& operator=(Dummy&& _oth) noexcept
	{
		s = std::move(_oth.s);
		return *this;
	}

	std::string s;
};

int main()
{
	utils::WeakSlotMap<int> slotMap(static_cast<Dummy*>(nullptr));
	std::unordered_set<std::string> contents;

	auto insert = [&](int key, const std::string& _str)
	{
		slotMap.emplace<Dummy>(key, _str);
		contents.insert(_str);
	};

	EXPECT(slotMap.empty(), "Construct an empty slotmap.");

	insert(4, "4444");
	EXPECT(slotMap.size() == 1, "Insert a single element.");
	EXPECT(slotMap.at<Dummy>(4).s == "4444", "Retrieve a single element.");

	constexpr int indicies[] = { 2,5,8,11,3 };
	for (int i = 0; i < 5; ++i)
		insert(indicies[i], std::to_string(i) + "aabbccddeeffgg");
	EXPECT(slotMap.size() == 6, "Insert multiple elements with capacity increase.");
	EXPECT(slotMap.at<Dummy>(4).s == "4444", "Retrieve a single element after capacity increase.");
	for (int i = 0; i < 5; ++i)
	{
		EXPECT(slotMap.at<Dummy>(indicies[i]).s == std::to_string(i) + "aabbccddeeffgg", "Retrieve multiple elements.");
	}

	for (const Dummy& dummy : slotMap.iterate<Dummy>())
	{
		EXPECT(contents.contains(dummy.s), "Iterate over elements.");
		contents.erase(dummy.s);
	}
	EXPECT(contents.empty(), "Iterate over all elements.");

	slotMap.clear();
	EXPECT(constructed + moveConstructed == destroyed, "All constructed objects have been destroyed.");

	return testsFailed;
}