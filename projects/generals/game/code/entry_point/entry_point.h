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

extern "C" _declspec(dllexport) UINT startGame(HINSTANCE hInst); // точка входа в игру
void runRenderWindow(HINSTANCE hInst, HWND hwnd, UINT& result); // отдельный поток для запуска окна с рендером

// план:
// следующее: заняться движком(запустить двиг как игру, продумать основные моменты, подключить освещение(амбиент, спекулар и диффуз)) -- current TODO
// получать все доступные монитору разрешения - и позволять менять разрешение ИГРЫ для этих разрешений(это относится к настройкам, ещё не продумано)
// придумать канал между game.dll и render_dx11.dll - чтобы между ними можно было пересылать игровые объекты