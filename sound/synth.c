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
#include <string.h>

#define SAMPLE_RATE  44100
#define BLOCK        1024
#define NBUFS        4
#define PI2          6.283185307179586
#define NUM_KEYS     10
#define MAX_VOICES   16

#define ATTACK_MS    10.0
#define DECAY_MS     120.0
#define RELEASE_MS   200.0
#define SUSTAIN_LVL  0.55

#define SCOPE_W      60
#define SCOPE_H      9
#define SCOPE_HOP    4

#define CUTOFF_MIN   200.0
#define CUTOFF_MAX   8000.0

static int g_wavetype = 0;
static double g_cutoff = 3000.0;

static HWAVEOUT g_wo = NULL;
static short g_buf[NBUFS][BLOCK * 2];
static WAVEHDR g_hdr[NBUFS];
static volatile int g_next = 0;
static volatile int g_stop = 0;

static short g_scope_a[SCOPE_W];
static short g_scope_b[SCOPE_W];
static volatile int g_scope_ready = 0;
static volatile double g_peak = 0.0;

static const int g_keycodes[NUM_KEYS] = {'A','S','D','F','G','H','J','K','L',VK_OEM_1};
static const int g_keymidi[NUM_KEYS]  = {60,62,64,65,67,69,71,72,74,76};
static int g_keyheld[NUM_KEYS] = {0};

typedef struct {
    int active, owner;
    double phase, inc, env, t, vel;
    int stage;
} Voice;
static Voice g_voices[MAX_VOICES];
typedef struct { double low, band; } Filter;
static Filter g_filter = {0,0};

static HANDLE g_hout = INVALID_HANDLE_VALUE;
static COORD  g_scope_pos = {0, 0};

static double note_freq(int midi) { return 440.0*pow(2.0,(midi-69)/12.0); }

static int voice_alloc(int owner, int midi)
{
    int i;
    for (i=0;i<MAX_VOICES;i++) if(!g_voices[i].active) break;
    if (i==MAX_VOICES) return -1;
    {
        Voice *v=&g_voices[i];
        v->active=1; v->owner=owner; v->phase=0; v->inc=note_freq(midi)/SAMPLE_RATE;
        v->env=0; v->stage=0; v->t=0; v->vel=0.75;
    }
    return i;
}

static void voice_release_owner(int owner)
{
    int i;
    for (i=0;i<MAX_VOICES;i++)
        if (g_voices[i].active && g_voices[i].owner==owner && g_voices[i].stage<3) {
            g_voices[i].stage=3; g_voices[i].t=0;
        }
}

static double voice_tick(Voice *v)
{
    double s;
    if(!v->active) return 0.0;
    s = (g_wavetype==0) ? sin(v->phase*PI2) : ((v->phase<0.5)?1.0:-1.0);
    v->t += 1.0/SAMPLE_RATE;
    switch(v->stage){
    case 0:
        v->env = v->t/(ATTACK_MS/1000.0);
        if(v->env>=1){v->env=1;v->stage=1;v->t=0;}
        break;
    case 1: {
        double k=v->t/(DECAY_MS/1000.0);
        if(k>=1){v->env=SUSTAIN_LVL;v->stage=2;}
        else v->env=1.0-(1.0-SUSTAIN_LVL)*k;
        break;
    }
    case 2: v->env=SUSTAIN_LVL; break;
    case 3: {
        double k=v->t/(RELEASE_MS/1000.0);
        if(k>=1){v->active=0;v->env=0;}
        else v->env=SUSTAIN_LVL*(1.0-k);
        break;
    }
    default: v->env=0;
    }
    v->phase += v->inc;
    if(v->phase>=1.0) v->phase -= 1.0;
    return s*v->env*v->vel;
}

static double process_filter(double x)
{
    double f = 2.0*sin(PI2*(g_cutoff/SAMPLE_RATE)/2.0);
    double q = 0.8, high, band, low;
    if(f>0.99) f=0.99;
    high = x - g_filter.low - q*g_filter.band;
    band = f*high + g_filter.band;
    low  = f*band + g_filter.low;
    g_filter.low=low; g_filter.band=band;
    return low;
}

static void fill(int idx)
{
    short *b = g_buf[idx];
    int i,k,ci=0,pick=SCOPE_HOP;
    double chunk[SCOPE_W];
    double pk = 0.0;

    for(i=0;i<BLOCK;i++){
        double mix=0.0;
        for(k=0;k<MAX_VOICES;k++) mix += voice_tick(&g_voices[k]);
        mix = process_filter(mix);
        mix *= 0.25;
        if(mix> 1.0) mix= 1.0;
        if(mix<-1.0) mix=-1.0;

        b[i*2+0] = (short)(mix*16000.0);
        b[i*2+1] = (short)(mix*16000.0);

        if(--pick<=0){
            if(ci<SCOPE_W){
                chunk[ci++] = mix;
                if(fabs(mix)>pk) pk = fabs(mix);
            }
            pick = SCOPE_HOP;
        }
    }

    {
        short *dst = g_scope_ready ? g_scope_b : g_scope_a;
        for(i=0;i<ci;i++) dst[i] = (short)(chunk[i] * 32767.0);
        for(;i<SCOPE_W;i++) dst[i] = 0;
        g_scope_ready = !g_scope_ready;
        g_peak = pk;
    }
}

static void CALLBACK wave_cb(HWAVEOUT hwo,UINT msg,DWORD_PTR inst,DWORD_PTR p1,DWORD_PTR p2)
{
    (void)hwo;(void)inst;(void)p1;(void)p2;
    if(msg!=WOM_DONE || g_stop) return;
    {
        int idx=g_next;
        g_next=(g_next+1)%NBUFS;
        fill(idx);
        waveOutWrite(g_wo,&g_hdr[idx],sizeof(WAVEHDR));
    }
}

