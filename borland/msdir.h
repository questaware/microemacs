extern Char * match_fn_re(Char*, Char*);
extern Char * match_fn_re_ic(Char*, Char*, int /*bool */);
extern Cc     msimp_fn_re_ic(Char*, Char*, int /*bool */);

extern Cc   msd_init (Char *, Char *, int);
extern Char * msd_nfile(int * fnoffs);
extern Cc msd_push (void);
extern Char * msd_pop (void);
extern int  msd_getprops (Char * fn);


extern Set16     msd_attrs;
extern Int       msd_size;
extern Int       msd_mtime;

extern struct stat msd_stat;

#if S_MSDOS
# define msd_nlinks() 1
#else
# ifdef S_IFLNK
/*# define msd_isslink() (**msd_stat.st_nlink+**(msd_stat.st_mode & S_IFLNK))*/
# define msd_isslink() (msd_attrs & MSD_SLINK)
/*# define msd_isslink() ((msd_stat.st_mode & S_IFMT) == S_IFLNK)*/
# else
#  include        <sys/stat.h>
#  define msd_isslink() 0
  error error
# endif
#endif





#define MSD_ROFILE    1
#define MSD_HIDFILE   2
#define MSD_SYSFILE   4
#define MSD_VOLNAME   8
#define MSD_DIRY     16
#define MSD_CHGD     32
#define MSD_NOPERM   64
#define MSD_MATCHED 128
#define MSD_SLINK   256

#define MSD_INTERNAL 256
#define MSD_SIMPLE   512
#define MSD_NOCHD   1024

#define MSD_REPEAT 0x1000
#define MSD_AUTO   0x2000
#define MSD_POST   0x4000
#define MSD_IC	   0x8000
#define MSD_PRE    MSD_DIRY

