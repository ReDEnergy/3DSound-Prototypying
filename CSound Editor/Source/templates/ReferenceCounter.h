#pragma once
#include <unordered_map>
#include <vector>

template <class T>
class ReferenceCounter
{
public:
	ReferenceCounter() {};
	~ReferenceCounter() {};

	void Add(T *E)
	{
		if (entities.find(E) != entities.end()) {
			entities[E]++;
		}
		else {
			entities[E] = 1;
		}
	}

	// TODO: Delete the entry when counter reaches 0
	// ATTENTION: All references to a pointer must be tracked by this class

	unsigned int Remove(T *E)
	{
		if (entities.find(E) != entities.end())
		{
			if (entities[E] == 1) {
				entities.erase(E);
				return 0;
			}
			else {
				entities[E]--;
				return entities[E];
			}
		}

		cout << "ERROR: ENTRY NOT TRACKED" << endl;
		return 0;
	}

	vector<T*> GetEntities()
	{
		vector<T*> entries;
		for (auto E : entities) {
			entries.push_back(E.first);
		}
		return entries;
	}

private:
	unordered_map<T*, uint> entities;
};