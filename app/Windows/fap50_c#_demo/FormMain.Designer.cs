namespace FAP50Demo
{
    partial class FormMain
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
            btn_start = new Button();
            btn_save_file = new Button();
            pictureBoxLogo = new PictureBox();
            device_info = new Label();
            timer_reset = new System.Windows.Forms.Timer(components);
            panel1 = new Panel();
            textBoxStatus = new RichTextBox();
            panel3 = new Panel();
            cb_score_ver = new ComboBox();
            label2 = new Label();
            cb_voice = new ComboBox();
            label1 = new Label();
            language = new Label();
            cb_lang = new ComboBox();
            groupBox1 = new GroupBox();
            radio_signature = new RadioButton();
            radio_any_finger = new RadioButton();
            radio_442F = new RadioButton();
            radio_442R = new RadioButton();
            fpImage2 = new PictureBox();
            fpImage0 = new PictureBox();
            fpImage1 = new PictureBox();
            fpImage3 = new PictureBox();
            fpImage5 = new PictureBox();
            fpImage6 = new PictureBox();
            fpImage7 = new PictureBox();
            fpImage8 = new PictureBox();
            fpImage4 = new PictureBox();
            fpImage9 = new PictureBox();
            fpImage_left4 = new PictureBox();
            fpImage_right4 = new PictureBox();
            fpImage_left_thumb = new PictureBox();
            fpImage_right_thumb = new PictureBox();
            panel2 = new Panel();
            ((System.ComponentModel.ISupportInitialize)pictureBoxLogo).BeginInit();
            panel1.SuspendLayout();
            panel3.SuspendLayout();
            groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)fpImage2).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage0).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage1).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage3).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage5).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage6).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage7).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage8).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage4).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage9).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_left4).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_right4).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_left_thumb).BeginInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_right_thumb).BeginInit();
            panel2.SuspendLayout();
            SuspendLayout();
            // 
            // btn_start
            // 
            btn_start.Location = new Point(15, 345);
            btn_start.Margin = new Padding(4, 4, 4, 4);
            btn_start.Name = "btn_start";
            btn_start.Size = new Size(153, 68);
            btn_start.TabIndex = 0;
            btn_start.Text = "Start";
            btn_start.UseVisualStyleBackColor = true;
            btn_start.Click += btn_start_Click;
            // 
            // btn_save_file
            // 
            btn_save_file.Location = new Point(182, 345);
            btn_save_file.Margin = new Padding(4, 4, 4, 4);
            btn_save_file.Name = "btn_save_file";
            btn_save_file.Size = new Size(146, 68);
            btn_save_file.TabIndex = 1;
            btn_save_file.Text = "Save File";
            btn_save_file.UseVisualStyleBackColor = true;
            btn_save_file.Click += btn_save_file_Click;
            // 
            // pictureBoxLogo
            // 
            pictureBoxLogo.Dock = DockStyle.Top;
            pictureBoxLogo.Image = Properties.Resources.pd;
            pictureBoxLogo.InitialImage = Properties.Resources.pd;
            pictureBoxLogo.Location = new Point(0, 0);
            pictureBoxLogo.Margin = new Padding(4, 4, 4, 4);
            pictureBoxLogo.Name = "pictureBoxLogo";
            pictureBoxLogo.Size = new Size(348, 177);
            pictureBoxLogo.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBoxLogo.TabIndex = 3;
            pictureBoxLogo.TabStop = false;
            // 
            // device_info
            // 
            device_info.AutoSize = true;
            device_info.Location = new Point(9, 423);
            device_info.Margin = new Padding(4, 0, 4, 0);
            device_info.Name = "device_info";
            device_info.Size = new Size(18, 20);
            device_info.TabIndex = 4;
            device_info.Text = "...";
            // 
            // timer_reset
            // 
            timer_reset.Enabled = true;
            timer_reset.Interval = 200;
            timer_reset.Tick += timerReset_Tick;
            // 
            // panel1
            // 
            panel1.Controls.Add(textBoxStatus);
            panel1.Controls.Add(panel3);
            panel1.Controls.Add(pictureBoxLogo);
            panel1.Dock = DockStyle.Right;
            panel1.Location = new Point(1096, 0);
            panel1.Margin = new Padding(4, 4, 4, 4);
            panel1.Name = "panel1";
            panel1.Size = new Size(348, 895);
            panel1.TabIndex = 6;
            // 
            // textBoxStatus
            // 
            textBoxStatus.BorderStyle = BorderStyle.None;
            textBoxStatus.Dock = DockStyle.Fill;
            textBoxStatus.Location = new Point(0, 772);
            textBoxStatus.Margin = new Padding(4, 4, 4, 4);
            textBoxStatus.Name = "textBoxStatus";
            textBoxStatus.Size = new Size(348, 123);
            textBoxStatus.TabIndex = 7;
            textBoxStatus.Text = "";
            // 
            // panel3
            // 
            panel3.Controls.Add(cb_score_ver);
            panel3.Controls.Add(label2);
            panel3.Controls.Add(cb_voice);
            panel3.Controls.Add(label1);
            panel3.Controls.Add(language);
            panel3.Controls.Add(cb_lang);
            panel3.Controls.Add(groupBox1);
            panel3.Controls.Add(btn_start);
            panel3.Controls.Add(device_info);
            panel3.Controls.Add(btn_save_file);
            panel3.Dock = DockStyle.Top;
            panel3.Location = new Point(0, 177);
            panel3.Margin = new Padding(4, 4, 4, 4);
            panel3.Name = "panel3";
            panel3.Size = new Size(348, 595);
            panel3.TabIndex = 6;
            // 
            // cb_score_ver
            // 
            cb_score_ver.FormattingEnabled = true;
            cb_score_ver.Location = new Point(178, 287);
            cb_score_ver.Margin = new Padding(4, 4, 4, 4);
            cb_score_ver.Name = "cb_score_ver";
            cb_score_ver.Size = new Size(138, 28);
            cb_score_ver.TabIndex = 13;
            cb_score_ver.SelectedIndexChanged += cb_score_ver_SelectedIndexChanged_1;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(38, 290);
            label2.Margin = new Padding(4, 0, 4, 0);
            label2.Name = "label2";
            label2.Size = new Size(97, 20);
            label2.TabIndex = 12;
            label2.Text = "Quality Score";
            // 
            // cb_voice
            // 
            cb_voice.FormattingEnabled = true;
            cb_voice.Location = new Point(178, 237);
            cb_voice.Margin = new Padding(4, 4, 4, 4);
            cb_voice.Name = "cb_voice";
            cb_voice.Size = new Size(138, 28);
            cb_voice.TabIndex = 11;
            cb_voice.SelectedIndexChanged += cb_voice_volume_SelectedIndexChanged;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(38, 243);
            label1.Margin = new Padding(4, 0, 4, 0);
            label1.Name = "label1";
            label1.Size = new Size(111, 20);
            label1.TabIndex = 10;
            label1.Text = "Speech Volume";
            // 
            // language
            // 
            language.AutoSize = true;
            language.Location = new Point(38, 193);
            language.Margin = new Padding(4, 0, 4, 0);
            language.Name = "language";
            language.Size = new Size(126, 20);
            language.TabIndex = 9;
            language.Text = "Speech Language";
            // 
            // cb_lang
            // 
            cb_lang.FormattingEnabled = true;
            cb_lang.Location = new Point(178, 188);
            cb_lang.Margin = new Padding(4, 4, 4, 4);
            cb_lang.Name = "cb_lang";
            cb_lang.Size = new Size(138, 28);
            cb_lang.TabIndex = 8;
            cb_lang.SelectedIndexChanged += cb_lang_SelectedIndexChanged;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(radio_signature);
            groupBox1.Controls.Add(radio_any_finger);
            groupBox1.Controls.Add(radio_442F);
            groupBox1.Controls.Add(radio_442R);
            groupBox1.Location = new Point(23, 18);
            groupBox1.Margin = new Padding(4, 4, 4, 4);
            groupBox1.Name = "groupBox1";
            groupBox1.Padding = new Padding(4, 4, 4, 4);
            groupBox1.Size = new Size(302, 142);
            groupBox1.TabIndex = 7;
            groupBox1.TabStop = false;
            groupBox1.Text = "Sample Sequence";
            // 
            // radio_signature
            // 
            radio_signature.AutoSize = true;
            radio_signature.Location = new Point(20, 102);
            radio_signature.Margin = new Padding(4, 4, 4, 4);
            radio_signature.Name = "radio_signature";
            radio_signature.Size = new Size(93, 24);
            radio_signature.TabIndex = 8;
            radio_signature.TabStop = true;
            radio_signature.Text = "Signature";
            radio_signature.UseVisualStyleBackColor = true;
            radio_signature.Visible = false;
            // 
            // radio_any_finger
            // 
            radio_any_finger.AutoSize = true;
            radio_any_finger.Location = new Point(140, 36);
            radio_any_finger.Margin = new Padding(4, 4, 4, 4);
            radio_any_finger.Name = "radio_any_finger";
            radio_any_finger.Size = new Size(119, 24);
            radio_any_finger.TabIndex = 7;
            radio_any_finger.Text = "Some Fingers";
            radio_any_finger.UseVisualStyleBackColor = true;
            radio_any_finger.Visible = false;
            radio_any_finger.CheckedChanged += radio_any_finger_CheckedChanged;
            // 
            // radio_442F
            // 
            radio_442F.AutoSize = true;
            radio_442F.Checked = true;
            radio_442F.Location = new Point(20, 36);
            radio_442F.Margin = new Padding(4, 4, 4, 4);
            radio_442F.Name = "radio_442F";
            radio_442F.Size = new Size(94, 24);
            radio_442F.TabIndex = 5;
            radio_442F.TabStop = true;
            radio_442F.Text = "4-4-2 Flat";
            radio_442F.UseVisualStyleBackColor = true;
            radio_442F.CheckedChanged += radio_442F_CheckedChanged;
            // 
            // radio_442R
            // 
            radio_442R.AutoSize = true;
            radio_442R.Location = new Point(20, 68);
            radio_442R.Margin = new Padding(4, 4, 4, 4);
            radio_442R.Name = "radio_442R";
            radio_442R.Size = new Size(96, 24);
            radio_442R.TabIndex = 6;
            radio_442R.Text = "4-4-2 Roll";
            radio_442R.UseVisualStyleBackColor = true;
            radio_442R.CheckedChanged += radio_442R_CheckedChanged;
            // 
            // fpImage2
            // 
            fpImage2.BorderStyle = BorderStyle.FixedSingle;
            fpImage2.Location = new Point(466, 136);
            fpImage2.Margin = new Padding(4, 4, 4, 4);
            fpImage2.Name = "fpImage2";
            fpImage2.Size = new Size(106, 112);
            fpImage2.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage2.TabIndex = 2;
            fpImage2.TabStop = false;
            fpImage2.Paint += fpImage2_Paint;
            fpImage2.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage0
            // 
            fpImage0.BorderStyle = BorderStyle.FixedSingle;
            fpImage0.Location = new Point(142, 136);
            fpImage0.Margin = new Padding(4, 4, 4, 4);
            fpImage0.Name = "fpImage0";
            fpImage0.Size = new Size(106, 112);
            fpImage0.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage0.TabIndex = 0;
            fpImage0.TabStop = false;
            fpImage0.Paint += fpImage0_Paint;
            fpImage0.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage1
            // 
            fpImage1.BorderStyle = BorderStyle.FixedSingle;
            fpImage1.Location = new Point(304, 136);
            fpImage1.Margin = new Padding(4, 4, 4, 4);
            fpImage1.Name = "fpImage1";
            fpImage1.Size = new Size(106, 112);
            fpImage1.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage1.TabIndex = 1;
            fpImage1.TabStop = false;
            fpImage1.Paint += fpImage1_Paint;
            fpImage1.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage3
            // 
            fpImage3.BorderStyle = BorderStyle.FixedSingle;
            fpImage3.Location = new Point(628, 136);
            fpImage3.Margin = new Padding(4, 4, 4, 4);
            fpImage3.Name = "fpImage3";
            fpImage3.Size = new Size(106, 112);
            fpImage3.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage3.TabIndex = 3;
            fpImage3.TabStop = false;
            fpImage3.Paint += fpImage3_Paint;
            fpImage3.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage5
            // 
            fpImage5.BorderStyle = BorderStyle.FixedSingle;
            fpImage5.Location = new Point(142, 288);
            fpImage5.Margin = new Padding(4, 4, 4, 4);
            fpImage5.Name = "fpImage5";
            fpImage5.Size = new Size(106, 112);
            fpImage5.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage5.TabIndex = 5;
            fpImage5.TabStop = false;
            fpImage5.Paint += fpImage5_Paint;
            fpImage5.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage6
            // 
            fpImage6.BorderStyle = BorderStyle.FixedSingle;
            fpImage6.Location = new Point(304, 288);
            fpImage6.Margin = new Padding(4, 4, 4, 4);
            fpImage6.Name = "fpImage6";
            fpImage6.Size = new Size(106, 112);
            fpImage6.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage6.TabIndex = 6;
            fpImage6.TabStop = false;
            fpImage6.Paint += fpImage6_Paint;
            fpImage6.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage7
            // 
            fpImage7.BorderStyle = BorderStyle.FixedSingle;
            fpImage7.Location = new Point(466, 288);
            fpImage7.Margin = new Padding(4, 4, 4, 4);
            fpImage7.Name = "fpImage7";
            fpImage7.Size = new Size(106, 112);
            fpImage7.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage7.TabIndex = 7;
            fpImage7.TabStop = false;
            fpImage7.Paint += fpImage7_Paint;
            fpImage7.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage8
            // 
            fpImage8.BorderStyle = BorderStyle.FixedSingle;
            fpImage8.Location = new Point(628, 288);
            fpImage8.Margin = new Padding(4, 4, 4, 4);
            fpImage8.Name = "fpImage8";
            fpImage8.Size = new Size(106, 112);
            fpImage8.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage8.TabIndex = 8;
            fpImage8.TabStop = false;
            fpImage8.Paint += fpImage8_Paint;
            fpImage8.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage4
            // 
            fpImage4.BorderStyle = BorderStyle.FixedSingle;
            fpImage4.Location = new Point(791, 136);
            fpImage4.Margin = new Padding(4, 4, 4, 4);
            fpImage4.Name = "fpImage4";
            fpImage4.Size = new Size(106, 112);
            fpImage4.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage4.TabIndex = 4;
            fpImage4.TabStop = false;
            fpImage4.Paint += fpImage4_Paint;
            fpImage4.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage9
            // 
            fpImage9.BorderStyle = BorderStyle.FixedSingle;
            fpImage9.Location = new Point(791, 288);
            fpImage9.Margin = new Padding(4, 4, 4, 4);
            fpImage9.Name = "fpImage9";
            fpImage9.Size = new Size(106, 112);
            fpImage9.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage9.TabIndex = 9;
            fpImage9.TabStop = false;
            fpImage9.Paint += fpImage9_Paint;
            fpImage9.DoubleClick += fpImage_DoubleClick;
            // 
            // fpImage_left4
            // 
            fpImage_left4.BorderStyle = BorderStyle.FixedSingle;
            fpImage_left4.Cursor = Cursors.Hand;
            fpImage_left4.Location = new Point(142, 440);
            fpImage_left4.Margin = new Padding(4, 4, 4, 4);
            fpImage_left4.Name = "fpImage_left4";
            fpImage_left4.Size = new Size(115, 112);
            fpImage_left4.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage_left4.TabIndex = 13;
            fpImage_left4.TabStop = false;
            fpImage_left4.Paint += fpImageLeft_Paint;
            fpImage_left4.DoubleClick += fpImage_left4_DoubleClick;
            // 
            // fpImage_right4
            // 
            fpImage_right4.BorderStyle = BorderStyle.FixedSingle;
            fpImage_right4.Cursor = Cursors.Hand;
            fpImage_right4.Location = new Point(782, 440);
            fpImage_right4.Margin = new Padding(4, 4, 4, 4);
            fpImage_right4.Name = "fpImage_right4";
            fpImage_right4.Size = new Size(115, 112);
            fpImage_right4.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage_right4.TabIndex = 13;
            fpImage_right4.TabStop = false;
            fpImage_right4.Paint += fpImageRight_Paint;
            fpImage_right4.DoubleClick += fpImage_right4_DoubleClick;
            // 
            // fpImage_left_thumb
            // 
            fpImage_left_thumb.BorderStyle = BorderStyle.FixedSingle;
            fpImage_left_thumb.Cursor = Cursors.Hand;
            fpImage_left_thumb.Location = new Point(388, 440);
            fpImage_left_thumb.Margin = new Padding(4, 4, 4, 4);
            fpImage_left_thumb.Name = "fpImage_left_thumb";
            fpImage_left_thumb.Size = new Size(115, 112);
            fpImage_left_thumb.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage_left_thumb.TabIndex = 13;
            fpImage_left_thumb.TabStop = false;
            fpImage_left_thumb.Paint += fpImageThumbLeft_Paint;
            fpImage_left_thumb.DoubleClick += fpImage_left_thumb_DoubleClick;
            // 
            // fpImage_right_thumb
            // 
            fpImage_right_thumb.BorderStyle = BorderStyle.FixedSingle;
            fpImage_right_thumb.Cursor = Cursors.Hand;
            fpImage_right_thumb.Location = new Point(537, 440);
            fpImage_right_thumb.Margin = new Padding(4, 4, 4, 4);
            fpImage_right_thumb.Name = "fpImage_right_thumb";
            fpImage_right_thumb.Size = new Size(115, 112);
            fpImage_right_thumb.SizeMode = PictureBoxSizeMode.Zoom;
            fpImage_right_thumb.TabIndex = 14;
            fpImage_right_thumb.TabStop = false;
            fpImage_right_thumb.Paint += fpImageThumbRight_Paint;
            fpImage_right_thumb.DoubleClick += fpImage_right_thumb_DoubleClick;
            // 
            // panel2
            // 
            panel2.BackColor = Color.White;
            panel2.Controls.Add(fpImage_right_thumb);
            panel2.Controls.Add(fpImage_left_thumb);
            panel2.Controls.Add(fpImage_right4);
            panel2.Controls.Add(fpImage_left4);
            panel2.Controls.Add(fpImage9);
            panel2.Controls.Add(fpImage4);
            panel2.Controls.Add(fpImage8);
            panel2.Controls.Add(fpImage7);
            panel2.Controls.Add(fpImage6);
            panel2.Controls.Add(fpImage5);
            panel2.Controls.Add(fpImage3);
            panel2.Controls.Add(fpImage1);
            panel2.Controls.Add(fpImage0);
            panel2.Controls.Add(fpImage2);
            panel2.Dock = DockStyle.Fill;
            panel2.Location = new Point(0, 0);
            panel2.Margin = new Padding(4, 4, 4, 4);
            panel2.Name = "panel2";
            panel2.Size = new Size(1096, 895);
            panel2.TabIndex = 7;
            panel2.Resize += panel2_Resize;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1444, 895);
            Controls.Add(panel2);
            Controls.Add(panel1);
            Margin = new Padding(4, 4, 4, 4);
            Name = "FormMain";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "iMD FAP50 Demo C#";
            FormClosing += FormMain_FormClosing;
            Load += FormMain_Load;
            ((System.ComponentModel.ISupportInitialize)pictureBoxLogo).EndInit();
            panel1.ResumeLayout(false);
            panel3.ResumeLayout(false);
            panel3.PerformLayout();
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)fpImage2).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage0).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage1).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage3).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage5).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage6).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage7).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage8).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage4).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage9).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_left4).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_right4).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_left_thumb).EndInit();
            ((System.ComponentModel.ISupportInitialize)fpImage_right_thumb).EndInit();
            panel2.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private Button btn_start;
        private Button btn_save_file;
        private RadioButton radio_442R;
        private RadioButton radio_442F;
        private RadioButton radio_any_finger;
        private ComboBox cb_score_ver;
        private ComboBox cb_lang;
        private ComboBox cb_voice;

        private PictureBox pictureBoxLogo;
        private Label device_info;
        private System.Windows.Forms.Timer timer_reset;
        private Panel panel1;
        private Panel panel3;
        private RichTextBox textBoxStatus;
        private GroupBox groupBox1;
        private Label label1;
        private Label language;
        private Label label2;
        private RadioButton radio_signature;
        private PictureBox fpImage2;
        private PictureBox fpImage0;
        private PictureBox fpImage1;
        private PictureBox fpImage3;
        private PictureBox fpImage5;
        private PictureBox fpImage6;
        private PictureBox fpImage7;
        private PictureBox fpImage8;
        private PictureBox fpImage4;
        private PictureBox fpImage9;
        private PictureBox fpImage_left4;
        private PictureBox fpImage_right4;
        private PictureBox fpImage_left_thumb;
        private PictureBox fpImage_right_thumb;
        private Panel panel2;
    }
}