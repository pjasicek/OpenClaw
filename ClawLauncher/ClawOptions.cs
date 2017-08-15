using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace ClawLauncher
{
    public struct ClawConfig
    {
        // Video
        public Point Resolution;
        public float Scale;
        public bool bVerticalSync;
        public bool bFullscreen;
        public bool bFullscreenDesktop;

        // Audio
        public int Frequency;
        public int SoundChannels;
        public int MixingChannels;
        public int ChunkSize;
        public int SoundVolume;
        public int MusicVolume;
        public bool bSoundOn;
        public bool bMusicOn;
        
        // Assets
        public string RezArchivePath;
        public string ZipArchivePath;
        public string SavesFilePath;
        public int ResourceCacheSize;
    }

    public partial class ClawOptions : Form
    {
        private ClawConfig m_ClawConfig;

        public ClawOptions()
        {
            InitializeComponent();

            CenterToScreen();

            String configPath = GetClawConfigPath();
            m_ClawConfig = ParseClawConfig(configPath);

            SetupControlsFromConfig(m_ClawConfig);
        }

        private String GetClawConfigPath()
        {
            String[] possibleClawConfigPaths = 
            {
                "/usr/share/captainclaw/config.xml",
                System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "/config.xml"
            };

            String clawConfigPath = "";
            foreach (string possibleConfig in possibleClawConfigPaths)
            {
                if (System.IO.File.Exists(possibleConfig))
                {
                    clawConfigPath = possibleConfig;
                    break;
                }
            }

            if (String.IsNullOrEmpty(clawConfigPath))
            {
                String searchedPaths = "";
                foreach (string possibleConfig in possibleClawConfigPaths)
                {
                    searchedPaths += possibleConfig + "\n";
                }

                MessageBox.Show("Could not locate CaptainClaw config.xml on your system.\nSearched paths:\n\n" + searchedPaths, "CaptainClaw bin not found", MessageBoxButtons.OK, MessageBoxIcon.Error);

                this.Close();
                Application.Exit();
            }

            return clawConfigPath;
        }

        private void SaveOptionsToConfig()
        {

        }

        private ClawConfig ParseClawConfig(String configPath)
        {
            ClawConfig config = new ClawConfig();

            XmlDocument configDoc = new XmlDocument();
            configDoc.Load(configPath);

            XmlElement root = configDoc.DocumentElement;

            // ============= Video ==============
            XmlNode videoRoot = root.SelectSingleNode("Display");

            config.Resolution.X = int.Parse(videoRoot.SelectSingleNode("Size").Attributes["width"].Value);
            config.Resolution.Y = int.Parse(videoRoot.SelectSingleNode("Size").Attributes["height"].Value);
            config.Scale = float.Parse(videoRoot.SelectSingleNode("Scale").InnerText);
            config.bVerticalSync = bool.Parse(videoRoot.SelectSingleNode("UseVerticalSync").InnerText);
            config.bFullscreen = bool.Parse(videoRoot.SelectSingleNode("IsFullscreen").InnerText);
            config.bFullscreenDesktop = bool.Parse(videoRoot.SelectSingleNode("IsFullscreenDesktop").InnerText);

            // ============= Audio ==============
            XmlNode audioRoot = root.SelectSingleNode("Audio");

            config.Frequency = int.Parse(audioRoot.SelectSingleNode("Frequency").InnerText);
            config.SoundChannels = int.Parse(audioRoot.SelectSingleNode("SoundChannels").InnerText);
            config.MixingChannels = int.Parse(audioRoot.SelectSingleNode("MixingChannels").InnerText);
            config.SoundVolume = int.Parse(audioRoot.SelectSingleNode("SoundVolume").InnerText);
            config.MusicVolume = int.Parse(audioRoot.SelectSingleNode("MusicVolume").InnerText);
            config.bSoundOn = bool.Parse(audioRoot.SelectSingleNode("SoundOn").InnerText);
            config.bMusicOn = bool.Parse(audioRoot.SelectSingleNode("MusicOn").InnerText);

            // ============= Assets ==============
            XmlNode assetsRoot = root.SelectSingleNode("Assets");

            config.RezArchivePath = assetsRoot.SelectSingleNode("RezArchive").InnerText;
            config.ZipArchivePath = assetsRoot.SelectSingleNode("CustomArchive").InnerText;
            config.SavesFilePath = assetsRoot.SelectSingleNode("SavesFile").InnerText;
            config.ResourceCacheSize = int.Parse(assetsRoot.SelectSingleNode("ResourceCacheSize").InnerText);

            // ============= Console ==============
            XmlNode consoleRoot = root.SelectSingleNode("Console");

            // ============= Gameplay ==============
            XmlNode gameplayRoot = root.SelectSingleNode("Gameplay");

            return config;
        }

        void SetupControlsFromConfig(ClawConfig config)
        {
            // ============= Video ==============

            // Video Resolution
            Video_ResolutionComboBox.SelectedItem =
                Video_ResolutionComboBox.Items[Video_ResolutionComboBox.Items.Count - 1];
            string resolutionCmpText = config.Resolution.X + "x" + config.Resolution.Y;
            foreach (var item in Video_ResolutionComboBox.Items)
            {
                string cbItemStr = item.ToString();
                if (cbItemStr.Contains(resolutionCmpText))
                {
                    Video_ResolutionComboBox.SelectedItem = item;
                    break;
                }
            }

            // Video Scale
            Video_ScaleTextBox.Text = config.Scale.ToString();

            // Video Vertical Sync
            if (config.bVerticalSync)
            {
                Video_VerticalSyncCheckBox.Checked = true;
            }
            else
            {
                Video_VerticalSyncCheckBox.Checked = false;
            }

            // Video Display Mode
            if (!config.bFullscreen && !config.bFullscreenDesktop)
            {
                // Windowed
                Video_DisplayModeComboBox.SelectedItem =
                    Video_DisplayModeComboBox.Items[0];
            }
            else if (config.bFullscreen)
            {
                // Fullscreen Desktop
                Video_DisplayModeComboBox.SelectedItem =
                    Video_DisplayModeComboBox.Items[2];
            }
            else
            {
                // Fullscreen 
                Video_DisplayModeComboBox.SelectedItem =
                    Video_DisplayModeComboBox.Items[1];
            }
        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void OkButton_Click(object sender, EventArgs e)
        {
            SaveOptionsToConfig();
            this.Close();
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
