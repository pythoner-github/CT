#include <cups/cups.h>
#include <cups/ppd.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <zlib.h>

#include "Printer.h"
#include "gui_global.h"
#include "../display/gui_func.h"
#include "PeripheralMan.h"
#include "ViewSystem.h"

using std::string;
using std::vector;

namespace {
    const char *kFirmwarePath = "./res/printer/firmware";
}

Printer::Printer(void)
{
    m_printers.clear();
}

// lpstat.c show_printers
bool Printer::GetPrinters(void)
{
    int num_dests = 0;          /* Number of user destinations */
    cups_dest_t	*dests = NULL;  /* User destinations */

    cupsFreeDests(num_dests, dests);
    num_dests = cupsGetDests(&dests);

    static const char *pattrs[] = { /* Attributes we need for printers... */
        "printer-name",
        "printer-state",
        "printer-state-message",
        "printer-state-reasons",
        "printer-type",
        "printer-info",
        "printer-location",
        "printer-make-and-model",
        "printer-uri-supported"
    };
    static const char *jattrs[] = { /* Attributes we need for jobs... */
        "job-id"
    };

    /*
     * Build a CUPS_GET_PRINTERS request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     *    requested-attributes
     *    requesting-user-name
     */

    ipp_t *request = ippNewRequest(CUPS_GET_PRINTERS);

    ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
            "requested-attributes", sizeof(pattrs) / sizeof(pattrs[0]),
            NULL, pattrs);

    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
            NULL, cupsUser());
    /*
     * Do the request and get back a response...
     */
    ipp_t *response;                /* IPP Response */
    ipp_t *jobs;                    /* IPP Get Jobs response */
    ipp_attribute_t *attr;          /* Current attribute */
    ipp_attribute_t *jobattr;       /* Job ID attribute */
    ipp_attribute_t *reasons;       /* Job state reasons attribute */
    const char *printer;            /* Printer name */
    const char *message;            /* Printer state message */
    const char *description;        /* Description of printer */
    const char *location;           /* Location of printer */
    const char *make_model;         /* Make and model of printer */
    const char *uri;                /* URI of printer */
    ipp_pstate_t pstate;            /* Printer state */
    cups_ptype_t ptype;             /* Printer type */
    int jobid;                      /* Job ID of current job */
    char printer_uri[HTTP_MAX_URI]; /* Printer URI */

    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption());
    if (http == NULL) {
        PRINTF("GetPrinters Error: Unable to connect to server\n");
        cupsFreeDests(num_dests, dests);
        return false;
    }

    if ((response = cupsDoRequest(http, request, "/")) != NULL) {
        if (response->request.status.status_code > IPP_OK_CONFLICT) {
            ippDelete(response);
            cupsFreeDests(num_dests, dests);
            return false;
        }

        /*
         * Loop through the printers returned in the list and display
         * their status...
         */
        int cnt;
        for (cnt = 0, attr = response->attrs; attr != NULL; attr = attr->next, cnt++) {
            /*
             * Skip leading attributes until we hit a job...
             */

            while (attr != NULL && attr->group_tag != IPP_TAG_PRINTER)
                attr = attr->next;

            if (attr == NULL)
                break;

            /*
             * Pull the needed attributes from this job...
             */

            printer     = NULL;
            ptype       = CUPS_PRINTER_LOCAL;
            pstate      = IPP_PRINTER_IDLE;
            message     = NULL;
            description = NULL;
            location    = NULL;
            make_model  = NULL;
            reasons     = NULL;
            uri         = NULL;
            jobid       = 0;

            while (attr != NULL && attr->group_tag == IPP_TAG_PRINTER) {
                if (!strcmp(attr->name, "printer-name") && attr->value_tag == IPP_TAG_NAME)
                    printer = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-state") && attr->value_tag == IPP_TAG_ENUM)
                    pstate = (ipp_pstate_t)attr->values[0].integer;
                else if (!strcmp(attr->name, "printer-type") && attr->value_tag == IPP_TAG_ENUM)
                    ptype = (cups_ptype_t)attr->values[0].integer;
                else if (!strcmp(attr->name, "printer-state-message") && attr->value_tag == IPP_TAG_TEXT)
                    message = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-info") && attr->value_tag == IPP_TAG_TEXT)
                    description = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-location") && attr->value_tag == IPP_TAG_TEXT)
                    location = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-make-and-model") && attr->value_tag == IPP_TAG_TEXT)
                    make_model = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-uri-supported") && attr->value_tag == IPP_TAG_URI)
                    uri = attr->values[0].string.text;
                else if (!strcmp(attr->name, "printer-state-reasons") && attr->value_tag == IPP_TAG_KEYWORD)
                    reasons = attr;
                attr = attr->next;
            }

            /*
             * See if we have everything needed...
             */

            if (printer == NULL) {
                if (attr == NULL)
                    break;
                else
                    continue;
            }

            /*
             * If the printer state is "IPP_PRINTER_PROCESSING", then grab the
             * current job for the printer.
             */

            if (pstate == IPP_PRINTER_PROCESSING) {
                /*
                 * Build an IPP_GET_JOBS request, which requires the following
                 * attributes:
                 *
                 *    attributes-charset
                 *    attributes-natural-language
                 *    printer-uri
                 *    limit
                 *    requested-attributes
                 */

                request = ippNewRequest(IPP_GET_JOBS);

                request->request.op.operation_id = IPP_GET_JOBS;
                request->request.op.request_id   = 1;

                ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                        "requested-attributes",
                        sizeof(jattrs) / sizeof(jattrs[0]), NULL, jattrs);

                httpAssembleURIf(HTTP_URI_CODING_ALL, printer_uri, sizeof(printer_uri),
                        "ipp", NULL, "localhost", 0, "/printers/%s", printer);
                ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
                        "printer-uri", NULL, printer_uri);

                if ((jobs = cupsDoRequest(http, request, "/")) != NULL) {
                    /*
                     * Get the current active job on this queue...
                     */

                    ipp_jstate_t jobstate = IPP_JOB_PENDING;
                    jobid = 0;

                    for (jobattr = jobs->attrs; jobattr; jobattr = jobattr->next) {
                        if (!jobattr->name) {
                            if (jobstate == IPP_JOB_PROCESSING)
                                break;
                            else
                                continue;
                        }

                        if (!strcmp(jobattr->name, "job-id") && jobattr->value_tag == IPP_TAG_INTEGER)
                            jobid = jobattr->values[0].integer;
                        else if (!strcmp(jobattr->name, "job-state") && jobattr->value_tag == IPP_TAG_ENUM)
                            jobstate = (ipp_jstate_t)jobattr->values[0].integer;
                    }

                    if (jobstate != IPP_JOB_PROCESSING)
                        jobid = 0;

                    ippDelete(jobs);
                }
            }

            /*
             * Display it...
             */
            printer_attr prt;
            switch (pstate) {
                case IPP_PRINTER_IDLE :
                    PRINTF("printer %s is idle.  enabled\n", printer);
                    prt.status = _("idle");
                    break;
                case IPP_PRINTER_PROCESSING :
                    PRINTF("printer %s now printing jobid is %d. enabled\n", printer, jobid);
                    prt.status = _("printing");
                    break;
                case IPP_PRINTER_STOPPED :
                    PRINTF("printer %s disabled\n", printer);
                    prt.status = _("disable");
                    break;
            }
            prt.name = printer;
            if (cnt < num_dests)
                prt.isDefault = (bool)dests[cnt].is_default;

            if ((message && *message) || pstate == IPP_PRINTER_STOPPED) {
                if (!message || !*message) {
                    PRINTF("\treason unknown\n");
                    prt.message = "reason unknown";
                } else {
                    PRINTF("\t%s\n", message);
                    prt.message = message;
                }
            }
            m_printers.push_back(prt);

            for (int i = 0; i < num_dests; i++)
                if (!strcasecmp(printer, dests[i].name) && dests[i].instance) {
                    printer_attr prt_instance;
                    switch (pstate) {
                        case IPP_PRINTER_IDLE :
                            PRINTF("printer %s/%s is idle. enabled\n", printer, dests[i].instance);
                            prt_instance.status = _("idle");
                            break;
                        case IPP_PRINTER_PROCESSING :
                            PRINTF("printer %s/%s now printing jobid is %d enabled\n", printer, dests[i].instance, jobid);
                            prt_instance.status = _("printing");
                            break;
                        case IPP_PRINTER_STOPPED :
                            PRINTF("printer %s/%s disabled \n", printer, dests[i].instance);
                            prt_instance.status = _("disable");
                            break;
                    }
                    string prt_name = printer;
                    string instance_name = dests[i].instance;
                    prt_instance.name = prt_name + "/" + instance_name;
                    prt_instance.isDefault = (bool)dests[i].is_default;

                    if ((message && *message) || pstate == IPP_PRINTER_STOPPED) {
                        if (!message || !*message) {
                            PRINTF("\treason unknown\n");
                            prt_instance.message = "reason unknown";
                        } else {
                            PRINTF("\t%s\n", message);
                            prt_instance.message = message;
                        }
                    }
                    m_printers.push_back(prt_instance);
                }

            if (attr == NULL)
                break;
        }
        ippDelete(response);
    }
    cupsFreeDests(num_dests, dests);

    return true;
}

