namespace gui
{
    partial class main
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(main));
            this.MainMenu = new System.Windows.Forms.MenuStrip();
            this.menuTools = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsNet = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsModeDraw = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsModeDrawWireframe = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsModeDrawSolid = new System.Windows.Forms.ToolStripMenuItem();
            this.openFile = new System.Windows.Forms.OpenFileDialog();
            this.panel1 = new System.Windows.Forms.Panel();
            this.selectModel = new System.Windows.Forms.RadioButton();
            this.noTool = new System.Windows.Forms.RadioButton();
            this.PanelDirectX3D = new System.Windows.Forms.Panel();
            this.mainSplit = new System.Windows.Forms.SplitContainer();
            this.renderAndModelListPanels = new System.Windows.Forms.SplitContainer();
            this.listModels = new System.Windows.Forms.SplitContainer();
            this.listModelsButton = new System.Windows.Forms.Button();
            this.listFiles = new components.Controls.TabControl2(this.components);
            this.PageListStaticModels = new System.Windows.Forms.TabPage();
            this.listFilesStaticModels = new components.Controls.ListViewsAllFiles(this.components);
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.MainMenu.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.mainSplit)).BeginInit();
            this.mainSplit.Panel1.SuspendLayout();
            this.mainSplit.Panel2.SuspendLayout();
            this.mainSplit.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.renderAndModelListPanels)).BeginInit();
            this.renderAndModelListPanels.Panel1.SuspendLayout();
            this.renderAndModelListPanels.Panel2.SuspendLayout();
            this.renderAndModelListPanels.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.listModels)).BeginInit();
            this.listModels.Panel1.SuspendLayout();
            this.listModels.Panel2.SuspendLayout();
            this.listModels.SuspendLayout();
            this.listFiles.SuspendLayout();
            this.PageListStaticModels.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainMenu
            // 
            this.MainMenu.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.MainMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuTools});
            this.MainMenu.Location = new System.Drawing.Point(0, 0);
            this.MainMenu.Name = "MainMenu";
            this.MainMenu.Size = new System.Drawing.Size(1231, 24);
            this.MainMenu.TabIndex = 0;
            this.MainMenu.Text = "menuStrip1";
            // 
            // menuTools
            // 
            this.menuTools.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuToolsNet,
            this.menuToolsModeDraw});
            this.menuTools.ImageTransparentColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuTools.Name = "menuTools";
            this.menuTools.Size = new System.Drawing.Size(95, 20);
            this.menuTools.Text = "Инструменты";
            // 
            // menuToolsNet
            // 
            this.menuToolsNet.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuToolsNet.Checked = true;
            this.menuToolsNet.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuToolsNet.ImageTransparentColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuToolsNet.Name = "menuToolsNet";
            this.menuToolsNet.Size = new System.Drawing.Size(180, 22);
            this.menuToolsNet.Text = "Сетка";
            this.menuToolsNet.Click += new System.EventHandler(this.menuToolsNetClick);
            // 
            // menuToolsModeDraw
            // 
            this.menuToolsModeDraw.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuToolsModeDraw.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuToolsModeDrawWireframe,
            this.menuToolsModeDrawSolid});
            this.menuToolsModeDraw.Name = "menuToolsModeDraw";
            this.menuToolsModeDraw.Size = new System.Drawing.Size(180, 22);
            this.menuToolsModeDraw.Text = "Режим отрисовку";
            // 
            // menuToolsModeDrawWireframe
            // 
            this.menuToolsModeDrawWireframe.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuToolsModeDrawWireframe.Name = "menuToolsModeDrawWireframe";
            this.menuToolsModeDrawWireframe.Size = new System.Drawing.Size(180, 22);
            this.menuToolsModeDrawWireframe.Tag = "0";
            this.menuToolsModeDrawWireframe.Text = "Каркас";
            this.menuToolsModeDrawWireframe.Click += new System.EventHandler(this.menuToolsModeDrawClick);
            // 
            // menuToolsModeDrawSolid
            // 
            this.menuToolsModeDrawSolid.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.menuToolsModeDrawSolid.Checked = true;
            this.menuToolsModeDrawSolid.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuToolsModeDrawSolid.Name = "menuToolsModeDrawSolid";
            this.menuToolsModeDrawSolid.Size = new System.Drawing.Size(180, 22);
            this.menuToolsModeDrawSolid.Tag = "1";
            this.menuToolsModeDrawSolid.Text = "Объем";
            this.menuToolsModeDrawSolid.Click += new System.EventHandler(this.menuToolsModeDrawClick);
            // 
            // openFile
            // 
            this.openFile.Filter = "Wirefram Object|*.obj";
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.panel1.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.panel1.Controls.Add(this.selectModel);
            this.panel1.Controls.Add(this.noTool);
            this.panel1.Location = new System.Drawing.Point(0, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(56, 555);
            this.panel1.TabIndex = 2;
            // 
            // selectModel
            // 
            this.selectModel.Appearance = System.Windows.Forms.Appearance.Button;
            this.selectModel.AutoSize = true;
            this.selectModel.Image = ((System.Drawing.Image)(resources.GetObject("selectModel.Image")));
            this.selectModel.Location = new System.Drawing.Point(10, 47);
            this.selectModel.Name = "selectModel";
            this.selectModel.Size = new System.Drawing.Size(38, 38);
            this.selectModel.TabIndex = 1;
            this.selectModel.UseVisualStyleBackColor = true;
            // 
            // noTool
            // 
            this.noTool.Appearance = System.Windows.Forms.Appearance.Button;
            this.noTool.AutoSize = true;
            this.noTool.Checked = true;
            this.noTool.Image = global::gui.Properties.Resources.no_tool;
            this.noTool.Location = new System.Drawing.Point(10, 3);
            this.noTool.Name = "noTool";
            this.noTool.Size = new System.Drawing.Size(38, 38);
            this.noTool.TabIndex = 0;
            this.noTool.TabStop = true;
            this.noTool.UseVisualStyleBackColor = true;
            // 
            // PanelDirectX3D
            // 
            this.PanelDirectX3D.AllowDrop = true;
            this.PanelDirectX3D.Location = new System.Drawing.Point(0, 0);
            this.PanelDirectX3D.Name = "PanelDirectX3D";
            this.PanelDirectX3D.Size = new System.Drawing.Size(953, 387);
            this.PanelDirectX3D.TabIndex = 3;
            this.PanelDirectX3D.DragDrop += new System.Windows.Forms.DragEventHandler(this.PanelDirectX3D_DragDrop);
            this.PanelDirectX3D.DragEnter += new System.Windows.Forms.DragEventHandler(this.PanelDirectX3D_DragEnter);
            this.PanelDirectX3D.Resize += new System.EventHandler(this.RenderPanelResize);
            // 
            // mainSplit
            // 
            this.mainSplit.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mainSplit.Location = new System.Drawing.Point(62, 24);
            this.mainSplit.Name = "mainSplit";
            // 
            // mainSplit.Panel1
            // 
            this.mainSplit.Panel1.Controls.Add(this.renderAndModelListPanels);
            this.mainSplit.Panel1MinSize = 10;
            // 
            // mainSplit.Panel2
            // 
            this.mainSplit.Panel2.Controls.Add(this.splitContainer2);
            this.mainSplit.Panel2MinSize = 100;
            this.mainSplit.Size = new System.Drawing.Size(1157, 555);
            this.mainSplit.SplitterDistance = 953;
            this.mainSplit.TabIndex = 4;
            this.mainSplit.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.mainSplit_SplitterMoved);
            // 
            // renderAndModelListPanels
            // 
            this.renderAndModelListPanels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.renderAndModelListPanels.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.renderAndModelListPanels.IsSplitterFixed = true;
            this.renderAndModelListPanels.Location = new System.Drawing.Point(0, 0);
            this.renderAndModelListPanels.Name = "renderAndModelListPanels";
            this.renderAndModelListPanels.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // renderAndModelListPanels.Panel1
            // 
            this.renderAndModelListPanels.Panel1.Controls.Add(this.PanelDirectX3D);
            // 
            // renderAndModelListPanels.Panel2
            // 
            this.renderAndModelListPanels.Panel2.Controls.Add(this.listModels);
            this.renderAndModelListPanels.Size = new System.Drawing.Size(953, 555);
            this.renderAndModelListPanels.SplitterDistance = 387;
            this.renderAndModelListPanels.TabIndex = 5;
            // 
            // listModels
            // 
            this.listModels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listModels.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.listModels.IsSplitterFixed = true;
            this.listModels.Location = new System.Drawing.Point(0, 0);
            this.listModels.Name = "listModels";
            this.listModels.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // listModels.Panel1
            // 
            this.listModels.Panel1.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.listModels.Panel1.CausesValidation = false;
            this.listModels.Panel1.Controls.Add(this.listModelsButton);
            this.listModels.Panel1MinSize = 10;
            // 
            // listModels.Panel2
            // 
            this.listModels.Panel2.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.listModels.Panel2.Controls.Add(this.listFiles);
            this.listModels.Size = new System.Drawing.Size(953, 164);
            this.listModels.SplitterDistance = 25;
            this.listModels.TabIndex = 4;
            // 
            // listModelsButton
            // 
            this.listModelsButton.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.listModelsButton.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.listModelsButton.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listModelsButton.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.listModelsButton.Location = new System.Drawing.Point(0, 0);
            this.listModelsButton.Name = "listModelsButton";
            this.listModelsButton.Size = new System.Drawing.Size(953, 25);
            this.listModelsButton.TabIndex = 0;
            this.listModelsButton.UseVisualStyleBackColor = false;
            this.listModelsButton.Click += new System.EventHandler(this.listModelsButton_Click);
            // 
            // listFiles
            // 
            this.listFiles.Controls.Add(this.PageListStaticModels);
            this.listFiles.Controls.Add(this.tabPage2);
            this.listFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listFiles.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
            this.listFiles.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.listFiles.Location = new System.Drawing.Point(0, 0);
            this.listFiles.Name = "listFiles";
            this.listFiles.SelectedIndex = 0;
            this.listFiles.Size = new System.Drawing.Size(953, 135);
            this.listFiles.TabIndex = 0;
            // 
            // PageListStaticModels
            // 
            this.PageListStaticModels.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.PageListStaticModels.Controls.Add(this.listFilesStaticModels);
            this.PageListStaticModels.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.PageListStaticModels.Location = new System.Drawing.Point(4, 25);
            this.PageListStaticModels.Name = "PageListStaticModels";
            this.PageListStaticModels.Padding = new System.Windows.Forms.Padding(3);
            this.PageListStaticModels.Size = new System.Drawing.Size(945, 106);
            this.PageListStaticModels.TabIndex = 0;
            this.PageListStaticModels.Text = "Каталог объектов";
            // 
            // listFilesStaticModels
            // 
            this.listFilesStaticModels.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.listFilesStaticModels.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.listFilesStaticModels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listFilesStaticModels.Extension = ".pro_object";
            this.listFilesStaticModels.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.listFilesStaticModels.Location = new System.Drawing.Point(3, 3);
            this.listFilesStaticModels.MultiSelect = false;
            this.listFilesStaticModels.Name = "listFilesStaticModels";
            this.listFilesStaticModels.RootCatalogue = "engine_resource\\models";
            this.listFilesStaticModels.Size = new System.Drawing.Size(939, 100);
            this.listFilesStaticModels.TabIndex = 0;
            this.listFilesStaticModels.UseCompatibleStateImageBehavior = false;
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.tabPage2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tabPage2.Location = new System.Drawing.Point(4, 25);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(945, 106);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "tabPage2";
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.BackColor = System.Drawing.SystemColors.HotTrack;
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.splitContainer2.Size = new System.Drawing.Size(200, 555);
            this.splitContainer2.SplitterDistance = 280;
            this.splitContainer2.TabIndex = 0;
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ClientSize = new System.Drawing.Size(1231, 579);
            this.Controls.Add(this.mainSplit);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.MainMenu);
            this.DoubleBuffered = true;
            this.MainMenuStrip = this.MainMenu;
            this.Name = "main";
            this.Text = "main";
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.GlobalKeyDown);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.GlobalKeyUp);
            this.Resize += new System.EventHandler(this.GlobalResize);
            this.MainMenu.ResumeLayout(false);
            this.MainMenu.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.mainSplit.Panel1.ResumeLayout(false);
            this.mainSplit.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.mainSplit)).EndInit();
            this.mainSplit.ResumeLayout(false);
            this.renderAndModelListPanels.Panel1.ResumeLayout(false);
            this.renderAndModelListPanels.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.renderAndModelListPanels)).EndInit();
            this.renderAndModelListPanels.ResumeLayout(false);
            this.listModels.Panel1.ResumeLayout(false);
            this.listModels.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.listModels)).EndInit();
            this.listModels.ResumeLayout(false);
            this.listFiles.ResumeLayout(false);
            this.PageListStaticModels.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.MenuStrip MainMenu;
        private System.Windows.Forms.OpenFileDialog openFile;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.RadioButton selectModel;
        private System.Windows.Forms.RadioButton noTool;
        private System.Windows.Forms.Panel PanelDirectX3D;
        private System.Windows.Forms.SplitContainer mainSplit;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ToolStripMenuItem menuTools;
        private System.Windows.Forms.ToolStripMenuItem menuToolsNet;
        private System.Windows.Forms.ToolStripMenuItem menuToolsModeDraw;
        private System.Windows.Forms.ToolStripMenuItem menuToolsModeDrawWireframe;
        private System.Windows.Forms.ToolStripMenuItem menuToolsModeDrawSolid;
        private System.Windows.Forms.SplitContainer listModels;
        private System.Windows.Forms.SplitContainer renderAndModelListPanels;
        private System.Windows.Forms.Button listModelsButton;
        private components.Controls.TabControl2 listFiles;
        private System.Windows.Forms.TabPage PageListStaticModels;
        private System.Windows.Forms.TabPage tabPage2;
        private components.Controls.ListViewsAllFiles listFilesStaticModels;
    }
}