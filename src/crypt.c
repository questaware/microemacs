/*	Crypt:	Encryption routines for MicroEMACS
		written by Dana Hoggatt and Daniel Lawrence
*/
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

#if	CRYPT

#if 0

static int Pascal mod95(register int val)

{
	/*  The mathematical MOD does not match the computer MOD  */

	/*  Yes, what I do here may look strange, but it gets the
		job done, and portably at that.  */

	while (val >= 9500)
		val -= 9500;
	while (val >= 950)
		val -= 950;
	while (val >= 95)
		val -= 95;
	while (val < 0)
		val += 95;
	return val;
}
#endif
/**********
 *	ucrypt - in place encryption/decryption of a buffer
 *
 *	(C) Copyright 1986, Dana L. Hoggatt
 *	1216, Beck Lane, Lafayette, IN
 *
 *	When consulting directly with the author of this routine, 
 *	please refer to this routine as the "DLH-POLY-86-B CIPHER".  
 *
 *	This routine was written for Dan Lawrence, for use in V3.8 of
 *	MicroEMACS, a public domain text/program editor.  
 *
 *	I kept the following goals in mind when preparing this function:
 *
 *	    1.	All printable characters were to be encrypted back
 *		into the printable range, control characters and
 *		high-bit characters were to remain unaffected.	this
 *		way, encrypted would still be just as cheap to 
 *		transmit down a 7-bit data path as they were before.
 *
 *	    2.	The encryption had to be portable.  The encrypted 
 *		file from one computer should be able to be decrypted 
 *		on another computer.
 *
 *	    3.	The encryption had to be inexpensive, both in terms
 *		of speed and space.
 *
 *	    4.	The system needed to be secure against all but the
 *		most determined of attackers.
 *
 *	For encryption of a block of data, one calls crypt passing 
 *	a pointer to the data block and its length. The data block is 
 *	encrypted in place, that is, the encrypted output overwrites 
 *	the input.  Decryption is totally isomorphic, and is performed 
 *	in the same manner by the same routine.  
 *
 *	Before using this routine for encrypting data, you are expected 
 *	to specify an encryption key.  This key is an arbitrary string,
 *	to be supplied by the user.  To set the key takes two calls to 
 *	crypt().  First, you call 
 *
 *		crypt(NULL, vector)
 *
 *	This resets all internal control information.  Typically (and 
 *	specifically in the case on MICRO-emacs) you would use a "vector" 
 *	of 0.  Other values can be used to customize your editor to be 
 *	"incompatable" with the normally distributed version.  For 
 *	this purpose, the best results will be obtained by avoiding
 *	multiples of 95.
 *
 *	Then, you "encrypt" your password by calling 
 *
 *		crypt(pass, strlen(pass))
 *
 *	where "pass" is your password string.  Crypt() will destroy 
 *	the original copy of the password (it becomes encrypted), 
 *	which is good.	You do not want someone on a multiuser system 
 *	to peruse your memory space and bump into your password.  
 *	Still, it is a better idea to erase the password buffer to 
 *	defeat memory perusal by a more technical snooper.  
 *
 *	For the interest of cryptologists, at the heart of this 
 *	function is a Beaufort Cipher.	The cipher alphabet is the 
 *	range of printable characters (' ' to '~'), all "control" 
 *	and "high-bit" characters are left unaltered.
 *
 *	The key is a variant autokey, derived from a wieghted sum 
 *	of all the previous clear text and cipher text.  A counter 
 *	is used as salt to obiterate any simple cyclic behavior 
 *	from the clear text, and key feedback is used to assure 
 *	that the entire message is based on the original key, 
 *	preventing attacks on the last part of the message as if 
 *	it were a pure autokey system.
 *
 *	Overall security of encrypted data depends upon three 
 *	factors:  the fundamental cryptographic system must be 
 *	difficult to compromise; exhaustive searching of the key 
 *	space must be computationally expensive; keys and plaintext 
 *	must remain out of sight.  This system satisfies this set
 *	of conditions to within the degree desired for MicroEMACS.
 *
 *	Though direct methods of attack (against systems such as 
 *	this) do exist, they are not well known and will consume 
 *	considerable amounts of computing time.  An exhaustive
 *	search requires over a billion investigations, on average.
 *
 *	The choice, entry, storage, manipulation, alteration, 
 *	protection and security of the keys themselves are the 
 *	responsiblity of the user.  
 *
 **********/

int g_key_ = 0;	/* 29 bit encipherment key */
int g_salt = 0;	/* salt to spice up key with */


