#ifndef _PRINTER_H_
#define _PRINTER_H_

#include <vector>
#include <string>

class Printer
{
public:
    Printer(void);
    ~Printer(void) {}

    struct printer_attr {
        std::string name;
        std::string status;
        std::string message;
        bool isDefault;
    };

    bool GetPrinters(void);
    void GetJobs(void);
	bool IsSonyPrinter();
	char* GetDefaultPrinterPPD(void);
    bool SetDefault(const char *printer);
    int print(char *printer, const char* filename, const char *grp_options, int copies);
    bool CancelAllJobs(void);
    bool GetUsbPrinterUri(std::string &uri);
    bool AddPrinter(const char* printer, const char* device_uri, const char* ppd_file);
    const std::vector<printer_attr>* get_printers(void) { return &m_printers; }
    bool DeletePrinter(const char *printer);
	void LoadFirmware(const std::string uri);
private:
    bool SetDevice(const char* printer, const char* device_uri);
    bool SetPPDFile(const char *printer, const char *file);
    bool ValidateName(const char *name);
    bool EnablePrinter(const char *printer);
    std::vector<printer_attr> m_printers;
};

#endif /* _PRINTER_H_ */
