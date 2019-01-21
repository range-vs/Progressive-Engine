#pragma once

// c++
#include <fstream>
#include <windows.h>
#include <memory>
#include <string>

// c
#include <time.h>

// my
#include "..\..\Logger\Logger.h"

using std::unique_ptr;

struct Deleter
{
	template<class T>
	void operator()(const T obj)
	{
		delete obj;
	}
};

template<class T>
using brain_memory = unique_ptr<T, Deleter>;

template<class T>
T* SetNewValuesToArray(T* arr, const std::initializer_list<T>& arr1, size_t size)
{
	int i(0);
	for (auto a(arr1.begin()); a != arr1.end(); a++)
		arr[i++] = *a;
	return arr;
}