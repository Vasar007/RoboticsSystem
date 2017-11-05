#include<iostream>
#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<thread>
#include<mutex>
#include<queue>

using namespace std;

struct RobotCoord {
	int _xr, _yr, Zr, Uw, Up, Uz, _segTime, _typeOfMoving, _control;
	bool operator != (RobotCoord rc) {
		return rc._control != _control ||
			rc._segTime != _segTime ||
			rc._typeOfMoving != _typeOfMoving ||
			rc.Up != Up ||
			rc.Uw != Uw ||
			rc.Uz != Uz ||
			rc._xr != _xr ||
			rc._yr != _yr ||
			rc.Zr != Zr;
	}
	RobotCoord() {
		_xr = _yr = Zr = Uw = Up = Uz = _segTime = _typeOfMoving = _control = 0;
	}
};

template<typename T>
void ParalelRead(mutex *mt, bool *f, queue<T> *Q, mutex *mt2,int size,pair<char,T> *M,int pos) {
	int tmp = 0;
	while (0 == 0) {
		if (tmp >= 20) {
			(*mt).lock();
			if (!(*f)) {
				(*mt).unlock();
				break;
			}
			(*mt).unlock();
			tmp = 0;
		}
		if (M[pos].first == 1) {
			(*mt2).lock();
			(*Q).push(M[pos].second);
			(*mt2).unlock();
			M[(pos + size - 1) % size].first = 0;
			pos = (pos + 1) % size;
		}
		tmp++;
	}
}

template<typename T>
class MemorySharingRead {
	HANDLE hFileMemory;
	mutex mt,mt2;
	thread th;
	bool f;
	queue<T> Q;
public:
	int _size = 0;
	MemorySharingRead(LPCWSTR name, int size = 128, int physMemory = 8192) {
		//name = "Global\\" + name;
		if (sizeof(pair<char,T>)*_size + 1 > physMemory) {
			std::exception exp("number of coordinates needs more memory than in bufer");
			throw exp;
			return;
		}
		SetLastError((DWORD)0);
		hFileMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, physMemory, name);
		if (hFileMemory == NULL) {
			cout << GetLastError() << " " << ERROR_ALREADY_EXISTS << endl;
			return;
		}
		_size = size;

		pair<char,T> *M;
		M = (pair<char, T>*)MapViewOfFile(hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

		int curPos = 0;
		if (M[curPos].first == 1) {
			while (curPos < _size && M[curPos].first == 1) {
				curPos++;
			}
			if (curPos == _size) {
				curPos = -1;
			}
			else {
				while (curPos < _size && M[curPos].first == 0) {
					curPos++;
				}
				if (curPos == _size)
					curPos = 0;
			}
		}
		else {
			while (curPos < _size && M[curPos].first == 0) {
				curPos++;
			}
			if (curPos == _size)
				curPos = -1;
		}
		curPos = (curPos + 1) % _size;
		M[(curPos + _size - 1) % _size].first = 1;

		f = true;
		thread tnpThread(ParalelRead<T>, &mt2, &f, &Q, &mt, _size, M, curPos);
		th.swap(tnpThread);
	}
	T pull() {
		T object;
		mt.lock();
		if (Q.empty()) {
			std::exception exp("trying to get object form empty queue");
			throw exp;
		}
		object = Q.front();
		Q.pop();
		mt.unlock();
		return object;
	}
	bool empty() {
		mt.lock();
		bool tmpf = Q.empty();
		mt.unlock();
		return tmpf;
	}
	~MemorySharingRead() {
		mt2.lock();
		f = false;
		mt2.unlock();
		th.join();
		CloseHandle(hFileMemory);
	}
};

int main() {
	//wchar_t str1[] = L"skdfhgsdkjvisdu\0";
	wchar_t str1[] = L"qwertyuiop\0";
	MemorySharingRead<RobotCoord> buf(str1,128);
	if (buf._size == 0) {
		cout << "oi";
		int a;
		cin >> a;
		return 0;
	}
	int num = 0,tmp=0,prev_num=0;
	unsigned long long curTime = clock(),prev;
	prev = curTime;
	while (0 == 0) {
		if (_kbhit()) {
			char c = _getch();
			if (c == '+')
				break;
			if (c == '-') {
				curTime = clock();
				cout << num - prev_num << " " << curTime - prev << endl;
				prev = clock();
				prev_num = num;
			}
		}
		RobotCoord rc;
		if (!buf.empty()) {
			rc = buf.pull();
			num++;
			if (num == 1)
				prev = clock();
			curTime = clock();
			cout << curTime - prev << " " << rc._xr << " " << rc._yr << " " << rc.Zr << " "
				<< rc.Uw << " " << rc.Up << " " << rc.Uz << " "
				<< rc._segTime << " " << rc._typeOfMoving << " " << rc._control << "\n";
			prev = curTime;
		}
	}
	return 0;
}

// 100 points ~ 3 ms