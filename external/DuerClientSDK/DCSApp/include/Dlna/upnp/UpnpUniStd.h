#ifndef DUEROS_DCS_APP_UPNPUNISTD_H
#define DUEROS_DCS_APP_UPNPUNISTD_H

#ifdef WIN32
/* Do not #include <unistd.h> on WIN32. */
#else /* WIN32 */
#include <unistd.h> /* for close() */
#endif /* WIN32 */

#endif /* UPNPUNISTD_H */
