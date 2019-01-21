using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace components.Controls
{
    public partial class ListViewsAllFiles : ListView
    {
        public string RootCatalogue { get; set; } // текущий каталог сканирования файлов и папок
        public string Extension { get; set; } // расширение файлов для добавления

        private ImageList imgList; // список иконок
        private ListView rootListView; // текущий элемент
        private Cursor copyObjectCursor; // курсор, при копировании объекта в сцену

        public ListViewsAllFiles()
        {
            InitializeComponent();
        }

        public ListViewsAllFiles(IContainer container)
        {
            container.Add(this);

            InitializeComponent();
        }

        public void CreateHierarchy(Control owner)
        {
            copyObjectCursor = new Cursor(Cursor.Current.Handle);
            IntPtr colorcursorhandle = LoadCursorFromFile(@"cursors/copy_model.cur");
            copyObjectCursor.GetType().InvokeMember("handle", BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.SetField, null, copyObjectCursor, new object[] { colorcursorhandle });

            LoadImageList();
            ScanRootCatalogue(owner);
        }

        private void LoadImageList()
        {
            imgList = new ImageList();
            imgList.Images.Add("0", Properties.Resources.folder_back);
            imgList.Images.Add("1", Properties.Resources.folder);
            imgList.Images.Add("2", Properties.Resources.file);
            imgList.ImageSize = new Size(64, 64);
            LargeImageList = imgList;
        }

        private void ScanRootCatalogue(Control owner)
        {
            rootListView = this; // текущий отображаемый элемент 
            rootListView.AllowDrop = true;
            rootListView.MouseMove += MouseMoving;
            rootListView.DoubleClick += ListViewDoubleClick; // процедура клика по элементу
            rootListView.GiveFeedback += DragAndDropGiveFeedback;
            rootListView.MouseUp += MouseUpping;

            Stack<Trio<string, ListView, ListView>> st = new Stack<Trio<string, ListView, ListView>>(); // стек, для добавления элементов в верном порядке
            ListView tmp = null; // предыдущий элемент(возврат назад)
            string root = RootCatalogue; // стартовый и текущий каталог
            var current = rootListView; // текущий элемент
            var extension = Extension; // расширение файлов, для вывода в list view

            while (true)
            {
                var dir = new DirectoryInfo(root);
                foreach (var d in dir.GetDirectories())
                {
                    var collect = d.EnumerateFiles();
                    bool inFiles = false;
                    foreach (var f in collect)
                        if (f.Extension == extension)
                        {
                            inFiles = true;
                            break;
                        }
                    if (inFiles)
                    {
                        var t = current.Items.Add(new ListViewItem() { Text = d.ToString(), ImageKey = "1" });
                        ListView lv = new ListView()
                        {
                            Location = new System.Drawing.Point(current.Location.X, current.Location.Y),
                            Size = new System.Drawing.Size(current.Size.Width, current.Size.Height),
                            MultiSelect = false,
                            LargeImageList = imgList,
                            Visible = false,
                            Dock = current.Dock,
                            BackColor = current.BackColor,
                            Font = current.Font,
                            AllowDrop = true
                        };
                        lv.DoubleClick += ListViewDoubleClick;
                        lv.MouseMove += MouseMoving;
                        lv.GiveFeedback += DragAndDropGiveFeedback;
                        lv.MouseUp += MouseUpping;
                        t.Tag = lv;
                        owner.Controls.Add(lv);

                        st.Push(new Trio<string, ListView, ListView>(d.FullName, lv, current));
                    }
                }
                if (tmp != null) // добавляем папку возврата назад(в начало списка, и если текущий корень не пуст, т.е. имеет другие каталоги)
                {
                    var t = current.Items.Insert(0, new ListViewItem() { Text = "..", ImageKey = "0" });
                    t.Tag = tmp;
                }

                foreach (var f in dir.GetFiles())
                {
                    if (f.Extension == extension)
                    {
                        var tf = current.Items.Add(new ListViewItem() { Text = f.ToString(), ImageKey = "2" });
                        tf.Tag =  f.FullName;
                    }
                }
                if (st.Count == 0)
                    break;
                var _st = st.Pop();
                current = _st.Node;
                tmp = _st.NodeParent;
                root = _st.Path;
            }
        }

        private void ListViewDoubleClick(object sender, EventArgs e)
        {
            ListView lv = (ListView)sender;
            var selected = lv.SelectedItems[0];
            if (selected.Tag is ListView)
            {
                rootListView.Hide();
                rootListView = (ListView)selected.Tag;
                rootListView.Show();
            }
        }

        [DllImport("user32.dll")]
        public static extern IntPtr LoadCursorFromFile(string filename);

        private void MouseMoving(object sender, MouseEventArgs e)
        {
            ListView listView = sender as ListView;
            if (e.Button == MouseButtons.Left && listView.SelectedIndices.Count > 0 && !(listView.SelectedItems[0].Tag is ListView))
                DoDragDrop((string)listView.SelectedItems[0].Tag, DragDropEffects.Copy); // вызов копирования элемента
        }

        private void DragAndDropGiveFeedback(object sender, System.Windows.Forms.GiveFeedbackEventArgs e)
        {
            e.UseDefaultCursors = false;
            Cursor = copyObjectCursor;
            if (e.Effect == DragDropEffects.Copy)
            {
                if (Cursor.Current != copyObjectCursor)
                    Cursor.Current = copyObjectCursor;
            }
        }

        private void MouseUpping(object sender, MouseEventArgs e)
        {
            if(Cursor == copyObjectCursor)
                Cursor = Cursors.Default;
        }


    }

    struct Trio<TKey, TValue, TValue2>
    {
        public TKey Path { get; set; }
        public TValue Node { get; set; }
        public TValue2 NodeParent { get; set; }

        public Trio(TKey p, TValue n, TValue2 np)
        {
            Path = p;
            Node = n;
            NodeParent = np;
        }
    }
}
