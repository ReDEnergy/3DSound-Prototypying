#pragma once

#include <string>
#include <vector>

using namespace std;

namespace Utils
{
	template <class T>
	string Join(std::vector<T> values, const char* separator = ",")
	{
		string text = "";
		if (values.size()) {
			text += to_string(values[0]);
			for (unsigned int i = 1; i < values.size(); i++) {
				text += ", ";
				text += to_string(values[i]);
			}
		}
		return text;
	}
}
