#ifndef MCP_PCI_H
#define MCP_PCI_H
#include <stdint.h>


class McpPci {
public:
    McpPci(void);
    ~McpPci(void);

    bool createHandles(void);
    bool freeHandles(void);;
    bool freeHandle(void);
    bool sendData(unsigned int data);
    bool readData(unsigned short * pData, int l, int* lread);

    static const bool SUCCEED = true;
    static const bool FAILED  = false;
private:
    int fTheFd;


};
#endif //MCP_PCI_H
