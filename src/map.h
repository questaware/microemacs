#if USE_MAPSTREAM
#define update_map(map)  (map)->update_ct += 1
#else
#define update_map(map) 
#endif

			/* all these enumeration values cannot change */
#define T_DOMINT1     1
#define T_DOMINT2     2
#define T_DOMINT3     3
#define T_DOMINT4     4
#define T_DOMSTR      5
#define T_DOMCHAR0    8  /* variable length */
#define T_DOMCHAR2   10
#define T_DOMCHAR3   11



#if SIZEOF_PTR <= 4
#define FOFFS_MAP 24
#else
#define FOFFS_MAP 32
#endif

#if 0
#if LITTLEENDIAN
#define mk_format(ktyp, koffs, enylen) \
 (Format_t)((enylen << 16)+(koffs<<8)+ktyp)
#else
#define mk_format(ktyp, koffs, enylen) \
 (Format_t)((((ktyp<<8)+koffs)<< 16) + enylen)
#endif
#endif


typedef struct Format_s
{ Byte     key_type;
  Byte     key_offs;
  Short    eny_len;
} Format_t, *Format;

typedef struct Map_s
{ Format_t     format;
  Byte * 			 table;
//const Char * srch_key;	/* These 2 form a pair.   See Note 1. */
#ifndef MINIMAP
  Short    last_ix;
  Short    max_len;	/* in bytes */
  Short    curr_len;	/* in bytes */
#endif
  Short    curr_mult;   /* in entries */
#if USE_MAPSTREAM
  Int	   	 update_ct;
  Byte	   is_mallocd;
  Byte     filler[3];   /* must be long aligned for 64 bit machines */
#endif
#ifndef MINIMAP
  Byte     c[1];    /* actually Mapeny_t *//* must be aligned to SIZEOF_PTR */
#endif
} Map_t, *Map;

#ifndef MINIMAP

#define mk_const_map(typ,keyoffs, tbl, deduct) \
		     			{{typ, keyoffs, sizeof(tbl[0])}, (Byte*)tbl,\
	/*last_ix*/  -1, \
	/*max_len*/		sizeof(tbl),  sizeof(tbl)-(deduct)*sizeof(tbl[0]), \
	/*curr_mult*/ sizeof(tbl)/sizeof(tbl[0])-(deduct) \
              }
#else
#define mk_const_map(typ,keyoffs, tbl, deduct) \
		     			{{typ, keyoffs, sizeof(tbl[0])}, (Byte*)tbl,\
	/*srch_key*/  \
	/*curr_mult*/ sizeof(tbl)/sizeof(tbl[0])-(deduct), \
              }
#endif

typedef union
{ Char   domint1;
  Short  domint2;
  Int    domint4;
  Char * domstr;
  Char   domchararr[4];
} Key_t, *Key;
											/* Note 1. The user must maintain the contents of the location 
												 pointed at constant when streaming for type T_DOMSTR */
typedef struct Mapstrm_s
{ Map	    map;
  Int       update_ct;
  Key_t     key;		/* the key last seen */
} Mapstrm_t, * Mapstrm;

#define map_alloc(reclen, len)  (Map)aalloc(FOFFS_MAP + (reclen) * (len));

Map mk_map(Map c, Format_t format, Vint len);
void map_cache_clear(Map map)	;

Vint binary(int wh, const char * key);
Cc map_add_(Map * map_ref, Byte * rec);
Byte * map_find_(Map map, Byte * table, void * key);
void map_remove_last_(Map * map_ref, Byte * table);
Mapstrm mk_strm(Mapstrm strm, Map map, void * key);
Byte * map_next_(Mapstrm strm, Byte * table);


#define map_add(map, rec) map_add_((map), (Byte*)(rec))
#define map_find(map, key) map_find_((map), &(map)->c[0], (void*)(key))
#define map_remove_last(map) map_remove_last_(map, &(*map)->c[0])
#define map_next(strm)    map_next_((strm), &((strm)->map->c)[0])


#define binary_const(wh,key)    binary(wh,key)
#define map_find_const(map,tbl,key) map_find_((map), (Byte*)(tbl), (void*)(key))
#define map_next_const(strm,tbl)    map_next_((strm), (Byte*)(tbl))
