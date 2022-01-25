/* String instructions */

#ifdef VM_STRING_SUPPORT

void midSTRING(WM_STRING* dst, WM_STRING* str, WM_INT from, WM_INT to);
WM_INT findSTRING(WM_STRING* haystack, WM_STRING* needle, WM_INT search_ofs);

#ifdef VM_WIDECHAR
#define VM_COPY_CHARS	wcsncpy
#define VM_COMPARE_CHARS wcsncmp
#define VM_SNPRINTF swprintf 
#else
#define VM_COPY_CHARS	strncpy
#define VM_COMPARE_CHARS strncmp
#define VM_SNPRINTF snprintf 
#endif

#include <stdio.h>  

#if defined(_MSC_VER)

#include <stdio.h>
#include <float.h>
#define snprintf _snprintf

#ifndef NAN
const unsigned long dNAN[1] = {0x7ff80000};
#define NAN (*( float* )dNAN)
#endif

#endif

void VMCLASS_PREFIX WMC_String(WM_BYTE opt)
{
	WM_ADDRESS wDst = GetProgramAddress();  /* DST */

	switch(opt & 0xFF)
	{
		case VMF_STRASGN & 0xFF:		/* String assignment */
			{
				WM_ADDRESS p1 = GetProgramAddress();
				WM_STRING s;
				getSTRING(p1, &s);
				setSTRING(wDst, &s);
				break;
			}

		case VMF_LEN_STRING_INT & 0xFF:		/* Returns length of the string */
			{
				WM_STRING s;
				getSTRING(GetProgramAddress(), &s);
				setINT(wDst, s.length);
				break;
			}

		case VMF_LEFT_STRING_INT & 0xFF:	/* Returns beginning of the string */
			{
				WM_STRING par1, wRes;
				WM_INT len;
				getSTRING(wDst, &wRes);
				getSTRING(GetProgramAddress(), &par1);
		
				len = getINT(GetProgramAddress());

				midSTRING(&wRes, &par1, 0, len);

				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_RIGHT_STRING_INT & 0xFF:	/* Returns end of the string */
			{
				WM_STRING par1, wRes;
				WM_INT  len;
				getSTRING(wDst, &wRes);
				getSTRING(GetProgramAddress(), &par1);
		
				len = getINT(GetProgramAddress());

				midSTRING(&wRes, &par1, par1.length - len, len);

				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_MID_STRING_INT_INT & 0xFF:	/* Returns substring */
			{
				WM_STRING par1, wRes;
				WM_INT from, len;
				getSTRING(wDst, &wRes);
				getSTRING(GetProgramAddress(), &par1);
		
				len = getINT(GetProgramAddress());
				from = getINT(GetProgramAddress()) - 1;  // 1-based index

				midSTRING(&wRes, &par1, from, len);

				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_INSERT_STRING_STRING_INT & 0xFF:	/* Inserts incl string into src at offset position */
			{
				WM_STRING src, incl, wRes;
				WM_INT offset;
				getSTRING(wDst, &wRes);
				getSTRING(GetProgramAddress(), &src);
				getSTRING(GetProgramAddress(), &incl);
		
				offset = getINT(GetProgramAddress()) - 1;

				if (offset < 0)
					offset = 0;

				if (offset > src.length)
					offset = src.length;

				wRes.length = 0;
				copySTRING(&wRes, 0, &src, 0, offset);	/* first part */
				copySTRING(&wRes, offset, &incl, 0, incl.length);	/* inserted part */
				copySTRING(&wRes, offset + incl.length, &src, offset, src.length-offset);	/* last part */

				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_DELETE_STRING_INT_INT & 0xFF:	/* Inserts incl string into src at offset position */
			{
				WM_STRING par1, wRes;
				WM_INT from, len;
				getSTRING(wDst, &wRes);
				getSTRING(GetProgramAddress(), &par1);
		
				from = getINT(GetProgramAddress()) - 1;
				len = getINT(GetProgramAddress());

				if (from < 0)
					from = 0;

				if (len + from > par1.length)
					len = par1.length - from;

				wRes.length = 0;
				copySTRING(&wRes, 0, &par1, 0, from);	/* first part */
				copySTRING(&wRes, from, &par1, from + len, par1.length-len-from);	/* last part */

				setSTRING(wDst, &wRes);
				break;
			}			

		case VMF_FIND_INT_STRING_STRING & 0xFF:	/* Finds substring position in string. If not found returns -1. */
		case VMF_FINDEX_INT_STRING_STRING_INT_INT & 0xFF:	/* Finds substring position in string. If not found returns -1. */
			{
				WM_STRING haystack, needle;
				WM_INT search_offs, search_len;
				WM_INT wRes = -1;
				getSTRING(GetProgramAddress(), &haystack);
				getSTRING(GetProgramAddress(), &needle);

				if ((opt & 0xFF) == (VMF_FIND_INT_STRING_STRING & 0xFF))
				{
					search_offs = 0;
					search_len = haystack.length;
				}
				else
				{
					search_offs = getINT(GetProgramAddress()) - 1;
					search_len = getINT(GetProgramAddress());
				}

				if (search_offs < 0)
					search_offs = 0;

				if (search_offs > haystack.length - needle.length)
					break;

				if (search_offs + search_len > haystack.length)
					search_len = haystack.length - search_offs; 	

				wRes = findSTRING(&haystack, &needle, search_offs);
				if (wRes > search_offs + search_len)
					wRes = -1;

				setINT(wDst, wRes);
				break;
			}
		
		case VMF_REPLACE_STRING_STRING_INT_INT & 0xFF:
			{
				WM_STRING src, replacement, wRes;
				WM_INT search_offs, search_len;
				WM_INT pos = -1;
				getSTRING(GetProgramAddress(), &src);
				getSTRING(GetProgramAddress(), &replacement);

				search_offs = getINT(GetProgramAddress()) - 1;
				search_len = getINT(GetProgramAddress());

				setSTRING(wDst, &src);	/* first make a copy */
				getSTRING(wDst, &wRes);		/* working output */

				if (search_offs < 0)
					search_offs = 0;

				if (search_offs > src.length)
					break;

				if (search_offs + search_len > src.length)
					search_len = src.length - search_offs; 

				copySTRING(&wRes, search_offs, &replacement, 0, search_len);	/* inserted part */

				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_FINDREPLACE_STRING_STRING_INT_INT_STRING & 0xFF:	/* Replaces occurences of first string with other string */
			{
				WM_STRING haystack, needle, replacement, wRes;
				WM_INT search_offs, search_len;
				WM_INT pos = -1;
				getSTRING(GetProgramAddress(), &haystack);
				getSTRING(GetProgramAddress(), &needle);

				search_offs = getINT(GetProgramAddress()) - 1;
				search_len = getINT(GetProgramAddress());

				getSTRING(GetProgramAddress(), &replacement);

				if (search_offs < 0)
					search_offs = 0;

				setSTRING(wDst, &haystack);	/* first make a copy */
				getSTRING(wDst, &wRes);		/* working output */

				if (search_offs > haystack.length - needle.length)
					break;

				if (search_offs + search_len > haystack.length)
					search_len = haystack.length - search_offs; 

				int ofsfix = 0;
				int search_from = search_offs;
				while((pos = findSTRING(&haystack, &needle, search_from)) != -1)
				{
					copySTRING(&wRes, pos + ofsfix, &replacement, 0, replacement.length);	/* inserted part */
					copySTRING(&wRes, pos + ofsfix + replacement.length, &haystack, pos + needle.length, haystack.length - pos - needle.length);	/* copy tail */					
					ofsfix += replacement.length - needle.length;
					wRes.length += replacement.length - needle.length;		/* adjust size */
					search_from = pos + needle.length;

					if (search_from > search_offs + search_len)
						break;
				}

				setSTRING(wDst, &wRes);
				break;
			}
			
		case VMF_STRING_TO_REAL & 0xFF:	/* Converts STRING to REAL value. Returns NAN if value cannot be converted and sets bit 16#10 returned by GET_STATUS_WORD1 function */
			{
				WM_STRING par1;

				//WM_DWORD nan = 0xFFE00000;

				WM_REAL wRes = 0;
				//memcpy(&wRes, &nan, sizeof(wRes));
				WM_BOOL success = 0;

				WM_REAL div = 1;

				getSTRING(GetProgramAddress(), &par1);		/* input */
				WM_CHARACTER* s = par1.chars;

				int i = 0;

				while(*s == ' ') { i++; s++; };

				if (*s == '-')
				{
					s++;
					i++;
					div = -1;
				};

				int has_dot = 0;

				for (; i<par1.length; i++)
				{
					if ((*s == '.') && !has_dot)
					{
						has_dot = 1; 
					}
					else
					{
						int dig = *s - '0';
						if (dig >= 0 && dig <= 9)
						{
							if (has_dot)
								div /= 10.0f;

							if (!success)
							{
								success = 1;
								wRes = 0.0;
							}
							wRes = wRes * 10.0f + (WM_REAL)dig;
						}
						else
							break;
					}
					s++;
				};


				if (!success)
					wStatus1 |= WMSTAT_BADFORMAT;
				else
					wStatus1 &= ~WMSTAT_BADFORMAT;

				wRes = wRes * div;
				setREAL(wDst, wRes);
				break;
			}

		case VMF_STRING_TO_INT & 0xFF:	/* Converts STRING to INT value. Returns NAN if value cannot be converted and sets bit 16#10 returned by GET_STATUS_WORD1 function */
			{
				WM_STRING par1;

				WM_INT wRes = 0;
				WM_BOOL success = 0;

				WM_INT div = 1;

				getSTRING(GetProgramAddress(), &par1);		/* input */
				WM_CHARACTER* s = par1.chars;

				int i = 0;

				while(*s == ' ') { i++; s++; };

				if (*s == '-')
				{
					s++;
					i++;
					div = -1;
				};

				for (; i<par1.length; i++)
				{
					int dig = *s - '0';
					if (dig >= 0 && dig <= 9)
					{
						if (!success)
						{
							success = 1;
						}
						wRes = wRes * 10 + (WM_INT)dig;
					}
					else
						break;
					s++;
				};


				if (!success)
					wStatus1 |= WMSTAT_BADFORMAT;
				else
					wStatus1 &= ~WMSTAT_BADFORMAT;

				wRes = wRes * div;
				setINT(wDst, wRes);
				break;
			}

		case VMF_INT_TO_STRING & 0xFF:	/* Converts INT to STRING value. If value is negative then include '-' character before number */
			{
				WM_STRING wRes;
				WM_INT par1;
				getSTRING(wDst, &wRes);
				par1 = getINT(GetProgramAddress());
				int buf_size = (wRes.chars_size+1)*sizeof(WM_CHARACTER);
				WM_CHARACTER* buf = (WM_CHARACTER*)malloc(buf_size);
				int len = VM_SNPRINTF(buf, wRes.chars_size+1, "%d", par1);
				if (len == -1)
					wRes.length = 0;		// does not fit
				else
				{
					VM_COPY_CHARS(wRes.chars, buf, len);
					wRes.length = len;
				}
				free(buf);
				setSTRING(wDst, &wRes);
				break;
			}

		case VMF_REAL_TO_STRING & 0xFF:	/* Converts REAL to STRING value. */
			{
				WM_STRING wRes;
				WM_REAL par1;
				getSTRING(wDst, &wRes);
				par1 = getREAL(GetProgramAddress());

				if (par1 != par1)	//NaN
				{
					WM_CHARACTER nan[] = "NaN";
					VM_COPY_CHARS(wRes.chars, nan, 3);
					wRes.length = 3;
				}
				else
				{
					int buf_size = (wRes.chars_size+1)*sizeof(WM_CHARACTER);
					WM_CHARACTER* buf = (WM_CHARACTER*)malloc(buf_size);
					int len = VM_SNPRINTF(buf, wRes.chars_size+1, "%g", (double)par1);
					if (len == -1)
						wRes.length = 0;		// does not fit
					else
					{
						VM_COPY_CHARS(wRes.chars, buf, len);
						wRes.length = len;
					}
					free(buf);
				}
				setSTRING(wDst, &wRes);
				break;
			}
		
#ifdef VM_LREAL_SUPPORT
		case VMF_STRING_TO_LREAL & 0xFF:	/* Converts STRING to REAL value. Returns NAN if value cannot be converted and sets bit 16#10 returned by GET_STATUS_WORD1 function */
			{
				WM_STRING par1;

				//WM_DWORD nan = 0xFFE00000;

				WM_LREAL wRes = 0;//0;
				//memcpy(&wRes, &nan, sizeof(wRes));
				WM_BOOL success = 0;

				WM_LREAL div = 1;

				getSTRING(GetProgramAddress(), &par1);		/* input */
				WM_CHARACTER* s = par1.chars;

				int i = 0;

				while(*s == ' ') { i++; s++; };

				if (*s == '-')
				{
					s++;
					i++;
					div = -1;
				};

				int has_dot = 0;

				for (; i<par1.length; i++)
				{
					if ((*s == '.') && !has_dot)
					{
						has_dot = 1; 
					}
					else
					{
						int dig = *s - '0';
						if (dig >= 0 && dig <= 9)
						{
							if (has_dot)
								div /= 10.0f;

							if (!success)
							{
								success = 1;
								wRes = 0.0;
							}
							wRes = wRes * 10.0f + (WM_LREAL)dig;
						}
						else
							break;
					}
					s++;
				};


				if (!success)
					wStatus1 |= WMSTAT_BADFORMAT;
				else
					wStatus1 &= ~WMSTAT_BADFORMAT;

				wRes = wRes * div;
				setLREAL(wDst, wRes);
				break;
			}
#endif
		case VMF_STRING_TO_DINT & 0xFF:	/* Converts STRING to INT value. Returns NAN if value cannot be converted and sets bit 16#10 returned by GET_STATUS_WORD1 function */
			{
				WM_STRING par1;

				WM_DINT wRes = 0;
				WM_BOOL success = 0;

				WM_DINT div = 1;

				getSTRING(GetProgramAddress(), &par1);		/* input */
				WM_CHARACTER* s = par1.chars;

				int i = 0;

				while(*s == ' ') { i++; s++; };

				if (*s == '-')
				{
					s++;
					i++;
					div = -1;
				};

				for (; i<par1.length; i++)
				{
					int dig = *s - '0';
					if (dig >= 0 && dig <= 9)
					{
						if (!success)
						{
							success = 1;
						}
						wRes = wRes * 10 + (WM_DINT)dig;
					}
					else
						break;
					s++;
				};


				if (!success)
					wStatus1 |= WMSTAT_BADFORMAT;
				else
					wStatus1 &= ~WMSTAT_BADFORMAT;

				wRes = wRes * div;
				setDINT(wDst, wRes);
				break;
			}


		case VMF_DINT_TO_STRING & 0xFF:	/* Converts DINT to STRING value. If value is negative then include '-' character before number */
			{
				WM_STRING wRes;
				WM_DINT par1;
				getSTRING(wDst, &wRes);
				par1 = getDINT(GetProgramAddress());
				int buf_size = (wRes.chars_size+1)*sizeof(WM_CHARACTER);
				WM_CHARACTER* buf = (WM_CHARACTER*)malloc(buf_size);
				int len = VM_SNPRINTF(buf, wRes.chars_size+1, "%ld", par1);
				if (len == -1)
					wRes.length = 0;		// does not fit
				else
				{
					VM_COPY_CHARS(wRes.chars, buf, len);
					wRes.length = len;
				}
				free(buf);
				setSTRING(wDst, &wRes);
				break;
			}

#ifdef VM_LREAL_SUPPORT
		case VMF_LREAL_TO_STRING & 0xFF:	/* Converts LREAL to STRING value. One integer digit, dot, 7 mantisa characters, 'E�', 3 characters of mantisa. If value is negative then include '-' character before whole number. Always occupies approx. 15 characters. */
			{
				WM_STRING wRes;
				WM_LREAL par1;
				getSTRING(wDst, &wRes);
				par1 = getLREAL(GetProgramAddress());

				if (par1 != par1)	//NaN
				{
					WM_CHARACTER nan[] = "NaN";
					VM_COPY_CHARS(wRes.chars, nan, 3);
					wRes.length = 3;
				}
				else
				{
					int buf_size = (wRes.chars_size+1)*sizeof(WM_CHARACTER);
					WM_CHARACTER* buf = (WM_CHARACTER*)malloc(buf_size);
					int len = VM_SNPRINTF(buf, wRes.chars_size+1, "%g", par1);
					if (len == -1)
						wRes.length = 0;		// does not fit
					else
					{
						VM_COPY_CHARS(wRes.chars, buf, len);
						wRes.length = len;
					}
					free(buf);
				}
				setSTRING(wDst, &wRes);
				break;
			}
#endif

		case VMF_STRING_TO_LINT & 0xFF:	/* Converts STRING to INT value. Returns NAN if value cannot be converted and sets bit 16#10 returned by GET_STATUS_WORD1 function */
			{
				WM_STRING par1;

				WM_LINT wRes = 0;
				WM_BOOL success = 0;

				WM_LINT div = 1;

				getSTRING(GetProgramAddress(), &par1);		/* input */
				WM_CHARACTER* s = par1.chars;

				int i = 0;

				while(*s == ' ') { i++; s++; };

				if (*s == '-')
				{
					s++;
					i++;
					div = -1;
				};

				for (; i<par1.length; i++)
				{
					int dig = *s - '0';
					if (dig >= 0 && dig <= 9)
					{
						if (!success)
						{
							success = 1;
						}
						wRes = wRes * 10 + (WM_LINT)dig;
					}
					else
						break;
					s++;
				};


				if (!success)
					wStatus1 |= WMSTAT_BADFORMAT;
				else
					wStatus1 &= ~WMSTAT_BADFORMAT;

				wRes = wRes * div;
				setLINT(wDst, wRes);
				break;
			}


		case VMF_LINT_TO_STRING & 0xFF:	/* Converts LINT to STRING value. If value is negative then include '-' character before number */
			{
				WM_STRING wRes;
				WM_LINT par1;
				getSTRING(wDst, &wRes);
				par1 = getLINT(GetProgramAddress());
				int buf_size = (wRes.chars_size+1)*sizeof(WM_CHARACTER);
				WM_CHARACTER* buf = (WM_CHARACTER*)malloc(buf_size);
				int len = VM_SNPRINTF(buf, wRes.chars_size+1, "%lld", par1);
				if (len == -1)
					wRes.length = 0;		// does not fit
				else
				{
					VM_COPY_CHARS(wRes.chars, buf, len);
					wRes.length = len;
				}
				free(buf);
				setSTRING(wDst, &wRes);
				break;
			}			
				
		case VMF_CHAR_AT_STRING_INT & 0xFF:		/* Returns string character at the given position */
			{
				WM_STRING str;
				WM_INT pos;
				WM_CHARACTER ch = 0;
				getSTRING(GetProgramAddress(), &str);
				pos = getINT(GetProgramAddress());
				if ((pos >= 0) && (pos < str.length))
					ch = str.chars[pos-1]; 

				setBYTE(wDst, ch);
				break;
			}
			
		case VMF_CHAR_SET_STRING_INT_BYTE & 0xFF:		/* Sets string character at the given position */
			{
				WM_STRING str;
				WM_INT pos;
				WM_INT ret = -1;
				WM_CHARACTER ch = 0;
				getSTRING(GetProgramAddress(), &str);
				pos = getINT(GetProgramAddress());
				ch = getBYTE(GetProgramAddress());
				if ((pos >= 0) && (pos < str.length))
				{
					str.chars[pos-1] = ch;
					ret = pos;
				}

				setINT(wDst, ret);
				break;
			}

		default:
			bResult = WM_UNKNOWN;
			break;
	}
}


void copySTRING(WM_STRING* dst, WM_INT dst_offset, WM_STRING* src, WM_INT src_offset, WM_INT length)
{
	int actual_length = length;

	if (dst->chars_size < dst_offset + length)
		actual_length = dst->chars_size - dst_offset;

	if (actual_length > 0)
		VM_COPY_CHARS(dst->chars + dst_offset, src->chars + src_offset, actual_length);

	int rightmost = dst_offset + actual_length;	
	if (dst->length < rightmost)
		dst->length = rightmost;		/* grow only */

}

WM_BOOL lessSTRING(WM_STRING* s1, WM_STRING* s2)
{
	int len = s1->length < s2->length ? s1->length : s2->length;

	int scp = VM_COMPARE_CHARS(s1->chars, s2->chars, len);

	if (scp < 0)
		return 1;
	else
	if (scp == 0)	// equal up to common length
		if (s1->length < s2->length)	// check the lengths
			return 1;

	return 0;
}


/* is equal */

WM_BOOL equSTRING(WM_STRING* s1, WM_STRING* s2)
{
	if (s1->length != s2->length)
		return 0;

	int scp = VM_COMPARE_CHARS(s1->chars, s2->chars, s1->length);

	return (scp == 0);
}

void midSTRING(WM_STRING* dst, WM_STRING* str, WM_INT from, WM_INT len)
{
	dst->length = 0;

	if (from < 0)
		from = 0;
	if (from > str->length)	/* from left up to string size */
		return;
	if (from + len > str->length)	/* from left up to string size */
		len = str->length - from;
	if (len > dst->chars_size)	/* buffer check */
		len = dst->chars_size;

	copySTRING(dst, 0, str, from, len);
	dst->length = len;
}

WM_INT findSTRING(WM_STRING* haystack, WM_STRING* needle, WM_INT search_ofs)
{
        int i;
        int needle_len = needle->length;
		WM_CHARACTER* phaystack = haystack->chars + search_ofs;

        if ((needle_len == 0) || (search_ofs > haystack->length))
                return -1;

        for (i=0; (int)(haystack->length-needle_len)>=i; i++)
        {
                if ((phaystack[0] == needle->chars[0]) && VM_COMPARE_CHARS(phaystack, needle->chars, needle_len) == 0)
                        return i + search_ofs;
				phaystack++;
        }
        return -1;
}

void VMCLASS_PREFIX getSTRING(WM_ADDRESS src, WM_STRING* dst)  /* get string variable */
		{
		dst->length = getBYTE(src);
		dst->chars_size = getBYTE(src+1);
		dst->padding = getWORD(src+2);
		dst->chars = (WM_CHARACTER*)getDataPtr(src+4);
		}
	
void VMCLASS_PREFIX setSTRING(WM_ADDRESS dst, WM_STRING* src)	/* set string variable */
	{
		setBYTE(dst, src->length);
		int maxlen = getBYTE(dst+1);
		setWORD(dst+2, src->padding);
		VM_COPY_CHARS((WM_CHARACTER*)getDataPtr(dst+4), src->chars, src->length > maxlen ? maxlen : src->length);
	}

#endif