void Printer::GetJobs(void)
{

}

bool Printer::IsSonyPrinter()
{
	char *file = GetDefaultPrinterPPD();
	ppd_file_t *ppd;
	bool ret = 0;
	if((ppd = ppdOpenFile(file)) == NULL)
	{
		PRINTF("ppdOpenfile error!\n");
        return ret;
	}
    //printf("%s-%s: Printer's manufacturer is %s\n", __FILE__, __FUNCTION__, ppd->manufacturer);
	if(ppd->manufacturer)
	{
		char *ptr = strcasestr(ppd->manufacturer, "SONY");
		if(ptr)
			ret = 1;
	}
	else
	{
		PRINTF("Printer's manufacturer is NULL\n");
	}
	ppdClose(ppd);
	if(file)
	{
		unlink(file);
		free(file);
	}
	return ret;
}
char* Printer::GetDefaultPrinterPPD(void)
{
	char *ppd;
	cups_dest_t	*dests;         /* Destinations */
    int num_dests = cupsGetDests(&dests);
    cups_dest_t *dest;          /* Current destination */
    if (num_dests == 0 || !dests || (dest = cupsGetDest(NULL, NULL, num_dests, dests)) == NULL) {
        printf("Unknown printer or class!\n");
        cupsFreeDests(num_dests, dests);
        return false;
    }
	ppd = g_strdup(cupsGetPPD(dest->name));
    cupsFreeDests(num_dests, dests);
    PRINTF("ppd is %s",ppd);
	return ppd;
}
bool Printer::SetDefault(const char *printer)
{
    if (printer == NULL)
        return false;

    char *instance;
    if ((instance = strrchr((char *)printer, '/')) != NULL)
        *instance++ = '\0';

    cups_dest_t	*dests;         /* Destinations */
    int num_dests = cupsGetDests(&dests);

    cups_dest_t *dest;          /* Current destination */
    if (num_dests == 0 || !dests || (dest = cupsGetDest(printer, instance, num_dests, dests)) == NULL) {
        PRINTF("Unknown printer or class!\n");
        cupsFreeDests(num_dests, dests);
        return false;
    }

    /*
     * Set the default destination...
     */
    for (int i = 0; i < num_dests; i++)
        dests[i].is_default = 0;

    dest->is_default = 1;

    cupsSetDests(num_dests, dests);

    int num_options = 0;        /* Number of options */
    cups_option_t *options;     /* Options */
    for (int i = 0; i < dest->num_options; i++)
        if (cupsGetOption(dest->options[i].name, num_options, options) == NULL)
            num_options = cupsAddOption(dest->options[i].name,
                    dest->options[i].value,
                    num_options, &options);

    cupsFreeDests(num_dests, dests);
    return true;
}