static void scan_keys(void)
{
    int i;
    for(i=0;i<NUM_KEYS;i++){
        int down = (GetAsyncKeyState(g_keycodes[i]) & 0x8000) != 0;
        if(down && !g_keyheld[i]){ g_keyheld[i]=1; voice_alloc(i,g_keymidi[i]); }
        else if(!down && g_keyheld[i]){ g_keyheld[i]=0; voice_release_owner(i); }
    }
}

static void render_scope(void)
{
    char line[SCOPE_W + 4];
    short *src = g_scope_ready ? g_scope_b : g_scope_a;
    int i, j, mid = SCOPE_H / 2;
    COORD pos;

    for(i = 0; i < SCOPE_H; i++){
        memset(line, ' ', sizeof(line));
        line[0] = ' '; line[1] = ' ';
        line[SCOPE_W + 2] = '\0';

        /* 先画中线 */
        if(i == mid){
            for(j = 0; j < SCOPE_W; j++) line[2 + j] = '-';
        }

        /* 计算波形点的y坐标：从下往上数，保证正负对称 */
        for(j = 0; j < SCOPE_W; j++){
            double sample_norm = (double)src[j] / 32767.0;
            int y = mid - (int)(sample_norm * (double)mid);
            if(y < 0) y = 0;
            if(y >= SCOPE_H) y = SCOPE_H - 1;
            if(y == i) line[2 + j] = '*';
        }

        pos.X = 0;
        pos.Y = g_scope_pos.Y + (SHORT)i;
        SetConsoleCursorPosition(g_hout, pos);
        printf("%s", line);
    }

    {
        char buf[128];
        int bars = (int)(g_peak * 40.0);
        char bar[41];
        if(bars > 40) bars = 40;
        for(i = 0; i < 40; i++) bar[i] = (i < bars) ? '#' : '.';
        bar[40] = '\0';
        snprintf(buf, sizeof(buf), "  level: %s  cutoff: %.0fHz", bar, g_cutoff);
        pos.X = 0;
        pos.Y = g_scope_pos.Y + SCOPE_H;
        SetConsoleCursorPosition(g_hout, pos);
        printf("%s", buf);
    }
    fflush(stdout);
}

int main(void)
{
    WAVEFORMATEX wf;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int i;
    int last_w=0,last_c=0,last_v=0;

    g_hout = GetStdHandle(STD_OUTPUT_HANDLE);

    ZeroMemory(&wf,sizeof(wf));
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = 2;
    wf.nSamplesPerSec  = SAMPLE_RATE;
    wf.wBitsPerSample  = 16;
    wf.nBlockAlign     = 4;
    wf.nAvgBytesPerSec = SAMPLE_RATE*4;

    if(waveOutOpen(&g_wo,WAVE_MAPPER,&wf,
        (DWORD_PTR)wave_cb,0,CALLBACK_FUNCTION)!=MMSYSERR_NOERROR){
        printf("failed to open audio device\n");
        return 1;
    }

    for(i=0;i<NBUFS;i++){
        ZeroMemory(&g_hdr[i],sizeof(WAVEHDR));
        g_hdr[i].lpData = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = BLOCK*2*sizeof(short);
        waveOutPrepareHeader(g_wo,&g_hdr[i],sizeof(WAVEHDR));
        fill(i);
        waveOutWrite(g_wo,&g_hdr[i],sizeof(WAVEHDR));
    }

    printf("=== Mini Synth Sprint 3 : 滤波器 + 示波器 ===\n");
    printf("play: a s d f g h j k l ;   wave: w   cutoff: c/v   quit: q\n");

    if(GetConsoleScreenBufferInfo(g_hout,&csbi)){
        g_scope_pos.X = 0;
        g_scope_pos.Y = csbi.dwCursorPosition.Y;
    }
    for(i=0;i<SCOPE_H+1;i++) printf("\n");

    while(!g_stop){
        scan_keys();

        if(GetAsyncKeyState('W') & 0x8000){
            if(!last_w){ last_w=1; g_wavetype=!g_wavetype; }
        } else last_w=0;

        if(GetAsyncKeyState('C') & 0x8000){
            if(!last_c){ last_c=1; g_cutoff*=1.25;
                if(g_cutoff>CUTOFF_MAX) g_cutoff=CUTOFF_MAX; }
        } else last_c=0;

        if(GetAsyncKeyState('V') & 0x8000){
            if(!last_v){ last_v=1; g_cutoff/=1.25;
                if(g_cutoff<CUTOFF_MIN) g_cutoff=CUTOFF_MIN; }
        } else last_v=0;

        if(GetAsyncKeyState('Q') & 0x8000){ g_stop=1; break; }

        render_scope();
        g_peak *= 0.92;
        Sleep(25);
    }

    {
        COORD pos;
        int n;
        char blank[80];
        memset(blank,' ',sizeof(blank));
        blank[79]='\0';
        for(n=0;n<SCOPE_H+1;n++){
            pos.X=0; pos.Y=g_scope_pos.Y+n;
            SetConsoleCursorPosition(g_hout,pos);
            printf("%s", blank);
        }
        pos.X=0; pos.Y=g_scope_pos.Y+SCOPE_H+1;
        SetConsoleCursorPosition(g_hout,pos);
    }

    printf("bye.\n");
    Sleep(250);
    waveOutReset(g_wo);
    for(i=0;i<NBUFS;i++) waveOutUnprepareHeader(g_wo,&g_hdr[i],sizeof(WAVEHDR));
    waveOutClose(g_wo);
    (void)csbi;
    return 0;
}
