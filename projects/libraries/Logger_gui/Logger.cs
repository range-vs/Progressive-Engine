using System;
using System.Diagnostics;
using System.IO;
using System.Data;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Text;

namespace Log
{
    public class Logger : IDisposable
    {
        private static string tab = "";
        private string file;
        private string func;
        private static string file_name;

        [DllImport("psapi.dll")]
        static extern uint GetModuleFileNameEx(IntPtr hProcess, IntPtr hModule,
        [Out] StringBuilder lpBaseName, [In] [MarshalAs(UnmanagedType.U4)] int nSize);

        public Logger()
        {
            StackFrame sf = new StackFrame(1, true);  // берем предыдущий кадр
            file = sf.GetFileName();
            if (sf.GetMethod().Name.StartsWith(".")) 
                func = sf.GetMethod().DeclaringType.Name + ".Constructor";
            else
                func = sf.GetMethod().Name;
            func += "(";
            foreach (var t in sf.GetMethod().GetParameters())
                func += t.ToString() + ", ";
            if (sf.GetMethod().GetParameters().Length > 0)
                func = func.Remove(func.Length - 2);
            func += ")";
            _log(file, func, sf.GetFileLineNumber());
        }

        private void _log(string file, string func, int l = -1)
        {
            string _out;
            if (l != -1)
            {
                _out = tab + "[" + DateTime.Now.ToString() + "] Input function. ID Thread: " +
                    Thread.CurrentThread.ManagedThreadId + ", file name: " + file +
                    ", function name: " + func + ", line: " + l + Environment.NewLine;
                tab += "   ";
            }
            else
            {
                tab = tab.Remove(tab.Length - 3);
                _out = tab + "[" + DateTime.Now.ToString() + "] Output function. ID Thread: " +
                  Thread.CurrentThread.ManagedThreadId + ", file name: " + file +
                  ", function name: " + func + Environment.NewLine;
            }
            FileStream f;
            while (true)
            {
                try
                {
                    f = new FileStream(file_name, FileMode.Append);
                    break;
                }
                catch (Exception)
                {
                    Thread.Sleep(50);
                }
            }
            StreamWriter sw = new StreamWriter(f);
            sw.Write(_out);
            sw.Close();
        }

        public void Dispose()
        {
            _log(file, func);
        }

        public static void Init(string module_name)
        {
            file_name = module_name;
            file_name = file_name.Remove(0, file_name.LastIndexOf('\\') != -1 ? file_name.LastIndexOf('\\') + 1 : 0);
            file_name = file_name.Insert(0, "logs/"); // +  System.IO.Path.GetFileName(Application.ExecutablePath);
            if (file_name.LastIndexOf('.') != -1)
                file_name = file_name.Remove(file_name.LastIndexOf('.'));
            file_name += ".log";
            if (File.Exists(file_name))
                File.Delete(file_name);
        }

        public static void WriteMessage(string m)
        {
            FileStream f = new FileStream(file_name, FileMode.Append);
            StreamWriter sw = new StreamWriter(f);
            sw.WriteLine(m);
            sw.Close();
        }
    }


    public class Null: IDisposable
    {
        public Null(){ }

        public void Dispose()
        {}

        public static void Init(string module_name){}

        public static void WriteMessage(string m){}

    }

}

