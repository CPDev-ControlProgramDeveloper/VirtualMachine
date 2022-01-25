#ifdef VM_MATH_SUPPORT

#include <math.h>
#include <stdlib.h>

/* EXP and ABS implementation */

void VMCLASS_PREFIX WMC_EXPT(WM_BYTE opt)
{
        WM_ADDRESS wDst = GetProgramAddress();  /* DST */

        switch(opt & 0x0F)
        {
                case VMF_EXPT_SINT_SINT & 0x000F:               /* SINT ops */
                        {
                                WM_DINT wRes = 1, i;
                                WM_SINT pod = getSINT(GetProgramAddress());
                                WM_SINT wyk = getSINT(GetProgramAddress());
                                
                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                for (i = wyk; i > 0; i /= 2) {
                                        if (i % 2 == 1) {
                        wRes = wRes * pod;
                                        }
                                        pod = pod * pod;
                                }

                                setDINT(wDst, wRes);
                        }
                        break;

                case VMF_EXPT_INT_SINT & 0x000F:                /* INT & SINT */
                        {
                                WM_DINT wRes = 1, i;
                                WM_INT pod = getINT(GetProgramAddress());
                                WM_SINT wyk = getSINT(GetProgramAddress());
                                
                                //while(wyk-- > 0)
                                //      wRes *= pod;

                                for (i = wyk; i > 0; i /= 2) {
                                        if (i % 2 == 1) {
                        wRes = wRes * pod;
                                        }
                                        pod = pod * pod;
                                }
                                
                                setDINT(wDst, wRes);
                        }
                        break;

                case VMF_EXPT_DINT_SINT & 0x000F:               /* DINT & SINT */
                        {
                                WM_DINT wRes = 1, i;
                                WM_DINT pod = getDINT(GetProgramAddress());
                                WM_SINT wyk = getSINT(GetProgramAddress());
                                
                                //while(wyk-- > 0)
                                //      wRes *= pod;

                                for (i = wyk; i > 0; i /= 2) {
                                        if (i % 2 == 1) {
                        wRes = wRes * pod;
                                        }
                                        pod = pod * pod;
                                }
                        
                                setDINT(wDst, wRes);

                        }
                        break;

                case VMF_EXPT_REAL_SINT & 0x000F:               /* REAL ops */
                        {
                                WM_REAL wRes = 1;
                                WM_REAL pod = getREAL(GetProgramAddress());
                                WM_SINT wyk = getSINT(GetProgramAddress());
                                
                                wRes = (WM_REAL)((exp(log(pod)*wyk)));

                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                setREAL(wDst, wRes);
                        }
                        break;

                case VMF_EXPT_REAL_INT & 0x000F:                /* REAL ops */
                        {
                                WM_REAL wRes = 1;
                                WM_REAL pod = getREAL(GetProgramAddress());
                                WM_INT  wyk = getINT(GetProgramAddress());
                                
                                wRes = (WM_REAL)((exp(log(pod)*wyk)));

                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                setREAL(wDst, wRes);
                        }
                        break;

                case VMF_EXPT_REAL_DINT & 0x000F:               /* REAL ops */
                        {
                                WM_REAL wRes = 1;
                                WM_REAL pod = getREAL(GetProgramAddress());
                                WM_DINT wyk = getDINT(GetProgramAddress());
                                
                                wRes = (WM_REAL)((exp(log(pod)*wyk)));

                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                setREAL(wDst, wRes);
                        }
                        break;

                case VMF_EXPT_REAL_REAL & 0x000F:               /* REAL ops */
                        {
                                WM_REAL wRes = 1;
                                WM_REAL pod = getREAL(GetProgramAddress());
                                WM_REAL wyk = getREAL(GetProgramAddress());
                                
                                wRes = (WM_REAL)((exp(log(pod)*wyk)));

                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                setREAL(wDst, wRes);
                        }
                        break;
#ifdef VM_LREAL_SUPPORT
                case VMF_EXPT_LREAL_LREAL & 0x000F:               /* LREAL ops */
                        {
                                WM_LREAL wRes = 1;
                                WM_LREAL pod = getLREAL(GetProgramAddress());
                                WM_LREAL wyk = getLREAL(GetProgramAddress());
                                
                                wRes = (WM_LREAL)((exp(log(pod)*wyk)));

                                //while(wyk-- > 0)
                                //      wRes *= pod;
                                
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                default:
					bResult = WM_UNKNOWN;
        }
        return;
}

void VMCLASS_PREFIX WMC_ABS(WM_BYTE opt)
{
        WM_ADDRESS wDst = GetProgramAddress();  /* DST */

        switch(opt & 0x0F)
        {
                case VMF_ABS_SINT & 0x000F:             /* SINT ops */
                        {
                                WM_SINT wRes = getSINT(GetProgramAddress());
                                if (wRes<0) wRes = -wRes;
                                setSINT(wDst, wRes);
                        }
                        break;

                case VMF_ABS_INT & 0x000F:              /* INT ops */
                        {
                                WM_INT wRes = abs(getINT(GetProgramAddress()));
                                setINT(wDst, wRes);
                        }
                        break;

                case VMF_ABS_DINT & 0x000F:             /* DINT ops */
                        {
                                WM_DINT wRes = labs(getDINT(GetProgramAddress()));
                                setDINT(wDst, wRes);
                        }
                        break;

                case VMF_ABS_LINT & 0x000F:             /* LINT ops */
                        {
                                WM_LINT wRes = getLINT(GetProgramAddress());
                                if (wRes<0) wRes = -wRes;
                                setLINT(wDst, wRes);
                        }
                        break;

                case VMF_ABS_BYTE & 0x000F:             /* BYTE ops */
                        {
                                WM_BYTE wRes = getBYTE(GetProgramAddress());               // to nic nie robi !!!!!
                                setBYTE(wDst, wRes);
                        }
                        break;

                case VMF_ABS_WORD & 0x000F:             /* WORD ops */
                        {
                                WM_WORD wRes = getWORD(GetProgramAddress());               // to nic nie robi !!!!!
                                setWORD(wDst, wRes);
                        }
                        break;

                case VMF_ABS_DWORD & 0x000F:            /* DWORD ops */
                        {
                                WM_DWORD wRes = getDWORD(GetProgramAddress());             // to nic nie robi !!!!!
                                setDWORD(wDst, wRes);
                        }
                        break;

                case VMF_ABS_LWORD & 0x000F:            /* LWORD ops */
                        {
                                WM_LWORD wRes = getLWORD(GetProgramAddress());             // to nic nie robi !!!!!
                                setLWORD(wDst, wRes);
                        }
                        break;

                case VMF_ABS_REAL & 0x000F:             /* REAL ops */
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                if (wRes<0) wRes = -wRes;
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_ABS_LREAL & 0x000F:            /* REAL ops */
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                if (wRes<0) wRes = -wRes;
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                default:
					bResult = WM_UNKNOWN;
        }
        return;
}

void VMCLASS_PREFIX WMC_MATH(WM_BYTE opt)
{
        WM_ADDRESS wDst = GetProgramAddress();  /* DST */

        switch(opt & 0xFF)
        {

                case VMF_SQRT_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)sqrt(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_SQRT_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)sqrt(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_LN_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)log(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;
#ifdef VM_LREAL_SUPPORT
                case VMF_LN_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)log(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_LOG_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)log10(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;
#ifdef VM_LREAL_SUPPORT
                case VMF_LOG_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)log10(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_EXP_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)exp(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_EXP_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)exp(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_SIN_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)sin(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_SIN_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)sin(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_COS_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)cos(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_COS_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)cos(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_TAN_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)tan(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_TAN_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)tan(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_ASIN_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)asin(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_ASIN_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)asin(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_ACOS_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)acos(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_ACOS_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)acos(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_ATAN_REAL & 0x00FF:
                        {
                                WM_REAL wRes = getREAL(GetProgramAddress());
                                wRes = (WM_REAL)atan(wRes);
                                setREAL(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_ATAN_LREAL & 0x00FF:
                        {
                                WM_LREAL wRes = getLREAL(GetProgramAddress());
                                wRes = (WM_LREAL)atan(wRes);
                                setLREAL(wDst, wRes);
                        }
                        break;
#endif
                case VMF_TRUNC_REAL & 0x00FF:
                        {
                                WM_DINT wRes = (WM_DINT)getREAL(GetProgramAddress());
                                setDINT(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_TRUNC_LREAL & 0x00FF:
                        {
                                WM_LINT wRes = (WM_LINT)getLREAL(GetProgramAddress());
                                setLINT(wDst, wRes);
                        }
                        break;
#endif
                case VMF_ROUND_REAL & 0x00FF:
                        {
                                WM_REAL val = getREAL(GetProgramAddress());
                                WM_DINT wRes = (val >= 0) ? (WM_DINT)(val + 0.5) : (WM_DINT)(val - 0.5); 

                                setDINT(wDst, wRes);
                        }
                        break;

#ifdef VM_LREAL_SUPPORT
                case VMF_ROUND_LREAL & 0x00FF:
                        {
                                WM_LREAL val = getLREAL(GetProgramAddress());
                                WM_LINT wRes = (val >= 0) ? (WM_LINT)(val + 0.5) : (WM_LINT)(val - 0.5); 


                                setLINT(wDst, wRes);
                        }
                        break;
#endif
                default:
					bResult = WM_UNKNOWN;
        }
        return;
}



void VMCLASS_PREFIX WMC_EXPT_ABS(WM_BYTE opt)
{
        if (opt < 0x10)
                WMC_EXPT(opt);
        else if (opt < 0x20)
                WMC_ABS(opt);
        else
                WMC_MATH(opt);
}

#endif /* VM_MATH_SUPPORT */