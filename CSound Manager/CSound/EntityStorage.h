#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

template <class T>
class EntityStorage
{
	public:
		EntityStorage(const char *prefix = "prefix");
		~EntityStorage() {};

		void Clear();

		T* Get(const char* name) const;
		T* Create(const char* name);
		string Create();
		vector<T*> GetEntries() const;

		void Set(const char* name, T* entity);

		bool Rename(const char* name, const char* newName);
		void Remove(const char* name);

	protected:
		const string prefix;
		unordered_map <string, T*> storage;
};

template<class T>
inline EntityStorage<T>::EntityStorage(const char * prefix)
	: prefix(prefix)
{
}

template<class T>
inline void EntityStorage<T>::Clear()
{
	storage.clear();
}

template<class T>
inline void EntityStorage<T>::Set(const char * name, T * entity)
{
	storage[name] = entity;
}

template<class T>
inline T * EntityStorage<T>::Get(const char * name) const
{
	if (storage.find(name) != storage.end()) {
		return storage.at(name);
	}
	#ifdef _DEBUG
	cout << "Template: [" << name << "] was not found " << endl;
	#endif

	return nullptr;
}

template<class T>
inline bool EntityStorage<T>::Rename(const char * name, const char * newName)
{
	auto pit = storage.find(name);
	auto nit = storage.find(newName);
	if (nit != storage.end() || pit == nit)
		return false;

	storage[newName] = storage[name];
	storage.erase(name);

	return true;
}

template<class T>
inline void EntityStorage<T>::Remove(const char * name)
{
	storage.erase(name);
}

template<class T>
inline T * EntityStorage<T>::Create(const char * name)
{
	if (storage.find(name) != storage.end())
		return nullptr;

	T* entity = new T();
	storage[name] = entity;
	return entity;
}

template<class T>
inline string EntityStorage<T>::Create()
{
	string name;
	T* entry = nullptr;
	unsigned int ID = 0;

	while (entry == nullptr)
	{
		name = prefix;
		name.append(to_string(ID));
		entry = Create(name.c_str());
		ID++;
	}

	return name;
}

template<class T>
inline vector<T*> EntityStorage<T>::GetEntries() const
{
	vector<T*> entries;
	for (auto item : storage) {
		entries.push_back(item.second);
	}
	return entries;
}
