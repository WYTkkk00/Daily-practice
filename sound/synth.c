/* synth.c - Sprint 1: winmm 正弦波合成器
 * 编译: gcc synth.c -o synth.exe -lwinmm
 * 运行: .\synth.exe
 * 说明: 打开后持续发声 440Hz,回车退出。先听个响,后面再加包络和波形。
 */
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define BLOCK       1024          /* 每块帧数 */
#define NBUFS       4             /* 缓冲块数 */
#define PI2         6.283185307179586

static HWAVEOUT  g_wo = NULL;
static short     g_buf[NBUFS][BLOCK * 2];   /* 立体声,每帧2个short */
static WAVEHDR   g_hdr[NBUFS];
static volatile int g_next = 0;             /* 下一块要填的索引 */
static volatile int g_stop = 0;

static double g_phase = 0.0;
static double g_inc   = 0.0;

static void fill(int idx)
{
    short *b = g_buf[idx];
    int i;
    for (i = 0; i < BLOCK; i++) {
        short v = (short)(sin(g_phase * PI2) * 12000.0);
        b[i*2+0] = v;
        b[i*2+1] = v;
        g_phase += g_inc;
        if (g_phase >= 1.0) g_phase -= 1.0;
    }
}

/* 系统回调:一块播完了,填下一块继续送 */
static void CALLBACK wave_cb(HWAVEOUT hwo, UINT msg, DWORD_PTR inst,
                             DWORD_PTR p1, DWORD_PTR p2)
{
    (void)hwo; (void)inst; (void)p1; (void)p2;
    if (msg != WOM_DONE) return;
    if (g_stop) return;

    {
        int idx = g_next;
        g_next = (g_next + 1) % NBUFS;
        fill(idx);
        waveOutWrite(g_wo, &g_hdr[idx], sizeof(WAVEHDR));
    }
}

int main(void)
{
    WAVEFORMATEX wf;
    int i;
    DWORD t0;

    ZeroMemory(&wf, sizeof(wf));
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = 2;
    wf.nSamplesPerSec  = SAMPLE_RATE;
    wf.wBitsPerSample  = 16;
    wf.nBlockAlign     = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    wf.cbSize          = 0;

    g_inc = 440.0 / SAMPLE_RATE;

    if (waveOutOpen(&g_wo, WAVE_MAPPER, &wf,
                    (DWORD_PTR)wave_cb, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        printf("failed to open audio device\n");
        return 1;
    }

    /* 先把所有块都填好并送入队列 */
    for (i = 0; i < NBUFS; i++) {
        ZeroMemory(&g_hdr[i], sizeof(WAVEHDR));
        g_hdr[i].lpData         = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = BLOCK * 2 * sizeof(short);
        waveOutPrepareHeader(g_wo, &g_hdr[i], sizeof(WAVEHDR));
        fill(i);
        waveOutWrite(g_wo, &g_hdr[i], sizeof(WAVEHDR));
    }

    printf("playing 440Hz for 10 seconds...\n");
    fflush(stdout);

    /* 稳稳播 10 秒,期间什么都不做,让回调去喂数据 */
    t0 = GetTickCount();
    while (GetTickCount() - t0 < 10000) {
        Sleep(50);
    }

    printf("done.\n");
    g_stop = 1;
    waveOutReset(g_wo);
    for (i = 0; i < NBUFS; i++)
        waveOutUnprepareHeader(g_wo, &g_hdr[i], sizeof(WAVEHDR));
    waveOutClose(g_wo);
    return 0;
}

