using FAP50Demo;
using Microsoft.VisualBasic;
using OpenCvSharp;
using OpenCvSharp.Extensions;
using OpenCvSharp.Internal.Vectors;
using OpenCvSharp.Text;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Transactions;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;
using static OpenCvSharp.Stitcher;
using static System.Formats.Asn1.AsnWriter;

enum E_FINGER_POSITION
{
    FINTER_POSITION_UNKNOW_FINGER = 0,

    FINGER_POSITION_RIGHT_THUMB,
    FINGER_POSITION_RIGHT_INDEX,
    FINGER_POSITION_RIGHT_MIDDLE,
    FINGER_POSITION_RIGHT_RING,
    FINGER_POSITION_RIGHT_LITTLE,

    FINGER_POSITION_LEFT_THUMB,
    FINGER_POSITION_LEFT_INDEX,
    FINGER_POSITION_LEFT_MIDDLE,
    FINGER_POSITION_LEFT_RING,
    FINGER_POSITION_LEFT_LITTLE,

    FINGER_POSITION_LEFT_FOUR = 13,
    FINGER_POSITION_RIGHT_FOUR,    
    FINGER_POSITION_BOTH_THUMBS,
    FINGER_POSITION_SOME_FINGERS,
    FINGER_POSITION_SIGNATURE,
    FINGER_POSITION_SIZE,
};
public struct Score2Num
{
    public byte num;

    public bool R1
    {
        get => (num & (1 << 3)) != 0;
        set => num = value ? (byte)(num | (1 << 3)) : (byte)(num & ~(1 << 3));
    }

    public bool R2
    {
        get => (num & (1 << 2)) != 0;
        set => num = value ? (byte)(num | (1 << 2)) : (byte)(num & ~(1 << 2));
    }

    public bool R3
    {
        get => (num & (1 << 1)) != 0;
        set => num = value ? (byte)(num | (1 << 1)) : (byte)(num & ~(1 << 1));
    }

    public bool R4
    {
        get => (num & (1 << 0)) != 0;
        set => num = value ? (byte)(num | (1 << 0)) : (byte)(num & ~(1 << 0));
    }

    public bool L1
    {
        get => R4;
        set => R4 = value;
    }

    public bool L2
    {
        get => R3;
        set => R3 = value;
    }

    public bool L3
    {
        get => R2;
        set => R2 = value;
    }

    public bool L4
    {
        get => R1;
        set => R1 = value;
    }

    public bool R0
    {
        get => (num & (1 << 0)) != 0;
        set => num = value ? (byte)(num | (1 << 0)) : (byte)(num & ~(1 << 0));
    }

    public bool L0
    {
        get => (num & (1 << 1)) != 0;
        set => num = value ? (byte)(num | (1 << 1)) : (byte)(num & ~(1 << 1));
    }

    public override string ToString()
    {
        return $"num=0x{num:X2} [R4:{R4}, R3:{R3}, R2:{R2}, R1:{R1}]";
    }
}
enum Video_ShowPhase
{
    GuideFrame,
    Scanning,
    Validate
}
namespace FAP50Demo
{
    partial class FormAutoCapture : Form
    {
        SampleSequence workType = 0;
        int workIndex = 0, IMG_WIDTH = 1600, IMG_HEIGHT = 1000;       
        
        //Async method 
        private CancellationTokenSource _ctsScan;
        private CancellationTokenSource _ctsBlink;
        private CancellationTokenSource _ctsSign;
        private bool _isScanRunning = false;
        private int sign_show_cnt = 0;
        private bool _isScanning;
        private bool btn_blink = false;
        private Bitmap _latestFrame;
        private bool _uiUpdating = false;

        //for InitFingerPanelImages
        private Dictionary<FINGER_POSITION, List<string>> guideFramePaths = new();
        private Dictionary<E_FINGER_POSITION, Mat> scanning_img = new Dictionary<E_FINGER_POSITION, Mat>();
        private Dictionary<int, Mat> loading_img = new Dictionary<int, Mat>();
        private string baseDir, videoBasePath, ValidImgFPath;
        private FINGER_POSITION currentFinger = FINGER_POSITION.LEFT_FOUR;
        private int currentFrameIndex = 0;
        private int currentGuideIndex = 0;
        private List<FINGER_POSITION> scanSequence = new();
        private FINGER_POSITION currentWorkPosition = FINGER_POSITION.UNKNOW_FINGER;
        private int scanningFrameIndex = 0;
        private E_FINGER_POSITION curretScanningPOS = E_FINGER_POSITION.FINGER_POSITION_LEFT_FOUR;
        private FINGER_POSITION CheckPOS = FINGER_POSITION.LEFT_FOUR;
        private bool isFingerValided = false;
        private bool isFingerON_ScanDone = false;
        private System.Windows.Forms.Timer guideFrameTimer;
        //
        private System.Windows.Forms.Timer VideoShowTimer;
        //private Video_ShowPhase VideoShowPhase = Video_ShowPhase.GuideFrame;
        public FormAutoCapture()
        {
            InitializeComponent();
            InitializeFormParams();         
        }
        
        ~FormAutoCapture()
        {
            
        }