// grp_options is space-delimited name/value pairs
// like this "name=value [name=value ... ]"
int Printer::print(char *printer, const char* filename, const char *grp_options, int copies)
{
    int num_options = 0;           /* Number of options */
    cups_option_t *options = NULL; /* Options */
    int num_dests = 0;             /* Number of destinations */
    cups_dest_t *dests;            /* Destinations */

    char *instance;
    cups_dest_t *dest;          /* Selected destination */

    if (access(filename, R_OK) != 0) {
        PRINTF("Error: unable to access \"%s\"\n", filename);
        return -1;
    }

    if (printer == NULL) {
        num_dests = cupsGetDests(&dests);
        if ((dest = cupsGetDest(NULL, NULL, num_dests, dests)) != NULL) {
            printer = dest->name;

            for (int i = 0; i < dest->num_options; i ++)
                if (cupsGetOption(dest->options[i].name, num_options, options) == NULL){
                    num_options = cupsAddOption(dest->options[i].name,
                            dest->options[i].value,
                            num_options, &options);
                }
        }
    }
    if (printer == NULL)
	{
        cupsFreeDests(num_dests, dests);
        return -2;
	}
#if 0
    else if ((instance = strrchr(printer, '/')) != NULL)
        *instance++ = '\0';
    if (num_dests == 0)
        num_dests = cupsGetDests(&dests);
    if ((dest = cupsGetDest(printer, instance, num_dests, dests)) != NULL) {
        for (int i = 0; i < dest->num_options; i++)
            if (cupsGetOption(dest->options[i].name, num_options, options) == NULL)
                num_options = cupsAddOption(dest->options[i].name,
                        dest->options[i].value,
                        num_options, &options);
    }
    if (printer && !cupsGetDest(printer, NULL, num_dests, dests)) {
        PRINTF("%s: Error : non-existent destination!\n", printer);
        return -1;
    }
#endif

    char copies_buf[10];
    sprintf(copies_buf, "%d", copies);
    if (copies > 0)
        num_options = cupsAddOption("copies", copies_buf, num_options, &options);
    if (grp_options)
        num_options = cupsParseOptions(grp_options, num_options, &options);

    const int num_files = 1;
    const char *files[1];    /* Files to print */
    files[0] = filename;

    int job_id = cupsPrintFiles(printer, num_files, files, filename, num_options, options);
    if (job_id < 1) {
        PRINTF("Print Error: %s\n", cupsLastErrorString());
		cupsFreeOptions(num_options, options);
		cupsFreeDests(num_dests, dests);
        return -1;
    }

	cupsFreeOptions(num_options, options);
	cupsFreeDests(num_dests, dests);

    return 0;
}

