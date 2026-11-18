#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SAMPLE_RATE  44100
#define BLOCK        1024
#define NBUFS        4
#define PI2          6.283185307179586
#define MAX_VOICES   16

#define ATTACK_MS    10.0
#define DECAY_MS     120.0
#define RELEASE_MS   200.0
#define SUSTAIN_LVL  0.55

#define CUTOFF_MIN   200.0
#define CUTOFF_MAX   8000.0

#define WIN_W        800
#define WIN_H        420
#define WAVE_H       (WIN_H - 90)
#define RING_SIZE    8192          /* 环形缓冲,存的是一段连续采样 */

/* ---------------- 全局 ---------------- */
static int    g_wavetype = 0;
static double g_cutoff   = 3000.0;

static HWAVEOUT g_wo = NULL;
static short g_buf[NBUFS][BLOCK * 2];
static WAVEHDR g_hdr[NBUFS];
static volatile int g_next = 0;
static volatile int g_stop = 0;

/* 环形缓冲:音频线程写,UI 读 */
static short  g_ring[RING_SIZE];
static volatile int g_ring_w = 0;   /* 写入位置 */

/* 窗口 */
static HWND g_hwnd = NULL;

/* ---------------- 声部 ---------------- */
typedef struct {
    int active, owner;
    double phase, inc, env, t, vel;
    int stage;
} Voice;
static Voice g_voices[MAX_VOICES];

typedef struct { double low, band; } Filter;
static Filter g_filter = {0,0};

static double note_freq(int midi) { return 440.0 * pow(2.0, (midi-69)/12.0); }

static int voice_alloc(int owner, int midi)
{
    int i;
    for (i = 0; i < MAX_VOICES; i++) if (!g_voices[i].active) break;
    if (i == MAX_VOICES) return -1;
    {
        Voice *v = &g_voices[i];
        v->active = 1; v->owner = owner; v->phase = 0;
        v->inc = note_freq(midi) / SAMPLE_RATE;
        v->env = 0; v->stage = 0; v->t = 0; v->vel = 0.75;
    }
    return i;
}

static void voice_release_owner(int owner)
{
    int i;
    for (i = 0; i < MAX_VOICES; i++)
        if (g_voices[i].active && g_voices[i].owner == owner && g_voices[i].stage < 3) {
            g_voices[i].stage = 3;
            g_voices[i].t = 0;
        }
}

static double voice_tick(Voice *v)
{
    double s;
    if (!v->active) return 0.0;
    s = (g_wavetype == 0) ? sin(v->phase * PI2) : ((v->phase < 0.5) ? 1.0 : -1.0);

    v->t += 1.0 / SAMPLE_RATE;
    switch (v->stage) {
    case 0:
        v->env = v->t / (ATTACK_MS / 1000.0);
        if (v->env >= 1.0) { v->env = 1.0; v->stage = 1; v->t = 0; }
        break;
    case 1: {
        double k = v->t / (DECAY_MS / 1000.0);
        if (k >= 1.0) { v->env = SUSTAIN_LVL; v->stage = 2; }
        else v->env = 1.0 - (1.0 - SUSTAIN_LVL) * k;
        break;
    }
    case 2: v->env = SUSTAIN_LVL; break;
    case 3: {
        double k = v->t / (RELEASE_MS / 1000.0);
        if (k >= 1.0) { v->active = 0; v->env = 0; }
        else v->env = SUSTAIN_LVL * (1.0 - k);
        break;
    }
    default: v->env = 0;
    }
    v->phase += v->inc;
    if (v->phase >= 1.0) v->phase -= 1.0;
    return s * v->env * v->vel;
}

static double process_filter(double x)
{
    double f = 2.0 * sin(PI2 * (g_cutoff / SAMPLE_RATE) / 2.0);
    double q = 0.8, high, band, low;
    if (f > 0.99) f = 0.99;
    high = x - g_filter.low - q * g_filter.band;
    band = f * high + g_filter.band;
    low  = f * band + g_filter.low;
    g_filter.low = low; g_filter.band = band;
    return low;
}

