#ifndef MY_SHOWER_DIFINITION
#define MY_SHOWER_DIFINITION
#pragma once

template <typename T>
void myInterface::MyShower::addLog(std::string str, T obj)
{
	_logs.emplace_back(Field<T>("+ " + str, obj));
}

#endif