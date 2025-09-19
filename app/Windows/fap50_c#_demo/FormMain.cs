using FAP50Demo;
using OpenCvSharp.Text;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;
//using static System.Net.Mime.MediaTypeNames;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace FAP50Demo
{
    partial class FormMain : Form
    {
        string[,] score = new string[(int)GUI_SHOW_MODE.SIZE, (int)FINGER_POSITION.SIZE];
        Brush fontBrush = new SolidBrush(Color.DarkBlue);
        SampleSequence workType = SampleSequence.Flat442;
        GUI_SHOW_MODE now_mode = GUI_SHOW_MODE.NONE;
        //
        public string[] StandbyVideoPaths { get; private set; }
        private System.Windows.Forms.Timer StandbyVideoShowTimer;
        // standby video file list default path is "trunk3\x64\Release\panel\video\rossi_plus_fixed"
        private List<string> fileList = new List<string>();
        private int current_standby_Img_Index = 0;
        private System.Windows.Forms.Timer ShowDemoVideoTimer;
        private bool isInternalPannelExist = false;
        private bool isStandbyTimerWorks = false;
        public FormMain()
        {
            InitializeComponent();
            string exeDir = Path.GetDirectoryName(Application.ExecutablePath)!;
            string projectRoot = Path.GetFullPath(Path.Combine(exeDir, @"..\..\..\..\..\.."));
            //StandbyVideoPath = Path.Combine(projectRoot, @"x64\Release\panel\video\rossi_plus_fixed");
            StandbyVideoPaths = new string[]
            {
                Path.Combine(projectRoot, @"x64\Release\panel\video\rossi_preview"),
                Path.Combine(projectRoot, @"x64\Release\panel\video\all_flow_with_text"),
                Path.Combine(projectRoot, @"x64\Release\panel\video\signature"),
                Path.Combine(projectRoot, @"x64\Release\panel\video\rossi_plus_fixed")                           
            };
            InitShowDemoVideoTimer();
        }
        void FormMain_Load(object sender, EventArgs e)
        {
            Set_RNDIS_IP();
            if (!Directory.Exists(@".\DB"))
                Directory.CreateDirectory(@".\DB");

            string[] langs = { "English", "Chinese", "Spanish", "User define" };
            cb_lang.Items.AddRange(langs);
            cb_lang.SelectedIndex = 0;

            string[] volums = { "Small", "Medium", "Large", "Mute" };
            cb_voice.Items.AddRange(volums);
            cb_voice.SelectedIndex = 0;

            string[] score_vers = { "NFIQ", "NFIQ2" };
            cb_score_ver.Items.AddRange(score_vers);
            cb_score_ver.SelectedIndex = 0;

            ReSize();
            OpenDevice();
            Connect_Pannel();
            ShowDemoVideoTimer.Start();

            //DBG: sizeof(SystemProperty) = 176
            //DBG: sizeof(ImageProperty) = 64
            //DBG: sizeof(ImageStatus) = 56

            //dbg($"c# sizeof(SystemProperty)={Marshal.SizeOf(typeof(SystemProperty))}");
            //dbg($"c# sizeof(ImageProperty)={Marshal.SizeOf(typeof(ImageProperty))}");
            //dbg($"c# sizeof(ImageStatus)={Marshal.SizeOf(typeof(ImageStatus))}");
        }

        void Set_RNDIS_IP()
        {
            try
            {                
                var assembly = Assembly.GetExecutingAssembly();                               
                string resourceName = "FAP50Demo.set_rndis_ip.ps1";
                string tempScriptPath = Path.Combine(Path.GetTempPath(), "set_rndis_ip.ps1");
                using (Stream stream = assembly.GetManifestResourceStream(resourceName))
                using (StreamReader reader = new StreamReader(stream))
                {
                    string scriptContent = reader.ReadToEnd();
                    File.WriteAllText(tempScriptPath, scriptContent);
                }

                ProcessStartInfo psi = new ProcessStartInfo
                {                    
                    FileName = "powershell.exe",
                    Arguments = $"-ExecutionPolicy Bypass -File \"{tempScriptPath}\"",
                    CreateNoWindow = true,        
                    UseShellExecute = false,      
                    RedirectStandardOutput = true,
                    RedirectStandardError = true  
                };

                using (Process process = Process.Start(psi))
                {
                    if (process != null)
                    {
                        process.WaitForExit();
                        if (process.ExitCode != 0)
                        {
                            //MessageBox.Show($"Fail to run PowerShell，ExitCode={process.ExitCode}\nFor More information {Path.Combine(Path.GetTempPath(), "ps_log.txt")}");
                        }
                    }
                    else
                    {
                        //MessageBox.Show("cannot run PowerShell。");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Fail to set RNDIS IP-Address with PowerShell：" + ex.Message);
            }
        }
        void SetButtonState(Boolean state)
        {

        }
        void OpenDevice()
        {
            SystemProperty p = default;
            IMD_RESULT res = imd_fap50.get_system_property(ref p);
            if (res != IMD_RESULT.SUCCESS)
            {
                dbg("device_reset()");
                res = imd_fap50.device_reset();
                if (res != IMD_RESULT.SUCCESS)
                {
                    device_info.ForeColor = Color.Red;
                    device_info.Text = $"reset fail: {res.ToString()}";
                    return;
                }
            }

            imd_fap50.get_system_property(ref p);
            device_info.ForeColor = Color.Green;
            string productSN = Marshal.PtrToStringAnsi(p.product_sn);
            string productModel = Marshal.PtrToStringAnsi(p.product_model);
            unsafe
            {
                device_info.Text =
                    $"FW: {(char)p.fw_ver[2]}{p.fw_ver[1]:X}.{p.fw_ver[0]:X2}" +
                    $"\nChip ID: 0x{p.chip_id:X4}" +
                    $"\nSDK: {p.fap50_lib_ver[2]}.{p.fap50_lib_ver[1]}.{p.fap50_lib_ver[0]}" +
                    $"\nS/N: {productSN}" +
                    $"\nModel: {productModel}";
            }
            string part = productSN.Substring(7, 1);
            bool bInternalPannel = part.Contains("D");
            radio_signature.Visible = bInternalPannel;
            isInternalPannelExist = bInternalPannel;
        }
        void timerReset_Tick(object sender, EventArgs e)
        {

        }
        void panel2_Resize(object sender, EventArgs e)
        {
            ReSize();
        }
        void ReSize()
        {
            int cw = (panel2.ClientSize.Width) / 5;
            int ch = ((panel2.ClientSize.Height) / 5 * 3) / 2;
            if ((cw > 0 && ch > 0) == false)
                return;

            //--------------------------
            fpImage0.Left = 0;
            fpImage0.Top = 0;
            fpImage0.Width = cw;
            fpImage0.Height = ch;

            fpImage1.Left = cw;
            fpImage1.Top = 0;
            fpImage1.Width = cw;
            fpImage1.Height = ch;

            fpImage2.Left = cw * 2;
            fpImage2.Top = 0;
            fpImage2.Width = cw;
            fpImage2.Height = ch;

            fpImage3.Left = cw * 3;
            fpImage3.Top = 0;
            fpImage3.Width = cw;
            fpImage3.Height = ch;

            fpImage4.Left = cw * 4;
            fpImage4.Top = 0;
            fpImage4.Width = cw;
            fpImage4.Height = ch;

            //--------------------------
            fpImage5.Left = 0;
            fpImage5.Top = ch - 1;
            fpImage5.Width = cw;
            fpImage5.Height = ch;

            fpImage6.Left = cw;
            fpImage6.Top = ch - 1;
            fpImage6.Width = cw;
            fpImage6.Height = ch;

            fpImage7.Left = cw * 2;
            fpImage7.Top = ch - 1;
            fpImage7.Width = cw;
            fpImage7.Height = ch;

            fpImage8.Left = cw * 3;
            fpImage8.Top = ch - 1;
            fpImage8.Width = cw;
            fpImage8.Height = ch;

            fpImage9.Left = cw * 4;
            fpImage9.Top = ch - 1;
            fpImage9.Width = cw;
            fpImage9.Height = ch;

            //--------------------------
            int ct = panel2.ClientSize.Height - ch * 2;
            cw = (panel2.ClientSize.Width) / 10;
            fpImage_left4.Left = 0;
            fpImage_left4.Top = ch * 2 - 2;
            fpImage_left4.Width = fpImage2.Left;
            fpImage_left4.Height = ct;

            fpImage_left_thumb.Left = fpImage2.Left;
            fpImage_left_thumb.Top = ch * 2 - 2;
            fpImage_left_thumb.Width = cw * 1;
            fpImage_left_thumb.Height = ct;

            fpImage_right_thumb.Left = fpImage3.Left - cw;
            fpImage_right_thumb.Top = ch * 2 - 2;
            fpImage_right_thumb.Width = cw;
            fpImage_right_thumb.Height = ct;

            fpImage_right4.Left = fpImage3.Left;
            fpImage_right4.Top = ch * 2 - 2;
            fpImage_right4.Width = cw * 4 + 1;
            fpImage_right4.Height = ct;
        }

        void fpImage0_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("1.L. THUMB", this.Font, fontBrush, new PointF(10, fpImage0.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.LEFT_THUMB], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage1_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("2.L. INDEX", this.Font, fontBrush, new PointF(10, fpImage1.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.LEFT_INDEX], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage2_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("3.L. MIDDLE", this.Font, fontBrush, new PointF(10, fpImage2.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.LEFT_MIDDLE], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage3_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("4.L. RING", this.Font, fontBrush, new PointF(10, fpImage3.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.LEFT_RING], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage4_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("5.L. LITTLE", this.Font, fontBrush, new PointF(10, fpImage4.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.LEFT_LITTLE], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage5_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("6.R. THUMB", this.Font, fontBrush, new PointF(10, fpImage5.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.RIGHT_THUMB], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage6_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("7.R. INDEX", this.Font, fontBrush, new PointF(10, fpImage6.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.RIGHT_INDEX], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage7_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("8.R. MIDDLE", this.Font, fontBrush, new PointF(10, fpImage7.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.RIGHT_MIDDLE], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage8_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("9.R. RING", this.Font, fontBrush, new PointF(10, fpImage8.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.RIGHT_RING], Font, fontBrush, new PointF(10, 10));
        }
        void fpImage9_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("10.R. LITTLE", this.Font, fontBrush, new PointF(10, fpImage9.Height - 20));
            e.Graphics.DrawString(score[(int)now_mode, (int)FINGER_POSITION.RIGHT_LITTLE], Font, fontBrush, new PointF(10, 10));
        }
        void fpImageLeft_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("LEFT FOUR FINGERS", this.Font, fontBrush, new PointF(10, fpImage_left4.Height - 20));
            e.Graphics.DrawString(score[(int)GUI_SHOW_MODE.FLAT, (int)FINGER_POSITION.LEFT_FOUR], Font, fontBrush, new PointF(10, 10));
        }
        void fpImageRight_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("RIGHT FOUR FINGERS", this.Font, fontBrush, new PointF(10, fpImage_right4.Height - 20));
            e.Graphics.DrawString(score[(int)GUI_SHOW_MODE.FLAT, (int)FINGER_POSITION.RIGHT_FOUR], Font, fontBrush, new PointF(10, 10));
        }
        void fpImageThumbLeft_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("L. THUMB", this.Font, fontBrush, new PointF(10, fpImage_left_thumb.Height - 20));
            e.Graphics.DrawString(score[(int)GUI_SHOW_MODE.FLAT, (int)FINGER_POSITION.LEFT_THUMB], Font, fontBrush, new PointF(10, 10));
        }
        void fpImageThumbRight_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawString("R. THUMB", this.Font, fontBrush, new PointF(10, fpImage_right_thumb.Height - 20));
            e.Graphics.DrawString(score[(int)GUI_SHOW_MODE.FLAT, (int)FINGER_POSITION.RIGHT_THUMB], Font, fontBrush, new PointF(10, 10));
        }

        void radio_442F_CheckedChanged(object sender, EventArgs e)
        {
            if (radio_442F.Checked)
            {
                fpImage0.Cursor = Cursors.Default;
                fpImage1.Cursor = Cursors.Default;
                fpImage2.Cursor = Cursors.Default;
                fpImage3.Cursor = Cursors.Default;
                fpImage4.Cursor = Cursors.Default;
                fpImage5.Cursor = Cursors.Default;
                fpImage6.Cursor = Cursors.Default;
                fpImage7.Cursor = Cursors.Default;
                fpImage8.Cursor = Cursors.Default;
                fpImage9.Cursor = Cursors.Default;
            }
        }
        void radio_442R_CheckedChanged(object sender, EventArgs e)
        {
            if (radio_442R.Checked)
            {
                fpImage0.Cursor = Cursors.Hand;
                fpImage1.Cursor = Cursors.Hand;
                fpImage2.Cursor = Cursors.Hand;
                fpImage3.Cursor = Cursors.Hand;
                fpImage4.Cursor = Cursors.Hand;
                fpImage5.Cursor = Cursors.Hand;
                fpImage6.Cursor = Cursors.Hand;
                fpImage7.Cursor = Cursors.Hand;
                fpImage8.Cursor = Cursors.Hand;
                fpImage9.Cursor = Cursors.Hand;
            }
        }
        void radio_any_finger_CheckedChanged(object sender, EventArgs e)
        {
            if (radio_any_finger.Checked)
            {
                fpImage0.Cursor = Cursors.Default;
                fpImage1.Cursor = Cursors.Default;
                fpImage2.Cursor = Cursors.Default;
                fpImage3.Cursor = Cursors.Default;
                fpImage4.Cursor = Cursors.Default;
                fpImage5.Cursor = Cursors.Default;
                fpImage6.Cursor = Cursors.Default;
                fpImage7.Cursor = Cursors.Default;
                fpImage8.Cursor = Cursors.Default;
                fpImage9.Cursor = Cursors.Default;
            }
        }
        void fpImage_two_thumbs_DoubleClick(object sender, EventArgs e)
        {
            //FormFlat formFlat = new FormFlat();
            //formFlat.SetSplitType(0);
            //formFlat.SetCaptureCount(2);
            //if (formFlat.ShowDialog() == DialogResult.OK)
            //{
            //    if (capMode != 2)
            //    {
            //        MemoryStream ms1 = new MemoryStream(fpdevice.flat1bmp);
            //        fpImage0.Image = System.Drawing.Image.FromStream(ms1);
            //        ms1.Close();

            //        MemoryStream ms2 = new MemoryStream(fpdevice.flat2bmp);
            //        fpImage5.Image = System.Drawing.Image.FromStream(ms2);
            //        ms2.Close();

            //        Array.Copy(fpdevice.flat1data, spt0data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat2data, spt5data, fpdevice.FAP30_RAW_SIZE);
            //    }
            //    MemoryStream ms3 = new MemoryStream(fpdevice.flat1bmp);
            //    fpImageThumbLeft.Image = System.Drawing.Image.FromStream(ms3);
            //    ms3.Close();

            //    MemoryStream ms4 = new MemoryStream(fpdevice.flat2bmp);
            //    fpImageThumbRight.Image = System.Drawing.Image.FromStream(ms4);
            //    ms4.Close();

            //    Array.Copy(fpdevice.rawdata, flat2data, fpdevice.RESDATA_SIZE);
            //}
        }
        void fpImage_left4_DoubleClick(object sender, EventArgs e)
        {
            //FormFlat formFlat = new FormFlat();
            //formFlat.SetSplitType(0);
            //formFlat.SetCaptureCount(4);
            //if (formFlat.ShowDialog() == DialogResult.OK)
            //{
            //    if (capMode != 2)
            //    {
            //        MemoryStream ms1 = new MemoryStream(fpdevice.flat1bmp);
            //        fpImage1.Image = System.Drawing.Image.FromStream(ms1);
            //        ms1.Close();

            //        MemoryStream ms2 = new MemoryStream(fpdevice.flat2bmp);
            //        fpImage2.Image = System.Drawing.Image.FromStream(ms2);
            //        ms2.Close();

            //        MemoryStream ms3 = new MemoryStream(fpdevice.flat3bmp);
            //        fpImage3.Image = System.Drawing.Image.FromStream(ms3);
            //        ms3.Close();

            //        MemoryStream ms4 = new MemoryStream(fpdevice.flat4bmp);
            //        fpImage4.Image = System.Drawing.Image.FromStream(ms4);
            //        ms4.Close();

            //        Array.Copy(fpdevice.flat1data, spt1data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat2data, spt2data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat3data, spt3data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat4data, spt4data, fpdevice.FAP30_RAW_SIZE);
            //    }
            //    MemoryStream ms5 = new MemoryStream(fpdevice.bmpdata);
            //    fpImageLeft.Image = System.Drawing.Image.FromStream(ms5);
            //    ms5.Close();

            //    Array.Copy(fpdevice.rawdata, flat0data, fpdevice.RESDATA_SIZE);
            //}
        }
        void fpImage_right4_DoubleClick(object sender, EventArgs e)
        {
            //FormFlat formFlat = new FormFlat();
            //formFlat.SetSplitType(0);
            //formFlat.SetCaptureCount(4);
            //if (formFlat.ShowDialog() == DialogResult.OK)
            //{
            //    if (capMode != 2)
            //    {
            //        MemoryStream ms1 = new MemoryStream(fpdevice.flat1bmp);
            //        fpImage6.Image = System.Drawing.Image.FromStream(ms1);
            //        ms1.Close();

            //        MemoryStream ms2 = new MemoryStream(fpdevice.flat2bmp);
            //        fpImage7.Image = System.Drawing.Image.FromStream(ms2);
            //        ms2.Close();

            //        MemoryStream ms3 = new MemoryStream(fpdevice.flat3bmp);
            //        fpImage8.Image = System.Drawing.Image.FromStream(ms3);
            //        ms3.Close();

            //        MemoryStream ms4 = new MemoryStream(fpdevice.flat4bmp);
            //        fpImage9.Image = System.Drawing.Image.FromStream(ms4);
            //        ms4.Close();

            //        Array.Copy(fpdevice.flat1data, spt6data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat2data, spt7data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat3data, spt8data, fpdevice.FAP30_RAW_SIZE);
            //        Array.Copy(fpdevice.flat4data, spt9data, fpdevice.FAP30_RAW_SIZE);
            //    }
            //    MemoryStream ms5 = new MemoryStream(fpdevice.bmpdata);
            //    fpImageRight.Image = System.Drawing.Image.FromStream(ms5);
            //    ms5.Close();

            //    Array.Copy(fpdevice.rawdata, flat1data, fpdevice.RESDATA_SIZE);
            //}
        }
        void fpImage_right_thumb_DoubleClick(object sender, EventArgs e)
        {
            fpImage_two_thumbs_DoubleClick(sender, e);
        }
        void fpImage_left_thumb_DoubleClick(object sender, EventArgs e)
        {
            fpImage_two_thumbs_DoubleClick(sender, e);
        }
        void fpImage_DoubleClick(object sender, EventArgs e)
        {
            //if (capMode == 2)
            //{
            //    FormRollImage formRollImage = new FormRollImage();
            //    if (formRollImage.ShowDialog() == DialogResult.OK)
            //    {
            //        MemoryStream ms1 = new MemoryStream(fpdevice.rollbmp);
            //        if (sender == fpImage0)
            //        {
            //            fpImage0.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol01img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage1)
            //        {
            //            fpImage1.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol11img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage2)
            //        {
            //            fpImage2.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol21img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage3)
            //        {
            //            fpImage3.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol31img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage4)
            //        {
            //            fpImage4.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol41img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage5)
            //        {
            //            fpImage5.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol51img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage6)
            //        {
            //            fpImage6.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol61img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage7)
            //        {
            //            fpImage7.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol71img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage8)
            //        {
            //            fpImage8.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol81img, fpdevice.RAWDATA_SIZE);
            //        }
            //        else if (sender == fpImage9)
            //        {
            //            fpImage9.Image = System.Drawing.Image.FromStream(ms1);
            //            Array.Copy(fpdevice.rollimg, rol91img, fpdevice.RAWDATA_SIZE);
            //        }
            //        ms1.Close();
            //    }
            //}
        }

        void btn_start_Click(object sender, EventArgs e)
        {
            
            StopShowDemoVideoTimer();
            FormAutoCapture formAutoCapture = new FormAutoCapture();
            if (radio_442F.Checked)
            {
                workType = SampleSequence.Flat442;
                now_mode = GUI_SHOW_MODE.FLAT;
            }
            else if (radio_442R.Checked)
            {
                workType = SampleSequence.Flat442R;
                now_mode = GUI_SHOW_MODE.ROLL;
            }
            else if (radio_signature.Checked)
            {
                workType = SampleSequence.Signature;
                now_mode = GUI_SHOW_MODE.SIGNATURE;
            }

            IMD_RESULT res = imd_fap50.device_reset();
            if (res != IMD_RESULT.SUCCESS)
            {
                device_info.Text = res.ToString();
                return;
            }
            Connect_Pannel();

            formAutoCapture.SetWorkType(workType);
            formAutoCapture.ShowDialog();
            imd_fap50.set_led_speech_standby_mode();

            List<ShowImg> show_imgs = new List<ShowImg>();
            ShowDemoVideoTimer.Start();
            //roll 10 fingers
            if (workType == SampleSequence.Flat442R)
            {
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.LEFT_THUMB, fpImage0));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.LEFT_INDEX, fpImage1));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.LEFT_MIDDLE, fpImage2));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.LEFT_RING, fpImage3));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.LEFT_LITTLE, fpImage4));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_THUMB, fpImage5));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_INDEX, fpImage6));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_MIDDLE, fpImage7));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_RING, fpImage8));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_LITTLE, fpImage9));
            }
            //flat 10 fingers
            else if (workType == SampleSequence.Flat442)
            {
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_THUMB, fpImage0));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_INDEX, fpImage1));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_MIDDLE, fpImage2));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_RING, fpImage3));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_LITTLE, fpImage4));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_THUMB, fpImage5));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_INDEX, fpImage6));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_MIDDLE, fpImage7));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_RING, fpImage8));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_LITTLE, fpImage9));
            }

            // flat left_4, left_thumb, right_thumb, right_4
            if (workType == SampleSequence.Flat442 || workType == SampleSequence.Flat442R)
            {
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_FOUR, fpImage_left4));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_FOUR, fpImage_right4));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_THUMB, fpImage_left_thumb));
                show_imgs.Add(new ShowImg(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_THUMB, fpImage_right_thumb));
            }

            ImageProperty img_pty = default;
            foreach (var m in show_imgs)
            {
                img_pty.mode = GUI_SHOW_MODE.FLAT;
                img_pty.pos = m.pos;
                imd_fap50.get_image(ref img_pty);

                //set NFIQ score
                unsafe
                {
                    string str_score = "";
                    switch (img_pty.score_size)
                    {
                        case 1:
                            str_score = $"{img_pty.score_array[0]}";
                            break;
                        case 2:
                            str_score = $"{img_pty.score_array[0]}-{img_pty.score_array[1]}";
                            break;
                        case 4:
                            if (m.pos == FINGER_POSITION.LEFT_FOUR)
                                str_score = $"{img_pty.score_array[3]}-{img_pty.score_array[2]}-{img_pty.score_array[1]}-{img_pty.score_array[0]}";
                            else
                                str_score = $"{img_pty.score_array[0]}-{img_pty.score_array[1]}-{img_pty.score_array[2]}-{img_pty.score_array[3]}";
                            break;
                        default:
                            break;
                    }
                    score[(int)img_pty.mode, (int)img_pty.pos] = str_score;
                }

                if (img_pty.width != 0)
                    m.pic_box.Image = FormAutoCapture.U8PtrToBitmap32(img_pty.img, img_pty.width, img_pty.height);

                //10 images replace by roll image.
                if (workType == SampleSequence.Flat442R)
                {
                    img_pty.mode = GUI_SHOW_MODE.ROLL;
                    img_pty.pos = m.pos;
                    imd_fap50.get_image(ref img_pty);
                    unsafe
                    {
                        score[(int)img_pty.mode, (int)img_pty.pos] = $"{img_pty.score_array[0]}";
                    }

                    if (img_pty.width == 0 || m.pic_box == fpImage_left_thumb || m.pic_box == fpImage_right_thumb)
                        continue;

                    m.pic_box.Image = FormAutoCapture.U8PtrToBitmap32(img_pty.img, img_pty.width, img_pty.height);
                }
            }

            //show signature image.
            if (workType == SampleSequence.Signature)
            {
                img_pty.mode = GUI_SHOW_MODE.FLAT;
                img_pty.pos = FINGER_POSITION.SIGNATURE;
                imd_fap50.get_image(ref img_pty);
                fpImage_left4.Image = FormAutoCapture.U8PtrToBitmap32(img_pty.img, img_pty.width, img_pty.height);
            }

        }
        /*
        private void btn_save_file_Click(object sender, EventArgs e)
        {
            //save 10 roll fingers images
            if (workType == SampleSequence.Flat442R)
            {
                for (int i = 0; i < 10; i++)
                    imd_fap50.save_file(GUI_SHOW_MODE.ROLL, FINGER_POSITION.RIGHT_THUMB + i, "db/test.png");
            }

            //save 10 flat fingers images
            for (int i = 0; i < 10; i++)
                imd_fap50.save_file(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_THUMB + i, "db/test.png");

            //save flat left_4, two_thumbs, right_4
            imd_fap50.save_file(GUI_SHOW_MODE.FLAT, FINGER_POSITION.RIGHT_FOUR, "db/test.png");
            imd_fap50.save_file(GUI_SHOW_MODE.FLAT, FINGER_POSITION.LEFT_FOUR, "db/test.png");
            imd_fap50.save_file(GUI_SHOW_MODE.FLAT, FINGER_POSITION.BOTH_THUMBS, "db/test.png");
        }*/
        
        private void btn_save_file_Click(object sender, EventArgs e)
        {
            string exePath = Application.StartupPath;
            string dbPath = Path.Combine(exePath, "db");

            if (!Directory.Exists(dbPath))
                Directory.CreateDirectory(dbPath);

            using (SaveFileDialog sfd = new SaveFileDialog())
            {
                sfd.Title = "Save fingerprint images";
                sfd.Filter = "PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|WSQ (*.wsq)|*.wsq|Raw (*.raw)|*.raw|JPEG2000 (*.jp2)|*.jp2|ISO 19794-4 (*.fir)|*.fir";
                sfd.FileName = DateTime.Now.ToString("yyyyMMdd_HHmmss"); 
                sfd.InitialDirectory = dbPath;

                if (sfd.ShowDialog() != DialogResult.OK)
                    return;

                string filePath = sfd.FileName;
                string extension = Path.GetExtension(filePath).ToLower().TrimStart('.'); // "png", "bmp", etc.
                string basePath = Path.GetDirectoryName(filePath);
                string timestamp = DateTime.Now.ToString("yyyyMMdd_HHmmss");

                var labels = new PictureBox[] {
                fpImage5, // RIGHT_THUMB
                fpImage6, // RIGHT_INDEX
                fpImage7, // RIGHT_MIDDLE
                fpImage8, // RIGHT_RING
                fpImage9, // RIGHT_LITTLE
                fpImage0, // LEFT_THUMB
                fpImage1, // LEFT_INDEX
                fpImage2, // LEFT_MIDDLE
                fpImage3, // LEFT_RING
                fpImage4, // LEFT_LITTLE
                fpImage_right4,   // RIGHT_FOUR
                fpImage_left4     // LEFT_FOUR
                };

                var fingerPositions = new FINGER_POSITION[] {
                    FINGER_POSITION.RIGHT_THUMB,
                    FINGER_POSITION.RIGHT_INDEX,
                    FINGER_POSITION.RIGHT_MIDDLE,
                    FINGER_POSITION.RIGHT_RING,
                    FINGER_POSITION.RIGHT_LITTLE,
                    FINGER_POSITION.LEFT_THUMB,
                    FINGER_POSITION.LEFT_INDEX,
                    FINGER_POSITION.LEFT_MIDDLE,
                    FINGER_POSITION.LEFT_RING,
                    FINGER_POSITION.LEFT_LITTLE,
                    FINGER_POSITION.RIGHT_FOUR,
                    FINGER_POSITION.LEFT_FOUR
                };

                string[] fingerNames = {
                    "right_thumb",
                    "right_index",
                    "right_middle",
                    "right_ring",
                    "right_little",
                    "left_thumb",
                    "left_index",
                    "left_middle",
                    "left_ring",
                    "left_little",
                    "right_four",
                    "left_four"
                };

                int saved = 0;
                for (int i = 0; i < labels.Length; i++)
                {
                    if (labels[i] == null || labels[i].Image == null)
                        continue;

                    string savePath = Path.Combine(basePath,
                        $"{timestamp}_{fingerNames[i]}.{extension}");

                    IMD_RESULT res = 0;
                    if (workType == SampleSequence.Flat442) 
                        res = imd_fap50.save_file(GUI_SHOW_MODE.FLAT, fingerPositions[i], savePath);
                    else if(workType == SampleSequence.Flat442R)
                        res = imd_fap50.save_file(GUI_SHOW_MODE.ROLL, fingerPositions[i], savePath);


                    if (res == 0)
                    {
                        saved++;
                        Console.WriteLine($"Successfully saved: {savePath}");
                    }
                    else
                    {
                        Console.WriteLine($"Failed to save {savePath}, error {res}");
                    }
                }

                if (labels[0] != null && labels[5] != null)
                {
                    string savePath = Path.Combine(basePath,
                            $"{timestamp}_both_thumbs.{extension}");

                    IMD_RESULT res = 0;
                    if (workType == SampleSequence.Flat442) 
                        res = imd_fap50.save_file(GUI_SHOW_MODE.FLAT, FINGER_POSITION.BOTH_THUMBS, savePath);
                    else if (workType == SampleSequence.Flat442R)
                        res = imd_fap50.save_file(GUI_SHOW_MODE.ROLL, FINGER_POSITION.BOTH_THUMBS, savePath);

                    if (res == 0)
                    {
                        saved++;
                        Console.WriteLine($"Successfully saved: {savePath}");
                    }
                    else
                    {
                        Console.WriteLine($"Failed to save {savePath}, error {res}");
                    }
                }

                MessageBox.Show($"Saved {saved} fingerprint(s) as {extension}.",
                                "Save done",
                                MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }
        
        private void cb_lang_SelectedIndexChanged(object sender, EventArgs e)
        {
            SystemProperty p = default;
            IMD_RESULT res = imd_fap50.get_system_property(ref p);
            p.speech_language = cb_lang.SelectedIndex;
            imd_fap50.set_system_property(ref p);
        }
        private void cb_voice_volume_SelectedIndexChanged(object sender, EventArgs e)
        {
            SystemProperty p = default;
            IMD_RESULT res = imd_fap50.get_system_property(ref p);
            p.speech_volume = cb_voice.SelectedIndex;
            imd_fap50.set_system_property(ref p);
        }
        private void cb_score_ver_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            SystemProperty p = default;
            IMD_RESULT res = imd_fap50.get_system_property(ref p);
            p.nfiq_ver = (NFIQ_VERSION)cb_score_ver.SelectedIndex;
            imd_fap50.set_system_property(ref p);
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopStandbyVideo();
            Disconnect_Pannel();
        }
        void dbg(string msg)
        {
#if DEBUG
            Debug.WriteLine(msg);  // 在調試模式下輸出
#else
            Trace.WriteLine(msg);  // 在發佈模式下也輸出
#endif
        }

        //Initialize ShowDemoVideoTimer
        private void InitShowDemoVideoTimer()
        {
            ShowDemoVideoTimer = new System.Windows.Forms.Timer();
            ShowDemoVideoTimer.Interval = 2000;
            ShowDemoVideoTimer.Tick += (s, e) => ShowDemoVideoTimer_Tick();
        } 

        private void ShowDemoVideoTimer_Tick()
        {
            if(!isStandbyTimerWorks)
            {
                
                StopStandbyVideo();
                StartStandbyVideo();
            }            
        }

        private void StopShowDemoVideoTimer()
        {
            if (ShowDemoVideoTimer != null && ShowDemoVideoTimer.Enabled)
            {
                ShowDemoVideoTimer.Stop();
                dbg("Show demo video timer stopped.");
            }

            StopStandbyVideo();
        }

        //Implement Show standby video feature
        private void InitStandbyVideoPath()
        {
            try
            {
                fileList.Clear();
                foreach (var path in StandbyVideoPaths)
                {
                    if (!Directory.Exists(path))
                    {
                        dbg($"Directory not found: {path}");
                        continue;
                    }

                    string listPath = Path.Combine(path, "list.txt");
                    if (!File.Exists(listPath))
                    {
                        MessageBox.Show($"list.txt not exist in directory: {path}！");
                        continue;
                    }

                    var lines = File.ReadAllLines(listPath);
                    foreach (var line in lines)
                    {
                        string trimmed = line.Trim();
                        if (trimmed == "EOF") break;

                        string imagePath = Path.Combine(path, trimmed);
                        if (File.Exists(imagePath))
                        {
                            fileList.Add(imagePath);
                        }
                    }
                }

                current_standby_Img_Index = 0;
                if (fileList.Count == 0)
                {
                    MessageBox.Show("No standby video files found！");
                }
            }
            catch (Exception ex)
            {
                dbg($"Error initializing standby video path: {ex.Message}");
            }
        }
        private void InitShowStandbyVideoTimer()
        {
            StandbyVideoShowTimer = new System.Windows.Forms.Timer();
            StandbyVideoShowTimer.Interval = 50; // Set the interval to 1 second
            StandbyVideoShowTimer.Tick += (s, e) => StandbyVideoShowT_Tick();
        }
        private void StandbyVideoShowT_Tick()
        {
            if(fileList.Count == 0)
            {
                dbg("No standby video files available.");
                return;
            }

            string imagePath = fileList[current_standby_Img_Index];
            if (File.Exists(imagePath))
            {
                try
                {
                    byte[] imgBytes = File.ReadAllBytes(imagePath);
                    bool sent = IMD_FAP50_SDK_PANEL.send_jpg_fap50_panel(imgBytes, (uint)imgBytes.Length);
                    if (!sent)
                        dbg($"Fail to send jpg to Panel: {imagePath}");
                }
                catch (Exception ex)
                {
                    dbg($"Fail to read/send JPG: {ex.Message}");
                }
            }

            current_standby_Img_Index++;
            //dbg($"Send jpg to Panel: {imagePath}");
            if (current_standby_Img_Index >= fileList.Count)
                current_standby_Img_Index = 0;
        }
        
        private void Connect_Pannel()
        {
            if (!isInternalPannelExist)
                return;

            bool fconnectResult = false;
            if (!IMD_FAP50_SDK_PANEL.IsConnected)
            {
                fconnectResult =IMD_FAP50_SDK_PANEL.Connect("192.168.100.10", 1812);
                if(!fconnectResult)
                    dbg("Cannot connect to Panel via 192.168.100.10");
            }
            else
            {
                dbg("The Pannel is connected!!");
                return;
            }
        }
        private void Disconnect_Pannel()
        {
            if (!isInternalPannelExist)
                return;
            if (IMD_FAP50_SDK_PANEL.IsConnected)
            {
                IMD_FAP50_SDK_PANEL.Disconnect();              
                dbg("Disconnected from FAP50 Panel.");
            }
            dbg($"Pannel is disconnected = {IMD_FAP50_SDK_PANEL.IsConnected}");
        }
        private void StartStandbyVideo()
        {
            InitStandbyVideoPath();
            InitShowStandbyVideoTimer();
            Connect_Pannel();
            StandbyVideoShowTimer.Start();
            dbg("Standby video timer started.");
            isStandbyTimerWorks = true;
        }
        private void StopStandbyVideo()
        {
            if (StandbyVideoShowTimer != null && StandbyVideoShowTimer.Enabled)
            {
                StandbyVideoShowTimer.Stop();
                dbg("Standby video timer stopped.");
                isStandbyTimerWorks = false;
            }            
        }
    }        
}
public static class IMD_FAP50_SDK_PANEL
{
    private static bool isConnected = false;
    [DllImport("lib_imd_fap50_method.dll", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool connect_fap50_panel(string host, ushort port);

    [DllImport("lib_imd_fap50_method.dll", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool send_jpg_fap50_panel(byte[] buffer, uint size);

    [DllImport("lib_imd_fap50_method.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void disconnect_fap50_panel();
    /*
    public static bool Connect(string host, ushort port)
    {
        if (!isConnected)
        {
            isConnected = connect_fap50_panel(host, port);
        }
        return isConnected;
    }*/

    public static bool Connect(string host, ushort port)
    {
        if (!isConnected)
        {
            var sw = Stopwatch.StartNew();

            isConnected = connect_fap50_panel(host, port);

            sw.Stop();
            Trace.WriteLine($"[TIMING] connect_fap50_panel took {sw.ElapsedMilliseconds} ms");
        }
        return isConnected;
    }

    public static void Disconnect()
    {
        if (isConnected)
        {
            disconnect_fap50_panel();
            isConnected = false;
        }
    }
    public static bool IsConnected => isConnected;
}
struct ShowImg
{
    public GUI_SHOW_MODE mode;
    public FINGER_POSITION pos;
    public PictureBox pic_box;
    public ShowImg(GUI_SHOW_MODE mode, FINGER_POSITION pos, PictureBox pic_box)
    {
        this.mode = mode;
        this.pos = pos;
        this.pic_box = pic_box;
    }
};
enum SampleSequence
{
    Flat442,
    Flat442R,
    FlatSomefinters,
    Signature,
}
