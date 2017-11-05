#include<iostream>
#include<Windows.h>
#include<string>
#include<fstream>
#include<stdio.h>
#include<queue>
#include<thread>
#include<mutex>
#include<conio.h>

#define LOCAL_DEBUG
#define JOINT

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
void F(pair<char, T> *sPos, int _size, queue<T> *Q, mutex *mt, bool *f, mutex *mt2, int tmpPos) {
	int tmp = 0;
	while (0 == 0) {
		if (tmp >= 100) {
			(*mt2).lock();
			if (!(*f)) {
				(*mt2).unlock();
				return;
			}
			(*mt2).unlock();
			tmp = 0;
		}
		tmp++;
		//cout << !(*Q).empty() << " " << (int)sPos[tmpPos].controler << endl;
		if (!(*Q).empty() && sPos[tmpPos].first == 0 && sPos[(tmpPos + 1) % _size].first == 0) {
			(*mt).lock();
			if ((*Q).empty())
				continue;
			T corCoord = (*Q).front();
			(*Q).pop();
			(*mt).unlock();
			sPos[tmpPos].second = corCoord;
			sPos[tmpPos].first = char(1);
			tmpPos = (tmpPos + 1) % _size;
		}
#ifdef LOCAL_DEBUG
		int tttmp = 0;
		static bool flag = false;
		if (!(*Q).empty() && sPos[tmpPos].first == 1) {
			tttmp++;
			flag = true;
		}
		if ((*Q).empty() && flag == true) {
			cout << "good" << tttmp << endl;
			tttmp = 0;
			flag = false;
		}
#endif  //LOCAL_DEBUG


	}
}

int serchPos(pair<char, RobotCoord> *M, int size) {
	int curPos = 0;
	if (M[curPos].first == 0) {
		while (curPos < size && M[curPos].first == 0) {
			curPos++;
		}
		if (curPos == size) {
			curPos = 0;
		}
		else {
			while (curPos < size && M[curPos].first == 1) {
				curPos++;
			}
			if (curPos == size)
				curPos = 0;
		}
	}
	else {
		while (curPos < size && M[curPos].first == 1) {
			curPos++;
		}
		if (curPos == size)
			curPos = 0;
	}
	return curPos;
}