bool Printer::CancelAllJobs(void)
{
    ipp_op_t op = IPP_PURGE_JOBS; /* Operation */
    ipp_t *request = NULL;        /* IPP request */
    const int purge = 1;          /* Purge or cancel jobs? */
    http_t *http = NULL;          /* HTTP connection to server */
    ipp_t *response;              /* IPP response */
    /*
     * Open a connection to the server...
     */
    if ((http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption())) == NULL) {
        PRINTF("CancelAllJobs Error : Unable to contact server!\n");
        return false;
    }
    request = ippNewRequest(op);

    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, "ipp://localhost/printers/");
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name", NULL, cupsUser());
    ippAddBoolean(request, IPP_TAG_OPERATION, "purge-jobs", purge);
    response = cupsDoRequest(http, request, "/admin/");

    if (response == NULL || response->request.status.status_code > IPP_OK_CONFLICT) {
        PRINTF("purge-jobs failed: %s\n", cupsLastErrorString());
        if (response)
            ippDelete(response);
        return false;
    }
    ippDelete(response);
    return true;
}

// lpinfo.c show_devices()
bool Printer::GetUsbPrinterUri(string &uri)
{
    ipp_t *request;		/* IPP Request */
    ipp_t *response;            /* IPP Response */
    ipp_attribute_t *attr;      /* Current attribute */
    const char *device_class;   /* Pointer to device-class */
    const char *device_id;      /* Pointer to device-id */
    const char *device_info;    /* Pointer to device-info */
    const char *device_make;    /* Pointer to device-make-and-model */
    const char *device_uri;     /* Pointer to device-uri */

    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption());
    if (http == NULL) {
        PRINTF("GetUsbPrinterUri Error : Unable to contact server!\n");
        return false;
    }
    /*
     * Build a CUPS_GET_DEVICES request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     */
    request = ippNewRequest(CUPS_GET_DEVICES);
    /*
     * Do the request and get back a response...
     */

    if ((response = cupsDoRequest(http, request, "/")) != NULL) {
        /*
         * Loop through the device list and display them...
         */

        if (response->request.status.status_code > IPP_OK_CONFLICT) {
            PRINTF("GetUsbPrinterUri: %s\n", cupsLastErrorString());
            ippDelete(response);
            return false;
        }

        for (attr = response->attrs; attr != NULL; attr = attr->next) {
            /*
             * Skip leading attributes until we hit a device...
             */

            while (attr != NULL && attr->group_tag != IPP_TAG_PRINTER)
                attr = attr->next;

            if (attr == NULL)
                break;

            /*
             * Pull the needed attributes from this device...
             */

            device_class = NULL;
            device_info  = NULL;
            device_make  = NULL;
            device_uri   = NULL;
            device_id    = "NONE";

            while (attr != NULL && attr->group_tag == IPP_TAG_PRINTER) {
                if (!strcmp(attr->name, "device-class") && attr->value_tag == IPP_TAG_KEYWORD)
                    device_class = attr->values[0].string.text;
                else if (!strcmp(attr->name, "device-info") && attr->value_tag == IPP_TAG_TEXT)
                    device_info = attr->values[0].string.text;
                else if (!strcmp(attr->name, "device-make-and-model") && attr->value_tag == IPP_TAG_TEXT)
                    device_make = attr->values[0].string.text;
                else if (!strcmp(attr->name, "device-uri") && attr->value_tag == IPP_TAG_URI)
                    device_uri = attr->values[0].string.text;
                else if (!strcmp(attr->name, "device-id") && attr->value_tag == IPP_TAG_TEXT)
                    device_id = attr->values[0].string.text;

                attr = attr->next;
            }

            /*
             * See if we have everything needed...
             */

            if (device_class == NULL || device_info == NULL || device_make == NULL || device_uri == NULL) {
                if (attr == NULL)
                    break;
                else
                    continue;
            }

            /*
             * Display the device...
             */


            string str_uri = device_uri;
            string str_device_id = device_id;
            if (string::npos != str_uri.find("usb://") // uri=usb://...
                    && strcmp(device_class, "direct") == 0 // class=direct
                    && string::npos != str_device_id.find("CLS:PRINTER")) {// device_id=...CLS=PRINTER;...
                PRINTF("\nDevice: uri=%s  class=%s  info=%s  make-and-model=%s  device-id=%s\n", device_uri, device_class, device_info, device_make, device_id);
                uri = device_uri;
            }

            if (attr == NULL)
                break;
        }
        ippDelete(response);
    } else {
        PRINTF("GetUsbPrinterUri: %s\n", cupsLastErrorString());
        return false;
    }

    return true;
}

