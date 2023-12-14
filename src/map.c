#include <stdlib.h>
#include <string.h>
#include "build.h"

#include "map.h"

#define LENINC 4096


static Vint compare(const Byte * key, Map map, Key tgt)

{ 
  switch (map->format.key_type)
  { when T_DOMSTR:  /* printf("Comparing %s %s\n", map->srch_key, tgt->domstr);*/
                    return  strcmp((Char*)key, tgt->domstr);
#ifndef MINIMAP
    when T_DOMCHAR0:return  1;  /* never matches */
    case T_DOMINT1: return  (Char)key - tgt->domint1;
    when T_DOMINT2: return  (Short)key - tgt->domint2;
    when T_DOMINT3: return  0; /* not implemented */
    when T_DOMINT4: return  (Int)key - tgt->domint4;
#endif
    otherwise     { Vint ix;
                    key -= T_DOMCHAR0;
						   /* printf("Compare %s %s\n", map->srch_key, tgt->domchararr);*/
  		  				    for (ix = T_DOMCHAR0-1;  ++ix < map->format.key_type; )
							      { Byte tch = tgt->domchararr[ix-T_DOMCHAR0];
											if (key[ix] != tch)
                				return key[ix] - tch;
		      		        if (tch == 0)
    		  		          break;
      			      	}
				  		      return OK;
		    					}
  }
}



#ifndef MINIMAP

Vint bin_unit_len;		/* secondary result */

#endif

extern Map_t g_namemap;
extern Map_t g_fnamemap;
extern Map_t g_evmap;

#ifndef S_WIN32
extern Map_t g_keymap;
extern Map_t g_capmap;
#endif
				/* +ve => found */
				/* -ve => before index (minus one based!) */
int binary(int wh, const char * key)

{ Map map =	wh <  0 ? &g_namemap  :
						wh == 0 ? &g_fnamemap :
#ifndef S_WIN32
						wh == 2 ? &g_keymap   :
						wh == 3 ?	&g_capmap   :
#endif
											&g_evmap;
//map->srch_key = key;
	Byte * table = map->table;
  Byte * keyfld = &table[map->format.key_offs];
  Vint low = 0;		/* search limit is entire list */
	Vint uppp1 = map->curr_mult;
#ifndef MINIMAP
  if (map->format.key_type == T_DOMCHAR0)		/* not binary */
  {
  	bin_unit_len = 1;

    if (uppp1 <= 0)
      return -1;
    while (true)
    { Byte * src = (Byte*)key;
      Vint comp = 0;
      Vint slow = low;

      for (; ; ++low)
      { if (comp == 0)
          comp = *src - (Vint)keyfld[low];
				if (keyfld[low] == 0)
				  break;
				++src;
      }
      low += map->format.key_offs + 1;
      map->format.eny_len = low - slow;

      if (comp >= 0 || low > map->curr_len)
        return comp == 0 ? slow : -(slow+1);
    }
  }
  else
#endif
	{	while (uppp1 > low)
    { int i = (low + uppp1) >> 1;			/* get the midpoint! */

      int comp = compare(key, map, (Key)&keyfld[map->format.eny_len*i]);
      if (comp == 0)
        return i;
      if (comp < 0)
        uppp1 = i;
      else
        low = i + 1;
    }

    return -(uppp1+1); /* one before->after; one to change base */
  }
}


#ifndef MINIMAP
											    /* need be called only if key points */
											    /* at the same location, but *key changes */
void map_cache_clear(Map map)	

{ map->last_ix = -1;
}



Cc map_add_(Map * map_ref, Byte * rec)

