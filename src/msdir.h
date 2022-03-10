#if S_WIN32 == 0
#define msd_ignore(x) ,x
#else
#define msd_ignore(x)
#endif

extern Cc     msimp_fn_re_ic(Char*, Char*, int /*bool */);

extern Cc   msd_init (const Char *, int);
extern Char * msd_nfile(void);
extern Cc msd_push (void);
extern Char * msd_pop (void);
extern int  msd_getprops (Char * fn);

extern struct stat msd_stat;


#define MSD_ROFILE    1
#define MSD_HIDFILE   2
#define MSD_SYSFILE   4
#define MSD_USEPATH   8
#define MSD_DIRY     16
#define MSD_CHGD     32
#define MSD_NOPERM   64
#define MSD_MATCHED 128
#define MSD_SLINK   256

#define MSD_INTERNAL 0x100
#define MSD_SIMPLE   0x200
#define MSD_NOCHD    0x400
#define MSD_REPEAT   0x800

#define MSD_STAY     0x1000
#define MSD_SHOWDIR  0x2000
#define MSD_POST     0x4000
#define MSD_IC	     0x8000
#define MSD_PRE    MSD_DIRY

