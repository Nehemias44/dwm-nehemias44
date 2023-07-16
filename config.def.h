/* See LICENSE file for copyright and license details. */

/* appearance */
 /* border pixel of windows */
static unsigned int borderpx  = 0;
/* gaps between windows */
static unsigned int gappx     = 20;
/* snap pixel */
static unsigned int snap      = 32;
/* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systraypinning = 0;
/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayonleft = 0;
/* systray spacing */
static const unsigned int systrayspacing = 2;
/* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int systraypinningfailfirst = 1;
/* 0 means no systray */
static const int showsystray        = 1;
/* 0 means no bar */
static int showbar            = 1;
/* 0 means bottom bar */
static int topbar = 1;

static const char buttonbar[] = "  ";
/* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const int user_bh            = 30;

static char font[]                  = "Iosevka:size=12";
static char dmenufont[]             = "Iosevka:size=12";
static const char *fonts[]          = { font, "Font Awesome 6 Free Solid:pixelsize=12", "siji:style=Medium:size=24"};
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const char *const autostart[] = {
	"xset", "-b", NULL,
	"xsetroot", "-cursor_name", "left_ptr", NULL,
	"xrdb", "-merge", "$HOME/.Xresources", NULL,	
	"sh", "-c", "$HOME/.fehbg", NULL,
	"picom", NULL,
	"dwmstatus", NULL,
	"mpd", NULL,
	"emacs", "--daemon", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2",  "3",  "4", "5",  "6",  "7",  "8",  "9"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "firefox",  NULL,       NULL,       1 << 3,       0,           -1 },
	{ "TelegramDesktop",  NULL,NULL,      1 << 6,            1,      -1 },
	{ "mpv",      NULL,       NULL,       0,            1,           -1 },
	{ "PacketTracer" , NULL,  NULL,       0,            1,           -1 },
	{ "Zathura",  NULL,       NULL,       0,            1,           -1 },
	{ "Emacs",    NULL,       NULL,       1 << 4,       1,           -1 },
	{ "Sxiv",     NULL,       NULL,        0,            1,           -1 },
	{ "ncmpcpp-ueberzug", NULL,NULL,      0,            1,           -1 },
};

/* layout(s) */
static float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
int nmaster     = 1;    /* number of clients in master area */
int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|||",      col },
};

/* key definitions */
#define MODKEY Mod4Mask
#define MOD    Mod4Mask
#define ALT    Mod1Mask
#define CTRL   ControlMask
#define SHIFT  ShiftMask

#define TAGKEYS(KEY,TAG) \
	{ 1, {{MOD,             KEY}},    view,           {.ui = 1 << TAG} }, \
	{ 1, {{MOD|CTRL,        KEY}},    toggleview,     {.ui = 1 << TAG} }, \
	{ 1, {{MOD|SHIFT,       KEY}},    tag,            {.ui = 1 << TAG} }, \
	{ 1, {{MOD|CTRL|SHIFT,  KEY}},    toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = { "wezterm", "start", "--position=main:30%,30%" , NULL };
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run",
				  "-fn", dmenufont,
				  "-nb", normbgcolor,
				  "-nf", normfgcolor,
				  "-sb", selfgcolor,
				  "-sf", selbgcolor,
				  "-h", "30",
				  NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
              { "font",               STRING,  &font },
              { "dmenufont",          STRING,  &dmenufont },
              { "normbgcolor",        STRING,  &normbgcolor },
              { "normbordercolor",    STRING,  &normbordercolor },
              { "normfgcolor",        STRING,  &normfgcolor },
              { "selbgcolor",         STRING,  &selbgcolor },
              { "selbordercolor",     STRING,  &selbordercolor },
              { "selfgcolor",         STRING,  &selfgcolor },
              { "borderpx",           INTEGER, &borderpx },
              { "snap",               INTEGER, &snap },
              { "showbar",            INTEGER, &showbar },
              { "topbar",             INTEGER, &topbar },
              { "nmaster",            INTEGER, &nmaster },
              { "resizehints",        INTEGER, &resizehints },
              { "mfact",              FLOAT,   &mfact },
};

#include <X11/XF86keysym.h>

#define AudioMute         XF86XK_AudioMute        
#define AudioLowerVolume  XF86XK_AudioLowerVolume 
#define AudioRaiseVolume  XF86XK_AudioRaiseVolume 
#define AudioMicMute      XF86XK_AudioMicMute                              
#define MonBrightnessDown XF86XK_MonBrightnessDown
#define MonBrightnessUp   XF86XK_MonBrightnessUp                          
#define AudioPlay         XF86XK_AudioPlay        
#define AudioStop         XF86XK_AudioStop        
#define AudioPrev         XF86XK_AudioPrev        	
#define AudioNext         XF86XK_AudioNext

