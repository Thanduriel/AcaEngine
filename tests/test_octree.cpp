#include "testutils.hpp"
#include <engine/utils/containers/octree.hpp>
#include <glm/glm.hpp>

int testOctree2D()
{
	using namespace glm;
	using TreeT = Utils::SparseOctree<int, 2, float>;

	struct Processor
	{
		bool descend(const TreeT::AABB& box)
		{
			++descends;
			return true;
		}

		void process(const TreeT::AABB& box, int val)
		{
			found.emplace_back(box, val);
			++processed;
		}

		void reset()
		{
			found.clear();
			descends = 0;
			processed = 0;
		}

		std::vector<std::pair<TreeT::AABB, int>> found;
		int descends = 0;
		int processed = 0;
	};

	int testsFailed = 0;
	TreeT tree;
	std::vector<std::pair<TreeT::AABB, int>> expectedElements;
	Processor proc;
	int counter = 0;
	auto insert = [&](const TreeT::AABB& aabb, int i)
	{
		tree.insert(aabb, i);
		expectedElements.emplace_back(aabb, i);
	};

	insert({ vec2(0.25f), vec2(0.75f) }, counter++);
	tree.traverse(proc);
	EXPECT(proc.descends == 1 && proc.processed == 1 && proc.found.size() == expectedElements.size(), "Insert element in root node.");
	for (auto& el : expectedElements)
		EXPECT(std::find(proc.found.begin(), proc.found.end(), el) != proc.found.end(), "Inserted elements can be retrieved.");

	proc.reset();
	insert({ vec2(0.1f), vec2(0.5f) }, counter++);
	tree.traverse(proc);
	EXPECT(proc.descends == 1 && proc.processed == 2, "Insert element at upper edge.");
	for (auto& el : expectedElements)
		EXPECT(std::find(proc.found.begin(), proc.found.end(), el) != proc.found.end(), "Inserted elements can be retrieved.");

	proc.reset();
	insert({ vec2(0.0f), vec2(0.49f) }, counter++);
	tree.traverse(proc);
	EXPECT(proc.descends == 2 && proc.processed == 3, "Insert subdividing element.");
	for (auto& el : expectedElements)
		EXPECT(std::find(proc.found.begin(), proc.found.end(), el) != proc.found.end(), "Inserted elements can be retrieved.");

	for (int i = 0; i < 16; ++i)
		tree.insert({ vec2(static_cast<float>(i) + 0.1f), vec2(static_cast<float>(i) + 1.51f) }, counter++);
	TreeT::AABBQuery query({ vec2(0.f, 4.f), vec2(42000.f, 5.f) });
	tree.traverse(query);
	EXPECT(query.hits.size() == 2, "AABB query.");
	EXPECT(std::find(query.hits.begin(), query.hits.end(), 6) != query.hits.end(), "AABB query.");
	EXPECT(std::find(query.hits.begin(), query.hits.end(), 7) != query.hits.end(), "AABB query.");

	return testsFailed;
}

int main() 
{
	return testOctree2D();
}
