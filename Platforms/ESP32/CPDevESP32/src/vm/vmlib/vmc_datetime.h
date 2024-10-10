
/* date & time support functions */


void VMCLASS_PREFIX WMC_DOW(WM_BYTE opt)
{
	switch(opt & 0xFF)
	{
		case VMF_GET_DAYOFWEEK_DATE & 0xFF:
			{
				WM_WORD par1;
				WM_DATE par2;
				par1 = GetProgramWord();
				getDATE(GetProgramWord(),par2);
				
				setINT(par1, dow(par2.date.month,par2.date.day,par2.date.year));
			}
			break;
		case VMF_GET_DAYOFWEEK_DATETIME & 0xFF:
			{
				WM_WORD par1;
				WM_DATE_AND_TIME par2;
				par1 = GetProgramWord();
				getDATE_AND_TIME(GetProgramWord(),par2);
				
				setINT(par1, dow(par2.date.date.month,par2.date.date.day,par2.date.date.year));

			}
			break;
		default:
			bResult = WM_UNKNOWN;
			break;
	}
	return;
}

/* is equal */

WM_BOOL equDATE(WM_DATE d1, WM_DATE d2)
{
/*
	if (d1.year != d2.year)
		return 0;

	if (((d1.month_dow & 0x0F)>> 4) != ((d2.month_dow & 0x0F) >> 4))
		return 0;

	if (d1.day != d2.day)
		return 0;
	return 1;
*/
	return (d1.dw_date == d2.dw_date);
}

WM_BOOL equTOD(WM_TOD t1, WM_TOD t2)
{
/*
	if (t1.hour != t2.hour)
		return 0;

	if (t1.min != t2.min)
		return 0;

	if (t1.sec != t2.sec)
		return 0;

	if (t1.csec != t2.csec)
		return 0;

	return 1;
*/
	return (t1.dw_tod == t2.dw_tod);
}

WM_BOOL equDATE_AND_TIME(WM_DATE_AND_TIME dt1, WM_DATE_AND_TIME dt2)
{
	if (!equDATE(dt1.date,dt2.date))
		return 0;

	if (!equTOD(dt1.tod,dt2.tod))
		return 0;
	return 1;
}

/* compare two, return 1 if first is older */

WM_BOOL lessDATE(WM_DATE d1, WM_DATE d2)
{
/*
	if (d1.year < d2.year)
		return 1;

	if (((d1.month_dow & 0x0F) >> 4) < ((d2.month_dow & 0x0F) >> 4))
		return 1;

	if (d1.day < d2.day)
		return 1;
	return 0;
*/
	return (d1.dw_date < d2.dw_date);
}

WM_BOOL lessTOD(WM_TOD t1, WM_TOD t2)
{
/*
	if (t1.hour < t2.hour)
		return 1;

	if (t1.min < t2.min)
		return 1;

	if (t1.sec < t2.sec)
		return 1;

	if (t1.csec < t2.csec)
		return 1;

	return 0;
*/
	return (t1.dw_tod < t2.dw_tod);
}

WM_BOOL lessDATE_AND_TIME(WM_DATE_AND_TIME dt1, WM_DATE_AND_TIME dt2)
{
	if (lessDATE(dt1.date,dt2.date))
		return 1;
	else
	if (equDATE(dt1.date,dt2.date))
		if (lessTOD(dt1.tod,dt2.tod))
			return 1;

	return 0;
}

int dow(int m, int d, int y)
{
	y-=m<3;
	return(y+y/4-y/100+y/400+"-bed=pen+mad."[m]+d)%7;
}