static Keychord keychords[] = {
       /* modifier        key        function        argument */
	{ 1, {{MOD,            XK_p     }}, spawn,          {.v = dmenucmd }},
	{ 1, {{MOD|SHIFT,      XK_Return}}, spawn,          {.v = termcmd }},
	{ 1, {{MOD,            XK_b     }}, togglebar,      {0}},
	{ 1, {{MOD,            XK_j     }}, focusstack,     {.i = +1 } },	
	{ 1, {{MOD,            XK_k     }}, focusstack,     {.i = -1 } },
	{ 1, {{MOD,            XK_i     }}, incnmaster,     {.i = +1 } },
	{ 1, {{MOD,            XK_d     }}, incnmaster,     {.i = -1 } },
	{ 1, {{MOD,            XK_h     }}, setmfact,       {.f = -0.05} },
	{ 1, {{MOD,            XK_l     }}, setmfact,       {.f = +0.05} },
	{ 1, {{MOD,            XK_Return}}, zoom,           {0} },
	{ 1, {{MOD,            XK_Tab   }}, view,           {0} },
	{ 1, {{MOD|SHIFT,      XK_c     }}, killclient,     {0} },
	{ 1, {{MOD,            XK_t     }}, setlayout,      {.v = &layouts[0]} },
	{ 1, {{MOD,            XK_f     }}, setlayout,      {.v = &layouts[1]} },
	{ 1, {{MOD,            XK_m     }}, setlayout,      {.v = &layouts[2]} },
	{ 1, {{MOD,            XK_c     }}, setlayout,      {.v = &layouts[3]} },	
	{ 1, {{MOD,            XK_space }}, setlayout,      {0} },
	{ 1, {{MOD|SHIFT,      XK_space }}, togglefloating, {0} },
	{ 1, {{MOD,            XK_Down  }}, moveresize,     {.v = "0x 25y 0w 0h" } },
	{ 1, {{MOD,            XK_Up    }}, moveresize,     {.v = "0x -25y 0w 0h" } },
	{ 1, {{MOD,            XK_Right }}, moveresize,     {.v = "25x 0y 0w 0h" } },
	{ 1, {{MOD,            XK_Left  }}, moveresize,     {.v = "-25x 0y 0w 0h" } },
	{ 1, {{MOD|SHIFT,      XK_Down  }}, moveresize,     {.v = "0x 0y 0w 25h" } },
	{ 1, {{MOD|SHIFT,      XK_Up    }}, moveresize,     {.v = "0x 0y 0w -25h" } },
	{ 1, {{MOD|SHIFT,      XK_Right }}, moveresize,     {.v = "0x 0y 25w 0h" } },
	{ 1, {{MOD|SHIFT,      XK_Left  }}, moveresize,     {.v = "0x 0y -25w 0h" } },
	{ 1, {{MOD|CTRL,       XK_Up    }}, moveresizeedge, {.v = "t"} },
	{ 1, {{MOD|CTRL,       XK_Down  }}, moveresizeedge, {.v = "b"} },
	{ 1, {{MOD|CTRL,       XK_Left  }}, moveresizeedge, {.v = "l"} },
	{ 1, {{MOD|CTRL,       XK_Right }}, moveresizeedge, {.v = "r"} },
	{ 1, {{MOD|CTRL|SHIFT, XK_Up    }}, moveresizeedge, {.v = "T"} },
	{ 1, {{MOD|CTRL|SHIFT, XK_Down  }}, moveresizeedge, {.v = "B"} },
	{ 1, {{MOD|CTRL|SHIFT, XK_Left  }}, moveresizeedge, {.v = "L"} },
	{ 1, {{MOD|CTRL|SHIFT, XK_Right }}, moveresizeedge, {.v = "R"} },
	{ 1, {{MOD,            XK_0     }}, view,           {.ui = ~0 } },
	{ 1, {{MOD|SHIFT,      XK_0     }}, tag,            {.ui = ~0 } },
	{ 1, {{MOD,            XK_comma }}, focusmon,       {.i = -1 } },
	{ 1, {{MOD,            XK_period}}, focusmon,       {.i = +1 } },
	{ 1, {{MOD|SHIFT,      XK_comma }}, tagmon,         {.i = -1 } },
	{ 1, {{MOD|SHIFT,      XK_period}}, tagmon,         {.i = +1 } },
	{ 1, {{MOD,            XK_minus }}, setgaps,        {.i = -1 } },
	{ 1, {{MOD,            XK_equal }}, setgaps,        {.i = +1 } },
	{ 1, {{MOD|SHIFT,      XK_equal }}, setgaps,        {.i = 0  } },
	
	TAGKEYS(          XK_1,                      0)
	TAGKEYS(          XK_2,                      1)
	TAGKEYS(          XK_3,                      2)
	TAGKEYS(          XK_4,                      3)
	TAGKEYS(          XK_5,                      4)
	TAGKEYS(          XK_6,                      5)
	TAGKEYS(          XK_7,                      6)
	TAGKEYS(          XK_8,                      7)
	TAGKEYS(          XK_9,                      8)
	
	{ 1, {{MOD|SHIFT,      XK_q      }}, quit,     {0} },
	{ 1, {{MOD|CTRL|SHIFT, XK_q      }}, quit,     {1} },
	{ 1, {{MOD,            XK_Print  }}, spawn, SHCMD("scrot '%Y-%m-%d_%H:%M_$wx$h.png' -e 'mv $f ~/Imágenes/Screenshots/'") },
	{ 1, {{MOD|SHIFT,      XK_Print  }}, spawn, SHCMD("scrot -s '%Y-%m-%d_%H:%M_$wx$h.png' -e 'mv $f ~/Imágenes/Screenshots/'") },
	{ 1, {{MOD|SHIFT,      XK_v      }}, spawn, SHCMD("st -g 80x24+200+200 -e nvim") },
	{ 1, {{MOD|SHIFT,      XK_r      }}, spawn, SHCMD("wezterm start ranger") },
	{ 1, {{MOD|SHIFT,      XK_n      }}, spawn, SHCMD("myNcmpcpp") },
	{ 1, {{MOD|SHIFT,      XK_f      }}, spawn, SHCMD("firefox") },
	{ 1, {{MOD|SHIFT,      XK_p      }}, spawn, SHCMD("dmenu_xresources") },
	{ 1, {{MOD|SHIFT,      XK_w      }}, spawn, SHCMD("sxiv -g 1000x600+780+420 -t ~/Imágenes/Wallpapers/*") },
	{ 1, {{MOD|CTRL|SHIFT, XK_c      }}, reload_xres,    {0} },

	{ 2, {{MOD, XK_e}, {0, XK_e}}, spawn, SHCMD("emacsclient -c -a 'emacs'") },
	{ 2, {{MOD, XK_e}, {0, XK_d}}, spawn, SHCMD("emacsclient -c -a 'emacs' --eval '(dired nil)'") },
	{ 2, {{MOD, XK_e}, {0, XK_a}}, spawn, SHCMD("emacsclient -c -a 'emacs' --eval '(org-agenda nil \"a\")'") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkButton,            0,           Button1,        spawn,          {.v = dmenucmd} },
	{ ClkTagBar,            MOD,         Button1,        tag,            {0} },
	{ ClkTagBar,            MOD,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,           Button2,        zoom,           {0} },
	{ ClkStatusText,        0,           Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MOD,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MOD,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MOD,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,           Button1,        view,           {0} },
	{ ClkTagBar,            0,           Button3,        toggleview,     {0} },
	{ ClkTagBar,            MOD,         Button1,        tag,            {0} },
	{ ClkTagBar,            MOD,         Button3,        toggletag,      {0} },
};

