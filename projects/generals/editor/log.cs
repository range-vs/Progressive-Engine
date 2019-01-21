using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using gui;

namespace console
{
    [StructLayout(LayoutKind.Sequential)]
    struct COPYDATASTRUCT
    {
        public IntPtr dwData;
        public int cbData;
        public IntPtr lpData;
    }

    public partial class Console : Form
    {
        private bool isRichTextBox;
        public Console()
        {
            InitializeComponent();
            Init();
            this.DoubleBuffered = true;
            this.isRichTextBox = false;
        }

        public RichTextBox GetLog() { return this.Log; }
        public void SetLog(String t) { this.Log.Text = t; }

        // для приёма данных от главной программы
        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                //case dll_function.WM_COPYDATA:
                //    {
                //        string message = "";
                //        unsafe
                //        {
                //            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)m.LParam;
                //            message = Marshal.PtrToStringAnsi(cds->lpData) + Environment.NewLine;
                //        }
                //        //this.Log.Invoke(new Action(() => {
                //        //    this.Log.Text += message;
                //        //    this.Log.SelectionStart = this.Log.Text.Length;
                //        //}));
                //        this.Log.Text += message;
                //        this.Log.SelectionStart = this.Log.Text.Length;
                //        break;
                //    }

                //case dll_function.WM_SETCURSOR:
                //    {
                //        if (isRichTextBox)
                //        {
                //            dll_function.SetCursor(Cursors.IBeam.Handle);
                //            m.Result = new IntPtr(1); // возврат истины
                //            return;
                //        }
                //        break;
                //    }

            }
            base.WndProc(ref m);
        }

        public void Init()
        {
            //this.Log.Invoke(new Action(() => {
            //    this.Log.Text = "Hello " + Environment.UserName + "!" + Environment.NewLine;
            //    this.Log.SelectionStart = this.Log.Text.Length;
            //}));
            this.Log.Text = "Hello " + Environment.UserName + "!" + Environment.NewLine;
            this.Log.SelectionStart = this.Log.Text.Length;
        }

        private void Console_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void ContexCopy_Click(object sender, EventArgs e)
        {
            if(this.Log.SelectedText != "")
                Clipboard.SetText(this.Log.SelectedText);
        }

        private void Log_MouseLeave(object sender, EventArgs e)
        {
           // dll_function.SetCursor(Cursors.Arrow.Handle);
            this.isRichTextBox = false;
        }

        private void Log_MouseMove(object sender, MouseEventArgs e)
        {
            this.isRichTextBox = true;
        }
    }

}

// TODO: реализовать обрезание текста в зависимости от размера