// lpadmin.c validate_name()
bool Printer::ValidateName(const char *name)
{
    const char	*ptr;			/* Pointer into name */
    /*
     * Scan the whole name...
     */

    for (ptr = name; *ptr; ptr ++)
        if (*ptr == '@')
            break;
        else if ((*ptr >= 0 && *ptr <= ' ') || *ptr == 127 || *ptr == '/' || *ptr == '#')
            return false;

    /*
     * All the characters are good; validate the length, too...
     */
    return ((ptr - name) < 128);
}

// lpadmin.c set_printer_device()
bool Printer::SetDevice(const char* printer, const char* device_uri)
{
    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption());

    ipp_t *request;                        /* IPP Request */
    ipp_t *response;                       /* IPP Response */
    char uri[HTTP_MAX_URI];                /* URI for printer/class */

    /*
     * Build a CUPS_ADD_PRINTER request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     *    printer-uri
     */

    request = ippNewRequest(CUPS_ADD_PRINTER);

    httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL,
            "localhost", 0, "/printers/%s", printer);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
            "printer-uri", NULL, uri);

    /*
     * Add the device URI...
     */

    if (device_uri[0] == '/') {
        /*
         * Convert filename to URI...
         */

        snprintf(uri, sizeof(uri), "file://%s", device_uri);
        ippAddString(request, IPP_TAG_PRINTER, IPP_TAG_URI, "device-uri", NULL,
                uri);
    }
    else
        ippAddString(request, IPP_TAG_PRINTER, IPP_TAG_URI, "device-uri", NULL,
                device_uri);

    /*
     * Do the request and get back a response...
     */

    if ((response = cupsDoRequest(http, request, "/admin/")) == NULL) {
        PRINTF("AddPrinter: %s\n", cupsLastErrorString());
        return false;
    } else if (response->request.status.status_code > IPP_OK_CONFLICT) {
        PRINTF("AddPrinter: %s\n", cupsLastErrorString());
        ippDelete(response);
        return false;
    } else {
        PRINTF("AddPrinter done!\n");
        ippDelete(response);
        return true;
    }
}

