using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Media;

namespace ClawLauncher
{
    public partial class ClawLauncherForm : Form
    {
        public static string ClawExecutablePath;
        public static string ClawConfigPath;

        public ClawLauncherForm()
        {
            InitializeComponent();

            CenterToScreen();

            PlayButton.TabStop = false;
            PlayButton.FlatStyle = FlatStyle.Flat;
            PlayButton.FlatAppearance.BorderSize = 3;
            PlayButton.FlatAppearance.BorderColor = Color.FromArgb(255, 0, 0, 0);

            OptionsButton.TabStop = false;
            OptionsButton.FlatStyle = FlatStyle.Flat;
            OptionsButton.FlatAppearance.BorderSize = 3;
            OptionsButton.FlatAppearance.BorderColor = Color.FromArgb(255, 0, 0, 0);

            ExitButton.TabStop = false;
            ExitButton.FlatStyle = FlatStyle.Flat;
            ExitButton.FlatAppearance.BorderSize = 3;
            ExitButton.FlatAppearance.BorderColor = Color.FromArgb(255, 0, 0, 0);

            PlayButton.MouseHover += new EventHandler(Button_OnMouseOver);
            OptionsButton.MouseHover += new EventHandler(Button_OnMouseOver);
            ExitButton.MouseHover += new EventHandler(Button_OnMouseOver);

            this.Activated += ClawLauncherForm_OnFormShown;
        }

        bool GetClawPaths()
        {
            string clawLauncherDir = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location);

            ClawConfigPath = "";
            ClawExecutablePath = "";

            if (IsLinux)
            {
                ClawExecutablePath = clawLauncherDir + "/" + "openclaw";

                if (File.Exists(clawLauncherDir + "/config.xml"))
                {
                    ClawConfigPath = clawLauncherDir + "/" + "config.xml";
                }
                else
                {
                    ClawConfigPath = Environment.GetFolderPath(Environment.SpecialFolder.Personal) + "/.config/openclaw/config.xml";
                }
            }
            else
            {
                ClawExecutablePath = clawLauncherDir + "/" + "OpenClaw.exe";
                ClawConfigPath = clawLauncherDir + "/" + "config.xml";
            }

            if (!File.Exists(ClawExecutablePath))
            {

                MessageBox.Show("Could not locate OpenClaw binary - it has to be in the same folder as is this Launcher.\nExpected location:\n\n" + ClawExecutablePath, "OpenClaw bin not found", MessageBoxButtons.OK, MessageBoxIcon.Error);

                //Application.Exit();
                //Close();
                return false;
            }

            if (!File.Exists(ClawConfigPath))
            {
                MessageBox.Show("Could not locate OpenClaw config.xml.\nExpected location:\n\n" + ClawConfigPath, "OpenClaw config.xml not found", MessageBoxButtons.OK, MessageBoxIcon.Error);

                //Application.Exit();
                //Close();
                return false;
            }

            ClawPathsLabel.Text = "Claw Binary Path:  " + ClawExecutablePath + "\n" +
                                  "Claw Config Path:  " + ClawConfigPath + "\n";
            ClawPathsLabel.Height *= 2;

            return true;
        }

        void Button_OnMouseOver(object sender, EventArgs e)
        {
            /*SoundPlayer audio = new SoundPlayer(ClawLauncher.Properties.Resources.slide_scissors);
            audio.Play();*/
        }

        public static bool IsLinux
        {
            get
            {
                int p = (int)Environment.OSVersion.Platform;
                return (p == 4) || (p == 6) || (p == 128);
            }
        }

        private void PlayButton_Click(object sender, EventArgs e)
        {
            /*SoundPlayer audio = new SoundPlayer(ClawLauncher.Properties.Resources.tap_warm);
            audio.Play();*/

            Process proc = new Process();
            proc.StartInfo.FileName = ClawExecutablePath;
            proc.StartInfo.UseShellExecute = false;
            proc.Start();

            Application.Exit();
        }

        private void OptionsButton_Click(object sender, EventArgs e)
        {
            ClawOptions optionsForm = new ClawOptions();
            optionsForm.Hide();
            optionsForm.ShowDialog(this);
        }

        private void ExitButton_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void ClawLauncherForm_Load(object sender, EventArgs e)
        {
            if (!GetClawPaths())
            {
                Application.Exit();
                Close();
                Environment.Exit(1);
                return;
            }

            SoundPlayer audio = new SoundPlayer(ClawLauncher.Properties.Resources.TITLE);
            audio.Play();
        }

        private void ClawLauncherForm_OnFormShown(object sender, EventArgs e)
        {
            
        }
    }
}