        public void SetWorkType(SampleSequence type)
        {
            workType = type;
            InitFingerScanningImages();
            InitVideoFrames();
        }
        public static Bitmap U8PtrToBitmap32(IntPtr img, int width, int height)
        {
            // 創建 Mat 指向非託管數據的灰階圖
            //Mat mat = new Mat(height, width, MatType.CV_8UC1, img);//old method
            Mat mat = Mat.FromPixelData(height, width, MatType.CV_8UC1, img);
            //Cv2.ImShow("show image", mat);//dbg

            // 如果需要轉換到 32bpp 格式
            Mat mat32bpp = new Mat();
            Cv2.CvtColor(mat, mat32bpp, ColorConversionCodes.GRAY2BGRA); // 8bpp to 32bpp

            // 將 OpenCV 的 Mat 轉換為 C# 的 Bitmap
            Bitmap bitmap = BitmapConverter.ToBitmap(mat32bpp);
            return bitmap;
        }

        void dbg(string msg)
        {
#if DEBUG
            Debug.WriteLine(msg);  // 在調試模式下輸出
#else
            Trace.WriteLine(msg);  // 在發佈模式下也輸出
#endif
        }
        private async void FormAutoCapture_Load(object sender, EventArgs e)
        {
            ReSize();
            //InitVideoShowTimer();
            _ctsScan = new CancellationTokenSource();
            _ctsBlink = new CancellationTokenSource();

            FINGER_POSITION[] pos = { FINGER_POSITION.UNKNOW_FINGER };
            switch (workType)
            {
                case SampleSequence.Flat442:
                    buttonRoll1.Visible = false;
                    buttonRoll2.Visible = false;
                    buttonRoll3.Visible = false;
                    buttonRoll4.Visible = false;
                    buttonRoll5.Visible = false;
                    buttonRoll6.Visible = false;
                    buttonRoll7.Visible = false;
                    buttonRoll8.Visible = false;
                    buttonRoll9.Visible = false;
                    buttonRoll10.Visible = false;
                    workIndex = -1;
                    //timer_flat.Start();
                    //timer_btn_blink.Start();
                    pos[0] = FINGER_POSITION.LEFT_FOUR;
                    StartBlinking();
                    StartScan();
                    imd_fap50.scan_start(GUI_SHOW_MODE.FLAT, ref pos[0], pos.Length);
                    break;

                case SampleSequence.Flat442R:
                    workIndex = -1;
                    //timer_flat.Start();
                    //timer_btn_blink.Start();
                    pos[0] = FINGER_POSITION.LEFT_FOUR;
                    StartBlinking();
                    StartScan();
                    imd_fap50.scan_start(GUI_SHOW_MODE.FLAT, ref pos[0], pos.Length);
                    break;

                case SampleSequence.FlatSomefinters:
                    break;
                case SampleSequence.Signature:
                    buttonRoll1.Visible = false;
                    buttonRoll2.Visible = false;
                    buttonRoll3.Visible = false;
                    buttonRoll4.Visible = false;
                    buttonRoll5.Visible = false;
                    buttonRoll6.Visible = false;
                    buttonRoll7.Visible = false;
                    buttonRoll8.Visible = false;
                    buttonRoll9.Visible = false;
                    buttonRoll10.Visible = false;
                    buttonFlat1.Visible = false;
                    buttonFlat2.Visible = false;
                    buttonFlat3.Visible = false;
                    btn_sign_done.Visible = true;
                    btn_sign_clear.Visible = true;

                    pos[0] = FINGER_POSITION.SIGNATURE;
                    currentWorkPosition = FINGER_POSITION.SIGNATURE;
                    curretScanningPOS = E_FINGER_POSITION.FINGER_POSITION_SIGNATURE;
                    IMD_RESULT res = imd_fap50.scan_start(GUI_SHOW_MODE.SIGNATURE, ref pos[0], pos.Length);
                    if (res == IMD_RESULT.NOT_SUPPORT)
                    {
                        MessageBox.Show("The feature is not surpport this device.", "Not surpport", MessageBoxButtons.OK);
                        Close();
                        return;
                    }
                    timer_sign.Start();                                        
                    break;
            }

            sampling_msg.Font = new Font(sampling_msg.Font.FontFamily, 14);
            sampling_msg.ForeColor = Color.Blue;
            sampling_msg.Text = "";
        }
        void FormAutoCapture_Resize(object sender, EventArgs e)
        {
            ReSize();
        }
        void FormAutoCapture_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(guideFrameTimer!= null)
                guideFrameTimer.Stop();
            if(timer_flat != null)
                timer_flat.Stop();
            StopScan();
            StopBlinking();            
            IMD_FAP50_SDK_PANEL.Disconnect();
            imd_fap50.scan_cancel();
        }

        void ReSize()
        {
            int sw = this.ClientSize.Width - panel1.Width;
            int sh = this.ClientSize.Height;
            double s = sw / sh;
            if (s < 1.6)
            {
                pictureBox1.Width = sw;
                pictureBox1.Height = pictureBox1.Width * IMG_HEIGHT / IMG_WIDTH;
                pictureBox1.Left = 0;
                pictureBox1.Top = (sh - pictureBox1.Height) / 2;
            }
            else
            {
                pictureBox1.Height = sh;
                pictureBox1.Width = pictureBox1.Height * IMG_WIDTH / IMG_HEIGHT;

                pictureBox1.Top = 0;
                pictureBox1.Left = (sw - pictureBox1.Width) / 2;
            }
        }
        //Timer for guide frame
        private void StartGuideFrameLoop()
        {
            guideFrameTimer = new System.Windows.Forms.Timer();
            guideFrameTimer.Interval = 50; 
            guideFrameTimer.Tick += (s, e) => ShowGuideFrame(s, e);
            guideFrameTimer.Start();
        }