static void fill(int idx)
{
    short *b = g_buf[idx];
    int i, k;
    for (i = 0; i < BLOCK; i++) {
        double mix = 0.0;
        for (k = 0; k < MAX_VOICES; k++) mix += voice_tick(&g_voices[k]);
        mix = process_filter(mix);
        mix *= 0.25;
        if (mix >  1.0) mix =  1.0;
        if (mix < -1.0) mix = -1.0;

        b[i*2+0] = (short)(mix * 16000.0);
        b[i*2+1] = (short)(mix * 16000.0);

        /* 写环形缓冲(单声道) */
        {
            int p = g_ring_w;
            g_ring[p] = (short)(mix * 32767.0);
            g_ring_w = (p + 1) % RING_SIZE;
        }
    }
}

static void CALLBACK wave_cb(HWAVEOUT hwo, UINT msg, DWORD_PTR inst, DWORD_PTR p1, DWORD_PTR p2)
{
    (void)hwo; (void)inst; (void)p1; (void)p2;
    if (msg != WOM_DONE || g_stop) return;
    {
        int idx = g_next;
        g_next = (g_next + 1) % NBUFS;
        fill(idx);
        waveOutWrite(g_wo, &g_hdr[idx], sizeof(WAVEHDR));
    }
}

/* ---------------- UI 绘制 ---------------- */
static void draw_wave(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    RECT rc;
    HBRUSH bg;
    HPEN pen, midpen;
    int i, mid;

    GetClientRect(hwnd, &rc);
    mid = WAVE_H / 2;

    /* 背景 */
    bg = CreateSolidBrush(RGB(16, 18, 24));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    /* 波形 */
    pen = CreatePen(PS_SOLID, 2, RGB(80, 235, 160));
    SelectObject(hdc, pen);

    {
        int w = rc.right;
        int step = RING_SIZE / w;    /* 每个像素取一个样本 */
        if (step < 1) step = 1;

        MoveToEx(hdc, 0, mid, NULL);
        for (i = 0; i < w; i++) {
            int p, y;
            int idx = g_ring_w - (w - i) * step;
            while (idx < 0) idx += RING_SIZE;
            idx %= RING_SIZE;
            p = g_ring[idx];
            y = mid - (int)((double)p / 32768.0 * (WAVE_H / 2 - 8));
            LineTo(hdc, i, y);
        }
    }
    DeleteObject(pen);

    /* 中线 */
    midpen = CreatePen(PS_SOLID, 1, RGB(50, 60, 80));
    SelectObject(hdc, midpen);
    MoveToEx(hdc, 0, mid, NULL);
    LineTo(hdc, rc.right, mid);
    DeleteObject(midpen);

    /* 底部 HUD */
    {
        char buf[128];
        HFONT font = CreateFontA(18, 0, 0, 0, FW_BOLD, 0, 0, 0,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 FF_DONTCARE, "Consolas");
        HGDIOBJ old = SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);

        SetTextColor(hdc, RGB(120, 200, 255));
                wsprintfA(buf, "waveform: %s   cutoff: %d Hz",
                  g_wavetype == 0 ? "SINE" : "SQUARE", (int)g_cutoff);

        TextOutA(hdc, 15, WIN_H - 78, buf, lstrlenA(buf));

        SetTextColor(hdc, RGB(120, 255, 160));
        TextOutA(hdc, 15, WIN_H - 54, "keys: a s d f g h j k l ;  |  w wave  c/v cutoff  q quit",
                 lstrlenA("keys: a s d f g h j k l ;  |  w wave  c/v cutoff  q quit"));

        SelectObject(hdc, old);
        DeleteObject(font);
    }

    ReleaseDC(hwnd, hdc);
}