template<typename T>
class MemorySharingWrite {
	HANDLE hFileMemory;
	thread th;
	queue<T> Q;
	mutex mt, mt2;
	bool f = true;
public:
	int _size = 0;
	MemorySharingWrite(LPCWSTR name, int size = 128, int physMemory = 8192) {
		if (sizeof(pair<char, T>)*_size + 1 > physMemory) {
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

		pair<char, T> *M;
		M = (pair<char, T>*)MapViewOfFile(hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

		int curPos = 0;
		if (M[curPos].first == 0) {
			while (curPos < _size && M[curPos].first == 0) {
				curPos++;
			}
			if (curPos == _size) {
				curPos = 0;
			}
			else {
				while (curPos < _size && M[curPos].first == 1) {
					curPos++;
				}
				if (curPos == _size)
					curPos = 0;
			}
		}
		else {
			while (curPos < _size && M[curPos].first == 1) {
				curPos++;
			}
			if (curPos == _size)
				curPos = 0;
		}



		thread bufThread(F<T>, M, _size, &Q, &mt, &f, &mt2, curPos);
		th.swap(bufThread);


	}
	void push(RobotCoord object) {
		mt.lock();
		Q.push(object);
		mt.unlock();
	}
	~MemorySharingWrite() {
		mt2.lock();
		f = false;
		mt2.unlock();
		th.join();
		CloseHandle(hFileMemory);
	}
};

int main() {
	wchar_t str1[] = L"skdfhgsdkjvisdu\0";
	MemorySharingWrite<RobotCoord> buf(str1, 128);
	if (buf._size == 0) {
		cout << "oi";
		int a;
		cin >> a;
		return 0;
	}
	string str;
	int x, y, z, uw, up, uz, st, tom, con;

	/*while (str!="exit") {
		string str1;
		cin >> str>>str1;
		HANDLE hFileMemory;
		hFileMemory = OpenFileMapping(FILE_MAP_READ, false, (LPCWSTR)(str1 + "\0").c_str());
		if (hFileMemory != NULL)
			std::cout << "bad\n";
		else
			std::cout << GetLastError() << "\n";
	}//*/

	RobotCoord rc;

	/*ifstream in("startPointsDec.txt");
	while (in.peek() != EOF) {
		in >> x >> y >> z >> uw >> up >> uz >> st >> tom >> con;
		rc.Xr = x;
		rc.Yr = y;
		rc.Zr = z;
		rc.Uw = uw;
		rc.Up = up;
		rc.Uz = uz;
		rc.segTime = st;
		rc.typeOfMoving = tom;
		rc.control = con;
		buf.push(rc);
	}
	in.close();//*/

	/*while (str != "exit") {
		cin >> str >> x >> y >> z >> uw >> up >> uz >> st >> tom >> con;
		rc.Xr = x;
		rc.Yr = y;
		rc.Zr = z;
		rc.Uw = uw;
		rc.Up = up;
		rc.Uz = uz;
		rc.segTime = st;
		rc.typeOfMoving = tom;
		rc.control = con;
		buf.push(rc);
	}//*/

	HANDLE hFileMemory;
	hFileMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 8192, str1);
	if (hFileMemory == NULL) {
		cout << GetLastError() << " " << ERROR_ALREADY_EXISTS << endl;
		return 1;
	}


	pair<char, RobotCoord> *M;
	M = (pair<char, RobotCoord>*)MapViewOfFile(hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	int curPos = serchPos(M, 128), step = 5000;

	rc._typeOfMoving = 0;
	rc._segTime = 1;
#ifdef JOINT
	/*rc.Xr = 55000;
	rc.Yr = -45000;
	rc.Zr = 0;
	rc.Uw = 0;
	rc.Up = 0;
	rc.Uz = 0;//*/
	rc._xr = 0;
	rc._yr = 0;
	rc.Zr = 0;
	rc.Uw = 0;
	rc.Up = -90000;
	rc.Uz = 0;//*/
#else
	rc.Xr = 985000;
	rc.Yr = 0;
	rc.Zr = 940000;
	rc.Uw = -180000;
	rc.Up = 0;
	rc.Uz = 0;
#endif
	/*
	while (0 == 0) {
		if (_kbhit()) {
			char c = _getch();
			if (c == 'a') {
				rc.Xr -= step;
				rc._segTime = 50;
			}
			else if (c == 'q') {
				rc.Xr += step;
				rc._segTime = 50;
			}
			else if (c == 's') {
				rc.Yr -= step;
				rc._segTime = 10;
			}
			else if (c == 'w') {
				rc.Yr += step;
				rc._segTime = 10;
			}
			else if (c == 'd') {
				rc.Zr -= step;
				rc._segTime = 10;
			}
			else if (c == 'e') {
				rc.Zr += step;
				rc._segTime = 10;
			}
			else if (c == 'f') {
				rc.Uw -= step;
				rc._segTime = 10;
			}
			else if (c == 'r') {
				rc.Uw += step;
				rc._segTime = 10;
			}
			else if (c == 'g') {
				rc.Up -= step;
				rc._segTime = 10;
			}
			else if (c == 't') {
				rc.Up += step;
				rc._segTime = 10;
			}
			else if (c == 'h') {
				rc.Uz -= step;
				rc._segTime = 10;
			}
			else if (c == 'y') {
				rc.Uz += step;
				rc._segTime = 10;
			}
			else if (c == 'H') {
#ifdef JOINT
				rc.Xr = 0;
				rc.Yr = 0;
				rc.Zr = 0;
				rc.Uw = 0;
				rc.Up = -90000;
				rc.Uz = 0;
				rc._segTime = 300;
#else
				rc.Xr = 985000;
				rc.Yr = 0;
				rc.Zr = 940000;
				rc.Uw = -180000;
				rc.Up = 0;
				rc.Uz = 0;
#endif
			}
		}
		if (M[curPos].first == 0 && M[(curPos + 1) % 128].first == 0) {
			M[curPos].second = rc;
			M[curPos].first = (char)1;
			curPos = (curPos + 1) % 128;
			cout << rc.Xr << " " << rc.Yr << " " << rc.Zr << " " << rc.Uw << " " << rc.Up << " " << rc.Uz << " " << rc._segTime << " " << rc._typeOfMoving << " " << rc._control << "\n";
		}
		else {
			cout << "error fill out\n";
		}
		Sleep(rc._segTime + 150);
	}//*/
	
	int ttk = 0;
	while (0 == 0) {
		if (ttk < 3)
			rc._xr += 10000;
		else if (ttk < 6)
			rc._yr += 10000;
		else if (ttk < 9)
			rc._xr -= 10000;
		else
			rc._yr -= 10000;
		if (M[curPos].first == 0 && M[(curPos + 1) % 128].first == 0) {
			M[curPos].second = rc;
			M[curPos].first = (char)1;
			curPos = (curPos + 1) % 128;
			cout << rc._xr << " " << rc._yr << " " << rc.Zr << " " << rc.Uw << " " << rc.Up << " " << rc.Uz << " " << rc._segTime << " " << rc._typeOfMoving << " " << rc._control << "\n";
		}
		else {
			cout << "error fill out\n";
		}
		Sleep(50);
		++ttk;
		if (ttk >= 12)
			ttk = 0;
	}//*/

	return 0;
}