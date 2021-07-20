using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;

using iTunesLib;

namespace itunesUtil
{
    public delegate void iTunesAction();

    public partial class ITU : Form
    {
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        private static extern bool RegisterHotKey(IntPtr hWnd, int id, int fsModifiers, int vk);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        private static extern bool UnregisterHotKey(IntPtr hWnd, int id);

        private iTunesApp itunes;

        private List<HotKey> hotkeys;
        private iTunesAction[] actionTable;

        private Keys currentHotkey;

        private bool listening = false;
        private int globid = 0;

        private const string keyFileName = "keydata.bin";

        public ITU()
        {
            InitializeComponent();
            hotkeyView.View = View.Details;
            actionBox.SelectedIndex = 0;
            keymodBox.SelectedIndex = 0;

            if (File.Exists(keyFileName))
            {
                hotkeys = Serializator.Deserialize<List<HotKey>>(keyFileName);
                foreach (HotKey hotkey in hotkeys)
                {
                    hotkey.id = globid++;
                    RegisterHotKey(Handle, hotkey.id, hotkey.mod, hotkey.key.GetHashCode());
                    addToView(hotkey);
                }                   
            }
            else
            {
                hotkeys = new List<HotKey>();
            }

            actionTable = new iTunesAction[4];
            actionTable[0] = itunes.NextTrack;
            actionTable[1] = itunes.PreviousTrack;
            actionTable[2] = itunes.Pause;
            actionTable[3] = itunes.Play;
        }

        private void addToView(HotKey hotkey)
        {
            string[] viewRow = new string[3];

            switch (hotkey.action)
            {
                case 0:  viewRow[0] = "Next Track"; break;
                case 1:  viewRow[0] = "Previous Track"; break;
                case 2:  viewRow[0] = "Pause"; break;
                case 3:  viewRow[0] = "Play"; break;
                default: viewRow[0] = "NULL"; break; // just to signal, shouldn't happen

            }
            switch (hotkey.mod)
            {
                case 0x1: viewRow[1] = "ALT"; break;
                case 0x2: viewRow[1] = "CTRL"; break;
                case 0x4: viewRow[1] = "SHIFT"; break;
                case 0x8: viewRow[1] = "WIN"; break;
                default:  viewRow[1] = "NULL"; break; // just to signal, shouldn't happen
            }
            viewRow[2] = hotkey.key.ToString();

            hotkeyView.Items.Add(new ListViewItem(viewRow));
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            if (m.Msg == 0x0312)
            {
                int id = m.WParam.ToInt32();
                foreach (HotKey hotkey in hotkeys)
                {
                    if (hotkey.id == id)
                    {
                        actionTable[hotkey.action]();
                        break;
                    }
                }
            }
        }

        private void itu_Closed(object sender, FormClosedEventArgs e)
        {
            foreach (HotKey hotkey in hotkeys)
                UnregisterHotKey(Handle, hotkey.id);
            Serializator.Serialize(keyFileName, hotkeys);
        }

        private void itu_Resize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Hide();
                notifyIcon.Visible = true;
            }
        }

        private void itu_KeyDown(object sender, KeyEventArgs e)
        {
            if (listening && e.KeyCode != Keys.Alt &&
                             e.KeyCode != Keys.Shift &&
                             e.KeyCode != Keys.Control &&
                             e.KeyCode != Keys.LWin &&
                             e.KeyCode != Keys.RWin)
            {
                currentHotkey = e.KeyCode;
                listening = false;
                listenBtn.Text = currentHotkey.ToString();
            }
        }

        private void notifyIcon_DoubleClick(object sender, MouseEventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
            notifyIcon.Visible = false;
        }

        private void listenBtn_Click(object sender, EventArgs e)
        {
            if (!listening)
            {
                currentHotkey = Keys.None;
                listening = true;
                listenBtn.Text = "Listening...";
            }
            else
            {
                listening = false;
                listenBtn.Text = "Listen for key";
            }
        }

        private void addBtn_Click(object sender, EventArgs e)
        {
            if (currentHotkey != Keys.None)
            {
                HotKey hotkey = new HotKey();
                hotkey.mod = 1 << keymodBox.SelectedIndex;
                hotkey.key = currentHotkey;
                hotkey.id = globid++;
                hotkey.action = actionBox.SelectedIndex;

                hotkeys.Add(hotkey);
                RegisterHotKey(this.Handle, hotkey.id, hotkey.mod, hotkey.key.GetHashCode());

                currentHotkey = Keys.None;

                addToView(hotkey);

                listenBtn.Text = "Listen for key";
            }
        }

        private void view_ColWidthChange(object sender, ColumnWidthChangingEventArgs e)
        {
            // Disable column resize
            // Still messy though
            e.NewWidth = hotkeyView.Columns[e.ColumnIndex].Width;
            e.Cancel = true;
        }

        private void view_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right &&
                hotkeyView.FocusedItem.Bounds.Contains(e.Location))
            {
                itemContext.Show(Cursor.Position);
            }
        }

        private void stripDelete_Click(object sender, EventArgs e)
        {
            // I have no idea how those containers behave in C#
            // Don't care - just do a dumb reassign and sort so that it works at least
            var viewInds = hotkeyView.SelectedIndices;
            int[] inds = new int[viewInds.Count];
            for (int i = 0; i < inds.Length; i++)
            {
                inds[i] = viewInds[i];
            }
            Array.Sort(inds);
            Array.Reverse(inds);

            // Remove hotkeys from view, list + unregister ids
            for (int i = 0; i < inds.Length; i++)
            {
                UnregisterHotKey(Handle, hotkeys[inds[i]].id);
                hotkeys.RemoveAt(inds[i]);

                hotkeyView.Items.RemoveAt(inds[i]);
            }
        }
    }
}