/* synth.c - Sprint 1: winmm 正弦波合成器
 * 编译: gcc synth.c -o synth.exe -lwinmm
 * 运行: .\synth.exe
 * 说明: 打开后持续发声 440Hz,回车退出。先听个响,后面再加包络和波形。
 */
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>

#define SAMPLE_RATE 44100
#define BLOCK       1024
#define NBUFS       4
#define PI2         6.283185307179586
#define NUM_KEYS    10
#define MAX_VOICES  16

/* ---------- 包络参数 (毫秒) ---------- */
#define ATTACK_MS   10.0
#define DECAY_MS    120.0
#define RELEASE_MS  200.0
#define SUSTAIN_LVL 0.55

/* ---------- 波形 ---------- */
#define WAVE_SINE   0
#define WAVE_SQUARE 1
static int g_wavetype = WAVE_SINE;

static HWAVEOUT g_wo = NULL;
static short    g_buf[NBUFS][BLOCK * 2];
static WAVEHDR  g_hdr[NBUFS];
static volatile int g_next = 0;
static volatile int g_stop = 0;

/* ---------- 键盘映射 ---------- */
static const int g_keycodes[NUM_KEYS] = {
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', VK_OEM_1
};
static const int g_keymidi[NUM_KEYS] = {
    60, 62, 64, 65, 67, 69, 71, 72, 74, 76
};
/* 记录每个琴键当前是否处于“按下->响”的状态 */
static int g_keyheld[NUM_KEYS] = {0};

/* ---------- 声部 ---------- */
typedef struct {
    int    active;
    int    owner;       /* 属于哪个按键索引,-1 表示无 */
    double phase;
    double inc;
    double env;
    int    stage;
    double t;
    double vel;
} Voice;

static Voice g_voices[MAX_VOICES];

static double note_freq(int midi)
{
    return 440.0 * pow(2.0, (midi - 69) / 12.0);
}

/* 找空闲声部并触发 */
static int voice_alloc(int owner, int midi)
{
    int i;
    for (i = 0; i < MAX_VOICES; i++) {
        if (!g_voices[i].active) break;
    }
    if (i == MAX_VOICES) return -1;     /* 满了 */
    {
        Voice *v = &g_voices[i];
        v->active = 1;
        v->owner  = owner;
        v->phase  = 0.0;
        v->inc    = note_freq(midi) / SAMPLE_RATE;
        v->env    = 0.0;
        v->stage  = 0;
        v->t      = 0.0;
        v->vel    = 0.75;
    }
    return i;
}

/* 让属于某个按键的声部进入 release */
static void voice_release_owner(int owner)
{
    int i;
    for (i = 0; i < MAX_VOICES; i++) {
        if (g_voices[i].active && g_voices[i].owner == owner &&
            g_voices[i].stage < 3) {
            g_voices[i].stage = 3;
            g_voices[i].t = 0.0;
        }
    }
}

static double voice_tick(Voice *v)
{
    double s;
    if (!v->active) return 0.0;

    if (g_wavetype == WAVE_SINE)
        s = sin(v->phase * PI2);
    else
        s = (v->phase < 0.5) ? 1.0 : -1.0;

    v->t += 1.0 / SAMPLE_RATE;
    switch (v->stage) {
    case 0:
        v->env = v->t / (ATTACK_MS / 1000.0);
        if (v->env >= 1.0) { v->env = 1.0; v->stage = 1; v->t = 0.0; }
        break;
    case 1:
    {
        double k = v->t / (DECAY_MS / 1000.0);
        if (k >= 1.0) { v->env = SUSTAIN_LVL; v->stage = 2; }
        else v->env = 1.0 - (1.0 - SUSTAIN_LVL) * k;
        break;
    }
    case 2:
        v->env = SUSTAIN_LVL;
        break;
    case 3:
    {
        double k = v->t / (RELEASE_MS / 1000.0);
        if (k >= 1.0) { v->active = 0; v->env = 0.0; }
        else v->env = SUSTAIN_LVL * (1.0 - k);
        break;
    }
    default:
        v->env = 0.0;
    }

    v->phase += v->inc;
    if (v->phase >= 1.0) v->phase -= 1.0;

    return s * v->env * v->vel;
}

static void fill(int idx)
{
    short *b = g_buf[idx];
    int i, k;
    for (i = 0; i < BLOCK; i++) {
        double mix = 0.0;
        for (k = 0; k < MAX_VOICES; k++)
            mix += voice_tick(&g_voices[k]);

        /* 软限幅:比直接截断更自然,和弦叠加时不刺耳 */
        mix *= 0.25;
        if (mix >  1.0) mix =  1.0;
        if (mix < -1.0) mix = -1.0;

        {
            short v = (short)(mix * 16000.0);
            b[i*2+0] = v;
            b[i*2+1] = v;
        }
    }
}

static void CALLBACK wave_cb(HWAVEOUT hwo, UINT msg, DWORD_PTR inst,
                             DWORD_PTR p1, DWORD_PTR p2)
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

/* 扫描键盘,按状态触发/释放音符 */
static void scan_keys(void)
{
    int i;
    for (i = 0; i < NUM_KEYS; i++) {
        int down = (GetAsyncKeyState(g_keycodes[i]) & 0x8000) != 0;
        if (down && !g_keyheld[i]) {
            g_keyheld[i] = 1;
            voice_alloc(i, g_keymidi[i]);        /* 新按键 -> 触发新声部 */
        } else if (!down && g_keyheld[i]) {
            g_keyheld[i] = 0;
            voice_release_owner(i);              /* 松键 -> 释放它自己的声部 */
        }
    }
}

int main(void)
{
    WAVEFORMATEX wf;
    int i;
    int last_wave_toggle = 0;

    ZeroMemory(&wf, sizeof(wf));
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = 2;
    wf.nSamplesPerSec  = SAMPLE_RATE;
    wf.wBitsPerSample  = 16;
    wf.nBlockAlign     = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

    if (waveOutOpen(&g_wo, WAVE_MAPPER, &wf,
                    (DWORD_PTR)wave_cb, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        printf("failed to open audio device\n");
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

    printf("=== Mini Synth Sprint 2.1 和弦版 ===\n");
    printf("hold: a s d f g h j k l ;   (C D E F G A B C D E)\n");
    printf("try:  a+d+g  (C major chord) or a+d+f\n");
    printf("w = waveform   q = quit\n");
    fflush(stdout);

    while (!g_stop) {
        scan_keys();

        /* 'w' 切换波形:用 GetAsyncKeyState 做边沿检测,避免重复触发 */
        if (GetAsyncKeyState('W') & 0x8000) {
            if (!last_wave_toggle) {
                last_wave_toggle = 1;
                g_wavetype = !g_wavetype;
                printf("waveform: %s\n", g_wavetype == WAVE_SINE ? "sine" : "square");
                fflush(stdout);
            }
        } else {
            last_wave_toggle = 0;
        }

        if (GetAsyncKeyState('Q') & 0x8000) {
            g_stop = 1;
            break;
        }

        Sleep(5);
    }

    printf("bye.\n");

    /* 等尾音放完再收设备 */
    Sleep(250);
    waveOutReset(g_wo);
    for (i = 0; i < NBUFS; i++)
        waveOutUnprepareHeader(g_wo, &g_hdr[i], sizeof(WAVEHDR));
    waveOutClose(g_wo);
    return 0;
}