        //Implement Async method
        private void StartScan()
        {
            if (_isScanRunning) return;

            _ctsScan = new CancellationTokenSource();
            _isScanRunning = true;
            _ = ScanLoopAsync(_ctsScan.Token);
            StartGuideFrameLoop();
        }

        private void StopScan()
        {
            if (!_isScanRunning) return;

            _ctsScan?.Cancel();
            _isScanRunning = false;
        }
        private async Task ScanLoopAsync(CancellationToken token)
        {
            bool isReScan = false;
            var img_status = default(ImageStatus);         
            FINGER_POSITION[] pos = { FINGER_POSITION.UNKNOW_FINGER };
            img_status.show_mode = GUI_SHOW_MODE.FLAT;
            try
            {
                while (!token.IsCancellationRequested)
                {
                    var res = imd_fap50.get_image_status(ref img_status);
                    if (res != IMD_RESULT.SUCCESS)
                    {
                        await Task.Delay(33, token);
                        continue;
                    }
                    
                    if(img_status.show_mode == GUI_SHOW_MODE.FLAT)
                    {
                        Handle_Flat_Mode_Scan(ref img_status);
                    }
                    else //GUI_SHOW_MODE.ROLL
                    {
                        Handle_Roll_mode_Scan(ref img_status);
                    }

                    if (img_status.img != IntPtr.Zero)
                    {
                        using (var bitmap = U8PtrToBitmap32(img_status.img, 1600, 1000))
                        {
                            if (img_status.finger_num > 0)
                            {
                                DrawDiamonds(bitmap, img_status);
                            }
                            
                            _latestFrame?.Dispose();
                            _latestFrame = (Bitmap)bitmap.Clone();
                            SafeUpdatePictureBox();
                        }
                    }
                    await Task.Delay(33, token);
                }
            }
            catch (TaskCanceledException)
            {
                
            }
        }
        private void SafeUpdatePictureBox()
        {
            if (_uiUpdating) return; 
            _uiUpdating = true;

            this.BeginInvoke((Action)(() =>
            {
                try
                {
                    if (isFingerON_ScanDone)
                        return;
                    if (_latestFrame != null)
                    {
                        pictureBox1.Image?.Dispose();
                        pictureBox1.Image = (Bitmap)_latestFrame.Clone();
                    }                   
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"[UI Update Error] {ex.Message}");
                }
                finally
                {
                    _uiUpdating = false;
                }
            }));
        }
        private void DrawDiamonds(Bitmap bitmap, ImageStatus imgStatus)
        {
            ImdDiamond[] diamonds = new ImdDiamond[imgStatus.finger_num];
            for (int i = 0; i < imgStatus.finger_num; i++)
            {
                IntPtr diamondPtr = (IntPtr)((long)imgStatus.contours + i * Marshal.SizeOf(typeof(ImdDiamond)));
                diamonds[i] = Marshal.PtrToStructure<ImdDiamond>(diamondPtr);
            }

            using (Graphics g = Graphics.FromImage(bitmap))
            using (Pen pen = new Pen(Color.DarkGreen, 2))
            {
                foreach (var diamond in diamonds)
                {
                    g.DrawLine(pen, (float)diamond.pa.x, (float)diamond.pa.y, (float)diamond.pb.x, (float)diamond.pb.y);
                    g.DrawLine(pen, (float)diamond.pa.x, (float)diamond.pa.y, (float)diamond.pd.x, (float)diamond.pd.y);
                    g.DrawLine(pen, (float)diamond.pc.x, (float)diamond.pc.y, (float)diamond.pb.x, (float)diamond.pb.y);
                    g.DrawLine(pen, (float)diamond.pc.x, (float)diamond.pc.y, (float)diamond.pd.x, (float)diamond.pd.y);
                }
            }
        }

        private void Handle_Flat_Mode_Scan(ref ImageStatus img_status)
        {
            bool fNeedReScan = false;
            if (img_status.is_finger_on == false && img_status.is_flat_done == false)
            {
                sampling_msg.ForeColor = Color.Red;
                sampling_msg.Text = $"Please put your finger. ({img_status.frame_rate:F1} fps)";                
            }
            else if (img_status.is_finger_on && img_status.is_flat_done == false)
            {
                sampling_msg.ForeColor = Color.Blue;
                sampling_msg.Text = $"Keep your fingers steady. ({img_status.frame_rate:F1} fps)";
                guideFrameTimer.Stop();
                ShowScanningFrame();
                scanningFrameIndex = 0;
            }
            else if (img_status.is_finger_on && img_status.is_flat_done)
            {
                sampling_msg.ForeColor = Color.Red;
                sampling_msg.Text = $"Sampling done. Please leave your finger. ";
                //ShowScanningFrame();
                isFingerON_ScanDone = true;
            }
            else if (img_status.is_finger_on == false && img_status.is_flat_done)
            {
                sampling_msg.ForeColor = Color.Green;
                sampling_msg.Text = "Sampling complete.";
                imd_fap50.scan_cancel();
                var p = new ImageProperty
                {
                    mode = img_status.show_mode,
                    pos = img_status.finger_position,
                    this_scan = true
                };
                var res = imd_fap50.get_image(ref p);
                ValidImgFPath = FindValidImageFile(ref p, out bool isValid);
                ShowValidFinger();

                switch (res)
                {
                    case IMD_RESULT.PUT_WRONG_HAND:
                        if (DialogResult.Yes == MessageBox.Show("You are using the wrong hand. \nDo you want to scan again?", "Wrong Hand", MessageBoxButtons.YesNo))
                        {
                            fNeedReScan = true;
                        }
                        break;

                    case IMD_RESULT.POOR_QUALITY_AND_CANTACT_IRON:
                    case IMD_RESULT.POOR_NFIQ_QUALITY:
                        if (DialogResult.Yes == MessageBox.Show("The image quality score is too low. \nDo you want to scan again?", "Poor Quality", MessageBoxButtons.YesNo))
                        {
                            fNeedReScan = true;
                        }
                        break;

                    case IMD_RESULT.POOR_QUALITY_AND_WRONG_HAND:
                        if (DialogResult.Yes == MessageBox.Show("You are using the wrong hand and The image quality score is too low. \nDo you want to scan again?", "Wrong Hand & Poor Quality", MessageBoxButtons.YesNo))
                        {
                            fNeedReScan = true;
                        }
                        break;
                    default:
                        break;
                }

                if (res == IMD_RESULT.SUCCESS || 
                   (res != IMD_RESULT.SUCCESS && fNeedReScan == false))
                {
                    workIndex++;
                    currentGuideIndex++;
                    curretScanningPOS++;
                    currentFrameIndex = 0;
                    guideFrameTimer.Start();                    
                    fNeedReScan = true;
                }

                if (fNeedReScan)
                {
                    img_status = default;
                    processScanStart(ref workIndex, false);
                    fNeedReScan = false;
                }
                isFingerON_ScanDone = false;
            }
        }
        private void Handle_Roll_mode_Scan(ref ImageStatus img_status)
        {
            bool fNeedReScan = false;

            sampling_msg.ForeColor = Color.Blue;
            if (img_status.is_finger_on == false && img_status.is_roll_done == false)
            {
                sampling_msg.ForeColor = Color.Red;
                sampling_msg.Text = $"Please place your finger. ({img_status.frame_rate:F1} fps)";
            }
            else if (img_status.is_finger_on && img_status.is_roll_done == false)
            {
                sampling_msg.Text = $"Keep roll... ({img_status.frame_rate:F1} fps)";
                guideFrameTimer.Stop();
                ShowScanningFrame();
                scanningFrameIndex = 0;
            }
            else if (img_status.is_finger_on && img_status.is_roll_done)
            {
                sampling_msg.ForeColor = Color.Red;
                sampling_msg.Text = $"Rolling done. Please leave your finger. ";
                ShowScanningFrame();                
            }
            else if (img_status.is_finger_on == false && img_status.is_roll_done)
            {
                sampling_msg.Text = "";
                var p = new ImageProperty
                {
                    mode = img_status.show_mode,
                    pos = img_status.finger_position,
                    this_scan = true
                };
                imd_fap50.scan_cancel();
                var res = imd_fap50.get_image(ref p);
                ValidImgFPath = FindValidImageFile(ref p, out bool isValid);
                ShowValidFinger();

               
                switch (res)
                {
                    case IMD_RESULT.PUT_WRONG_HAND:
                        if (DialogResult.Yes == MessageBox.Show("You are using the wrong hand. \nDo you want to scan again?", "Wrong Hand", MessageBoxButtons.YesNo))
                        {
                            fNeedReScan = true;
                        }
                        break;

                    case IMD_RESULT.POOR_QUALITY_AND_CANTACT_IRON:
                    case IMD_RESULT.POOR_NFIQ_QUALITY:
                        if (DialogResult.Yes == MessageBox.Show("The image quality score is too low. \nDo you want to scan again?", "Poor Quality", MessageBoxButtons.YesNo))
                        {

                            fNeedReScan = true;
                        }
                        break;

                    case IMD_RESULT.POOR_QUALITY_AND_WRONG_HAND:
                        if (DialogResult.Yes == MessageBox.Show("You are using the wrong hand and The image quality score is too low. \nDo you want to scan again?", "Wrong Hand & Poor Quality", MessageBoxButtons.YesNo))
                        {
                            fNeedReScan = true;
                        }
                        break;
                    default:
                        break;
                }

                if (res == IMD_RESULT.SUCCESS ||
                    (res != IMD_RESULT.SUCCESS && fNeedReScan == false))
                {
                    workIndex++;
                    if (p.pos == FINGER_POSITION.LEFT_LITTLE)
                    {
                        currentGuideIndex++;
                        curretScanningPOS = E_FINGER_POSITION.FINGER_POSITION_RIGHT_THUMB;
                    }
                    guideFrameTimer.Start();
                    fNeedReScan = true;
                }

                if (fNeedReScan)
                {
                    img_status = default;
                    processScanStart(ref workIndex, true);
                    fNeedReScan = false;
                }
            }
        }
        void timer_sign_Tick(object sender, EventArgs e)
        {
            if (imd_fap50.is_scan_busy() == false)
            {
                Close();
                return;
            }

            var currentPos = curretScanningPOS;
            ImageStatus img_status = default;
            IMD_RESULT res = imd_fap50.get_image_status(ref img_status);
            if (res != IMD_RESULT.SUCCESS)
                return;

            if (sign_show_cnt++ % 10 == 0)
            {
                sampling_msg.ForeColor = Color.Blue;
                sampling_msg.Text = $"Signature...({img_status.frame_rate:F1} fps)";
            }
            pictureBox1.Image = U8PtrToBitmap32(img_status.img, 1600, 1000);

            //Integrate the signature img to scanning mat image            
            if (!scanning_img.TryGetValue(currentPos, out var baseMat))
                return;
            Mat scanFrame = baseMat.Clone();
            Mat Sign_img = Mat.FromPixelData(1000, 1600, MatType.CV_8UC1, img_status.img).Clone();
            Mat resize_img = new Mat();
            Cv2.Resize(Sign_img, resize_img, new OpenCvSharp.Size(768, 480));

            Sign_img = new Mat(480, 800, MatType.CV_8UC1, new Scalar(255));
            Rect roi = new Rect(16, 0, resize_img.Cols, resize_img.Rows);
            resize_img.CopyTo(new Mat(Sign_img, roi));

            Mat mask = new Mat();
            Cv2.Threshold(Sign_img, mask, 192, 255, ThresholdTypes.BinaryInv);
            scanFrame.SetTo(new Scalar(0, 0, 0), mask);

            //Send to Pannel
            byte[] imgBytes;
            Mat rotated_img = new Mat();
            Cv2.Rotate(scanFrame, rotated_img, RotateFlags.Rotate90Clockwise);
            Cv2.ImEncode(".jpg", rotated_img, out imgBytes, new ImageEncodingParam(ImwriteFlags.JpegQuality, 60));
            bool sent = IMD_FAP50_SDK_PANEL.send_jpg_fap50_panel(imgBytes, (uint)imgBytes.Length);
            if (!sent)
                Debug.WriteLine("Fail to send jpg to panel");
        }
        private void StartBlinking()
        {
            _ctsBlink = new CancellationTokenSource();
            _ = BlinkLoopAsync(_ctsBlink.Token);
        }

        private void StopBlinking()
        {
            _ctsBlink?.Cancel();
        }
        private async Task BlinkLoopAsync(CancellationToken token)
        {
            try
            {
                btn_blink = true; 
                UpdateBlinkUI();
                while (!token.IsCancellationRequested)
                {
                    btn_blink = !btn_blink;
                    await Task.Delay(500, token);                    
                    UpdateBlinkUI();
                }
            }
            catch (TaskCanceledException)
            {
                
            }           
        }
        private void UpdateBlinkUI()
        {
            this.Invoke(() =>
            {
                switch (workIndex)
                {
                    case -1:
                        buttonFlat1.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 0:
                        buttonFlat1.BackColor = Color.LightGreen;
                        buttonFlat2.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 1:
                        buttonFlat2.BackColor = Color.LightGreen;
                        buttonFlat3.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 2:
                        buttonFlat3.BackColor = Color.LightGreen;
                        buttonRoll1.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 3:
                        buttonRoll1.BackColor = Color.LightGreen;
                        buttonRoll2.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 4:
                        buttonRoll2.BackColor = Color.LightGreen;
                        buttonRoll3.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 5:
                        buttonRoll3.BackColor = Color.LightGreen;
                        buttonRoll4.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 6:
                        buttonRoll4.BackColor = Color.LightGreen;
                        buttonRoll5.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 7:
                        buttonRoll5.BackColor = Color.LightGreen;
                        buttonRoll6.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 8:
                        buttonRoll6.BackColor = Color.LightGreen;
                        buttonRoll7.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 9:
                        buttonRoll7.BackColor = Color.LightGreen;
                        buttonRoll8.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 10:
                        buttonRoll8.BackColor = Color.LightGreen;
                        buttonRoll9.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    case 11:
                        buttonRoll9.BackColor = Color.LightGreen;
                        buttonRoll10.BackColor = btn_blink ? Color.LightSalmon : Color.LightGreen;
                        break;
                    default:
                        break;
                }
            });
        }
        private IMD_RESULT processScanStart(ref int workIndex, bool isRollingScan)
        {
            IMD_RESULT res = IMD_RESULT.FAIL;
            FINGER_POSITION[] pos = { FINGER_POSITION.UNKNOW_FINGER };
            GUI_SHOW_MODE ShowMode = new GUI_SHOW_MODE();
            if (!isRollingScan)
            {
                switch (workIndex)
                {
                    case -1:
                        //workIndex = 0;
                        pos[0] = FINGER_POSITION.LEFT_FOUR;
                        ShowMode = GUI_SHOW_MODE.FLAT;
                        break;
                    case 0:
                        //workIndex = 1;
                        pos[0] = FINGER_POSITION.RIGHT_FOUR;
                        ShowMode = GUI_SHOW_MODE.FLAT;
                        break;
                    case 1:
                        //workIndex = 2;
                        pos[0] = FINGER_POSITION.BOTH_THUMBS;
                        ShowMode = GUI_SHOW_MODE.FLAT;
                        break;
                    case 2:
                        //workIndex = 3;
                        if (workType == SampleSequence.Flat442R)
                        {
                            //timer_flat.Stop();
                            pos[0] = FINGER_POSITION.LEFT_THUMB;
                            ShowMode = GUI_SHOW_MODE.ROLL;
                            curretScanningPOS = E_FINGER_POSITION.FINGER_POSITION_LEFT_THUMB;
                            //timer_roll.Start();
                        }
                        else
                        {
                            DialogResult = DialogResult.OK;
                            res = IMD_RESULT.NOT_SUPPORT;
                        }
                        break;
                }

            }
            else
            {
                ShowMode = GUI_SHOW_MODE.ROLL;
                switch (workIndex)
                {
                    case 2:
                        pos[0] = FINGER_POSITION.LEFT_THUMB;
                        break;
                    case 3:
                        pos[0] = FINGER_POSITION.LEFT_INDEX;
                        break;
                    case 4:
                        pos[0] = FINGER_POSITION.LEFT_MIDDLE;
                        break;
                    case 5:
                        pos[0] = FINGER_POSITION.LEFT_RING;
                        break;
                    case 6:
                        pos[0] = FINGER_POSITION.LEFT_LITTLE;
                        break;

                    case 7:
                        pos[0] = FINGER_POSITION.RIGHT_THUMB;
                        break;
                    case 8:
                        pos[0] = FINGER_POSITION.RIGHT_INDEX;
                        break;
                    case 9:
                        pos[0] = FINGER_POSITION.RIGHT_MIDDLE;
                        break;
                    case 10:
                        pos[0] = FINGER_POSITION.RIGHT_RING;
                        break;
                    case 11:
                        pos[0] = FINGER_POSITION.RIGHT_LITTLE;
                        break;

                    default:
                        timer_roll.Stop();
                        DialogResult = DialogResult.OK;
                        break;
                }
            }
            res = imd_fap50.scan_start(ShowMode, ref pos[0], pos.Length);
            return res;
        }
        //End of Implement
        
        void btn_sign_clear_Click(object sender, EventArgs e)
        {
            imd_fap50.signature(SIGNATURE_ACTION.CLEAR);
        }
        void btn_sign_done_Click(object sender, EventArgs e)
        {
            imd_fap50.signature(SIGNATURE_ACTION.OK);
        }
        private void InitVideoShowTimer()
        {
            VideoShowTimer = new System.Windows.Forms.Timer();
            VideoShowTimer.Interval = 50; //units: milliseconds
            VideoShowTimer.Tick += VideoShowTimer_Tick;
        }

        //Add show video feature in the pictureBox_Video
        // Initialize form parameters if needed
        // This can include setting default values for controls, etc.
        private void InitializeFormParams()
        {
            string exeDir = Path.GetDirectoryName(Application.ExecutablePath)!;
            //Back to the trunk3 folder
            string projectRoot = Path.GetFullPath(Path.Combine(exeDir, @"..\..\..\..\..\.."));
            baseDir = Path.Combine(projectRoot, @"x64\Release");
            videoBasePath = Path.Combine(projectRoot, @"x64\Release\panel\video");

        }

        private Mat LoadImage(string relativePath)
        {
            string fullPath = Path.Combine(baseDir, relativePath.Replace("/", Path.DirectorySeparatorChar.ToString()));

            if (!File.Exists(fullPath))
                throw new FileNotFoundException($"Cannot find the image file：{fullPath}");

            return Cv2.ImRead(fullPath);
        }

        /*
            Function: InitFingerScanningImages
            Initializes the finger panel images used for different scanning modes.
            (scanning_img and loading_img dictionaries are populated with images)
        */
        private void InitFingerScanningImages()
        {
            try
            {
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_FOUR] = LoadImage("panel/RightHandPanel/RightHandPanel-Main.png");
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_FOUR] = LoadImage("panel/LeftHandPanel/LeftHandPanel-Main.png");
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_BOTH_THUMBS] = LoadImage("panel/ThumbsPanel/ThumbPanel-Main.png");


                //Image rightRolling = LoadImage("panel/RightRollingFingers/RightRolling-MainPanel.png");
                Mat rightRolling = LoadImage("panel/RightRollingFingers/RightRolling-MainPanel.png");
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_THUMB] = rightRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_INDEX] = rightRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_MIDDLE] = rightRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_RING] = rightRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_RIGHT_LITTLE] = rightRolling;


                //Image leftRolling = LoadImage("panel/LeftRollingFingers/LeftRolling-MainPanel.png");
                Mat leftRolling = LoadImage("panel/LeftRollingFingers/LeftRolling-MainPanel.png");
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_THUMB] = leftRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_INDEX] = leftRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_MIDDLE] = leftRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_RING] = leftRolling;
                scanning_img[E_FINGER_POSITION.FINGER_POSITION_LEFT_LITTLE] = leftRolling;


                scanning_img[E_FINGER_POSITION.FINGER_POSITION_SIGNATURE] = LoadImage("panel/SignaturePanel/SignatureScreen-MainPanel-NoButton.png");


                loading_img[1] = LoadImage("panel/LoadingScreen/WithButton/ScanningFrame-1.png");
                loading_img[2] = LoadImage("panel/LoadingScreen/WithButton/ScanningFrame-2.png");
                loading_img[3] = LoadImage("panel/LoadingScreen/WithButton/ScanningFrame-3.png");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Fail to load image: {ex.Message}");
            }
        }
        private Dictionary<string, FINGER_POSITION> menuMap = new()
        {
            { "left_4", FINGER_POSITION.LEFT_FOUR },
            { "right_4", FINGER_POSITION.RIGHT_FOUR },
            { "two_thumbs", FINGER_POSITION.BOTH_THUMBS },
            { "left_roll", FINGER_POSITION.LEFT_INDEX },
            { "right_roll", FINGER_POSITION.RIGHT_INDEX },
            { "signature", FINGER_POSITION.SIGNATURE },
            { "some_fingers", FINGER_POSITION.SOME_FINGERS }
        };
        private void InitVideoFrames()
        {
            scanSequence.Clear();            
            List<string> requiredMenus = new();

            switch (workType)
            {
                case SampleSequence.Flat442:
                    requiredMenus.AddRange(new[] { "left_4", "right_4", "two_thumbs" });
                    break;
                case SampleSequence.Flat442R:
                    requiredMenus.AddRange(new[] { "left_4", "right_4", "two_thumbs", "left_roll", "right_roll" });
                    break;
                case SampleSequence.Signature:
                    requiredMenus.Add("signature");
                    break;
                case SampleSequence.FlatSomefinters:
                    requiredMenus.Add("some_fingers");
                    break;
            }

            foreach (string menu in requiredMenus)
            {
                if (menuMap.TryGetValue(menu.ToLower(), out var pos_local))
                {
                    scanSequence.Add(pos_local);
                }

                string folderPath = Path.Combine(videoBasePath, menu);
                string listPath = Path.Combine(folderPath, "list.txt");

                if (!File.Exists(listPath)) continue;

                var framePaths = new List<string>();
                foreach (string line in File.ReadLines(listPath))
                {
                    string trimmed = line.Trim();
                    if (trimmed == "EOF") break;

                    string imagePath = Path.Combine(folderPath, trimmed);
                    if (File.Exists(imagePath))
                        framePaths.Add(imagePath);
                }

                if (framePaths.Count > 0)
                    guideFramePaths[pos_local] = framePaths;
            }
        }                
        private void ShowGuideFrame(object sender, EventArgs e)//private void ShowGuideFrame()
        {
            if (currentGuideIndex >= scanSequence.Count)
            {
                guideFrameTimer.Stop();
                return;
            }

            var currentPos = scanSequence[currentGuideIndex];

            if (!guideFramePaths.TryGetValue(currentPos, out var frameList) || frameList.Count == 0)
                return;

            if (currentFrameIndex >= frameList.Count)
                currentFrameIndex = 0;

            string framePath = frameList[currentFrameIndex];
            byte[] imgBytes = File.ReadAllBytes(framePath);
            currentFrameIndex++;

            try
            {
                using var bmp = new Bitmap(framePath);
                bmp.RotateFlip(RotateFlipType.Rotate270FlipNone);

                var GuideImage = new Bitmap(bmp, PictureBox_Video.Size); 

                if (PictureBox_Video.Image != null)
                    PictureBox_Video.Image.Dispose();

                bool sent = IMD_FAP50_SDK_PANEL.send_jpg_fap50_panel(imgBytes, (uint)imgBytes.Length);
                if (!sent)
                    dbg($"Fail to send jpg to Panel: {framePath}");
                PictureBox_Video.Image = GuideImage;
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"[GuideFrameTimer_Tick] Load error: {framePath}, {ex.Message}");
            }
        }     
        //private void ShowScanningFrame(object sender, EventArgs e)
        private void ShowScanningFrame()
        {
            var currentPos = curretScanningPOS;
            if (!scanning_img.TryGetValue(currentPos, out var baseMat))
                return;

            Mat scanFrame = baseMat.Clone();

            int loadingIdx = (scanningFrameIndex++ / 8) % 3 + 1;

            if (loading_img.TryGetValue(loadingIdx, out var loadingMat))
            {
                Rect roi = new Rect(260, 48, loadingMat.Width, loadingMat.Height);
                if (roi.X + roi.Width <= scanFrame.Width && roi.Y + roi.Height <= scanFrame.Height)
                {             
                    loadingMat.CopyTo(new Mat(scanFrame, roi));
                }
            }

            PictureBox_Video.Image?.Dispose();
            PictureBox_Video.Image = BitmapConverter.ToBitmap(scanFrame);

            byte[] imgBytes;
            Mat rotated_img = new Mat();
            Cv2.Rotate(scanFrame, rotated_img, RotateFlags.Rotate90Clockwise);
            Cv2.ImEncode(".jpg", rotated_img, out imgBytes, new ImageEncodingParam(ImwriteFlags.JpegQuality, 60));
            bool sent = IMD_FAP50_SDK_PANEL.send_jpg_fap50_panel(imgBytes, (uint)imgBytes.Length);
            if (!sent)
                Debug.WriteLine("Fail to send jpg to panel");


            scanFrame.Dispose();
        }

        private ImageCodecInfo GetEncoder(ImageFormat format)
        {
            return ImageCodecInfo.GetImageDecoders()
                .FirstOrDefault(c => c.FormatID == format.Guid);
        }
        private void ShowValidFinger()
        {            
            Mat score_img = LoadImage(ValidImgFPath);
            if (score_img.Empty()) return;

            if (!score_img.Empty())
            {
                PictureBox_Video.Image?.Dispose();
                PictureBox_Video.Image = BitmapConverter.ToBitmap(score_img);
                PictureBox_Video.Refresh();         
                Application.DoEvents();             

                byte[] imgBytes;
                Mat rotated_img = new Mat();
                Cv2.Rotate(score_img, rotated_img, RotateFlags.Rotate90Clockwise);
                Cv2.ImEncode(".jpg", rotated_img, out imgBytes, new ImageEncodingParam(ImwriteFlags.JpegQuality, 60));

                bool sent = IMD_FAP50_SDK_PANEL.send_jpg_fap50_panel(imgBytes, (uint)imgBytes.Length);
                if (!sent)
                    Debug.WriteLine("Fail to send jpg to panel");
                score_img.Dispose();
                Thread.Sleep(500);
            }
        }
        private string FindValidImageFile(ref ImageProperty p, out bool isValid) 
        {
            string ValidImgPath = "";
            var score = new Score2Num();
            isValid = false;
            int[] managedArray = new int[(int)ScoreArray.MaxSize];
            unsafe
            {
                fixed (int* pArr = p.score_array)
                {
                    Marshal.Copy((IntPtr)pArr, managedArray, 0, managedArray.Length);
                }
            }

            switch (p.score_size)
            {

                case 4:
                    if (p.pos == FINGER_POSITION.RIGHT_FOUR)
                    {

                        score.R1 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[0] <= p.score_min : managedArray[0] >= p.score_min;
                        score.R2 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[1] <= p.score_min : managedArray[1] >= p.score_min;
                        score.R3 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[2] <= p.score_min : managedArray[2] >= p.score_min;
                        score.R4 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[3] <= p.score_min : managedArray[3] >= p.score_min;

                        isValid = score.R1 && score.R2 && score.R3 && score.R4;
                        ValidImgPath = Path.Combine(baseDir, $"panel/RightHandPanel/Iterations/Iteration_{score.num}.png");
                    }
                    else if (p.pos == FINGER_POSITION.LEFT_FOUR)
                    {
                        score.L1 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[0] <= p.score_min : managedArray[0] >= p.score_min;
                        score.L2 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[1] <= p.score_min : managedArray[1] >= p.score_min;
                        score.L3 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[2] <= p.score_min : managedArray[2] >= p.score_min;
                        score.L4 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[3] <= p.score_min : managedArray[3] >= p.score_min;

                        isValid = score.L1 && score.L2 && score.L3 && score.L4;
                        ValidImgPath = Path.Combine(baseDir, $"panel/LeftHandPanel/Iterations/Iteration_{score.num}.png");
                    }
                    break;

                case 2:
                    if (p.pos == FINGER_POSITION.BOTH_THUMBS)
                    {
                        score.L0 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[0] <= p.score_min : managedArray[0] >= p.score_min;
                        score.R0 = p.score_ver == NFIQ_VERSION.V1 ? managedArray[1] <= p.score_min : managedArray[1] >= p.score_min;

                        isValid = score.L0 && score.R0;
                        ValidImgPath = Path.Combine(baseDir, $"panel/ThumbsPanel/Iterations/Iteration_{score.num}.png");
                    }
                    break;

                case 1:
                    bool passed = p.score_ver == NFIQ_VERSION.V1 ? managedArray[0] <= p.score_min : managedArray[0] >= p.score_min;
                    isValid = passed;

                    switch (p.pos)
                    {
                        case FINGER_POSITION.RIGHT_THUMB:
                        case FINGER_POSITION.RIGHT_INDEX:
                        case FINGER_POSITION.RIGHT_MIDDLE:
                        case FINGER_POSITION.RIGHT_RING:
                        case FINGER_POSITION.RIGHT_LITTLE:
                            ValidImgPath = Path.Combine(baseDir, $"panel/RightRollingFingers/Iterations/RollFinger-{(passed ? "Done" : "Retry")}.png");
                            break;

                        case FINGER_POSITION.LEFT_THUMB:
                        case FINGER_POSITION.LEFT_INDEX:
                        case FINGER_POSITION.LEFT_MIDDLE:
                        case FINGER_POSITION.LEFT_RING:
                        case FINGER_POSITION.LEFT_LITTLE:
                            ValidImgPath = Path.Combine(baseDir, $"panel/LeftRollingFingers/Iterations/RollFinger-{(passed ? "Done" : "Retry")}.png");
                            break;
                    }
                    break;
            }
            return ValidImgPath;
        }
        private void VideoShowTimer_Tick(object sender, EventArgs e)
        {           
            /*
            switch (VideoShowPhase)
            {
                case Video_ShowPhase.GuideFrame:
                    ShowGuideFrame(sender, e);
                    break;

                case Video_ShowPhase.Scanning:
                    ShowScanningFrame(sender, e);
                    break;

                case Video_ShowPhase.Validate:
                    ShowValidFinger(sender, e);
                    break;
            } 
            */
        }
        private void StopVideoShowTimer()
        {
            if (VideoShowTimer != null && VideoShowTimer.Enabled)
            {
                VideoShowTimer.Stop();
                VideoShowTimer.Dispose();
                VideoShowTimer = null;
            }
        }
    }
}
