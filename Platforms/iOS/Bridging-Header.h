#import "CPDevVM/vm/vm.h"
#import "CPDevVM/vm/vmplatform.h"
#import <Foundation/Foundation.h>

//int32_t ObjC_WMP_LoadData(NSData* data);
WM_BYTE getByte__(WM_ADDRESS address);
void setBool__(WM_ADDRESS address, WM_BOOL v);

WM_INT getInt__(WM_ADDRESS x);
void setInt__(WM_ADDRESS address, WM_INT v);

