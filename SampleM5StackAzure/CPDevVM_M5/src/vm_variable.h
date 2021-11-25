#ifndef VM_VARIABLE_H
#define VM_VARIABLE_H

#include "VM\vmtaskclass.h"
#ifdef DCP_SUPPORT
#include <tinyxml2.h>
#endif

class VMVariable
{
public:
	VMVariable(const char* name, WM_ADDRESS address, WM_BYTE size);
	WM_ADDRESS GetAddress();
    WM_BYTE GetSize();

private:
    const char* var_name;   
    WM_ADDRESS var_address; 
    WM_BYTE var_size;
};

class VMDCP
{
public:
    int Load(const char* file);
    VMVariable* InitVariable(const char* name);
private:
    unsigned char* dcpData;   
#ifdef DCP_SUPPORT
    tinyxml2::XMLDocument xmlDocument; 
    tinyxml2::XMLElement * xmlVar;
#endif        
};


#endif
