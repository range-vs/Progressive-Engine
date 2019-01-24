#include <Windows.h>

#include <thread>
#include <memory>

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

using StartGame = UINT (HINSTANCE, HWND);

extern "C" _declspec(dllexport) UINT startGame(HINSTANCE hInst); // ����� ����� � ����
void runRenderWindow(HINSTANCE hInst, HWND hwnd, UINT& result); // ��������� ����� ��� ������� ���� � ��������

// ����:
// ���������: �������� �������(��������� ���� ��� ����, ��������� �������� �������, ���������� ���������(�������, �������� � ������)) -- current TODO
// �������� ���������� ������ � ���������� ������ ����� �� ����� ����������
// ��������� ����� ����� game.dll � render_dx11.dll - ����� ����� ���� ����� ���� ���������� ������� �������
// ������� ������������ �� alt + tab - ������ ���