using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace NSCAM
{
    public partial class RawViewer : Form
    {
        const int WIDTH = 320, HEIGHT = 256;

        Bitmap bmp = new Bitmap(WIDTH, HEIGHT);

        public RawViewer()
        {
            InitializeComponent();
        }

        private void logOpen_FileOk(object sender, CancelEventArgs e)
        {
            byte[] buffer = File.ReadAllBytes(logOpen.FileName);
            const int IDLE = 0, RUN = 1, LOAD = 2;
            int state = IDLE;
            int ptr = 0, pos = 0;
            bmp = new Bitmap(WIDTH, HEIGHT);
            for (int i = 0; i < buffer.Length; i++)
            {
                switch (state)
                {
                    case RUN:
                        state = LOAD;
                        ptr = 0;
                        i -= 1;
                        break;
                    case LOAD:
                        if (buffer[i] == 'O' && buffer[i + 1] == 'K' && buffer[i + 2] == '!')
                            state = IDLE;
                        else
                        {
                            int val = buffer[i + 1] << 8 | buffer[i];
                            i += 1;
                            val = val >> 2;
                            bmp.SetPixel(ptr, pos, Color.FromArgb(val, val, val));
                            ptr += 1;
                        }
                        break;
                    case IDLE:
                        if (buffer[i] == 'N' && buffer[i + 1] == 'S')
                        {
                            string str = "";
                            str += (char)buffer[i + 2];
                            str += (char)buffer[i + 3];
                            str += (char)buffer[i + 4];
                            if (int.TryParse(str, System.Globalization.NumberStyles.HexNumber, null, out int res))
                                pos = res;
                            i += (1 + 3);
                            state = RUN;
                        }
                        break;
                }
            }

            picBox.Image = bmp;
        }

        private void logSave_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            logOpen.ShowDialog();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            if (picBox.Image != null)
                logSave.ShowDialog();
        }
    }
}
