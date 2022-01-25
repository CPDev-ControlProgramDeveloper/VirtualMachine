
/* stack emulation functions */

void VMCLASS_PREFIX push_DataOfsStack(WM_ADDRESS w)
{
	if (wDataOfsStackPtr<DATA_STACK_SIZE)
		wDataOfsStack[wDataOfsStackPtr++]=w;
	else
		VMP_Debug(DBG_VME_STACK_OVERFLOW, 0);
}

WM_ADDRESS VMCLASS_PREFIX pull_DataOfsStack(void)
{
	if (wDataOfsStackPtr)
		return wDataOfsStack[--wDataOfsStackPtr];
	else
	{
		VMP_Debug(DBG_VME_STACK_EMPTY, 0);	
		return 0;
	}
}

void VMCLASS_PREFIX push_CallingStack(WM_ADDRESS w)
{
	if (wCallingStackPtr<CALL_STACK_SIZE)
		wCallingStack[wCallingStackPtr++]=w;
	else
		VMP_Debug(DBG_VME_STACK_OVERFLOW, 1);
}

WM_ADDRESS VMCLASS_PREFIX pull_CallingStack(void)
{
	if (wCallingStackPtr)
		return wCallingStack[--wCallingStackPtr];
	else
	{
		VMP_Debug(DBG_VME_STACK_EMPTY, 1);	
		return 0;
	}
}

void VMCLASS_PREFIX clear_Stacks(void)
{
	wCallingStackPtr = 0;
	wDataOfsStackPtr = 0;
}
