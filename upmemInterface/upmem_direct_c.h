#include <stdint.h>
#include <stdbool.h>

typedef struct XDirectPIMInterface XDirectPimInterface;
typedef XDirectPimInterface* XDPI;

void deleteDirectPIMInterface(XDirectPimInterface* self);
void allocate(XDirectPimInterface* self, uint32_t nr_of_ranks, char* binary);
void deallocate(XDPI self);
void Launch(XDPI self, bool async);
void upmem_sync(XDPI self);
void PrintLog(XDPI self);
void SendToPIM(XDPI self, uint8_t** buffers, char* symbol_name, uint32_t symbol_offset, uint32_t length, bool async);
void ReceiveFromPIM(XDPI self, uint8_t** buffers, char* symbol_name, uint32_t symbol_offset, uint32_t length, bool async);
