using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Design;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Text;
using System.Windows.Forms;


namespace components.Controls
{
    public partial class TabControl2 : TabControl
    {
        //[Browsable(true)]
        //[EditorBrowsable(EditorBrowsableState.Always)]
        //public new TabPageCollection TabPages { get; } // скрыли родную коллекцию страниц

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Never)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Dictionary<int, DrawItemEventArgsSerializable> AllItemsDraw { get; set; } // запоминаем свойства кнопок - вкладок

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)] // теневой буфер
        public bool DoubleBuffer { get => this.DoubleBuffered; set => this.DoubleBuffered = value; }
        protected override bool DoubleBuffered { get => base.DoubleBuffered; set => base.DoubleBuffered = value; }

        public TabControl2()
        {
            InitializeComponent();
            InitComponent();
        }

        public TabControl2(IContainer container)
        {
            container.Add(this);

            InitializeComponent();
            InitComponent();
        }

        public void InitComponent()
        {
            DrawMode = TabDrawMode.OwnerDrawFixed;
            AllItemsDraw = new Dictionary<int, DrawItemEventArgsSerializable>();

            //UserPages.Add(new TabPage() { Text = "Вкладка 1", BorderStyle = BorderStyle.None, BackColor = SystemColors.ControlDarkDark });
            //Controls.Add(UserPages.ElementAt(0));
            //UserPages.Add(new TabPage() { Text = "Вкладка 2", BorderStyle = BorderStyle.None, BackColor = SystemColors.ControlDarkDark });
            //Controls.Add(UserPages.ElementAt(1));
        }


        public virtual void AddTabs(TabPage tabPage)
        {
            Controls.Add(tabPage);
        }

        protected override void OnDrawItem(DrawItemEventArgs e)
        {
            if (e.Index >= 0)
            {
                var drawArg = new DrawItemEventArgsSerializable(e.Bounds, e.Index);
                if (!AllItemsDraw.ContainsKey(e.Index))
                    AllItemsDraw.Add(e.Index, drawArg);
                else
                    AllItemsDraw[e.Index] = drawArg;
            }
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            if (m.Msg == 0x000F)
            {
                using (Graphics g = this.CreateGraphics())
                {
                    //Double buffering stuff...
                    BufferedGraphicsContext currentContext;
                    BufferedGraphics myBuffer;
                    currentContext = BufferedGraphicsManager.Current;
                    myBuffer = currentContext.Allocate(g,
                       this.ClientRectangle);

                    Rectangle r = ClientRectangle;

                    myBuffer.Graphics.FillRectangle(new SolidBrush(SystemColors.ControlDarkDark), r);

                    //Painting border
                    if (AllItemsDraw.Count != 0)
                    {
                        int indexPage = this.SelectedIndex;
                        r.Height = this.DisplayRectangle.Height + 1; //Using display rectangle hight because it excludes the tab headers already
                        r.Y = this.DisplayRectangle.Y - 1; //Same for Y coordinate
                        r.Width -= 5;
                        r.X += 3;
                        List<Point> poly = new List<Point>
                    {
                        new Point(AllItemsDraw[indexPage].Bounds.X, r.Y),
                        new Point(r.X, r.Y),
                        new Point(r.X, r.Y + r.Height),
                        new Point(r.X + r.Width, r.Y + r.Height),
                        new Point(r.X + r.Width, r.Y),
                        new Point(AllItemsDraw[indexPage].Bounds.X + AllItemsDraw[indexPage].Bounds.Width, r.Y)
                    };
                        var pen = new Pen(Color.Black, 1);
                        for (int i = 0; i < poly.Count - 1; i++)
                            myBuffer.Graphics.DrawLine(pen, poly[i], poly[i + 1]);
                    }

                    myBuffer.Render();
                    myBuffer.Dispose();

                    foreach (int index in AllItemsDraw.Keys)
                    {
                        UserDrawItem(AllItemsDraw[index]);
                    }
                }
            }
        }

        private void UserDrawItem(DrawItemEventArgsSerializable e)
        {
            if (e.Index >= 0 && e.Index < Controls.Count)
            {
                using (Graphics g = this.CreateGraphics())
                {
                    g.DrawRectangle(new Pen(Color.Black, 1), e.Bounds);
                    SizeF size = g.MeasureString(Controls[e.Index].Text, Font);

                    int startPosX = e.Bounds.X + (e.Bounds.Width - (int)size.Width) / 2;
                    int startPosY = e.Bounds.Y + (e.Bounds.Height - (int)size.Height) / 2;
                    Point posString = new Point(startPosX, startPosY);
                    g.DrawString(Controls[e.Index].Text, Font, new SolidBrush(Color.Black), posString);
                }
            }
        }

    }

    public partial class DrawItemEventArgsSerializable
    {
        public DrawItemEventArgsSerializable(Rectangle rect, int index)
        {
            Bounds = rect;
            Index = index;
        }

        public Rectangle Bounds { set;  get; }
        public int Index { get; }

    }

    public partial class TabPage2 : TabPage
    {
        public bool isWord { get; set; }
    }
}