#pragma once
#include <unordered_map>

template <class T, class P>
class CSoundList
{
	public:
		CSoundList() {};
		virtual ~CSoundList() {};

		virtual list<T*>& GetEntries();

		virtual void Add(T *entry);
		virtual void Remove(T *entry);
		virtual void Move(unsigned int oldPosition, unsigned int newPosition);

		virtual void Update() {};
		void PreventUpdate() {
			update = false;
		};
		void ResumeUpdate() {
			update = true;
		};

	protected:
		bool update = true;
		list<T*> entries;
};


template<class T, class P>
inline list<T*>& CSoundList<T, P>::GetEntries()
{
	return entries;
}

template <class T, class P>
inline void CSoundList<T, P>::Add(T * entry)
{
	entries.push_back(entry);
	entry->SetParent(static_cast<P*>(this));

	if (update) {
		Update();
	}
}

template <class T, class P>
inline void CSoundList<T, P>::Remove(T * entry)
{
	entries.remove(entry);
	entry->SetParent(static_cast<P*>(nullptr));

	if (update) {
		Update();
	}
}

template <class T, class P>
inline void CSoundList<T, P>::Move(unsigned int oldPosition, unsigned int newPosition)
{
	if (oldPosition > entries.size() || newPosition > entries.size()) {
		return;
	}
	auto it = entries.begin();
	std::advance(it, oldPosition);

	auto C = (*it);
	entries.erase(it);

	auto nit = entries.begin();
	std::advance(nit, newPosition);

	entries.insert(nit, 1, C);

	if (update) {
		Update();
	}
}