/* ---------------- 键盘映射 ---------------- */
static int key_to_owner(WPARAM vk)
{
    switch (vk) {
    case 'A': return 0;
    case 'S': return 1;
    case 'D': return 2;
    case 'F': return 3;
    case 'G': return 4;
    case 'H': return 5;
    case 'J': return 6;
    case 'K': return 7;
    case 'L': return 8;
    case VK_OEM_1: return 9;
    }
    return -1;
}
static int midi_of_owner[] = {60,62,64,65,67,69,71,72,74,76};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static int held[10] = {0};

    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, 1, 30, NULL);
        return 0;

    case WM_TIMER:
        draw_wave(hwnd);
        return 0;

    case WM_KEYDOWN:
        if (wp == 'Q' && (lp & (1<<30)) == 0) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        if (wp == 'W' && (lp & (1<<30)) == 0) {
            g_wavetype = !g_wavetype;
            draw_wave(hwnd);
            return 0;
        }
        if (wp == 'C' && (lp & (1<<30)) == 0) {
            g_cutoff *= 1.25;
            if (g_cutoff > CUTOFF_MAX) g_cutoff = CUTOFF_MAX;
            draw_wave(hwnd);
            return 0;
        }
        if (wp == 'V' && (lp & (1<<30)) == 0) {
            g_cutoff /= 1.25;
            if (g_cutoff < CUTOFF_MIN) g_cutoff = CUTOFF_MIN;
            draw_wave(hwnd);
            return 0;
        }
        {
            int owner = key_to_owner(wp);
            if (owner >= 0 && !held[owner] && (lp & (1<<30)) == 0) {
                held[owner] = 1;
                voice_alloc(owner, midi_of_owner[owner]);
            }
        }
        return 0;

    case WM_KEYUP:
        {
            int owner = key_to_owner(wp);
            if (owner >= 0 && held[owner]) {
                held[owner] = 0;
                voice_release_owner(owner);
            }
        }
        return 0;

    case WM_CLOSE:
        g_stop = 1;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

/* ---------------- 入口 ---------------- */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShow)
{
    WNDCLASSA wc;
    MSG msg;
    WAVEFORMATEX wf;
    int i;
    RECT rc = {0, 0, WIN_W, WIN_H};
    (void)hPrev; (void)lpCmdLine;

    /* 1. 注册窗口 */
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(16, 18, 24));
    wc.lpszClassName = "MiniSynthClass";
    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "RegisterClass failed", "Error", MB_OK);
        return 1;
    }

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hwnd = CreateWindowA("MiniSynthClass", "Mini Synth - Oscilloscope",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top,
                          NULL, NULL, hInst, NULL);
    if (!g_hwnd) {
        MessageBoxA(NULL, "CreateWindow failed", "Error", MB_OK);
        return 1;
    }
    ShowWindow(g_hwnd, nShow);
    UpdateWindow(g_hwnd);

    /* 2. 打开音频 */
    ZeroMemory(&wf, sizeof(wf));
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = 2;
    wf.nSamplesPerSec  = SAMPLE_RATE;
    wf.wBitsPerSample  = 16;
    wf.nBlockAlign     = 4;
    wf.nAvgBytesPerSec = SAMPLE_RATE * 4;

    if (waveOutOpen(&g_wo, WAVE_MAPPER, &wf,
                    (DWORD_PTR)wave_cb, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        MessageBoxA(NULL, "waveOutOpen failed", "Error", MB_OK);
        return 1;
    }

    for (i = 0; i < NBUFS; i++) {
        ZeroMemory(&g_hdr[i], sizeof(WAVEHDR));
        g_hdr[i].lpData         = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = BLOCK * 2 * sizeof(short);
        waveOutPrepareHeader(g_wo, &g_hdr[i], sizeof(WAVEHDR));
        fill(i);
        waveOutWrite(g_wo, &g_hdr[i], sizeof(WAVEHDR));
    }

    /* 3. 消息循环 */
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* 4. 收尾 */
    KillTimer(g_hwnd, 1);
    Sleep(200);
    waveOutReset(g_wo);
    for (i = 0; i < NBUFS; i++) waveOutUnprepareHeader(g_wo, &g_hdr[i], sizeof(WAVEHDR));
    waveOutClose(g_wo);
    return 0;
}