// lpadmin.c set_printer_file()
bool Printer::SetPPDFile(const char *printer, const char *file)
{
    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption());

    ipp_t *request;             /* IPP Request */
    ipp_t *response;            /* IPP Response */
    char uri[HTTP_MAX_URI];     /* URI for printer/class */

    /*
     * Build a CUPS_ADD_PRINTER request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     *    printer-uri
     */

    request = ippNewRequest(CUPS_ADD_PRINTER);

    httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL,
            "localhost", 0, "/printers/%s", printer);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
            "printer-uri", NULL, uri);

    /*
     * Do the request and get back a response...
     */

    response = cupsDoFileRequest(http, request, "/admin/", file);
    ippDelete(response);

    if (cupsLastError() > IPP_OK_CONFLICT) {
        PRINTF("SetPPDFile: %s\n", cupsLastErrorString());
        return false;
    } else {
        PRINTF("SetPPDFile done!\n");
        return true;
    }
}

bool Printer::AddPrinter(const char* printer, const char* device_uri, const char* ppd_file)
{
    if (!ValidateName(printer)) {
        PRINTF("AddPrinter: Printer name can only contain printable characters!\n");
        return false;
    }

    if (!SetDevice(printer, device_uri))
        return false;
    if (!SetPPDFile(printer, ppd_file))
        return false;
    if (!EnablePrinter(printer))
        return false;
    return true;
}

