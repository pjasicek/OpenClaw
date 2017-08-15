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

            SoundPlayer audio = new SoundPlayer(ClawLauncher.Properties.Resources.TITLE);
            audio.Play();
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

            string homePath = (Environment.OSVersion.Platform == PlatformID.Unix ||
                   Environment.OSVersion.Platform == PlatformID.MacOSX)
                ? Environment.GetEnvironmentVariable("HOME")
                : Environment.ExpandEnvironmentVariables("%HOMEDRIVE%%HOMEPATH%");

            string[] possibleClawExePaths = 
            {
                "CaptainClaw.exe",
                "Claw.exe",
                homePath + "/CaptainClaw/Build_Release/CaptainClaw.exe",
                homePath + "/CaptainClaw/Build_Release/captainclaw",
                "/usr/bin/captainclaw",
            };

            String clawExePath = "";
            foreach (string possibleExe in possibleClawExePaths)
            {
                if (System.IO.File.Exists(possibleExe))
                {
                    clawExePath = possibleExe;
                    break;
                }
            }

            if (String.IsNullOrEmpty(clawExePath))
            {
                String searchedPaths = "";
                foreach (string possibleExe in possibleClawExePaths)
                {
                    searchedPaths += possibleExe + "\n";
                }

                MessageBox.Show("Could not locate CaptainClaw binary on your system.\nSearched paths:\n\n" + searchedPaths, "CaptainClaw bin not found", MessageBoxButtons.OK, MessageBoxIcon.Error);

                Application.Exit();
                return;
            }

            Process proc = new Process();
            proc.StartInfo.FileName = clawExePath;
            proc.StartInfo.UseShellExecute = false;
            proc.Start();

            Application.Exit();
        }

        private void OptionsButton_Click(object sender, EventArgs e)
        {

        }

        private void ExitButton_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }
}
