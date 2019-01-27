using System;
using System.Runtime.InteropServices;
using System.Text;

namespace gui
{
    class dll_function
    {
        // render
        [DllImport("render_dx11.dll")]
        public static extern uint startEditor(IntPtr hInst, IntPtr mainWindow, IntPtr consoleWindow);

        // function editor <--> render
        [DllImport("render_dx11.dll")]
        public static extern IntPtr SetModeManipulations(int mode);

        [DllImport("render_dx11.dll")]
        public static extern IntPtr ResizeRenderWindow([Out] int[] size);
        [DllImport("render_dx11.dll")]
        public static extern IntPtr PressKey(int keyCode);
        [DllImport("render_dx11.dll")]
        public static extern IntPtr UnPressKey(int keyCode);
        [DllImport("render_dx11.dll")]
        public static extern IntPtr ReShowNet();
        [DllImport("render_dx11.dll")]
        public static extern IntPtr ReMode(int mode);
        [DllImport("render_dx11.dll")]
        public static extern IntPtr PastModelForMap(string path, int x, int y);
    }
}