// lpadmin.c enable_printer()
bool Printer::EnablePrinter(const char *printer) /* I - Printer to enable */
{
    ipp_t *request;             /* IPP Request */
    ipp_t *response;            /* IPP Response */
    char uri[HTTP_MAX_URI];     /* URI for printer/class */

    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption()); /* Server connection */
    if (http == NULL) {
        PRINTF("Enable Printer: Unable to connect to server: %s\n", strerror(errno));
        return false;
    }

    // PRINTF(("EnablePrinter(%p, \"%s\")\n", http, printer));

    /*
     * Build a CUPS_ADD_PRINTER request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     *    printer-uri
     *    printer-state
     *    printer-is-accepting-jobs
     */

    request = ippNewRequest(CUPS_ADD_PRINTER);

    httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL,
            "localhost", 0, "/printers/%s", printer);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
            "printer-uri", NULL, uri);

    ippAddInteger(request, IPP_TAG_PRINTER, IPP_TAG_ENUM, "printer-state",
            IPP_PRINTER_IDLE);

    ippAddBoolean(request, IPP_TAG_PRINTER, "printer-is-accepting-jobs", 1);

    /*
     * Do the request and get back a response...
     */

    if ((response = cupsDoRequest(http, request, "/admin/")) == NULL) {
        PRINTF("EnablePrinter: %s\n", cupsLastErrorString());
        return false;
    } else if (response->request.status.status_code > IPP_OK_CONFLICT) {
        PRINTF("EnablePrinter: %s\n", cupsLastErrorString());
        ippDelete(response);
        return false;
    } else {
        ippDelete(response);
        PRINTF("EnablePrinter done!\n");
        return true;
    }
}

// lpadmin.c delete_printer()
bool Printer::DeletePrinter(const char *printer)
{
    ipp_t *request;             /* IPP Request */
    ipp_t *response;            /* IPP Response */
    char uri[HTTP_MAX_URI];     /* URI for printer/class */

    http_t *http = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption());
    if (http == NULL) {
        PRINTF("Delete Printer: Unable to connect to server: %s\n", strerror(errno));
        return false;
    }

    // PRINTF("delete_printer(%p, \"%s\")\n", http, printer));

    /*
     * Build a CUPS_DELETE_PRINTER request, which requires the following
     * attributes:
     *
     *    attributes-charset
     *    attributes-natural-language
     *    printer-uri
     */

    request = ippNewRequest(CUPS_DELETE_PRINTER);

    httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp", NULL,
            "localhost", 0, "/printers/%s", printer);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI,
            "printer-uri", NULL, uri);

    /*
     * Do the request and get back a response...
     */

    if ((response = cupsDoRequest(http, request, "/admin/")) == NULL) {
        PRINTF("Delete Printer : %s\n", cupsLastErrorString());
        return false;
    } else if (response->request.status.status_code > IPP_OK_CONFLICT) {
        PRINTF("Delete Printer: %s\n", cupsLastErrorString());
        ippDelete(response);
        return false;
    } else {
        ippDelete(response);
        return true;
    }
}

void Printer::LoadFirmware(const string uri)
{
    char device[256];
    if (!PeripheralMan::GetInstance()->GetPrinterDev(device))
        return;

    if (!g_file_test(device, G_FILE_TEST_EXISTS)) {
        PRINTF("printer device node not found!\n");
        return;
    }

    char firmware[256];
    if (string::npos != uri.find("HP") && string::npos != uri.find("LaserJet")) {
        if (string::npos != uri.find("1000")) {
            sprintf(firmware, "%s/sihp1000.dl", kFirmwarePath);
        } else if (string::npos != uri.find("P1005") || string::npos != uri.find("P1007")) {
            sprintf(firmware, "%s/sihpP1005.dl", kFirmwarePath);
        } else if (string::npos != uri.find("1005")) {
            sprintf(firmware, "%s/sihp1005.dl", kFirmwarePath);
        } else if (string::npos != uri.find("1018")) {
            sprintf(firmware, "%s/sihp1018.dl", kFirmwarePath);
        } else if (string::npos != uri.find("1020")) {
            sprintf(firmware, "%s/sihp1020.dl", kFirmwarePath);
        } else if (string::npos != uri.find("P1006") || string::npos != uri.find("P1008")) {
            sprintf(firmware, "%s/sihpP1006.dl", kFirmwarePath);
        } else if (string::npos != uri.find("P1505")) {
            sprintf(firmware, "%s/sihpP1505.dl", kFirmwarePath);
        } else {
            return;
        }
        if (!g_file_test(firmware, G_FILE_TEST_EXISTS)) {
            PRINTF("printer firmware file not found!\n");
            return;
        }
        char command[256];
        sprintf(command, "cat %s > %s", firmware, device);
        _system_(command);
    }
}


