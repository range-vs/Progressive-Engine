namespace console
{
    partial class Console
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.Log = new System.Windows.Forms.RichTextBox();
            this.ContexMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ContexCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.ContexMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // Log
            // 
            this.Log.BackColor = System.Drawing.SystemColors.MenuText;
            this.Log.ContextMenuStrip = this.ContexMenu;
            this.Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Log.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.Log.ForeColor = System.Drawing.SystemColors.Window;
            this.Log.Location = new System.Drawing.Point(0, 0);
            this.Log.MaxLength = 2000000;
            this.Log.Name = "Log";
            this.Log.ReadOnly = true;
            this.Log.Size = new System.Drawing.Size(696, 302);
            this.Log.TabIndex = 0;
            this.Log.Text = "";
            this.Log.MouseLeave += new System.EventHandler(this.Log_MouseLeave);
            this.Log.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Log_MouseMove);
            // 
            // ContexMenu
            // 
            this.ContexMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ContexCopy});
            this.ContexMenu.Name = "ContexMenu";
            this.ContexMenu.Size = new System.Drawing.Size(140, 26);
            // 
            // ContexCopy
            // 
            this.ContexCopy.Name = "ContexCopy";
            this.ContexCopy.Size = new System.Drawing.Size(139, 22);
            this.ContexCopy.Text = "Копировать";
            this.ContexCopy.Click += new System.EventHandler(this.ContexCopy_Click);
            // 
            // Console
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(696, 302);
            this.Controls.Add(this.Log);
            this.Name = "Console";
            this.Text = "Console";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Console_FormClosing);
            this.ContexMenu.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox Log;
        private System.Windows.Forms.ContextMenuStrip ContexMenu;
        private System.Windows.Forms.ToolStripMenuItem ContexCopy;
    }
}