void
setlayoutex(const Arg *arg)
{
	setlayout(&((Arg) { .v = &layouts[arg->i] }));
}

void
viewex(const Arg *arg)
{
	view(&((Arg) { .ui = 1 << arg->ui }));
}

void
viewall(const Arg *arg)
{
	view(&((Arg){.ui = ~0}));
}

void
toggleviewex(const Arg *arg)
{
	toggleview(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagex(const Arg *arg)
{
	tag(&((Arg) { .ui = 1 << arg->ui }));
}

void
toggletagex(const Arg *arg)
{
	toggletag(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagall(const Arg *arg)
{
	tag(&((Arg){.ui = ~0}));
}

/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signame> [<type> <value>]"` */
static Signal signals[] = {
	/* signum           function */
	{ "focusstack",     focusstack },
	{ "setmfact",       setmfact },
	{ "togglebar",      togglebar },
	{ "incnmaster",     incnmaster },
	{ "togglefloating", togglefloating },
	{ "focusmon",       focusmon },
	{ "tagmon",         tagmon },
	{ "zoom",           zoom },
	{ "view",           view },
	{ "viewall",        viewall },
	{ "viewex",         viewex },
	{ "toggleview",     view },
	{ "toggleviewex",   toggleviewex },
	{ "tag",            tag },
	{ "tagall",         tagall },
	{ "tagex",          tagex },
	{ "toggletag",      tag },
	{ "toggletagex",    toggletagex },
	{ "killclient",     killclient },
	{ "quit",           quit },
	{ "setlayout",      setlayout },
	{ "setlayoutex",    setlayoutex },
	{ "reloadxres",     reload_xres },
};