{ Map map = *map_ref;
  Char * key = &rec[map->format.key_offs];
  Byte kt = map->format.key_type;
  key = kt  >  T_DOMSTR ?  key                :
  		  kt == T_DOMINT1 ? (Char*)key[0]	: /* warning OK */
  		  kt == T_DOMINT2 ? (Char*)*(Short*)key : /* warning OK */
  		  kt == T_DOMINT3 ? 0            	:
  		  		    *(Char**)key;
{ Vint ix = - binary(map, key, map->c) * bin_unit_len;
  if (ix <= OK)
  { map->last_ix = -ix;
    return HALTED;
  }

  ix -= bin_unit_len;
  map->last_ix = ix;
{ register Vint len = map->format.eny_len;
  
  if (len + map->curr_len > map->max_len)
  { if (len < LENINC)
      len = LENINC;
    len += map->curr_len;
    map = (Map)mallocz(FOFFS_MAP+1+len);
    if (map == null)
      return ENOSPACE;
    memcpy(map, *map_ref, fieldoffs(Map, c) + ix);
    map->is_mallocd = true;
    map->max_len = len;
  }

  len = map->curr_len - ix + 1;
  if (len > 0)
    memmove(&map->c[ix+map->format.eny_len], &(*map_ref)->c[ix], len);

  map->curr_len += map->format.eny_len;
#ifndef MINIMAP
  map->curr_mult += 1;
#endif
  update_map(map);
  memcpy(&map->c[ix], rec, map->format.eny_len);
  if (map != *map_ref && (*map_ref)->is_mallocd)
    free(*map_ref);
  *map_ref = map;
  return OK;
}}}



Byte * map_find_(Map map, Byte * table, void * key)

{ Vint ix = map->last_ix;
  Byte * eny = &table[ix];

  if (key == null && map->format.key_type >= T_DOMSTR)
    key = "";
  
  if (! in_range(ix,0, map->curr_len-1) || 
      compare(key, map, (Key)&eny[map->format.key_offs]) != OK)
  { ix = binary(map, key, table) * bin_unit_len;
    if (ix < OK)
    { map->last_ix = -(ix +  bin_unit_len);
      return null;
    }
    map->last_ix = ix;
    eny = &table[ix];
  }
  return eny;
}



void map_remove_last_(Map * map_ref, Byte * table)

{ Map map = *map_ref;
  if (map->curr_len-map->last_ix > 0)
	  memmove(&table[map->last_ix],
  	        &table[map->last_ix+map->format.eny_len], 
    	      map->curr_len-map->last_ix);
  map->curr_len -= map->format.eny_len;
#ifndef MINIMAP
  map->curr_mult -= 1;
#endif
  update_map(map);
}

#endif

#if USE_MAPSTREAM

		/* the key must be maintained constant if of type T_DOMSTR */
Mapstrm mk_strm(Mapstrm strm, Map map, void * key)

{ Vint bigsz = map->format.key_type - T_DOMCHAR0;
  if (strm == null)
    strm = (Mapstrm)mallocz(sizeof(Mapstrm_t) + (bigsz < 0 ? 0 : bigsz));

  strm->map        = map;
  strm->update_ct  = -1;
  strm->key.domstr = key;
  if (bigsz > 0)
    memcpy(&strm->key.domchararr[0], key, bigsz);
  return strm;
}



Byte * map_next_(Mapstrm strm, Byte * table)

{ Map map = strm->map;
  Vint bigsz = map->format.key_type - T_DOMCHAR0;

//map->srch_key = map->format.key_type < T_DOMCHAR0
//                     ? strm->key.domstr : &strm->key.domchararr[0];

  if (strm->update_ct == map->update_ct
      /* && ! (compare(map, (Key)&eny[map->format.key_offs]) != OK and
                  strm->ix < map->curr_len) */)
    map->last_ix += map->format.eny_len;
  else
  { if (map_find_(map, table, bigsz <= 0 ? strm->key.domstr
			                : &strm->key.domchararr[0]) != null and
        strm->update_ct != -1)			/* first time */
      map->last_ix += map->format.eny_len;
      
    strm->update_ct = map->update_ct;
  }

{ Byte * eny = &table[map->last_ix];
  if      (bigsz < 0)
  { bigsz = bigsz == T_DOMSTR - T_DOMCHAR0 ? sizeof(char*) : bigsz + T_DOMCHAR0;
  }
  else if (bigsz == 0)
  { bigsz = strlen(&eny[map->format.key_offs]) + 1;  /* requires a 0 at EO map */
    map->format.eny_len = map->format.key_offs + bigsz;
  }
  if (map->last_ix >= map->curr_len)
  { map->last_ix -= map->format.eny_len;
    return null;
  }

  memcpy(&strm->key.domchararr[0], &eny[map->format.key_offs], bigsz);
  return eny;
}}

#endif
