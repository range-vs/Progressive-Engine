using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace lanch
{
    public partial class HelloWindowProperties : Form
    {
        public HelloWindowProperties()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            // заполняем комбобокс с адаптерами
            int count = 0;
            IntPtr ptr = dll_function.GetListAdapters(ref count);
            string[] result = new string[count];
            IntPtr[] ptr_strings = new IntPtr[count];
            // копируем указатели на строки
            Marshal.Copy(ptr, ptr_strings, 0, count);
            for (int i = 0; i < count; i++)
            {
                result[i] = Marshal.PtrToStringAnsi(ptr_strings[i]);
                this.graphicsSelect.Items.Add(result[i]);
            }
            this.graphicsSelect.SelectedIndex = 0;

            // настройки комбобокса с типом приложения
            this.programStart.SelectedIndex = 0;

            // заполняем тип устройства
            IntPtr mes = dll_function.InfoTypeDevice();
            string message = Marshal.PtrToStringAnsi(mes);
            typeDevice.Text = message;
        }

        private void StartEngineButtonProc(object sender, EventArgs e)
        {
            // выбираем адаптер
            IntPtr address = dll_function.SetAdapter(this.graphicsSelect.SelectedIndex);
            string name = Marshal.PtrToStringAnsi(address);
            name += "\"";
            name = name.Insert(0, "\"");

            int type = 0;
            IntPtr res = dll_function.GetTypeAdapter(ref type);

            // выбираем тип ПО
            string type_program = this.programStart.SelectedIndex == 0 ? "-editor" : "-game";

            // запускаем выбранное ПО
            string program = type == 0 ? "integred_start.exe" : "discrete_start.exe";
            Process.Start(program, name + " " + type_program);
            //dll_function.WriteFileDataCurrentAdapter(); // пишем данные о выбранном адаптере в файл

            // закрываем лаунчер
            this.Close();
        }

        private void FormClosingProc(object sender, FormClosingEventArgs e)
        {
            dll_function.ReleaseData(); // очистка ресурсов в dll
        }
    }
}

// TODO: получать имя видеокарты в движке, дабы запускать с этой видеокартой