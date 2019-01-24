namespace lanch
{
    partial class HelloWindowProperties
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
            this.graphicsSelectLabel = new System.Windows.Forms.Label();
            this.graphicsSelect = new System.Windows.Forms.ComboBox();
            this.startButton = new System.Windows.Forms.Button();
            this.engineTitleText = new System.Windows.Forms.Label();
            this.programStartLabel = new System.Windows.Forms.Label();
            this.programStart = new System.Windows.Forms.ComboBox();
            this.typeDeviceLabel = new System.Windows.Forms.Label();
            this.typeDevice = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // graphicsSelectLabel
            // 
            this.graphicsSelectLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.graphicsSelectLabel.Location = new System.Drawing.Point(12, 59);
            this.graphicsSelectLabel.Name = "graphicsSelectLabel";
            this.graphicsSelectLabel.Size = new System.Drawing.Size(204, 21);
            this.graphicsSelectLabel.TabIndex = 0;
            this.graphicsSelectLabel.Text = "Выберите графический адаптер:";
            this.graphicsSelectLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // graphicsSelect
            // 
            this.graphicsSelect.BackColor = System.Drawing.SystemColors.Info;
            this.graphicsSelect.Cursor = System.Windows.Forms.Cursors.Hand;
            this.graphicsSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.graphicsSelect.FormattingEnabled = true;
            this.graphicsSelect.ImeMode = System.Windows.Forms.ImeMode.AlphaFull;
            this.graphicsSelect.Location = new System.Drawing.Point(222, 59);
            this.graphicsSelect.Name = "graphicsSelect";
            this.graphicsSelect.Size = new System.Drawing.Size(251, 21);
            this.graphicsSelect.TabIndex = 1;
            // 
            // startButton
            // 
            this.startButton.Location = new System.Drawing.Point(398, 158);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(75, 24);
            this.startButton.TabIndex = 2;
            this.startButton.Text = "Запустить";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.StartEngineButtonProc);
            // 
            // engineTitleText
            // 
            this.engineTitleText.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.engineTitleText.Location = new System.Drawing.Point(12, 9);
            this.engineTitleText.Name = "engineTitleText";
            this.engineTitleText.Size = new System.Drawing.Size(461, 20);
            this.engineTitleText.TabIndex = 4;
            this.engineTitleText.Text = "Progressive Engine build v 0.9, стартовые настройки:";
            this.engineTitleText.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // programStartLabel
            // 
            this.programStartLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.programStartLabel.Location = new System.Drawing.Point(12, 86);
            this.programStartLabel.Name = "programStartLabel";
            this.programStartLabel.Size = new System.Drawing.Size(204, 21);
            this.programStartLabel.TabIndex = 5;
            this.programStartLabel.Text = "Выберите приложение:";
            this.programStartLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // programStart
            // 
            this.programStart.BackColor = System.Drawing.SystemColors.Info;
            this.programStart.Cursor = System.Windows.Forms.Cursors.Hand;
            this.programStart.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.programStart.FormattingEnabled = true;
            this.programStart.ImeMode = System.Windows.Forms.ImeMode.AlphaFull;
            this.programStart.Items.AddRange(new object[] {
            "Игровой редактор",
            "Игра"});
            this.programStart.Location = new System.Drawing.Point(222, 86);
            this.programStart.Name = "programStart";
            this.programStart.Size = new System.Drawing.Size(251, 21);
            this.programStart.TabIndex = 6;
            // 
            // typeDeviceLabel
            // 
            this.typeDeviceLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.typeDeviceLabel.Location = new System.Drawing.Point(12, 110);
            this.typeDeviceLabel.Name = "typeDeviceLabel";
            this.typeDeviceLabel.Size = new System.Drawing.Size(204, 21);
            this.typeDeviceLabel.TabIndex = 7;
            this.typeDeviceLabel.Text = "Тип устройства:";
            this.typeDeviceLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // typeDevice
            // 
            this.typeDevice.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.typeDevice.Location = new System.Drawing.Point(222, 110);
            this.typeDevice.Name = "typeDevice";
            this.typeDevice.Size = new System.Drawing.Size(251, 21);
            this.typeDevice.TabIndex = 8;
            this.typeDevice.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // HelloWindowProperties
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.ClientSize = new System.Drawing.Size(485, 194);
            this.Controls.Add(this.typeDevice);
            this.Controls.Add(this.typeDeviceLabel);
            this.Controls.Add(this.programStart);
            this.Controls.Add(this.programStartLabel);
            this.Controls.Add(this.engineTitleText);
            this.Controls.Add(this.startButton);
            this.Controls.Add(this.graphicsSelect);
            this.Controls.Add(this.graphicsSelectLabel);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "HelloWindowProperties";
            this.Text = "Стартовые настройки";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormClosingProc);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label graphicsSelectLabel;
        private System.Windows.Forms.ComboBox graphicsSelect;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Label engineTitleText;
        private System.Windows.Forms.Label programStartLabel;
        private System.Windows.Forms.ComboBox programStart;
        private System.Windows.Forms.Label typeDeviceLabel;
        private System.Windows.Forms.Label typeDevice;
    }
}