int Pascal ucrypt(char * cptr, int len)
                    	    		  	/* buffer of characters to be encrypted */
                      		    		/* number of characters in the buffer */
{ int key = g_key_;

	while (len--)		/* for every character in the buffer */
	{ int cc = *cptr;		
				/* only encipher printable characters */
	  if (cc >= ' ' && cc <= '~')
	  {
	      /* Feed the upper few bits of the key back into itself.
      		 This ensures that the starting key affects the entire message.
      		 Use effort here to prevent the key from exceeding 29 bits.
      		 This is so the autokey calculation in a later statement won't 
      		 overflow making the key go negative. 
      		 Machine behavior in these cases tends not to be portable.  */

	    key = (key & 0x1FFFFFFFl) ^ ((key >> 29) & 0x3l);

	     /**  Down-bias the character, perform a Beaufort encipherment, and 
      		  up-bias the character again.	We want key to be positive  
      		  so the left shift here will be more portable and the 
      		  mod95() faster   **/
#if 0
	    cc = mod95((int)(key % 95) - (cc - ' ')) + ' ';
#else
	    cc = (int)(key % 95) - (cc - ' ');

 	    while (cc >= 9500)
	      cc -= 9500;
	    while (cc >= 950)
	      cc -= 950;
	    while (cc >= 95)
        cc -= 95;
	    while (cc < 0)
	      cc += 95;
	      
	    cc = cc + ' ';
#endif
	     /**  the salt spices up the key a little bit, helping to obscure 
      		  any patterns in the clear text, particularly when all the 
      		  characters (or long sequences of them) are the same.	We do 
      		  not want the salt to go negative, or it will affect the key 
      		  too radically.  It is always a good idea to chop off cyclics 
      		  to prime values.  **/

	    if (++g_salt >= 20857)	/* prime modulus */
	      g_salt = 0;

	     /**  our autokey (a special case of the running key) is being 
      		  generated by a weighted checksum of clear text, cipher 
      		  text, and salt.

      		  I had to allow the older broken key calculation to let us 
      		  decrypt old files.  I hope to take this out eventually.**/

	    key += key + (cc ^ *cptr) + g_salt;
	  }
	  *cptr++ = cc; 		/* put character back into buffer */
	}
	g_key_ = key;
	return OK;
}


static
 void Pascal initcrypt(int wh, char *cptr, unsigned len)
			/* buffer of characters to be encrypted */
			/* number of characters in the buffer */
{ g_key_ = wh * thread_id();
	g_salt = 0;
  ucrypt(cptr, len);
}


void Pascal double_crypt(char * cptr, int len)

{	char ch;
	int fix;
	
	for (fix = -1; (ch = cptr[++fix]) != 0 && ch != 9; )
		;
		
	if (ch != 0)
	{ int sl = strlen(cptr);
		++fix;
		initcrypt(0, cptr, fix);
		initcrypt(0, cptr, fix);
	  g_key_ += thread_id();
		ucrypt(cptr+fix,sl-fix); 
	}
}


#if 0

void Pascal cryptremote(char * remote)

{ if (remote != NULL)
	{	char remote_key[100];                    // Security by obscurity?
  	strcpy(remote_key, int_asc((int)(thread_id())));
 		initcrypt(1, remote_key, strlen(remote_key));
  	ucrypt(remote, strlen(remote));
  }
}

#endif

#define got_key(p) (*p != NULL)

																/* reset encryption key of current buffer */
int Pascal setekey(CRYPTKEY * key_ref)
	
{	int cc = got_key(key_ref);

	if (!cc)
	{	char mykey[NPAT]; 					/* new encryption string */
		int stt = strlen(strcpy(mykey, int_asc(thread_id())));
	{	int odisinp = g_disinp;
		g_disinp = -1; 							/* turn command input echo off */
		cc = mlreply(TEXT33, mykey+stt, NPAT - 15);
		g_disinp = odisinp;
		mlwrite(" ");								/* clear it off the bottom line */
		if (cc == TRUE)
		{	if (mykey[stt] == 0)
				strpcpy(mykey+stt, g_ekey, NPAT-15);
			else
			{ extern int g_chars_since_shift;

				if (g_chars_since_shift < 10000)
					strcat(mykey,int_asc(g_chars_since_shift));
			}

		  initcrypt(0, mykey, strlen(mykey)); /* re-encrypt it, seeding it to start */
			*key_ref = strdup(mykey);
		}
	}}
	return cc;
}



int Pascal setuekey(int f, int n)/* reset encryption key of current buffer */

{	int cc = setekey(&curbp->b_key);
	if (cc == TRUE)
		curbp->b_flag |= MDCRYPT;
	else
		curbp->b_flag &= ~MDCRYPT;
/*if (f)*/
  curbp->b_flag |= BFCHG;
//mbwrite("CRYPT");
	upmode();
	return TRUE;
}



void Pascal resetkey(CRYPTKEY * key_ref)	/* reset the encryption key if needed */

{	if (!got_key(key_ref))
    *key_ref = g_ekey;
  if (setekey(key_ref) == TRUE)
  { char * key = strdup(*key_ref);
    int sl  = strlen(key);
    int tl = strlen(int_asc(thread_id()));
		initcrypt(0, key, sl);	      					/* de-encrypt */
		initcrypt(0, key+tl, sl-tl);						/* re-encrypt, seeding it to start */
		free(key);
  }
}

#else
void resetkey(char ** key_ref)
{
}
#endif

