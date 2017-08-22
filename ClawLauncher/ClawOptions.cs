using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Windows;

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

            String configPath = ClawLauncherForm.ClawConfigPath;
            m_ClawConfig = ParseClawConfig(configPath);

            SetupControlsFromConfig(m_ClawConfig);
        }

        private void SaveOptionsToConfig()
        {
            // lol
            object o = null;
            EventArgs e = null;

            Video_ApplyChangesButton_Click(o, e);
            Audio_ApplyChangesButton_Click(o, e);
            Assets_ApplyChangesButton_Click(o, e);
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

            if (config.bFullscreen || config.bFullscreenDesktop)
            {
                // Custom resolution
                Video_ResolutionComboBox.SelectedItem =
                    Video_ResolutionComboBox.Items[Video_ResolutionComboBox.Items.Count - 1];

                Video_ResolutionComboBox.Enabled = false;
            }

            // ============= Audio ==============
            {
                // Frequency
                Audio_FrequencyComboBox.SelectedItem =
                    Audio_FrequencyComboBox.Items[Audio_FrequencyComboBox.Items.Count - 1];
                string cmpText = config.Frequency.ToString();
                foreach (var item in Audio_FrequencyComboBox.Items)
                {
                    string cbItemStr = item.ToString();
                    if (cbItemStr.Contains(cmpText))
                    {
                        Audio_FrequencyComboBox.SelectedItem = item;
                        break;
                    }
                }

                // 3D Sound Channles - fixed
                Audio_3DSoundChannelsComboBox.SelectedItem = Audio_3DSoundChannelsComboBox.Items[0];

                // Mixing Channels
                Audio_MixingChannelsSlider.Value = config.MixingChannels;
                Audio_MixingChannelsTextBox.Text = config.MixingChannels.ToString();

                // Sound Volume
                Audio_SoundVolumeSlider.Value = config.SoundVolume;
                Audio_SoundVolumeTextBox.Text = config.SoundVolume.ToString();

                // Music Volume
                Audio_MusicVolumeSlider.Value = config.MusicVolume;
                Audio_MusicVolumeTextBox.Text = config.MusicVolume.ToString();

                // Sound On
                Audio_SoundOnCheckBox.Checked = config.bSoundOn;

                // Music On
                Audio_MusicOnCheckBox.Checked = config.bMusicOn;
            }

            // ============= Assets ==============
            Assets_RezArchivePathTextBox.Text = config.RezArchivePath;
            Assets_ZipArchivePathTextBox.Text = config.ZipArchivePath;
            Assets_SavesFilePathTextBox.Text = config.SavesFilePath;
            Assets_ResCacheSizeTextBox.Text = config.ResourceCacheSize.ToString();
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

        private void Audio_MixingChannelsSlider_Scroll(object sender, EventArgs e)
        {
            Audio_MixingChannelsTextBox.Text = Audio_MixingChannelsSlider.Value.ToString();
        }

        private void Audio_SoundVolumeSlider_Scroll(object sender, EventArgs e)
        {
            Audio_SoundVolumeTextBox.Text = Audio_SoundVolumeSlider.Value.ToString();
        }

        private void Audio_MusicVolumeSlider_Scroll(object sender, EventArgs e)
        {
            Audio_MusicVolumeTextBox.Text = Audio_MusicVolumeSlider.Value.ToString();
        }

        private void Assets_RezArchivePathChooseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.FileName = "CLAW.REZ";
            ofd.Filter = "REZ files (*.REZ)|*.rez";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                Assets_RezArchivePathTextBox.Text = ofd.FileName;
            }
        }

        private void Assets_ZipArchivePathChooseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.FileName = "ASSETS.ZIP";
            ofd.Filter = "ZIP files (*.ZIP)|*.zip";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                Assets_ZipArchivePathTextBox.Text = ofd.FileName;
            }
        }

        private void Assets_SavesFilePathChooseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.FileName = "SAVES.XML";
            ofd.Filter = "XML files (*.XML)|*.xml";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                Assets_SavesFilePathTextBox.Text = ofd.FileName;
            }
        }

        private void Video_ApplyChangesButton_Click(object sender, EventArgs e)
        {
            XmlDocument configDoc = new XmlDocument();
            configDoc.Load(ClawLauncherForm.ClawConfigPath);

            XmlElement root = configDoc.DocumentElement;

            // ============= Video ==============
            XmlNode videoRoot = root.SelectSingleNode("Display");
            try
            {
                // Resolution
                if (!Video_ResolutionComboBox.SelectedItem.ToString().Contains("Custom"))
                {
                    string resolutionStr = Video_ResolutionComboBox.SelectedItem.ToString();
                    string widthStr = resolutionStr.Split('x')[0];
                    string heightStr = resolutionStr.Split('x').Last();
                    heightStr = heightStr.Split(' ')[0];

                    videoRoot.SelectSingleNode("Size").Attributes["width"].InnerText = widthStr;
                    videoRoot.SelectSingleNode("Size").Attributes["height"].InnerText = heightStr;
                }

                // Scale
                float dummy = 0;
                if (float.TryParse(Video_ScaleTextBox.Text, out dummy))
                {
                    videoRoot.SelectSingleNode("Scale").InnerText = Video_ScaleTextBox.Text;
                }

                // Vertical Sync
                videoRoot.SelectSingleNode("UseVerticalSync").InnerText = Video_VerticalSyncCheckBox.Checked.ToString().ToLower();

                // Display Mode
                string displayModeStr = Video_DisplayModeComboBox.SelectedItem.ToString();
                if (displayModeStr.Equals("Windowed Fullscreen"))
                {
                    videoRoot.SelectSingleNode("IsFullscreen").InnerText = "false";
                    videoRoot.SelectSingleNode("IsFullscreenDesktop").InnerText = "true";

                    videoRoot.SelectSingleNode("Size").Attributes["width"].InnerText = Screen.PrimaryScreen.Bounds.Width.ToString();
                    videoRoot.SelectSingleNode("Size").Attributes["height"].InnerText = Screen.PrimaryScreen.Bounds.Height.ToString();

                    // Custom resolution
                    Video_ResolutionComboBox.SelectedItem =
                        Video_ResolutionComboBox.Items[Video_ResolutionComboBox.Items.Count - 1];
                }
                else if (displayModeStr.Equals("Fullscreen"))
                {
                    videoRoot.SelectSingleNode("IsFullscreen").InnerText = "true";
                    videoRoot.SelectSingleNode("IsFullscreenDesktop").InnerText = "false";

                    videoRoot.SelectSingleNode("Size").Attributes["width"].InnerText = Screen.PrimaryScreen.Bounds.Width.ToString();
                    videoRoot.SelectSingleNode("Size").Attributes["height"].InnerText = Screen.PrimaryScreen.Bounds.Height.ToString();

                    // Custom resolution
                    Video_ResolutionComboBox.SelectedItem =
                        Video_ResolutionComboBox.Items[Video_ResolutionComboBox.Items.Count - 1];
                }
                else
                {
                    videoRoot.SelectSingleNode("IsFullscreen").InnerText = "false";
                    videoRoot.SelectSingleNode("IsFullscreenDesktop").InnerText = "false";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Something went wrong: \n\n" + ex.Message + "\n\nNo changes were made in Video section !", "Error applying changes", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            // ============= Video ==============
            XmlNode audioRoot = root.SelectSingleNode("Audio");
            try
            {
                // Frequency
                string frequencyStr = Audio_FrequencyComboBox.SelectedItem.ToString();
                if (frequencyStr.Equals("11025 Hz"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "11025";
                }
                else if (frequencyStr.Equals("22050 Hz"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "22050";
                }
                if (frequencyStr.Equals("44100 Hz"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "44100";
                }

                // Mixing Channels
                audioRoot.SelectSingleNode("MixingChannels").InnerText = Audio_MixingChannelsTextBox.Text;

                // Sound Volume
                audioRoot.SelectSingleNode("SoundVolume").InnerText = Audio_SoundVolumeTextBox.Text;

                // Music Volume
                audioRoot.SelectSingleNode("MusicVolume").InnerText = Audio_MusicVolumeTextBox.Text;

                // Sound On
                audioRoot.SelectSingleNode("SoundOn").InnerText = Audio_SoundOnCheckBox.Checked.ToString().ToLower();

                // Music On
                audioRoot.SelectSingleNode("MusicOn").InnerText = Audio_MusicOnCheckBox.Checked.ToString().ToLower();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Something went wrong: \n\n" + ex.Message + "\n\nNo changes were made in Audio section !", "Error applying changes", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            

            configDoc.Save(ClawLauncherForm.ClawConfigPath);
        }

        private void Audio_ApplyChangesButton_Click(object sender, EventArgs e)
        {
            XmlDocument configDoc = new XmlDocument();
            configDoc.Load(ClawLauncherForm.ClawConfigPath);

            XmlElement root = configDoc.DocumentElement;

            // ============= Audio ==============
            XmlNode audioRoot = root.SelectSingleNode("Audio");
            try
            {
                // Frequency
                string frequencyStr = Audio_FrequencyComboBox.SelectedItem.ToString();
                if (frequencyStr.Equals("11025 Hz"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "11025";
                }
                else if (frequencyStr.Equals("22050 Hz"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "22050";
                }
                if (frequencyStr.Equals("44100 Hz (Recommended)"))
                {
                    audioRoot.SelectSingleNode("Frequency").InnerText = "44100";
                }

                // Mixing Channels
                audioRoot.SelectSingleNode("MixingChannels").InnerText = Audio_MixingChannelsTextBox.Text;

                // Sound Volume
                audioRoot.SelectSingleNode("SoundVolume").InnerText = Audio_SoundVolumeTextBox.Text;

                // Music Volume
                audioRoot.SelectSingleNode("MusicVolume").InnerText = Audio_MusicVolumeTextBox.Text;

                // Sound On
                audioRoot.SelectSingleNode("SoundOn").InnerText = Audio_SoundOnCheckBox.Checked.ToString().ToLower();

                // Music On
                audioRoot.SelectSingleNode("MusicOn").InnerText = Audio_MusicOnCheckBox.Checked.ToString().ToLower();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Something went wrong: \n\n" + ex.Message + "\n\nNo changes were made in Audio section !", "Error applying changes", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }


            configDoc.Save(ClawLauncherForm.ClawConfigPath);
        }

        private void Assets_ApplyChangesButton_Click(object sender, EventArgs e)
        {
            XmlDocument configDoc = new XmlDocument();
            configDoc.Load(ClawLauncherForm.ClawConfigPath);

            XmlElement root = configDoc.DocumentElement;

            // ============= Assets ==============
            XmlNode assetsRoot = root.SelectSingleNode("Assets");
            try
            {
                // REZ Archive
                assetsRoot.SelectSingleNode("RezArchive").InnerText = Assets_RezArchivePathTextBox.Text;

                // ZIP Archive
                assetsRoot.SelectSingleNode("CustomArchive").InnerText = Assets_ZipArchivePathTextBox.Text;

                // Saves File
                assetsRoot.SelectSingleNode("SavesFile").InnerText = Assets_SavesFilePathTextBox.Text;

                // Resource Cache Size
                assetsRoot.SelectSingleNode("ResourceCacheSize").InnerText = Assets_ResCacheSizeTextBox.Text;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Something went wrong: \n\n" + ex.Message + "\n\nNo changes were made in Assets section !", "Error applying changes", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }


            configDoc.Save(ClawLauncherForm.ClawConfigPath);
        }

        private void Video_ScaleTextBox_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void Video_DisplayModeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (Video_DisplayModeComboBox.SelectedItem.ToString().Equals("Windowed"))
            {
                Video_ResolutionComboBox.Enabled = true;
            }
            else
            {
                Video_ResolutionComboBox.Enabled = false;

                // Custom resolution
                Video_ResolutionComboBox.SelectedItem =
                    Video_ResolutionComboBox.Items[Video_ResolutionComboBox.Items.Count - 1];
            }
        }
    }
}
