#include "vm_variable.h"
#include <cstddef>

#ifdef DCP_SUPPORT
using namespace tinyxml2;
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int VMDCP::Load(const char* file)
{

#ifdef DCP_SUPPORT

    printf("Loading DCP: %s", file);

	// open DCP file
    int fileDCP;
    struct stat buf;

    fileDCP = open(file, O_RDONLY);

	if (fileDCP == -1)
    {
        printf("Cannot read DCP !\n");  
		return -1;
    }

	//check size of the code

    long len = fstat(fileDCP, &buf);

    if (len == -1)
    {
        printf("Cannot determine DCP size !\n");  
		return -2;
    }

	// allocate memory for the code
	dcpData = (unsigned char*)malloc(buf.st_size+1);
	if (!dcpData)
    {
        printf("Cannot allocate DCP data !\n");  
		return -3;
    }

	// read the code into the allocated memory
    read(fileDCP, dcpData, buf.st_size);
    dcpData[buf.st_size] = 0;

	close(fileDCP);

    if(xmlDocument.Parse((const char*)dcpData)!= XML_SUCCESS)
    {
        printf("Bad DCP file !\n");  
        return -4;
    };

    // get root element (<CPDEV>)
    xmlVar = xmlDocument.FirstChildElement("CPDEV")->FirstChildElement("TARGET")->FirstChildElement("GLOBAL")->FirstChildElement("VAR");

    if (!xmlVar)
    {
        printf("DCP file not valid !\n");  
        return -5;
    }

    return 0;

#else

    return -1;

#endif

}

VMVariable* VMDCP::InitVariable(const char* name)
{

#ifdef DCP_SUPPORT
    XMLElement* xv = xmlVar;

    while(xv != NULL)
    {
        const char* vn;
        vn = xv->Attribute("LName");

        if (vn && !strcmp(name, vn))
        {
            unsigned int address;
            const char* saddr;
            saddr = xv->Attribute("Addr");
            sscanf(saddr, "%x", &address);

            int size;
            xv->QueryIntAttribute("Size",&size);
            VMVariable* var = new VMVariable(name, address, size);
            printf("%s found at %x\n",name, var->GetAddress());
            return var;
        }
        xv = xv->NextSiblingElement("VAR");
    }
#endif

    printf("%s not found !", name);  
    return NULL;

}

VMVariable::VMVariable(const char* name, WM_ADDRESS address, WM_BYTE size)
{
    var_name = name;
    var_address = address;
    var_size = size;
}

WM_ADDRESS VMVariable::GetAddress()
{
    return var_address;
}

WM_BYTE VMVariable::GetSize()
{
    return var_size;
}