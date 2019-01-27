using System;
using System.Threading;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using components.Controls;
using types;
using System.Collections.Generic;
using System.Collections;

namespace gui
{
    public partial class main : Form
    {
        private IntPtr hwnd_child;
        private string name_adapter;
        private Thread thDX;

        public main(System.String vs)
        {
            InitializeComponent();
            Init(vs);
            InitImageBuffers();
            InitManipulators();
            InitRender();
            InitPanels();
        }

        public void InitRender()
        {
            // init panel
            PanelDirectX3D.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
           | System.Windows.Forms.AnchorStyles.Left)
           | System.Windows.Forms.AnchorStyles.Right)));
            PanelDirectX3D.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            PanelDirectX3D.Name = "PanelDirectX3D";

            // thread
            thDX = new Thread(_init);
            thDX.IsBackground = true;

            // data
            types.ParamsRenderThread paramsRender = new types.ParamsRenderThread(this.PanelDirectX3D.Handle, Process.GetCurrentProcess().Handle);
            thDX.Start(paramsRender);

        }

        private void _init(object obj)
        {
            types.ParamsRenderThread paramsRender = (types.ParamsRenderThread)obj;
            dll_function.startEditor(paramsRender.hInstCurrentProcess, paramsRender.hwndMain, IntPtr.Zero);
        }

        private void InitManipulators()
        {
            noTool.Tag = 0; // нет инструмента
            noTool.Click += ManipulatorsButtonClick;
            noTool.BackColor = SystemColors.ControlDarkDark;
            noTool.FlatAppearance.BorderSize = 0;
            noTool.FlatStyle = FlatStyle.Flat;
            selectModel.Tag = 1; // модели можно выделять
            selectModel.Click += ManipulatorsButtonClick;
            selectModel.BackColor = SystemColors.ControlDarkDark;
            selectModel.FlatAppearance.BorderSize = 0;
            selectModel.FlatStyle = FlatStyle.Flat;
        }

        public void InitImageBuffers()
        {
            ImageList imgListButtonPanel = new ImageList();
            imgListButtonPanel.Images.Add("0", Properties.Resources.down_button);
            imgListButtonPanel.Images.Add("1", Properties.Resources.up_button);
            listModelsButton.ImageList = imgListButtonPanel;
            listModelsButton.ImageKey = "0";

        }

        private void Init(string vs)
        {
            this.DoubleBuffered = true;
            name_adapter = vs;
            hwnd_child = IntPtr.Zero;
            KeyPreview = true;
        }

        public void InitPanels()
        {
            listFilesStaticModels.CreateHierarchy(listFiles.TabPages[0]);
        }


        private void ManipulatorsButtonClick(object sender, EventArgs e)
        {
            int mode = Int32.Parse(((RadioButton)sender).Tag.ToString());
            dll_function.SetModeManipulations(mode);
        }

        private bool MouseOnRenderPanel()
        {
            var windowPosition = MousePosition;
            var point = PanelDirectX3D.PointToClient(windowPosition);
            Rectangle rect = new Rectangle(0, 0, PanelDirectX3D.Width, PanelDirectX3D.Height);
            GraphicsPath path = new GraphicsPath();
            path.AddRectangle(rect);
            if (path.IsVisible(point))
                return true;
            return false;
        }

        private void GlobalKeyDown(object sender, KeyEventArgs e)
        {
            if (MouseOnRenderPanel())
            {
                dll_function.PressKey((int)e.KeyCode);
            }
        }

        private void GlobalKeyUp(object sender, KeyEventArgs e)
        {
            if (MouseOnRenderPanel())
            {
                dll_function.UnPressKey((int)e.KeyCode);
            }
        }

        private void GlobalResize(object sender, EventArgs e)
        {
            ReDrawPanelRender();
            ResizeListModelsPanel();
        }

        private void ResizeListModelsPanel()
        {
            // изменение размеров нижней формы(пока только список объектов)
            listModels.Panel1.ClientSize = new Size(PanelDirectX3D.Width, listModels.Panel1.ClientSize.Height);
            listModels.Panel2.ClientSize = new Size(PanelDirectX3D.Width, listModels.Panel2.ClientSize.Height);
        }

        private void ReDrawPanelRender()
        {
            int[] size = new int[] { PanelDirectX3D.Width, PanelDirectX3D.Height };
            dll_function.ResizeRenderWindow(size);
        }

        private void RenderPanelResize(object sender, EventArgs e)
        {
            ReDrawPanelRender();
        }

        private void menuToolsNetClick(object sender, EventArgs e)
        {
            dll_function.ReShowNet();
            menuToolsNet.Checked = menuToolsNet.Checked ? false : true;
        }

        private void menuToolsModeDrawClick(object sender, EventArgs e)
        {
            ToolStripMenuItem btn = (ToolStripMenuItem)sender;
            dll_function.ReMode(Int32.Parse(btn.Tag.ToString()));

            var collect = menuToolsModeDraw.DropDownItems;
            for (int i = 0; i < collect.Count; i++)
                ((ToolStripMenuItem)collect[i]).Checked = false;
            btn.Checked = true;
        }

        private void listModelsButton_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            btn.ImageKey = btn.ImageKey == "0" ? "1" : "0";
            var s = listModels.Panel2.ClientSize;
            if (btn.ImageKey == "1")
            {
                listModels.Panel2.Hide();
                renderAndModelListPanels.SplitterDistance += s.Height;
            }
            else
            {
                listModels.Panel2.Show();
                renderAndModelListPanels.SplitterDistance -= s.Height;
            }
        }

        private void mainSplit_SplitterMoved(object sender, SplitterEventArgs e)
        {
            // правая панель всегда занимает 20% от всей панели
            SplitContainer sp = (SplitContainer)sender;
            int widthAll = sp.Size.Width; // 100%
            int widthIdentity = widthAll / 100; // 1%
            int currentProcent = e.SplitX / widthIdentity; // текущее кол-во % занятого простанства первой панелью
            if (currentProcent < 80)
            {
                int procent = widthIdentity * 80; // нужный размер панели
                sp.Panel1.ClientSize = new Size(procent, sp.Panel1.Height);
                sp.Panel2.ClientSize = new Size(widthAll - procent, sp.Panel2.Height);
                sp.SplitterDistance = procent;
            }
            ResizeListModelsPanel();
        }

        private void panelLists_DrawItem(object sender, DrawItemEventArgs e)
        {
            Brush b = new SolidBrush(SystemColors.ControlDarkDark);
            Rectangle r = e.Bounds; r.X -= 5; r.Y -= 5; r.Width += 5; r.Height += 5;
            e.Graphics.FillRectangle(b, e.Bounds);
        }

        private void PanelDirectX3D_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(string)))
                e.Effect = DragDropEffects.Copy;
        }

        private void PanelDirectX3D_DragDrop(object sender, DragEventArgs e)
        {
            if(e.Effect == DragDropEffects.Copy)
            {
                // Обрабатывать принятые данные
                string path = (string)e.Data.GetData(typeof(string));
                dll_function.PastModelForMap(path, MousePosition.X, MousePosition.Y);
            }
        }
    }

}

// WPF:
// TODO: цвет рамки окна - серый, реализовать
// TODO: menu - создать новый компонент, чтобы пункт меню при выделении закрашивался в тёмный цвет
// по нажатию на любую модель - отправлять данные в рендер.dll(написать функцию в dll и импортировать её в c# проект)
// TODO: установка моделей на карту
// list view - цвет скролла

// TODO: 
// разработать формат карты и сделать загрузку и сохранения уровня(пока только объекты)
// инстансинг на вычислительном шейдере для всего