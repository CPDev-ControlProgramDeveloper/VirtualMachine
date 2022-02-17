#include "vm_variable.h"

#include <M5Core2.h>

#ifdef DCP_SUPPORT
using namespace tinyxml2;
#endif

int VMDCP::Load(const char* file)
{

#ifdef DCP_SUPPORT
    /*
	if (!SD.begin()) {
		Serial.println("SD init failed!");
		while (1);
	}
	Serial.println("SD init ok.");
    */

	// open DCP file
    Serial.print("Loading: ");
    Serial.println(file);

	File fileDCP = SD.open(file, FILE_READ);

	if (!fileDCP)
    {
        Serial.println("Cannot read DCP !");  
		return -1;
    }

	//check size of the code
	long len = fileDCP.size();
	if (len == -1)
    {
        Serial.println("Cannot determine DCP size !");  
		return -2;
    }

	// allocate memory for the code
	dcpData = (unsigned char*)malloc(len+1);
	if (!dcpData)
    {
        Serial.println("Cannot allocate DCP data !");  
		return -3;
    }

	// read the code into the allocated memory
	fileDCP.read(dcpData, len);
    dcpData[len] = 0;

	fileDCP.close();

    if(xmlDocument.Parse((const char*)dcpData)!= XML_SUCCESS)
    {
        Serial.println("Bad DCP file !");  
        return -4;
    };

    // get root element (<CPDEV>)
    xmlVar = xmlDocument.FirstChildElement("CPDEV")->FirstChildElement("TARGET")->FirstChildElement("GLOBAL")->FirstChildElement("VAR");

    if (!xmlVar)
    {
        Serial.println("DCP file not valid !");  
        return -5;
    }

#endif

    return 0;
 
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
            Serial.print(name);
            Serial.print(" found at ");  
            Serial.println(var->GetAddress(), HEX);  
            return var;
        }
        xv = xv->NextSiblingElement("VAR");
    }
#endif

    Serial.print(name);
    Serial.println(" not found !");  
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