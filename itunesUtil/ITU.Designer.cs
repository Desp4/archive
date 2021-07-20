namespace itunesUtil
{
    partial class ITU
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ITU));
            this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.hotkeyView = new System.Windows.Forms.ListView();
            this.Action = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.ModKey = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Key = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.actionBox = new System.Windows.Forms.ComboBox();
            this.listenBtn = new System.Windows.Forms.Button();
            this.addBtn = new System.Windows.Forms.Button();
            this.keymodBox = new System.Windows.Forms.ComboBox();
            this.itemContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemContext.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon
            // 
            this.notifyIcon.BalloonTipIcon = System.Windows.Forms.ToolTipIcon.Info;
            this.notifyIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon.Icon")));
            this.notifyIcon.Text = "notifyIcon";
            this.notifyIcon.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon_DoubleClick);
            // 
            // hotkeyView
            // 
            this.hotkeyView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Action,
            this.ModKey,
            this.Key});
            this.hotkeyView.FullRowSelect = true;
            this.hotkeyView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.hotkeyView.HideSelection = false;
            this.hotkeyView.LabelWrap = false;
            this.hotkeyView.Location = new System.Drawing.Point(12, 51);
            this.hotkeyView.Name = "hotkeyView";
            this.hotkeyView.Size = new System.Drawing.Size(260, 120);
            this.hotkeyView.TabIndex = 0;
            this.hotkeyView.UseCompatibleStateImageBehavior = false;
            this.hotkeyView.ColumnWidthChanging += new System.Windows.Forms.ColumnWidthChangingEventHandler(this.view_ColWidthChange);
            this.hotkeyView.MouseClick += new System.Windows.Forms.MouseEventHandler(this.view_MouseClick);
            // 
            // Action
            // 
            this.Action.Tag = "";
            this.Action.Text = "Action";
            this.Action.Width = 120;
            // 
            // ModKey
            // 
            this.ModKey.Text = "Modifier";
            this.ModKey.Width = 70;
            // 
            // Key
            // 
            this.Key.Text = "Key";
            this.Key.Width = 70;
            // 
            // actionBox
            // 
            this.actionBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.actionBox.FormattingEnabled = true;
            this.actionBox.Items.AddRange(new object[] {
            "Next track",
            "Previous track",
            "Pause",
            "Play"});
            this.actionBox.Location = new System.Drawing.Point(13, 12);
            this.actionBox.Name = "actionBox";
            this.actionBox.Size = new System.Drawing.Size(121, 21);
            this.actionBox.TabIndex = 1;
            // 
            // listenBtn
            // 
            this.listenBtn.Location = new System.Drawing.Point(268, 12);
            this.listenBtn.Name = "listenBtn";
            this.listenBtn.Size = new System.Drawing.Size(79, 23);
            this.listenBtn.TabIndex = 3;
            this.listenBtn.Text = "Listen for key";
            this.listenBtn.UseVisualStyleBackColor = true;
            this.listenBtn.Click += new System.EventHandler(this.listenBtn_Click);
            // 
            // addBtn
            // 
            this.addBtn.Location = new System.Drawing.Point(353, 12);
            this.addBtn.Name = "addBtn";
            this.addBtn.Size = new System.Drawing.Size(75, 23);
            this.addBtn.TabIndex = 4;
            this.addBtn.Text = "Add hotkey";
            this.addBtn.UseVisualStyleBackColor = true;
            this.addBtn.Click += new System.EventHandler(this.addBtn_Click);
            // 
            // keymodBox
            // 
            this.keymodBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.keymodBox.FormattingEnabled = true;
            this.keymodBox.Items.AddRange(new object[] {
            "Alt",
            "Ctrl",
            "Shift",
            "Win"});
            this.keymodBox.Location = new System.Drawing.Point(140, 12);
            this.keymodBox.Name = "keymodBox";
            this.keymodBox.Size = new System.Drawing.Size(121, 21);
            this.keymodBox.TabIndex = 2;
            // 
            // itemContext
            // 
            this.itemContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteToolStripMenuItem});
            this.itemContext.Name = "itemContext";
            this.itemContext.Size = new System.Drawing.Size(108, 26);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.stripDelete_Click);
            // 
            // ITU
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.addBtn);
            this.Controls.Add(this.listenBtn);
            this.Controls.Add(this.keymodBox);
            this.Controls.Add(this.actionBox);
            this.Controls.Add(this.hotkeyView);
            this.KeyPreview = true;
            this.Name = "ITU";
            this.Text = "ITU";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.itu_Closed);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.itu_KeyDown);
            this.Resize += new System.EventHandler(this.itu_Resize);
            this.itemContext.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon;
        private System.Windows.Forms.ListView hotkeyView;
        private System.Windows.Forms.ComboBox actionBox;
        private System.Windows.Forms.Button listenBtn;
        private System.Windows.Forms.Button addBtn;
        private System.Windows.Forms.ColumnHeader Action;
        private System.Windows.Forms.ColumnHeader ModKey;
        private System.Windows.Forms.ColumnHeader Key;
        private System.Windows.Forms.ComboBox keymodBox;
        private System.Windows.Forms.ContextMenuStrip itemContext;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
    }
}

