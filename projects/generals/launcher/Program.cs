﻿using System;
using System.Diagnostics;
using System.Windows.Forms;

namespace launcher
{
    static class Program
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new lanch.HelloWindowProperties());
        }
    }
}
