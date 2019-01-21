using System;
using System.Diagnostics;
using System.Windows.Forms;


namespace gui
{
    public static class Program
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        public static void Main(string vs)
        {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new main(vs));
            
        }
    }
}
