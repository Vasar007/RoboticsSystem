#ifndef MY_SHOWER_INL
#define MY_SHOWER_INL

template <typename T>
void myInterface::MyShower::addLog(const std::string& str,const T& obj)
{
	_logs.emplace_back(Field<T>("+ " + str, obj));
}

#endif // MY_SHOWER_INL