namespace FAP50Demo
{
    partial class FormAutoCapture
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
            components = new System.ComponentModel.Container();
            panel1 = new Panel();
            PictureBox_Video = new PictureBox();
            btn_sign_done = new Button();
            btn_sign_clear = new Button();
            pictureBox2 = new PictureBox();
            buttonRoll10 = new Button();
            buttonRoll9 = new Button();
            buttonRoll8 = new Button();
            buttonRoll7 = new Button();
            buttonRoll6 = new Button();
            buttonRoll5 = new Button();
            buttonRoll4 = new Button();
            buttonRoll3 = new Button();
            buttonRoll2 = new Button();
            buttonRoll1 = new Button();
            buttonFlat3 = new Button();
            buttonFlat2 = new Button();
            buttonFlat1 = new Button();
            panel2 = new Panel();
            sampling_msg = new Label();
            pictureBox1 = new PictureBox();
            timer_btn_blink = new System.Windows.Forms.Timer(components);
            timer_flat = new System.Windows.Forms.Timer(components);
            timer_roll = new System.Windows.Forms.Timer(components);
            timer_sign = new System.Windows.Forms.Timer(components);
            panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)PictureBox_Video).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox2).BeginInit();
            panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
            SuspendLayout();
            // 
            // panel1
            // 
            panel1.Controls.Add(PictureBox_Video);
            panel1.Controls.Add(btn_sign_done);
            panel1.Controls.Add(btn_sign_clear);
            panel1.Controls.Add(pictureBox2);
            panel1.Controls.Add(buttonRoll10);
            panel1.Controls.Add(buttonRoll9);
            panel1.Controls.Add(buttonRoll8);
            panel1.Controls.Add(buttonRoll7);
            panel1.Controls.Add(buttonRoll6);
            panel1.Controls.Add(buttonRoll5);
            panel1.Controls.Add(buttonRoll4);
            panel1.Controls.Add(buttonRoll3);
            panel1.Controls.Add(buttonRoll2);
            panel1.Controls.Add(buttonRoll1);
            panel1.Controls.Add(buttonFlat3);
            panel1.Controls.Add(buttonFlat2);
            panel1.Controls.Add(buttonFlat1);
            panel1.Dock = DockStyle.Left;
            panel1.Location = new Point(0, 0);
            panel1.Name = "panel1";
            panel1.Size = new Size(288, 583);
            panel1.TabIndex = 0;
            // 
            // PictureBox_Video
            // 
            PictureBox_Video.Location = new Point(4, 9);
            PictureBox_Video.Name = "PictureBox_Video";
            PictureBox_Video.Size = new Size(281, 161);
            PictureBox_Video.SizeMode = PictureBoxSizeMode.Zoom;
            PictureBox_Video.TabIndex = 18;
            PictureBox_Video.TabStop = false;
            // 
            // btn_sign_done
            // 
            btn_sign_done.Location = new Point(211, 463);
            btn_sign_done.Name = "btn_sign_done";
            btn_sign_done.Size = new Size(61, 26);
            btn_sign_done.TabIndex = 17;
            btn_sign_done.Text = "Done";
            btn_sign_done.UseVisualStyleBackColor = true;
            btn_sign_done.Visible = false;
            btn_sign_done.Click += btn_sign_done_Click;
            // 
            // btn_sign_clear
            // 
            btn_sign_clear.Location = new Point(144, 463);
            btn_sign_clear.Name = "btn_sign_clear";
            btn_sign_clear.Size = new Size(61, 26);
            btn_sign_clear.TabIndex = 17;
            btn_sign_clear.Text = "Clear";
            btn_sign_clear.UseVisualStyleBackColor = true;
            btn_sign_clear.Visible = false;
            btn_sign_clear.Click += btn_sign_clear_Click;
            // 
            // pictureBox2
            // 
            pictureBox2.Dock = DockStyle.Bottom;
            pictureBox2.Location = new Point(0, 495);
            pictureBox2.Name = "pictureBox2";
            pictureBox2.Size = new Size(288, 88);
            pictureBox2.TabIndex = 16;
            pictureBox2.TabStop = false;
            // 
            // buttonRoll10
            // 
            buttonRoll10.Location = new Point(148, 413);
            buttonRoll10.Name = "buttonRoll10";
            buttonRoll10.Size = new Size(82, 41);
            buttonRoll10.TabIndex = 15;
            buttonRoll10.Text = "Roll Right Little";
            buttonRoll10.UseVisualStyleBackColor = true;
            // 
            // buttonRoll9
            // 
            buttonRoll9.Location = new Point(148, 366);
            buttonRoll9.Name = "buttonRoll9";
            buttonRoll9.Size = new Size(82, 41);
            buttonRoll9.TabIndex = 14;
            buttonRoll9.Text = "Roll Right Ring";
            buttonRoll9.UseVisualStyleBackColor = true;
            // 
            // buttonRoll8
            // 
            buttonRoll8.Location = new Point(148, 319);
            buttonRoll8.Name = "buttonRoll8";
            buttonRoll8.Size = new Size(82, 41);
            buttonRoll8.TabIndex = 13;
            buttonRoll8.Text = "Roll Right Middle";
            buttonRoll8.UseVisualStyleBackColor = true;
            // 
            // buttonRoll7
            // 
            buttonRoll7.Location = new Point(148, 273);
            buttonRoll7.Name = "buttonRoll7";
            buttonRoll7.Size = new Size(82, 41);
            buttonRoll7.TabIndex = 12;
            buttonRoll7.Text = "Roll Right Index";
            buttonRoll7.UseVisualStyleBackColor = true;
            // 
            // buttonRoll6
            // 
            buttonRoll6.Location = new Point(148, 226);
            buttonRoll6.Name = "buttonRoll6";
            buttonRoll6.Size = new Size(82, 41);
            buttonRoll6.TabIndex = 11;
            buttonRoll6.Text = "Roll Right Thumb";
            buttonRoll6.UseVisualStyleBackColor = true;
            // 
            // buttonRoll5
            // 
            buttonRoll5.Location = new Point(60, 413);
            buttonRoll5.Name = "buttonRoll5";
            buttonRoll5.Size = new Size(82, 41);
            buttonRoll5.TabIndex = 10;
            buttonRoll5.Text = "Roll Left Little";
            buttonRoll5.UseVisualStyleBackColor = true;
            // 
            // buttonRoll4
            // 
            buttonRoll4.Location = new Point(60, 366);
            buttonRoll4.Name = "buttonRoll4";
            buttonRoll4.Size = new Size(82, 41);
            buttonRoll4.TabIndex = 9;
            buttonRoll4.Text = "Roll Left Ring";
            buttonRoll4.UseVisualStyleBackColor = true;
            // 
            // buttonRoll3
            // 
            buttonRoll3.Location = new Point(60, 320);
            buttonRoll3.Name = "buttonRoll3";
            buttonRoll3.Size = new Size(82, 41);
            buttonRoll3.TabIndex = 8;
            buttonRoll3.Text = "Roll Left Middle";
            buttonRoll3.UseVisualStyleBackColor = true;
            // 
            // buttonRoll2
            // 
            buttonRoll2.Location = new Point(60, 273);
            buttonRoll2.Name = "buttonRoll2";
            buttonRoll2.Size = new Size(82, 41);
            buttonRoll2.TabIndex = 7;
            buttonRoll2.Text = "Roll Left Index";
            buttonRoll2.UseVisualStyleBackColor = true;
            // 
            // buttonRoll1
            // 
            buttonRoll1.Location = new Point(60, 226);
            buttonRoll1.Name = "buttonRoll1";
            buttonRoll1.Size = new Size(82, 41);
            buttonRoll1.TabIndex = 6;
            buttonRoll1.Text = "Roll Left Thumb";
            buttonRoll1.UseVisualStyleBackColor = true;
            // 
            // buttonFlat3
            // 
            buttonFlat3.Location = new Point(191, 176);
            buttonFlat3.Name = "buttonFlat3";
            buttonFlat3.Size = new Size(82, 41);
            buttonFlat3.TabIndex = 5;
            buttonFlat3.Text = "Flat Two Thumbs";
            buttonFlat3.UseVisualStyleBackColor = true;
            // 
            // buttonFlat2
            // 
            buttonFlat2.Location = new Point(103, 176);
            buttonFlat2.Name = "buttonFlat2";
            buttonFlat2.Size = new Size(82, 41);
            buttonFlat2.TabIndex = 4;
            buttonFlat2.Text = "Flat Right Four";
            buttonFlat2.UseVisualStyleBackColor = true;
            // 
            // buttonFlat1
            // 
            buttonFlat1.Location = new Point(15, 176);
            buttonFlat1.Name = "buttonFlat1";
            buttonFlat1.Size = new Size(82, 41);
            buttonFlat1.TabIndex = 3;
            buttonFlat1.Text = "Flat Left Four";
            buttonFlat1.UseVisualStyleBackColor = true;
            // 
            // panel2
            // 
            panel2.BackColor = Color.White;
            panel2.Controls.Add(sampling_msg);
            panel2.Controls.Add(pictureBox1);
            panel2.Dock = DockStyle.Fill;
            panel2.Location = new Point(288, 0);
            panel2.Name = "panel2";
            panel2.Size = new Size(976, 583);
            panel2.TabIndex = 1;
            // 
            // sampling_msg
            // 
            sampling_msg.AutoSize = true;
            sampling_msg.Location = new Point(2, 2);
            sampling_msg.Name = "sampling_msg";
            sampling_msg.Size = new Size(16, 15);
            sampling_msg.TabIndex = 2;
            sampling_msg.Text = "...";
            // 
            // pictureBox1
            // 
            pictureBox1.Location = new Point(92, 32);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new Size(805, 457);
            pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBox1.TabIndex = 1;
            pictureBox1.TabStop = false;
            // 
            // timer_btn_blink
            // 
            //timer_btn_blink.Interval = 500;
            //timer_btn_blink.Tick += timer_btn_blink_Tick;
            // 
            // timer_flat
            // 
            //timer_flat.Interval = 500;
            //timer_flat.Tick += timer_flat_Tick;
            // 
            // timer_roll
            // 
            //timer_roll.Tick += timer_roll_Tick;
            // 
            // timer_sign
            // 
            timer_sign.Interval = 30;
            timer_sign.Tick += timer_sign_Tick;
            // 
            // FormAutoCapture
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1264, 583);
            Controls.Add(panel2);
            Controls.Add(panel1);
            Name = "FormAutoCapture";
            StartPosition = FormStartPosition.CenterParent;
            Text = "Auto Capture";
            FormClosing += FormAutoCapture_FormClosing;
            Load += FormAutoCapture_Load;
            Resize += FormAutoCapture_Resize;
            panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)PictureBox_Video).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox2).EndInit();
            panel2.ResumeLayout(false);
            panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
            ResumeLayout(false);
        }

        #endregion

        private Panel panel1;
        private Panel panel2;
        private Button buttonFlat1;
        private Button buttonRoll10;
        private Button buttonRoll9;
        private Button buttonRoll8;
        private Button buttonRoll7;
        private Button buttonRoll6;
        private Button buttonRoll5;
        private Button buttonRoll4;
        private Button buttonRoll3;
        private Button buttonRoll2;
        private Button buttonRoll1;
        private Button buttonFlat3;
        private Button buttonFlat2;
        private System.Windows.Forms.Timer timer_btn_blink;
        private System.Windows.Forms.Timer timer_flat;
        private System.Windows.Forms.Timer timer_roll;
        private PictureBox pictureBox1;
        private PictureBox pictureBox2;
        private Label sampling_msg;
        private System.Windows.Forms.Timer timer_sign;
        private Button btn_sign_done;
        private Button btn_sign_clear;
        private PictureBox PictureBox_Video;
    }
}